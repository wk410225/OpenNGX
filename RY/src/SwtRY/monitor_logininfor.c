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

static TABFUN SYS_LOGININFOR_COLMAP[MAXCOLNUM]={
	{"info_id",      offsetof(sys_logininfor_def, info_id        ), 'I', "infoId",      "infoId",      ""},
	{"user_name",    offsetof(sys_logininfor_def, user_name      ), 'C', "userName",    "userName",    ""},
	{"ipaddr",       offsetof(sys_logininfor_def, ipaddr         ), 'C', "ipaddr",       "ipaddr",       ""},
	{"login_location", offsetof(sys_logininfor_def, login_location ), 'C', "loginLocation", "loginLocation", ""},
	{"browser",      offsetof(sys_logininfor_def, browser        ), 'C', "browser",      "browser",      ""},
	{"os",           offsetof(sys_logininfor_def, os             ), 'C', "os",           "os",           ""},
	{"status",       offsetof(sys_logininfor_def, status         ), 'C', "status",       "status",       ""},
	{"msg",          offsetof(sys_logininfor_def, msg            ), 'C', "msg",          "msg",          ""},
	{"login_time",   offsetof(sys_logininfor_def, login_time     ), 'T', "params[beginTime]",   "loginTime",   "params[endTime]"},
};

int monitor_logininfor(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "monitor_logininfor";
	char			sMsgSrcId[SRV_ID_LEN+1];
	char			sMethod[16];
	char			sUri[128];
	int				nRet;
	char			sSql[2048];
	char			where[256];
	char			set[2048];
	char			sTmpBuf[128];
	char			*IPC = sMsgBuf+ MSGHEADLEN;
	sys_logininfor_def	ptTbl;

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
	if (strcmp(sMethod, "DELETE" ) == 0 && strcmp(sUri, "/monitor/logininfor/clean") == 0) //清空
	{
		memset(sSql,  0x00, sizeof(sSql));
		strcpy(sSql, "truncate table sys_logininfor");
        HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", sSql);
		nRet = mysql_TBL_EXECUTE(sSql);
        if(nRet != 0)
        {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "delete error [%s]", sSql);
            return -1;
        }
        DbsCommit();
        root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "msg", "操作成功");
        cJSON_AddNumberToObject(root, "code", 200);
        json_string = cJSON_Print(root);
	}
	else if (strcmp(sMethod, "GET" ) == 0  && strncmp(sUri, "/monitor/logininfor/unlock/", strlen("/monitor/logininfor/unlock/")) == 0) //菜单列表
	{
        root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "msg", "操作成功");
        cJSON_AddNumberToObject(root, "code", 200);
        json_string = cJSON_Print(root);

	}
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(sUri, "/monitor/logininfor/list") == 0) //菜单列表
	{
    	sys_logininfor_def    ptTblMenu;

		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;

		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select count(1) as total_rows from sys_logininfor ");
        NGetQueryWhere(IPC, sSql,  SYS_LOGININFOR_COLMAP);
        memset((char*)&ptTblCommon, 0x00, sizeof(ptTblCommon));
        nRet = mysql_ONE_SELECT(sSql, &ptTblCommon, TBL_COMMON_COLMAP);
        if(nRet ==0 ){
            HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "ptCurrentTbl.total_rows=%d", ptTblCommon.total_rows);
            NS2J(root, &ptTblCommon, TBL_COMMON_COLMAP);
        }

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select * from sys_logininfor ");
		NGetQueryWhere(IPC, sSql,  SYS_LOGININFOR_COLMAP);
		NGetQuerySortLimit(IPC, sSql);
		//strcat(sSql, " order by info_id asc");
		HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", sSql);
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		parray= cJSON_CreateArray();
		while(1)
		{
			memset((char *)&ptTblMenu, 0x00, sizeof(ptTblMenu));
			nRet = mysql_TBL_FETCH(NULL, &ptTblMenu, SYS_LOGININFOR_COLMAP);
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
                NS2J(pdata, &ptTblMenu, SYS_LOGININFOR_COLMAP);
				cJSON_AddBoolToObject(pdata, "flag", 0);

				cJSON_AddItemToArray(parray, pdata);
			}
		}
		cJSON_AddItemToObject(root, "rows", parray); 
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string[%s]", cJSON_Print(root));
	}
	else if (strcmp(sMethod, "POST" ) == 0  && strcmp(sUri, "/monitor/logininfor/export") == 0) //角色导出
	{
		xlsxiowriter handle;
		if ((handle = xlsxiowrite_open("logininfor.xlsx", "岗位数据")) == NULL) 
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "xlsxiowrite_open error ");
			return -1;
		}
		//set row height
		xlsxiowrite_set_row_height(handle, 1);
		//how many rows to buffer to detect column widths
		xlsxiowrite_set_detection_rows(handle, 4);
		//write column names

		xlsxiowrite_add_column( handle,"岗位序号", 	0);
		xlsxiowrite_add_column( handle,"岗位编码",  0);
		xlsxiowrite_add_column( handle,"岗位名称",  0);
		xlsxiowrite_add_column( handle,"岗位排序",  0);
		xlsxiowrite_next_row(handle);


		sys_logininfor_def	ptTbl;

		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		memset(sSql,  0x00, sizeof(sSql));
		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;
		parray = cJSON_CreateArray();
		memset(sSql,  0x00, sizeof(sSql));
		strcpy(sSql, "select * from sys_logininfor ");
		NGetQueryWhere(IPC, sSql,  SYS_LOGININFOR_COLMAP);
		NGetQuerySortLimit(IPC, sSql );
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		while(1)
		{
			memset((char *)&ptTbl, 0x00, sizeof(ptTbl));
			nRet = mysql_TBL_FETCH(NULL, &ptTbl, SYS_LOGININFOR_COLMAP);
			if(nRet == 1403)
			{
				mysql_TBL_CLOSE(sSql, NULL);
				break;
			}
			else if(nRet == -1)
			{
				HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "fetch error [%s]", sSql);
				mysql_TBL_CLOSE(sSql, NULL);
				return -1;
			}
			else
			{
				PrintTbl(&ptTbl, SYS_LOGININFOR_COLMAP);
				xlsxiowrite_add_cell_int(handle, ptTbl.info_id);
				xlsxiowrite_add_cell_string(handle,   ptTbl.user_name);
				xlsxiowrite_add_cell_string(handle,   ptTbl.msg);
				xlsxiowrite_add_cell_string(handle,   ptTbl.login_time);
				xlsxiowrite_next_row(handle);
			}
		}
  		xlsxiowrite_close(handle);
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);

		SetIpcValue(sSql, "$HEADNUM", "2");
		
		SetIpcValue(sSql, "$KEY0", "Content-Type");
		SetIpcValue(sSql, "$VAL0", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet;charset=utf-8");
		SetIpcValue(sSql, "$KEY1", "Content-Disposition");
		SetIpcValue(sSql, "$VAL1", "attachment; filename=logininfor.xlsx");


		memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
		memcpy(sTmpBuf, sSql, 6);
		int nIpcLen = 6+atoi(sTmpBuf);
		memcpy(sMsgBuf+MSGHEADLEN, sSql, nIpcLen);

		FILE *file = fopen("logininfor.xlsx", "rb");
		int nFileLen = fread(sMsgBuf+MSGHEADLEN+nIpcLen, 1, 102400, file); 
		if (nFileLen > 0)
		{
			
		}
		fclose(file);

		int nMsgLen = nFileLen+MSGHEADLEN+nIpcLen;
		free(json_string);
		cJSON_Delete(root);
		
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
