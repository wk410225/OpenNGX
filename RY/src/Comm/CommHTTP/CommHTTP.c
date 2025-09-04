#include "CommHTTP.h"
#include <event2/event.h>
#include <event2/http.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#include <arpa/inet.h> 

#define SRV_SEQ_ID_LEN	2
char  gsSrvId[SRV_ID_LEN+1];
char  gsToSrvId[SRV_ID_LEN+1];
char  gsSrvSeq[SRV_SEQ_ID_LEN+1];
char  gsLogFile[LOG_NAME_LEN_MAX];

T_SrvMsq     gatSrvMsq[SRV_MSQ_NUM_MAX];

#define HTTP_BADMETHOD 405  /* method not allowed for this uri */
#define HTTPINTERNAL  500  /* internal error */

#define MAXCONN   30000
#define WORKERNUM 1         /* 工作子进程，用于接收应答 */

#define random(a, b) (rand()%(b-a))+a

static unsigned short shCurrent = 0;
static pid_t ChildPid[WORKERNUM];

typedef struct st_request{
    int index;
    struct evhttp_request *req;
    struct event *timer_ev;
} request_t;

request_t Reqs[MAXCONN + 1];

void get_client_ip(struct evhttp_request *req, char *ip_str, size_t ip_str_len) 
{
    const char *forwarded_for;
    struct evhttp_connection *conn;
    struct sockaddr *sa;
	struct evkeyvalq *headers = evhttp_request_get_input_headers(req);

    // 1. 首先检查常见的代理头 'X-Forwarded-For'
    forwarded_for = evhttp_find_header(headers, "X-Forwarded-For");
    if (forwarded_for != NULL) {
        // 注意：X-Forwarded-For 可能包含逗号分隔的IP列表，第一个是原始客户端IP
        snprintf(ip_str, ip_str_len, "%s", forwarded_for);
        return;
    }

    // 2. 检查 'X-Real-IP' 头（Nginx等常用）
    const char *real_ip = evhttp_find_header(headers, "X-Real-IP");
    if (real_ip != NULL) {
        snprintf(ip_str, ip_str_len, "%s", real_ip);
        return;
    }

    // 3. 如果没有代理头，直接从连接对象中获取socket的IP
    conn = evhttp_request_get_connection(req);
    if (conn) {
        sa = (struct sockaddr *)evhttp_connection_get_addr(conn);
        if (sa != NULL) {
            if (sa->sa_family == AF_INET) { // IPv4
                struct sockaddr_in *sin = (struct sockaddr_in *)sa;
                inet_ntop(AF_INET, &(sin->sin_addr), ip_str, ip_str_len);
            } else if (sa->sa_family == AF_INET6) { // IPv6
                struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;
                inet_ntop(AF_INET6, &(sin6->sin6_addr), ip_str, ip_str_len);
            } else {
                snprintf(ip_str, ip_str_len, "Unknown AF");
            }
            return;
        }
    }
    // 4. 如果所有方法都失败
    snprintf(ip_str, ip_str_len, "Unknown");
}

static int bind_socket(int port) 
{
    int r;
    int nfd;
    nfd = socket(AF_INET, SOCK_STREAM, 0);
    if (nfd < 0) return -1;

    int one = 1;
    r = setsockopt(nfd, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(int));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    r = bind(nfd, (struct sockaddr*)&addr, sizeof(addr));
    if (r < 0) return -1;
    r = listen(nfd, MAXCONN);
    if (r < 0) return -1;

    int flags;
    if ((flags = fcntl(nfd, F_GETFL, 0)) < 0 || fcntl(nfd, F_SETFL, flags | O_NONBLOCK) < 0)
        return -1;

    return nfd;
}

