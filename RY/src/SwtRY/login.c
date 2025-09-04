#include "Switch.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <bcrypt/bcrypt.h>

extern redisContext    *c; // Redis连接上下文
extern redisReply      *reply; // Redis命令回复

char *SaveRedisJson (char *IPC);
typedef struct {
    char os[64];
    char browser[64];
    char browser_version[32];
    char device[64];
} UserAgentInfo;

UserAgentInfo uaresult;
void parse_user_agent(const char *user_agent, UserAgentInfo *info);

void parse_user_agent(const char *user_agent, UserAgentInfo *info) {
    // 初始化默认值
    strcpy(info->os, "Window 10");
    strcpy(info->browser, "Chrome");
    strcpy(info->browser_version, "10.1");
    strcpy(info->device, "Computer");

    if (!user_agent || strlen(user_agent) == 0) {
        return;
    }

    char ua_lower[1024];
    strncpy(ua_lower, user_agent, sizeof(ua_lower) - 1);
    ua_lower[sizeof(ua_lower) - 1] = '\0';
    
    // 转换为小写便于匹配
    for (int i = 0; ua_lower[i]; i++) {
        ua_lower[i] = tolower(ua_lower[i]);
    }

    // 操作系统检测
    if (strstr(ua_lower, "windows nt 10.0") || strstr(ua_lower, "windows 10")) {
        strcpy(info->os, "Windows 10");
    } else if (strstr(ua_lower, "windows nt 6.3") || strstr(ua_lower, "windows 8.1")) {
        strcpy(info->os, "Windows 8.1");
    } else if (strstr(ua_lower, "windows nt 6.2") || strstr(ua_lower, "windows 8")) {
        strcpy(info->os, "Windows 8");
    } else if (strstr(ua_lower, "windows nt 6.1") || strstr(ua_lower, "windows 7")) {
        strcpy(info->os, "Windows 7");
    } else if (strstr(ua_lower, "mac os x") || strstr(ua_lower, "macintosh")) {
        strcpy(info->os, "macOS");
    } else if (strstr(ua_lower, "linux")) {
        strcpy(info->os, "Linux");
    } else if (strstr(ua_lower, "android")) {
        strcpy(info->os, "Android");
        strcpy(info->device, "Mobile");
    } else if (strstr(ua_lower, "iphone") || strstr(ua_lower, "ipad")) {
        strcpy(info->os, "iOS");
        strcpy(info->device, "Mobile");
    }

    // 浏览器检测
    if (strstr(ua_lower, "chrome/") && !strstr(ua_lower, "edg/")) {
        strcpy(info->browser, "Chrome");
        // 提取版本号
        const char *chrome_pos = strstr(ua_lower, "chrome/");
        if (chrome_pos) {
            const char *version_start = chrome_pos + 7;
            int i = 0;
            while (version_start[i] && (isdigit(version_start[i]) || version_start[i] == '.') && i < 31) {
                info->browser_version[i] = version_start[i];
                i++;
            }
            info->browser_version[i] = '\0';
        }
    } else if (strstr(ua_lower, "firefox/")) {
        strcpy(info->browser, "Firefox");
        const char *ff_pos = strstr(ua_lower, "firefox/");
        if (ff_pos) {
            const char *version_start = ff_pos + 8;
            int i = 0;
            while (version_start[i] && (isdigit(version_start[i]) || version_start[i] == '.') && i < 31) {
                info->browser_version[i] = version_start[i];
                i++;
            }
            info->browser_version[i] = '\0';
        }
    } else if (strstr(ua_lower, "safari/") && !strstr(ua_lower, "chrome/")) {
        strcpy(info->browser, "Safari");
    } else if (strstr(ua_lower, "edg/")) {
        strcpy(info->browser, "Edge");
        const char *edg_pos = strstr(ua_lower, "edg/");
        if (edg_pos) {
            const char *version_start = edg_pos + 4;
            int i = 0;
            while (version_start[i] && (isdigit(version_start[i]) || version_start[i] == '.') && i < 31) {
                info->browser_version[i] = version_start[i];
                i++;
            }
            info->browser_version[i] = '\0';
        }
    } else if (strstr(ua_lower, "opera/") || strstr(ua_lower, "opr/")) {
        strcpy(info->browser, "Opera");
    }

    // 设备类型检测
    if (strstr(ua_lower, "mobile") || strstr(ua_lower, "android") || 
        strstr(ua_lower, "iphone") || strstr(ua_lower, "ipad")) {
        strcpy(info->device, "Mobile");
    } else if (strstr(ua_lower, "tablet")) {
        strcpy(info->device, "Tablet");
    }
}

static struct tbl_common_def 
{
	long    page_num;
	long	page_size;
    long    total_rows;
	char	sort[128];
}ptTblCommon;

