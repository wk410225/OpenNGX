#include "Switch.h"
#include "SwtTxn.h"
#include <hiredis/hiredis.h>

char				gsSrvId[SRV_ID_LEN+1];
char        		gsSrvSeqId[SRV_SEQ_ID_LEN+1];
char        		gsLogFile[LOG_NAME_LEN_MAX];
T_SrvMsq    		gatSrvMsq[SRV_MSQ_NUM_MAX];
Tbl_txn_inf_Def     gatTxnInf[TXN_INF_NUM_MAX];
int  gnTxnInfNum = TXN_INF_NUM_MAX;
redisContext	*c; // Redis连接上下文
redisReply		*reply; // Redis命令回复

#define FLD_TXN_NUM_LEN 4

int redisConn()
{
	const char *hostname = "127.0.0.1"; // Redis服务器地址
    int port = 6379; // Redis服务器端口
    struct timeval timeout = { 1, 500000 }; // 连接超时时间 1.5 秒
    printf("Connecting to Redis at %s:%d...\n", hostname, port);
    // 1. 连接到Redis服务器
    c = redisConnectWithTimeout(hostname, port, timeout);
    if (c == NULL || c->err) {
       if (c) {
            printf("Connection error: %s\n", c->errstr);
            redisFree(c);
        } else {
            printf("Connection error: can't allocate redis context\n\n");
        }
		return -1;
    }
	return 0;
    printf("Connection successful!\n\n");
}

int redisTestConn()
{
    reply = redisCommand(c, "PING");
    printf("PING: %s\n", reply->str);
    freeReplyObject(reply); // 必须释放回复对象！
	if (c->err) {
        printf("Command error: %s\n", c->errstr);
        redisFree(c);
		return -1;
    }
	return 0;
}

int main( int argc, char **argv )
{
	char	sMsgSrcId[SRV_ID_LEN+1];
	char	sMsgBuf[MSQ_MSG_SIZE_MAX];
	char    sTmpLogFile[LOG_NAME_LEN_MAX];
	int		i, j;
	int		nRet;
	int		nMsgLen;
	int		nIndex;
	char    *tIpcIntTxn;
	long	lBeginTime, lEndTime;	
	struct  tms	tTMS;
	char 	sTxnNum[128];
	char 	sTmpBuf[128];
	nRet = SwitchInit (argc, argv);
	if ( nRet )
	{
	     printf("Switch: SwitchInit error %d\n", nRet);
 	     return nRet;
	}
	struct sigaction sa;
    sa.sa_handler = HandleExit;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGHUP);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
		HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "sigaction SIGTERM error, %d", errno);
        exit(EXIT_FAILURE);
    }
	lBeginTime = 0;
	lEndTime = 0;

	int iNumber , iIndex;
	while ( 1 )
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
		iNumber = (((SYSHEAD *)sMsgBuf)->iSender) & 0xffff;
        iIndex = ((unsigned int)(((SYSHEAD *)sMsgBuf)->iSender)) >> 16;
        HtLog(gsLogFile,HT_LOG_MODE_NORMAL,__FILE__,__LINE__,"begin [%d:%d] ", iIndex, iNumber);

		HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "DbsConnectTest begin");
		nRet = DbsConnectTest();
		if (nRet)
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "DbsConnectTest error, [%d]", nRet);

			for (j=0;j<5;j++)
			{
				nRet = DbsConnect();
				if (nRet)
					sleep(1);
				else
					break;
			}

			if (j == 5)
			{
				HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "after 5 times fail  exit");
				HandleExit(-10);
			}
		}
		HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "redisTestConn begin");
		nRet = redisTestConn();
		if (nRet)
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "DbsConnectTest error, [%d]", nRet);

			for (j=0;j<5;j++)
			{
				nRet = redisConn();
				if (nRet)
					sleep(1);
				else
					break;
			}

			if (j == 5)
			{
				HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "after 5 times fail  exit");
				HandleExit(-10);
			}
		}

		lBeginTime = times( &tTMS);
		HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "DbsConnectTest end success");
		HtDebugString(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, sMsgBuf, nMsgLen);

		GetIpcValueExt(sMsgBuf+64, "$MSGSRCID", sMsgSrcId, sizeof(sMsgSrcId));
		GetIpcValueExt(sMsgBuf+64, "$PATH", sTxnNum, sizeof(sTxnNum));
		HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "sTxnNum=[%s]", sTxnNum);

		for( i = 0; i < MAXTXNS; i++ )
		{
			if(strlen(gaTxns[i].caTxnNum) == 0)
				continue;
			if( memcmp(sTxnNum, gaTxns[i].caTxnNum, strlen(gaTxns[i].caTxnNum)) == 0 )
			{
				HtLog(gsLogFile, HT_LOG_MODE_ERROR,__FILE__,__LINE__,"gaTxns[%d].caTxnNum = %s", i, gaTxns[i].caTxnNum);
				memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
				strcpy(sTmpBuf, gaTxns[i].caTxnNum);
				break;
			}
		}
		if( i == MAXTXNS )
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR,__FILE__,__LINE__,"can't find tbl_txn_inf.txn_num [%s]", sTxnNum);	
		}
		else
		{
			memset(gsLogFile, 0x00, sizeof(gsLogFile));
			j = 0; 
			while(sTmpBuf[j] != 0x00)
			{
				if(sTmpBuf[j] == '/')
					sTmpBuf[j] = '_';
				j++;
			}
			sprintf (gsLogFile, "%s.%s.%s.log", argv[0], gsSrvSeqId, sTmpBuf+1);
			nRet = gaTxns[i].pfTxnFun(sMsgBuf, nIndex);
			memset(gsLogFile, 0x00, sizeof(gsLogFile));
			sprintf (gsLogFile, "%s.%s.log", argv[0], gsSrvSeqId);
		}
		lEndTime = times( &tTMS);
		HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__, __LINE__, "[%d:%d] end,used %ld ticks ", iIndex,iNumber ,lEndTime - lBeginTime);
	}
}
int SwitchInit (short argc, char **argv)
{
	int		i;
	int		nRet;
	long    lUsageKey;
	Tbl_srv_inf_Def	tTblSrvInf;

	strcpy (gsSrvId, argv[1]);
	strcpy (gsSrvSeqId, argv[2]);

	if (getenv(SRV_USAGE_KEY))
		lUsageKey=atoi (getenv(SRV_USAGE_KEY));
	else
		return -1;

	nRet = DbsConnect ();
	if (nRet)
	{
		HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "DbsConnect error, %d", nRet);
		return (nRet);
	}
	nRet = redisConn();
	if (nRet)
	{
		HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "redisConn error, %d", nRet);
		return (nRet);
	}

	memset ((char *)&tTblSrvInf, 0x00, sizeof (tTblSrvInf));
	tTblSrvInf.usage_key = lUsageKey;
	memcpy (tTblSrvInf.srv_id, gsSrvId, SRV_ID_LEN);
	nRet = DbsSRVINF (DBS_SELECT, &tTblSrvInf);
	if (nRet)
	{
		DbsDisconnect ();
		return (nRet);
	}
	sprintf (gsLogFile, "%s%s.%s.log", "SwtRY", gsSrvId, gsSrvSeqId);

	memset ((char *)gatSrvMsq, 0, SRV_MSQ_NUM_MAX * sizeof (T_SrvMsq));
	nRet = MsqInit (gsSrvId, gatSrvMsq);
	if (nRet)
	{
		HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "MsqInit error, %d", nRet);
		DbsDisconnect ();
		return (nRet);
	}