/* 把收到的报文写入消息队列 */
static int WriteToMsgQue(char *sMsg, int iLen, int iNum, struct timeval *tv)
{
    char sMsgBuf[MSQ_MSG_SIZE_MAX];
	memset(sMsgBuf, 0x00, sizeof(sMsgBuf));
    SESSIONHEAD stSessionHead;
    SYSHEAD stSysHead;
    int iSender, iMsgLen;

    /*填充系统头*/
    stSysHead.shMsgLen = SYSHEADLEN + SESSIONHEADLEN + iLen;
    stSysHead.shMsgType = 0;   
    iSender = (((unsigned int)shCurrent) << 16) | iNum;
    stSysHead.iSender = iSender;

     HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"iSender=[%d], [%d:%d]", iSender, shCurrent, iNum);

    shCurrent = (shCurrent == MAXCONN) ? 0 : shCurrent+1 ;
    memcpy(sMsgBuf , &stSysHead , SYSHEADLEN);

    /*填充会晤头*/
    memset((unsigned char *)&stSessionHead , 0, SESSIONHEADLEN);
    stSessionHead.caSessionMsgLen[0] = (SESSIONHEADLEN + iLen) >> 8;
    stSessionHead.caSessionMsgLen[1] = (SESSIONHEADLEN + iLen) & 0xff;
    sprintf(stSessionHead.tv_sec, "%ld", tv->tv_sec); 
    sprintf(stSessionHead.tv_usec, "%ld", tv->tv_usec); 

    memcpy(sMsgBuf + SYSHEADLEN, &stSessionHead, SESSIONHEADLEN);

    memcpy(sMsgBuf + SYSHEADLEN + SESSIONHEADLEN, sMsg, iLen);

    iMsgLen = SYSHEADLEN + SESSIONHEADLEN + iLen;

	int nRet= MsqSnd ("1106", gatSrvMsq, 0, iMsgLen, sMsgBuf);
    if (nRet)
    {
        HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "MsqSnd error, %d. %d[%s] ", nRet,errno,strerror(errno));
        return -1;
    }
    HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"写消息队列成功[%d]", iMsgLen);

    return 0;
}

/* HTTP请求超时回调函数 */
void timeout_cb(evutil_socket_t fd, short event, void *arg)
{
    long i = (long)arg;
    struct evhttp_request *req = Reqs[i].req;
    HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"HTTP请求超时 [%ld:%d]", i, Reqs[i].index);
    evhttp_send_error(req, HTTPINTERNAL, 0);  
	//evhttp_request_free(req); 
    Reqs[i].index = -1;
    Reqs[i].req = NULL;
}

void add_cors_headers(struct evhttp_request *req) {
    evhttp_add_header(evhttp_request_get_output_headers(req), "Access-Control-Allow-Origin", "*");
    evhttp_add_header(evhttp_request_get_output_headers(req), "Access-Control-Allow-Methods", "GET,POST,PATCH,PUT,DELETE,OPTIONS");
    evhttp_add_header(evhttp_request_get_output_headers(req), "Access-Control-Allow-Headers", "Content-Type, Authorization");
    evhttp_add_header(evhttp_request_get_output_headers(req), "Access-Control-Max-Age", "86400"); //秒
}

