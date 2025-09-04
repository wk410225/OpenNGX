#include "Switch.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <bcrypt/bcrypt.h>
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
    {"user_id", offsetof(sys_user_def, user_id), 'I'},
    {"dept_id", offsetof(sys_user_def, dept_id), 'I'},
    {"user_name", offsetof(sys_user_def, user_name), 'C'},
    {"nick_name", offsetof(sys_user_def, nick_name), 'C'},
    {"user_type", offsetof(sys_user_def, user_type), 'C'},
    {"email", offsetof(sys_user_def, email), 'C'},
    {"phonenumber", offsetof(sys_user_def, phonenumber), 'C'},
    {"sex", offsetof(sys_user_def, sex), 'C'},
    {"avatar", offsetof(sys_user_def, avatar), 'C'},
    {"password", offsetof(sys_user_def, password), 'C'},
    {"status", offsetof(sys_user_def, status), 'C'},
    {"del_flag", offsetof(sys_user_def, del_flag), 'C'},
    {"login_ip", offsetof(sys_user_def, login_ip), 'C'},
    {"login_date", offsetof(sys_user_def, login_date), 'C'},
    {"pwd_update_date", offsetof(sys_user_def, pwd_update_date), 'C'},
    {"create_by", offsetof(sys_user_def, create_by), 'C'},
    {"create_time", offsetof(sys_user_def, create_time), 'C'},
    {"update_by", offsetof(sys_user_def, update_by), 'C'},
    {"update_time", offsetof(sys_user_def, update_time), 'C'},
    {"remark", offsetof(sys_user_def, remark), 'C'},

};


static TABFUN SYS_USER_INDMAP[MAXINDNUM]={
    {"user_id", offsetof(sys_user_def, user_id), 'I'},
};



int logout(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "logout";
	char			sMsgSrcId[SRV_ID_LEN+1];
	char			sMethod[16];
	char			sUri[128];
	int				nRet;
	char			sSql[2048];
	char			where[256];
	char			set[2048];
	char			sTmpBuf[128];
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
	cJSON      *root,  *body, *item, *array;

	root= cJSON_CreateObject();
	cJSON_AddStringToObject(root, "msg", "操作成功");
	cJSON_AddNumberToObject(root, "code", 200);
    json_string = cJSON_Print(root);
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