static TABFUN TBL_COMMON_COLMAP[MAXCOLNUM]={
    {"total_rows",    offsetof(struct tbl_common_def, total_rows), 'L'},
};

static TABFUN SYS_USER_COLMAP[MAXCOLNUM]={
    {"user_id", offsetof(sys_user_def, user_id), 'I', "",  "userId",""},
    {"dept_id", offsetof(sys_user_def, dept_id), 'I', "deptId", "deptId", ""},
    {"user_name", offsetof(sys_user_def, user_name), 'C', "userName", "userName", ""},
    {"nick_name", offsetof(sys_user_def, nick_name), 'C', "nickName", "nickName", ""},
    {"user_type", offsetof(sys_user_def, user_type), 'C', "", "", ""},
    {"email", offsetof(sys_user_def, email), 'C', "email", "email", ""},
    {"phonenumber", offsetof(sys_user_def, phonenumber), 'C', "phonenumber", "phonenumber", ""},
    {"sex", offsetof(sys_user_def, sex), 'C', "sex", "sex", ""},
    {"avatar", offsetof(sys_user_def, avatar), 'C', "", "avatar", ""},
    {"password", offsetof(sys_user_def, password), 'C', "", "", ""},
    {"status", offsetof(sys_user_def, status), 'C', "status", "status", ""},
    {"del_flag", offsetof(sys_user_def, del_flag), 'C', "", "delFlag", ""},
    {"login_ip", offsetof(sys_user_def, login_ip), 'C', "", "loginIp", ""},
    {"login_date", offsetof(sys_user_def, login_date), 'C', "", "loginDate", ""},
    {"pwd_update_date", offsetof(sys_user_def, pwd_update_date), 'C', "", "pwdUpdateDate", ""},
    {"create_by", offsetof(sys_user_def, create_by), 'C', "", "createBy", ""},
    {"create_time", offsetof(sys_user_def, create_time), 'C', "", "createTime", ""},
    {"update_by", offsetof(sys_user_def, update_by), 'C', "", "updateBy", ""},
    {"update_time", offsetof(sys_user_def, update_time), 'C', "", "updateTime", ""},
    {"remark", offsetof(sys_user_def, remark), 'C', "remark", "remark", ""},
};
static TABFUN SYS_DEPT_COLMAP[MAXCOLNUM]={
    {"dept_id", offsetof(sys_dept_def, dept_id), 'I', "", "deptId", ""},
    {"parent_id", offsetof(sys_dept_def, parent_id), 'I', "", "parentId", ""},
    {"ancestors", offsetof(sys_dept_def, ancestors), 'C', "", "ancestors", ""},
    {"dept_name", offsetof(sys_dept_def, dept_name), 'C', "", "deptName", ""},
    {"order_num", offsetof(sys_dept_def, order_num), 'I', "", "orderNum", ""},
    {"leader", offsetof(sys_dept_def, leader), 'C', "", "leader", ""},
    {"phone", offsetof(sys_dept_def, phone), 'C', "", "phone", ""},
    {"email", offsetof(sys_dept_def, email), 'C', "", "email", ""},
    {"status", offsetof(sys_dept_def, status), 'C', "", "status", ""},
    {"del_flag", offsetof(sys_dept_def, del_flag), 'C', "", "delFlag", ""},
    {"create_by", offsetof(sys_dept_def, create_by), 'C', "", "createBy", ""},
    {"create_time", offsetof(sys_dept_def, create_time), 'C', "", "createTime", ""},
    {"update_by", offsetof(sys_dept_def, update_by), 'C', "", "updateBy", ""},
    {"update_time", offsetof(sys_dept_def, update_time), 'C', "", "updateTime", ""},
};

static TABFUN SYS_ROLE_COLMAP[MAXCOLNUM]={
    {"role_id", offsetof(sys_role_def, role_id), 'I', "", "roleId"},
    {"role_name", offsetof(sys_role_def, role_name), 'C', "","roleName"},
    {"role_key", offsetof(sys_role_def, role_key), 'C', "", "roleKey"},
    {"role_sort", offsetof(sys_role_def, role_sort), 'I', "", "roleSort"},
    {"data_scope", offsetof(sys_role_def, data_scope), 'C', "", "dataScope"},
    {"menu_check_strictly", offsetof(sys_role_def, menu_check_strictly), 'I', "", "menuCheckStrictly"},
    {"dept_check_strictly", offsetof(sys_role_def, dept_check_strictly), 'I', "", "deptCheckStrictly"},
    {"status", offsetof(sys_role_def, status), 'C', "", "status"},
    {"del_flag", offsetof(sys_role_def, del_flag), 'C', "", "delFlag"},
    {"create_by", offsetof(sys_role_def, create_by), 'C', "", "createBy"},
    {"create_time", offsetof(sys_role_def, create_time), 'C', "", "createTime"},
    {"update_by", offsetof(sys_role_def, update_by), 'C', "", "updateBy"},
    {"update_time", offsetof(sys_role_def, update_time), 'C', "", "updateTime"},
    {"remark", offsetof(sys_role_def, remark), 'C', "", "remark"},
};