/* HTTP请求处理回调函数 */
static void request_cb(struct evhttp_request *req, void *arg)
{
    struct event_base *base = (struct event_base *)arg;
    struct evbuffer *buf;
    char data_buf[MSQ_MSG_SIZE_MAX]; 
	char	sTmpStr[128];
    int  len, tlen;
    int  rc;

    struct timeval tv_req;
    struct timeval tmo = {15, 0};// 15s

    gettimeofday(&tv_req, 0);

    struct event *tmo_ev;
    tmo_ev = evtimer_new(base, timeout_cb, (void *)(long)shCurrent);
    evtimer_add(tmo_ev, &tmo);

    Reqs[shCurrent].timer_ev = tmo_ev;

    /* 给应答的http header设置值, 避免为空 */
    evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", "application/json");
    evhttp_add_header(evhttp_request_get_output_headers(req), "Connection", "close");
    Reqs[shCurrent].req = req;

/*
	int fd = evhttp_connection_get_fd(req);
	int keepalive = 1;
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
*/

    /*使用随机数*/
    int iNum;
    srand(tv_req.tv_usec);
    iNum = random(0, MAXCONN);

    Reqs[shCurrent].index = iNum;   

    memset(data_buf, 0, sizeof(data_buf));
	tlen = 0;
	len = 0;	
	const char *uri = evhttp_request_get_uri(req);
    HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"Received request for URI: %s", uri);

    struct evhttp_uri *decoded = evhttp_uri_parse(uri);
    if (!decoded) {
        HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"Failed to parse URI\n");
        return;
    }
    const char *path = evhttp_uri_get_path(decoded);
    const char *decoded_query = evhttp_uri_get_query(decoded);
	const char *query = NULL;
	if(decoded_query != NULL && strlen(decoded_query) > 0)
		query = evhttp_decode_uri(decoded_query);
    const char *fragment = evhttp_uri_get_fragment(decoded);
	SetIpcValue(data_buf, "$PATH", path);
	SetIpcValue(data_buf, "$QUERY", query);
	SetIpcValue(data_buf, "$FRAGMENT", fragment);
    free(decoded);

	struct evkeyvalq *headers;
    struct evkeyval *header;
    headers = evhttp_request_get_input_headers(req);
    for (header = headers->tqh_first; header; header = header->next.tqe_next) 
	{
		if (strcasecmp(header->key, "authorization") == 0)
		{
			SetIpcValue(data_buf, "$JWT", header->value+7);
		}
		else if(strcmp(header->key, "content-type") == 0)
		{
			SetIpcValue(data_buf, header->key, header->value);
		}
		else if(strcmp(header->key, "user-agent") == 0)
		{
			SetIpcValue(data_buf, header->key, header->value);
		}
		HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"%s:%s", header->key, header->value);
    }
	//evhttp_clear_headers(headers);
	SetIpcValue(data_buf, "$MSGSRCID", gsSrvId);
    switch (evhttp_request_get_command(req)) {
        case EVHTTP_REQ_GET:
			SetIpcValue(data_buf, "$METHOD", "GET");
			evhttp_parse_query_str(query, headers);
			for (header = headers->tqh_first; header; header = header->next.tqe_next) {
				HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"%s:%s", header->key, header->value);
				SetIpcValue(data_buf, header->key, header->value);
			}
			//evhttp_clear_headers(headers);
			if (strcmp(path, "/favicon.ico") == 0) {
				evhttp_send_error(req, HTTP_NOTFOUND, "No favicon");
				return;
			}
			tlen = strlen(data_buf);
            HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"This is a GET request");
            break;
        case EVHTTP_REQ_POST:
			SetIpcValue(data_buf, "$METHOD", "POST");
            HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"This is a POST request");
			get_client_ip(req, sTmpStr, 15); 
			SetIpcValue(data_buf, "$CLIENT_IP", sTmpStr);
		
			evhttp_parse_query_str(query, headers);
			for (header = headers->tqh_first; header; header = header->next.tqe_next) {
				HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"%s:%s", header->key, header->value);
			}
			//evhttp_clear_headers(headers);

			buf = evhttp_request_get_input_buffer(req);
			len = evbuffer_get_length(buf);
			sprintf(sTmpStr, "%d", len);
			SetIpcValue(data_buf, "$json_len", sTmpStr);
			tlen = strlen(data_buf);
			evbuffer_remove(buf, data_buf+tlen, len);
			break;
        case EVHTTP_REQ_OPTIONS:
            HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"This is OPTIONS request");
			add_cors_headers(req);
			evhttp_send_reply(req, HTTP_OK, "OK", NULL);
			return;
        case EVHTTP_REQ_HEAD:
            HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"This is HEAD request");
			return;
        case EVHTTP_REQ_PUT:
			SetIpcValue(data_buf, "$METHOD", "PUT");
            HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"This is PUT request");
			evhttp_parse_query_str(query, headers);
			for (header = headers->tqh_first; header; header = header->next.tqe_next) {
				HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"%s:%s", header->key, header->value);
				SetIpcValue(data_buf, header->key, header->value);
			}
			//evhttp_clear_headers(headers);

			buf = evhttp_request_get_input_buffer(req);
			len = evbuffer_get_length(buf);
			tlen = strlen(data_buf);
			if(len > 0)
			{
				evbuffer_remove(buf, data_buf+tlen, len);
			}
			break;
        case EVHTTP_REQ_DELETE:
			SetIpcValue(data_buf, "$METHOD", "DELETE");
            HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"This is DELETE request");
			evhttp_parse_query_str(query, headers);
			for (header = headers->tqh_first; header; header = header->next.tqe_next) {
				HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"%s:%s", header->key, header->value);
				SetIpcValue(data_buf, header->key, header->value);
			}
			//evhttp_clear_headers(headers);

			buf = evhttp_request_get_input_buffer(req);
			len = evbuffer_get_length(buf);
			tlen = strlen(data_buf);
			if(len > 0)
			{
				evbuffer_remove(buf, data_buf+tlen, len);
			}
			break;

        case EVHTTP_REQ_TRACE:
            HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"This is TRACE request");
			return;
        case EVHTTP_REQ_PATCH:
            HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"This is PATCH request");
			return;
        case EVHTTP_REQ_CONNECT:
            HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"This is CONNECT request");
			return;
        default:
            HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"Unsupported request method");
            break;
    }
	add_cors_headers(req);
	len += tlen;
    HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"接收到的请求报文,len=[%d], [%d:%d]\n%s", len, shCurrent, iNum, data_buf);

    rc = WriteToMsgQue(data_buf, len, iNum, &tv_req);
    if (rc < 0 ) 
	{
        HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"向交易平台转发请求失败");
        evhttp_send_reply(req, HTTPINTERNAL, "END", NULL);
        return; 
    }
}

