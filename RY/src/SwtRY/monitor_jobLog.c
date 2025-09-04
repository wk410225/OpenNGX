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

static TABFUN SYS_JOB_LOG_COLMAP[MAXCOLNUM]={
	{"job_log_id",   offsetof(sys_job_log_def, job_log_id     ), 'I', "jobLogId",   "jobLogId",   ""},
	{"job_name",     offsetof(sys_job_log_def, job_name       ), 'C', "jobName",     "jobName",     ""},
	{"job_group",    offsetof(sys_job_log_def, job_group      ), 'C', "jobGroup",    "jobGroup",    ""},
	{"invoke_target", offsetof(sys_job_log_def, invoke_target  ), 'C', "invokeTarget", "invokeTarget", ""},
	{"job_message",  offsetof(sys_job_log_def, job_message    ), 'C', "jobMessage",  "jobMessage",  ""},
	{"status",       offsetof(sys_job_log_def, status         ), 'C', "status",       "status",       ""},
	{"exception_info", offsetof(sys_job_log_def, exception_info ), 'C', "exceptionInfo", "exceptionInfo", ""},
	{"create_time",  offsetof(sys_job_log_def, create_time    ), 'T', "params[beginTime]",  "createTime",  "params[endTime]"},
};


int monitor_jobLog(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "monitor_jobLog";
	char			sMsgSrcId[SRV_ID_LEN+1];
	char			sMethod[16];
	char			sUri[128];
	int				nRet;
	char			sSql[2048];
	char			where[256];
	char			set[2048];
	char			sTmpBuf[128];
	char			*IPC = sMsgBuf+ MSGHEADLEN;
	sys_job_log_def	ptTbl;

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
	else if (strcmp(sMethod, "DELETE" ) == 0 &&  strcmp(sUri, "/monitor/jobLog/clean") ) //清空
	{
		memset(sSql,  0x00, sizeof(sSql));
		//sprintf(sSql, "delete from sys_job_log where job_log_id= %s", param);
		sprintf(sSql, "truncate table sys_job_log");
        HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", sSql);
		nRet = mysql_TBL_DELETE(sSql);
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
	else if (strcmp(sMethod, "GET" ) == 0  && memcmp(sUri, "/monitor/jobLog/", strlen("monitor/jobLog/")) == 0 && atoi(param) > 0) 
	{
    	sys_job_log_def    ptTblMenu;

		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;

		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select * from sys_job_log where job_log_id = %s", param);
		HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", sSql);
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		//parray= cJSON_CreateArray();
		while(1)
		{
			memset((char *)&ptTblMenu, 0x00, sizeof(ptTblMenu));
			nRet = mysql_TBL_FETCH(NULL, &ptTblMenu, SYS_JOB_LOG_COLMAP);
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
                NS2J(pdata, &ptTblMenu, SYS_JOB_LOG_COLMAP);
				//cJSON_AddItemToArray(parray, pdata);
			}
		}
		//cJSON_AddItemToObject(root, "data", parray); 
		cJSON_AddItemToObject(root, "data", pdata); 
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string[%s]", cJSON_Print(root));
	}
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(sUri, "/monitor/jobLog/list") == 0) //菜单列表
	{
    	sys_job_log_def    ptTblMenu;

		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;

		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select count(1) as total_rows from sys_job_log ");
        NGetQueryWhere(IPC, sSql,  SYS_JOB_LOG_COLMAP);
        memset((char*)&ptTblCommon, 0x00, sizeof(ptTblCommon));
        nRet = mysql_ONE_SELECT(sSql, &ptTblCommon, TBL_COMMON_COLMAP);
        if(nRet ==0 ){
            HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "ptCurrentTbl.total_rows=%d", ptTblCommon.total_rows);
            NS2J(root, &ptTblCommon, TBL_COMMON_COLMAP);
        }

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select * from sys_job_log ");
		NGetQueryWhere(IPC, sSql,  SYS_JOB_LOG_COLMAP);
		strcat(sSql, " order by job_log_id asc");
		HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", sSql);
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		parray= cJSON_CreateArray();
		while(1)
		{
			memset((char *)&ptTblMenu, 0x00, sizeof(ptTblMenu));
			nRet = mysql_TBL_FETCH(NULL, &ptTblMenu, SYS_JOB_LOG_COLMAP);
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
                NS2J(pdata, &ptTblMenu, SYS_JOB_LOG_COLMAP);
				//cJSON_AddBoolToObject(pdata, "flag", 0);
				cJSON_AddItemToArray(parray, pdata);
			}
		}
		cJSON_AddItemToObject(root, "rows", parray); 
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string[%s]", cJSON_Print(root));
	}
	else if (strcmp(sMethod, "POST" ) == 0  && strcmp(sUri, "/monitor/jobLog/export") == 0) //角色导出
	{
		xlsxiowriter handle;
		if ((handle = xlsxiowrite_open("jobLog.xlsx", "岗位数据")) == NULL) 
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

		sys_job_log_def	ptTbl;

		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		memset(sSql,  0x00, sizeof(sSql));
		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;
		parray = cJSON_CreateArray();
		memset(sSql,  0x00, sizeof(sSql));
		strcpy(sSql, "select * from sys_job ");
		NGetQueryWhere(IPC, sSql,  SYS_JOB_LOG_COLMAP);
		NGetQuerySortLimit(IPC, sSql );
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		while(1)
		{
			memset((char *)&ptTbl, 0x00, sizeof(ptTbl));
			nRet = mysql_TBL_FETCH(NULL, &ptTbl, SYS_JOB_LOG_COLMAP);
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
				PrintTbl(&ptTbl, SYS_JOB_LOG_COLMAP);
				xlsxiowrite_add_cell_int(handle, ptTbl.job_log_id);
				xlsxiowrite_add_cell_string(handle,   ptTbl.job_name);
				xlsxiowrite_add_cell_string(handle,   ptTbl.job_name);
				xlsxiowrite_add_cell_string(handle,   ptTbl.status);
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
		SetIpcValue(sSql, "$VAL1", "attachment; filename=job.xlsx");


		memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
		memcpy(sTmpBuf, sSql, 6);
		int nIpcLen = 6+atoi(sTmpBuf);
		memcpy(sMsgBuf+MSGHEADLEN, sSql, nIpcLen);

		FILE *file = fopen("job.xlsx", "rb");
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