static TABFUN SYS_USER_INDMAP[MAXINDNUM]={
    {"user_id", offsetof(sys_user_def, user_id), 'I'},
};



int login(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "login";
	char			sMsgSrcId[SRV_ID_LEN+1];
	char			sMethod[16];
	char			sUri[128];
	int				nRet;
	char			sSql[2048];
	char			where[256];
	char			set[2048];
	char			sTmpBuf[128];
	char			sClientIp[24];
	char			sUserAgent[1024];
	char			*IPC = sMsgBuf+ MSGHEADLEN;
	sys_user_def ptTbl;

	memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
	memcpy(sTmpBuf, IPC, 6);
	char			*json_string = sMsgBuf+MSGHEADLEN+6+atoi(sTmpBuf);
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s begin", sFuncName);
	showIpc(IPC);
	GetIpcValueExt(IPC, "$MSGSRCID", sMsgSrcId, sizeof(sMsgSrcId));
	GetIpcValueExt(IPC, "$METHOD", sMethod, sizeof(sMethod));
	GetIpcValueExt(IPC, "$PATH", sUri, sizeof(sUri));
	GetIpcValueExt(IPC, "$CLIENT_IP", sClientIp, sizeof(sClientIp));
	GetIpcValueExt(IPC, "User-Agent", sUserAgent, sizeof(sUserAgent));
	cJSON      *root,  *body, *item, *array;
	parse_user_agent(sUserAgent, &uaresult);
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", uaresult.os);
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", uaresult.browser);
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", uaresult.browser_version);
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", uaresult.device);

	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", json_string);
	if (strcmp(sMethod, "POST" ) == 0 )
	{
		root = cJSON_Parse(json_string);
		if (root == NULL) {
			const char *error_ptr = cJSON_GetErrorPtr();
			if (error_ptr != NULL) {
				HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "Error before: %s", error_ptr);
			}
			return -1;
		}
		item = cJSON_GetObjectItemCaseSensitive(root, "username");

		SetIpcValue(IPC, "userName", item->valuestring);

		memset(sSql,  0x00, sizeof(sSql));
		memset((char*)&ptTbl, 0x00, sizeof(ptTbl));
		//J2S(root, &ptTbl, SYS_USER_COLMAP);
		sprintf(sSql, "select user_id, password, user_name from sys_user where user_name='%s'", item->valuestring);
		HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", sSql );
		nRet = mysql_ONE_SELECT(sSql, &ptTbl, SYS_USER_COLMAP);
		if(nRet ==0 ){ 
			body = cJSON_CreateObject();
			item = cJSON_GetObjectItemCaseSensitive(root, "password");
			HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s=%s", item->valuestring, ptTbl.password);
			if (bcrypt_checkpw(item->valuestring, ptTbl.password) == 0 )
			{
				SetIpcLongValue(IPC, "userId", ptTbl.user_id);
				SetIpcLongValue(IPC, "deptId", ptTbl.dept_id);
				generate_jwt_token(IPC);
				cJSON_AddStringToObject(body, "msg", "操作成功");
				cJSON_AddNumberToObject(body, "code", 200);
				cJSON_AddStringToObject(body, "token", GetIpcPtr(IPC, "$JWT"));
				json_string = SaveRedisJson (IPC);
				reply = redisCommand(c, "SET login_tokens:%s %s EX %d", GetIpcPtr(IPC, "$UUID"), json_string, 3600);
			    freeReplyObject(reply);
				free(json_string);
			}
			else
			{
				cJSON_AddStringToObject(body, "msg", "密码错误");
				cJSON_AddNumberToObject(body, "code", 500);
			}
			json_string = cJSON_Print(body);
			HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string = %s", json_string);
			cJSON_Delete(body);
		}
	}
    //head=cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
	SetIpcValue(sSql, "$HEADNUM", "0000");
    memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
    memcpy(sTmpBuf, sSql, 6);
    int nIpcLen = 6+atoi(sTmpBuf);

    memcpy(sMsgBuf+MSGHEADLEN, sSql, nIpcLen);
    strcpy(sMsgBuf+MSGHEADLEN+nIpcLen, json_string);
    int nMsgLen = strlen(json_string)+MSGHEADLEN+nIpcLen;
	//strcpy(sMsgBuf+MSGHEADLEN, json_string);
	//int nMsgLen = strlen(json_string)+MSGHEADLEN;
	free(json_string);
	((SYSHEAD *)sMsgBuf)->shMsgLen = nMsgLen;
	HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "nMsgLen = %d", nMsgLen);
	HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "sMsgBuf->shMsgLen = %d", ((SYSHEAD *)sMsgBuf)->shMsgLen);
	nRet= MsqSnd (sMsgSrcId, gatSrvMsq, 0, nMsgLen, sMsgBuf);
    if (nRet)
    {
        HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "MsqSnd error, %d. %d[%s] ", nRet,errno,strerror(errno));
        return -1;
    }
	HtLog(	gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s end", sFuncName);
	return 0;
}