/* 返回应答给原HTTP请求方 */
static char *send_response(char *sMsgBuf, int iMsgLen)
{
    char *sMsgStart;
    int iLeftLen, iAMsgLen, iDataLen;
    unsigned int iNumber, iIndex;
    struct tm *tm_log;
    struct timeval tv;
    struct timeval tv_req;
    char	tm_buf[32], tm1_buf[32];
    struct evhttp_request *req;
    struct evbuffer *buff;
	char	sTmpBuf[MSQ_MSG_SIZE_MAX];
	char    *IPC = NULL;
	char	sIpcLen[16];
	int		nIpcLen = 0;
	char	sHeader[1024];
	char	sKey[128];
	char	sVal[512];
	int		nTotal;
	char	sTmpStr[128];

    SESSIONHEAD *pSess = NULL;

    sMsgStart = sMsgBuf;
    iLeftLen = iMsgLen;

    while(1) {
        if(iLeftLen >=  SYSHEADLEN ) { /* 有完整的数据包头 */
            /*系统头中含有报文的长度*/
            iDataLen = ((SYSHEAD *)sMsgStart)->shMsgLen;
             HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"SYSHEAD.shMsgLen = [%d]", iDataLen);

            iAMsgLen = iDataLen ; 

            if(iLeftLen >= iAMsgLen) 
			{
                /* 有完整的数据包 */
                /* 从报文里找到原来的HTTP请求索引 */
                iNumber = (((SYSHEAD *)sMsgStart)->iSender) & 0xffff;
                iIndex = ((unsigned int)(((SYSHEAD *)sMsgStart)->iSender)) >> 16;
                HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"send_response, [%d:%d]", iIndex, iNumber);

                if(Reqs[iIndex].index == iNumber)
                {
                    /* 清除timer event */
                    evtimer_del(Reqs[iIndex].timer_ev);

                    /* 从报文里找到原来的req句柄 */
                    req = Reqs[iIndex].req;
                    /* 设置http header */

					IPC = sMsgStart+SYSHEADLEN+SESSIONHEADLEN;
					memset(sIpcLen, 0x00, sizeof(sIpcLen));
					memcpy(sIpcLen, IPC, 6);
					nIpcLen = 6 + atoi(sIpcLen);
					GetIpcValueExt(IPC, "$HEADNUM", sTmpStr, sizeof(sTmpStr));
					nTotal = atoi(sTmpStr);

					struct evkeyvalq* headers = evhttp_request_get_output_headers(req);

					evhttp_clear_headers(headers);
					for (iNumber = 0; iNumber < nTotal; iNumber++)
					{
						sprintf(sTmpStr, "$KEY%d", iNumber);
						GetIpcValueExt(IPC, sTmpStr, sKey, sizeof(sKey));
						sprintf(sTmpStr, "$VAL%d", iNumber);
						GetIpcValueExt(IPC, sTmpStr, sVal, sizeof(sVal));
						evhttp_add_header(headers, sKey, sVal);
					}
                    evhttp_add_header(headers, "Content-Type", "application/json");
                    evhttp_add_header(headers, "Connection", "close");

					struct evkeyval *header;
					for (header = headers->tqh_first; header; header = header->next.tqe_next) {
						HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"res-header %s:%s", header->key, header->value);
					}
                    /* 新申请缓存放应答报文 */   
					buff = evbuffer_new();
					//memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
					memcpy(sTmpBuf, sMsgStart+SYSHEADLEN+SESSIONHEADLEN+nIpcLen, iDataLen-SYSHEADLEN-SESSIONHEADLEN-nIpcLen);
					sTmpBuf[iDataLen-SYSHEADLEN-SESSIONHEADLEN-nIpcLen] = 0x00;
					HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"%s", sTmpBuf);

					evbuffer_add(buff, sMsgStart+SYSHEADLEN+SESSIONHEADLEN+nIpcLen, iDataLen-SYSHEADLEN-SESSIONHEADLEN-nIpcLen);

					/* 向http客户端返回应答 */
					evhttp_send_reply(req, 200, "OK", buff);

					/* 释放缓存资源 */
					//evbuffer_free(buff);

                    gettimeofday(&tv, 0);
                    tm_log = localtime(&tv.tv_sec);
                    strftime(tm_buf, sizeof(tm_buf), "%H%M%S", tm_log);

                    pSess = (SESSIONHEAD *)(sMsgStart+SYSHEADLEN);
                    tv_req.tv_sec = atol(pSess->tv_sec);
                    tv_req.tv_usec = atol(pSess->tv_usec);
                    tm_log = localtime(&tv_req.tv_sec);
                    strftime(tm1_buf, sizeof(tm1_buf), "%H%M%S", tm_log);

                    int tm_spent = (tv.tv_sec-tv_req.tv_sec)*1000+(tv.tv_usec-tv_req.tv_usec)/1000;
                    HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"流水[%s],请求[%s.%06ld],应答[%s.%06ld],耗时[%d]毫秒%s", pSess->caSrvStan, tm1_buf, tv_req.tv_usec, tm_buf, tv.tv_usec, tm_spent, tm_spent>1000?"<--":"");
                } 
				else 
				{
                     HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"该笔交易已被请求方放弃 [%d:%d]", iIndex, Reqs[iIndex].index);
                }
                sMsgStart += iAMsgLen;
                iLeftLen -= iAMsgLen;
            } 
			else
                break;
        } 
		else
            break;
    }

    if(iLeftLen > 0)
        memmove(sMsgBuf, sMsgStart, iLeftLen);

    return sMsgBuf + iLeftLen;

}

