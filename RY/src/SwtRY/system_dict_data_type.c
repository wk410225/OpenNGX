#include "Switch.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <bcrypt/bcrypt.h>
#include  "multipart.h"
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
    {"total_rows",    offsetof(struct tbl_common_def, total_rows), 'L'},
};

static TABFUN SYS_DICT_TYPE_COLMAP[MAXCOLNUM]={
	{"dict_id",      offsetof(sys_dict_type_def, dict_id        ), 'I', "dictId",      "dictId",      ""},
	{"dict_name",    offsetof(sys_dict_type_def, dict_name      ), 'C', "dictName",    "dictName",    ""},
	{"dict_type",    offsetof(sys_dict_type_def, dict_type      ), 'C', "dictType",    "dictType",    ""},
	{"status",       offsetof(sys_dict_type_def, status         ), 'C', "status",       "status",     ""},
	{"create_by",    offsetof(sys_dict_type_def, create_by      ), 'C', "createBy",    "createBy",    ""},
	{"create_time",  offsetof(sys_dict_type_def, create_time    ), 'T', "createTime",  "createTime",  ""},
	{"update_by",    offsetof(sys_dict_type_def, update_by      ), 'C', "updateBy",    "updateBy",    ""},
	{"update_time",  offsetof(sys_dict_type_def, update_time    ), 'T', "updateTime",  "updateTime",  ""},
	{"remark",       offsetof(sys_dict_type_def, remark         ), 'C', "remark",       "remark",     ""},
};


static TABFUN SYS_DICT_DATA_COLMAP[MAXCOLNUM]={
    {"dict_code", offsetof(sys_dict_data_def, dict_code), 'I', "dictCode", "dictCode"},
    {"dict_sort", offsetof(sys_dict_data_def, dict_sort), 'I', "dictSort", "dictSort"},
    {"dict_label", offsetof(sys_dict_data_def, dict_label), 'C', "dictLable", "dictLabel"},
    {"dict_value", offsetof(sys_dict_data_def, dict_value), 'C', "dictValue", "dictValue"},
    {"dict_type", offsetof(sys_dict_data_def, dict_type), 'C', "dictType", "dictType"},
    {"css_class", offsetof(sys_dict_data_def, css_class), 'C', "cssClass", "cssClass"},
    {"list_class", offsetof(sys_dict_data_def, list_class), 'C', "listClass", "listClass"},
    {"is_default", offsetof(sys_dict_data_def, is_default), 'C', "isDefault", "isDefault"},
    {"status", offsetof(sys_dict_data_def, status), 'C', "status", "status"},
    {"create_by", offsetof(sys_dict_data_def, create_by), 'C', "createBy", "createBy"},
    {"create_time", offsetof(sys_dict_data_def, create_time), 'T', "createTime", "createTime"},
    {"update_by", offsetof(sys_dict_data_def, update_by), 'C', "updateBy", "updateBy"},
    {"update_time", offsetof(sys_dict_data_def, update_time), 'T', "updateTime", "updateTime"},
    {"remark", offsetof(sys_dict_data_def, remark), 'C', "remark", "remark"},
};


int system_dict_data_type(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "system_dict_data_type";
	char			sMsgSrcId[SRV_ID_LEN+1];
	char			sMethod[16];
	char			sUri[128];
	int				nRet;
	char			sSql[2048];
	char			where[256];
	char			set[2048];
	char			sTmpBuf[128];
	char			*IPC = sMsgBuf+ MSGHEADLEN;
	sys_dict_type_def	ptTbl;
	sys_dict_data_def 	ptTblData;

	memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
	memcpy(sTmpBuf, IPC, 6);
	char			*json_string = sMsgBuf+MSGHEADLEN+6+atoi(sTmpBuf);
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s begin", sFuncName);
	showIpc(IPC);
	GetIpcValueExt(IPC, "$MSGSRCID", sMsgSrcId, sizeof(sMsgSrcId));
	GetIpcValueExt(IPC, "$METHOD", sMethod, sizeof(sMethod));
	GetIpcValueExt(IPC, "$PATH", sUri, sizeof(sUri));
	cJSON      *root,  *body, *item, *array;

	char			*param= get_last_component(sUri);

	//if (strcmp(sMethod, "GET" ) == 0 && memcmp(sUri, "/system/dict/data/type/", strlen("/system/dict/data/type/)")) == 0 )
	if (strcmp(sMethod, "GET" ) == 0 )
	{
		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);
		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "select * from sys_dict_type where status = '0' and dict_type='%s'", param);
		memset((char*)&ptTbl, 0x00, sizeof(ptTbl));
		nRet = mysql_ONE_SELECT(sSql, &ptTbl, SYS_DICT_TYPE_COLMAP);
		if(nRet !=0 ) { 
			HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "error %s", sSql);
			return -1;
		}
		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "select  * from sys_dict_data  where dict_type='%s'", param);
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;
		//cJSON *pmeta= NULL, *cmeta= NULL;
		parray= cJSON_CreateArray();
		while(1)
		{
			memset((char *)&ptTblData, 0x00, sizeof(ptTblData));
			nRet = mysql_TBL_FETCH(NULL, &ptTblData, SYS_DICT_DATA_COLMAP);
			if(nRet == 1403)
			{
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
                NS2J(pdata, &ptTblData, SYS_DICT_DATA_COLMAP);
				if(ptTblData.is_default[0] == 'Y')
					cJSON_AddBoolToObject(pdata, "default", 1);
				else
					cJSON_AddBoolToObject(pdata, "default", 0);
				cJSON_AddItemToArray(parray, pdata); 
			}
		}
		cJSON_AddItemToObject(root, "data", parray); 
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string[%s]", cJSON_Print(root));
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
