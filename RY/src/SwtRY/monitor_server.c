#include "Switch.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <bcrypt/bcrypt.h>
#include "multipart.h"
#include "xlsxio_read.h"
#include "xlsxio_write.h"
static struct tbl_common_def 
{
	long    page_num;
	long	page_size;
    long    total_rows;
	char	sort[128];
}ptTblCommon;

static TABFUN TBL_COMMON_COLMAP[MAXCOLNUM]={
    {"total_rows",    offsetof(struct tbl_common_def, total_rows), 'I', "", "total", ""},
};

typedef struct 
{
	char sKey[64];
	char sDsp[64];
}st_data_key_dsp;

int monitor_server(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "monitor_server";
	char			sMsgSrcId[SRV_ID_LEN+1];
	char			sMethod[16];
	char			sUri[128];
	int				nRet;
	char			sSql[2048];
	char			where[256];
	char			set[2048];
	char			sTmpBuf[128];
	char			*IPC = sMsgBuf+ MSGHEADLEN;

	memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
	memcpy(sTmpBuf, IPC, 6);
	char			*json_string = sMsgBuf+MSGHEADLEN+6+atoi(sTmpBuf);
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s begin", sFuncName);
	showIpc(IPC);
	GetIpcValueExt(IPC, "$MSGSRCID", sMsgSrcId, sizeof(sMsgSrcId));
	GetIpcValueExt(IPC, "$METHOD", sMethod, sizeof(sMethod));
	GetIpcValueExt(IPC, "$PATH", sUri, sizeof(sUri));
	cJSON      *root,  *body, *item, *array;
	char       *param= get_last_component(sUri);

	int         user_id;
    if (validate_jwt_token(IPC, &user_id) != 0 )
    {
        root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "msg", "jwt验证不过");
        cJSON_AddNumberToObject(root, "code", 401);
        json_string = cJSON_Print(root);
        memset(sMethod, 0x00, sizeof(sMethod));
    }
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(sUri, "/monitor/server") == 0)  //redis信息
	{
		root = cJSON_Parse("{\"msg\":\"操作成功\",\"code\":200,\"data\":{\"cpu\":{\"cpuNum\":2,\"total\":189000.0,\"sys\":28.04,\"used\":12.17,\"wait\":0.0,\"free\":33.33},\"mem\":{\"total\":1.7,\"used\":0.74,\"free\":0.96,\"usage\":43.32},\"jvm\":{\"total\":104.06,\"max\":421.5,\"free\":44.19,\"version\":\"17.0.15\",\"home\":\"/usr/lib/jvm/java-17-openjdk-17.0.15.0.6-2.el9.x86_64\",\"used\":59.87,\"startTime\":\"2025-08-24 11:00:50\",\"name\":\"OpenJDK 64-Bit Server VM\",\"usage\":57.54,\"runTime\":\"1天16小时53分钟\",\"inputArgs\":\"[]\"},\"sys\":{\"computerName\":\"localhost.localdomain\",\"computerIp\":\"127.0.0.1\",\"userDir\":\"/root/RuoYi-Vue-master\",\"osName\":\"Linux\",\"osArch\":\"amd64\"},\"sysFiles\":[{\"dirName\":\"/\",\"sysTypeName\":\"xfs\",\"typeName\":\"/\",\"total\":\"17.4 GB\",\"free\":\"9.0 GB\",\"used\":\"8.4 GB\",\"usage\":48.42},{\"dirName\":\"/boot\",\"sysTypeName\":\"xfs\",\"typeName\":\"/dev/sda1\",\"total\":\"960 MB\",\"free\":\"655 MB\",\"used\":\"305 MB\",\"usage\":31.75}]}}");
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string = %s", json_string);
	}
	
	//head=cJSON_PrintUnformatted(root);
	cJSON_Delete(root);
	SetIpcValue(sSql, "$HEADNUM", "0");
    memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
    memcpy(sTmpBuf, sSql, 6);
    int nIpcLen = 6+atoi(sTmpBuf);

    memcpy(sMsgBuf+MSGHEADLEN, sSql, nIpcLen);
    strcpy(sMsgBuf+MSGHEADLEN+nIpcLen, json_string);
    int nMsgLen = strlen(json_string)+MSGHEADLEN+nIpcLen;
	free(json_string);
	((SYSHEAD *)sMsgBuf)->shMsgLen = nMsgLen;
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