/* 从管道读取应答回调 */
void pipe_cb(evutil_socket_t iPipeFd, short event, void *arg)
{
    struct event *ev = (struct event *)arg;
    static char sMsgBuf[2 * MSQ_MSG_SIZE_MAX];
    static char *pBufStart = sMsgBuf;
	char		sTmpBuf[128];
    int		iRet;

    HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"处理已完成交易...");

    while(1) {
        memset(pBufStart, 0, sizeof(sMsgBuf) - (pBufStart - sMsgBuf));

        iRet = read(iPipeFd, pBufStart, MSQ_MSG_SIZE_MAX);
        if(iRet < 0) {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
                HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__, "[%ld]字节未处理", pBufStart - sMsgBuf);
            else
                HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"读管道错:%s", strerror(errno));
            break;
        }
        else if(iRet > 0) {
			//memset(sTmpBuf, 0, sizeof(sTmpBuf));
			//sprintf(sTmpBuf, "%%%ds", iRet+pBufStart-sMsgBuf-64 );
            //HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"%s", sTmpBuf);
            //HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,sTmpBuf, sMsgBuf+64);
            pBufStart = send_response(sMsgBuf, iRet+pBufStart-sMsgBuf);
        }
        else {
            HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"收到EOF,关闭管道");
            close(iPipeFd);
            event_del(ev);
            break;
        }
    }
}