char *SaveRedisJson (char *IPC)
{
	char			*json_string;
	cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "browser", "Chrome 13");
    cJSON_AddStringToObject(root, "os", "Windows 10");
    cJSON_AddNumberToObject(root, "userId", GetIpcLongValue(IPC, "userId"));
    cJSON_AddStringToObject(root, "userName", GetIpcPtr(IPC, "userName"));
    cJSON_AddNumberToObject(root, "deptId", GetIpcLongValue(IPC, "deptId"));
    cJSON_AddNumberToObject(root, "loginTime", GetIpcLongValue(IPC, "$CREATE_TIME"));
    cJSON_AddNumberToObject(root, "expireTime", GetIpcLongValue(IPC,"$EXPIRE_TIME"));
    cJSON_AddStringToObject(root, "ipaddr", GetIpcPtr(IPC, "$CLIENT_IP"));
    cJSON_AddStringToObject(root, "loginLocation", "内网IP");
    cJSON_AddStringToObject(root, "tokenId", GetIpcPtr(IPC, "$UUID"));
    cJSON_AddStringToObject(root, "permissions", "Set[\"*:*:*\"]");


	sys_user_def    ptTbl;
	sys_dept_def    ptTblDept;
	sys_role_def    ptTblRole;
	int				nRet ;
	char			sSql[4096];

	cJSON *pdata = NULL, *cdata = NULL;
	cJSON *parray = NULL, *carray = NULL;


	memset(sSql,  0x00, sizeof(sSql));
	sprintf(sSql, "select *  from sys_user where user_name='%s'", GetIpcPtr(IPC, "userName"));
	memset((char*)&ptTbl, 0x00, sizeof(ptTbl));
	nRet = mysql_ONE_SELECT(sSql, &ptTbl, SYS_USER_COLMAP);
	if(nRet !=0 ){
		HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error [%s]", sSql);
		return NULL;
	}
	cJSON *user = cJSON_CreateObject();
	NS2J(user, &ptTbl, SYS_USER_COLMAP);

	if(strcmp("admin", GetIpcPtr(IPC, "userName")) == 0)
		cJSON_AddBoolToObject(user, "admin", 1);
	else 
		cJSON_AddBoolToObject(user, "admin", 0);

	memset(sSql,  0x00, sizeof(sSql));
	sprintf(sSql, "select *  from sys_dept where dept_id=%d", ptTbl.dept_id);
	memset((char*)&ptTblDept, 0x00, sizeof(ptTblDept));
	nRet = mysql_ONE_SELECT(sSql, &ptTblDept, SYS_DEPT_COLMAP);
	if(nRet !=0 ){
		HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error [%s]", sSql);
		return NULL;
	}
    cJSON_AddStringToObject(root, "deptName", ptTblDept.dept_name);
	cJSON *dept= cJSON_CreateObject();
	NS2J(dept, &ptTblDept, SYS_DEPT_COLMAP);
	cJSON_AddItemToObject(user, "dept", dept);

	memset(sSql,  0x00, sizeof(sSql));
	sprintf(sSql, "select *  from sys_role  where status ='0' and role_id in (select  GROUP_CONCAT(role_id) from sys_user_role  where user_id=%d )", ptTbl.user_id);
	HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", sSql);
	mysql_TBL_CURSOR(sSql, NULL);
	mysql_TBL_OPEN(sSql, NULL);
	parray= cJSON_CreateArray();
	while(1)
	{
		memset((char *)&ptTblRole, 0x00, sizeof(ptTblRole));
		nRet = mysql_TBL_FETCH(NULL, &ptTblRole, SYS_ROLE_COLMAP);
		if(nRet == 1403)
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "FETCH over");
			mysql_TBL_CLOSE(sSql, NULL);
			break;
		}
		else if(nRet == -1)
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "fetch error [%s]", sSql);
			mysql_TBL_CLOSE(sSql, NULL);
			break;
		}
		else
		{
			pdata = cJSON_CreateObject();
			NS2J(pdata, &ptTblRole, SYS_ROLE_COLMAP);
			cJSON_AddItemToArray(parray, pdata);
		}
	}
	cJSON_AddItemToObject(user, "roles", parray); 
	cJSON_AddItemToObject(root, "user", user); 

	json_string = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);
	HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string[%s]", json_string);
	return json_string;
}