/*
	memset( (char *)gatTxnInf, 0, sizeof(gatTxnInf) );
	nRet = DbsTxnInfLoad (&gnTxnInfNum, gatTxnInf);
	if (nRet)
	{
		HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "DbsTxnInfLoad error, %d", nRet);
		DbsDisconnect ();
		return (nRet);
	}
*/
	return 0;
}
#if 0
int GetTxnInfoIndex( char *sMsgSrcId, char *sTxnNum, char *sMsgDestId, int *nIndex )
{
	char	sFuncName[] = "GetTxnInfoIndex";
	int		i;

	HtLog(	gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "sTxnNum %4.4s begin  sMsgSrcId %4.4s",
			sTxnNum, sMsgSrcId);

	*nIndex = -1;

	for( i = 0; i < gnTxnInfNum; i++ )
	{
		if ( memcmp(gatTxnInf[i].msg_src_id, sMsgSrcId, SRV_ID_LEN) == 0 &&
				memcmp(gatTxnInf[i].txn_num, sTxnNum, FLD_TXN_NUM_LEN) == 0 
				)
		{
			*nIndex = i;
			break;
		}
	}

	if( i == gnTxnInfNum )
	{
		for( i = 0; i < gnTxnInfNum; i++ )
		{
			if (!memcmp(gatTxnInf[i].msg_src_id, sMsgSrcId, SRV_ID_LEN-2) && 
					!memcmp(gatTxnInf[i].txn_num, sTxnNum, FLD_TXN_NUM_LEN)) 
			{
				*nIndex = i;
				break;
			}
		}
	}

	if( i == gnTxnInfNum )
	{
		HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, 
				"txn num %4.4s from server %4.4s  dest server %4.4s not configed in tbl_txn_inf",
				sTxnNum, sMsgSrcId, sMsgDestId);
		return -1;
	}

	HtLog( gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s end", sFuncName);
	return 0;
}
#endif


void HandleExit (int n)
{
	DbsDisconnect ();
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "Switch exits");
	exit( 1 );
}