void sigterm_cb(evutil_socket_t iFd, short event, void *arg)
{
    struct event *me = arg;
    int i;

    for (i=0; i<WORKERNUM; i++) {
        kill(ChildPid[i], SIGTERM);
    } 
    event_del(me);
}

void sigchld_cb(evutil_socket_t iFd, short event, void *arg)
{
    struct event_base *base = arg;
    int iStatus;
    pid_t pid;

    while ((pid = waitpid(-1, &iStatus, WNOHANG)) > 0) {
         HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"Child[%ld] exit", (long)pid);
    }

    struct timeval delay = { 2, 0 };
    event_base_loopexit(base, &delay);
}

static int iHttpDaemon(int fd[][2], int size)
{
    int i, r;
    int sockfd;
    struct event_base *ev_base = NULL;
    struct evhttp *httpd = NULL;
    struct event *sig_ev = NULL;
    struct event *pipe_ev = NULL;


    if ((sockfd = bind_socket(28093)) < 0 )
    {
         HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"绑定端口[%ld]失败", 28093);
        return -1;
    }
    HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__, "HTTP监听端口[%ld]", 28093);


    ev_base = event_base_new();
    if (ev_base == NULL) return -1;

    httpd = evhttp_new(ev_base);
    if (httpd == NULL) return -1;

	evhttp_set_max_body_size(httpd, 50 * 1024 * 1024);
	evhttp_set_max_headers_size(httpd, 1 * 1024 * 1024);

    r = evhttp_accept_socket(httpd, sockfd);
    if (r != 0) return -1;

	//evhttp_set_allowed_methods(httpd, EVHTTP_REQ_GET | EVHTTP_REQ_POST | EVHTTP_REQ_PUT | EVHTTP_REQ_HEAD | EVHTTP_REQ_PATCH | EVHTTP_REQ_OPTIONS | EVHTTP_REQ_DELETE);
	evhttp_set_allowed_methods(httpd, EVHTTP_REQ_GET | EVHTTP_REQ_POST | EVHTTP_REQ_PUT | EVHTTP_REQ_OPTIONS | EVHTTP_REQ_DELETE);
	//evhttp_set_timeout(httpd, 30);

    evhttp_set_gencb(httpd, request_cb, ev_base);

    for (i=0; i<size; i++) {
        pipe_ev = event_new(ev_base, fd[i][0], EV_READ|EV_PERSIST, pipe_cb, event_self_cbarg());
        event_add(pipe_ev, NULL);
    }

    sig_ev = evsignal_new(ev_base, SIGTERM, sigterm_cb, event_self_cbarg());
    event_add(sig_ev, NULL);

    sig_ev = evsignal_new(ev_base, SIGCHLD, sigchld_cb, (void *)ev_base);
    event_add(sig_ev, NULL);

    event_base_dispatch(ev_base);

    return 0;
}

void sig_term(int signo)
{
    HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"recv SIGTERM, exit");
    exit(0);
}

/* 子进程读入消息队列, 将报文转交父进程 */
int iMsgDaemon(int xiPipeNo )
{
    int  iRc=-1, nRet ,iLen;
    signal(SIGTERM, sig_term);
    //prctl(PR_SET_PDEATHSIG, SIGTERM);

	int		nMsgLen ;
	char	sMsgBuf[MSQ_MSG_SIZE_MAX];
    for(;;) 
	{
		HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__, __LINE__, "MAIN:  ================= W A I T I N G ====================");
        memset ((char *)&sMsgBuf, 0, sizeof(sMsgBuf) );
        nMsgLen = MSQ_MSG_SIZE_MAX;

		nRet = MsqRcv (gsSrvId, gatSrvMsq, 0, MSQ_RCV_MODE_BLOCK, &nMsgLen, sMsgBuf);
        if (nRet)
        {
            if (nRet != ERR_CODE_MSQ_BASE + EINTR)
            {
                HtLog( gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "MsqRcv error, %d", nRet);
                return nRet;
            }
            else
                continue;
        }
        iRc = write(xiPipeNo, sMsgBuf, nMsgLen);
        if(iRc < 0 ){
             HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"写向管道错[%d]:%s",iRc, strerror(errno));
            continue;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int             nRet;
    int             j;
    int             port;
    int             lUsageKey,liX;
	int				i;
    int				iPipeNo[WORKERNUM][2];

    setbuf(stdout, NULL); 
    setbuf(stderr, NULL);

    if(argc < 5)
    {
        HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"Usage:%s srvid seq tosrvid port\n", argv[0]);
        exit(-1);
    }

    strcpy(gsSrvId, argv[1]);
    strcpy(gsSrvSeq, argv[2]);
    strcpy(gsToSrvId, argv[3]);
    port = atol(argv[4]);

    nRet = DbsConnect();
    if (nRet)
    {
        HtLog("CommHTTP.log ",HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"DbsConnect error=[%d] line=[%d]\n", nRet, __LINE__);
        exit(-2);
    }

    if (getenv("SRV_USAGE_KEY"))
        lUsageKey = atoi(getenv("SRV_USAGE_KEY"));
    else
    {
        HtLog("CommHTTP.log",HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"getenv SRV_USAGE_KEY error=[%d] line=[%d]\n", nRet, __LINE__);
        DbsDisconnect();
        exit(-3);
    }

	Tbl_srv_inf_Def     tTblSrvInf;
    memset ((char *)&tTblSrvInf, 0x00, sizeof (tTblSrvInf));
    tTblSrvInf.usage_key = lUsageKey;
    memcpy (tTblSrvInf.srv_id, gsSrvId, SRV_ID_LEN);
    nRet = DbsSRVINF(DBS_SELECT, &tTblSrvInf);
    if (nRet)
    {
        HtLog("CommHTTP.log",HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"DbsSRVINF nRet[%d]", nRet);
        DbsDisconnect();
        exit(-2);
    }

    CommonRTrim(tTblSrvInf.srv_name);
    sprintf(gsLogFile,"%s.%s.log", tTblSrvInf.srv_name, gsSrvSeq);

    memset((char *)gatSrvMsq, 0, sizeof (gatSrvMsq));
    nRet = MsqInit(gsSrvId, gatSrvMsq);
    if (nRet)
    {
        HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "MsqInit error,%d", nRet);
        exit(-3);
    }

    DbsDisconnect();

	signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    for (i=0; i<WORKERNUM; i++) {
        pipe(iPipeNo[i]);

        switch((ChildPid[i] = fork())) {
            case -1:
                return -1;
            case 0 : //子
                close(iPipeNo[i][0]); 
                iMsgDaemon(iPipeNo[i][1]);
            default: /* 父进程 */
                close(iPipeNo[i][1]);  
                fcntl(iPipeNo[i][0], F_SETFL, O_NONBLOCK);
        }
    }
    iHttpDaemon(iPipeNo, WORKERNUM);
    return 0;
}
