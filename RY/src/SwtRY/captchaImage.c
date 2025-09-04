#include "Switch.h"
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

static TABFUN SYS_CONFIG_COLMAP[MAXCOLNUM]={
	{"config_id", offsetof(sys_config_def, config_id), 'I'},
    {"config_name", offsetof(sys_config_def, config_name), 'C'},
    {"config_key", offsetof(sys_config_def, config_key), 'C'},
    {"config_value", offsetof(sys_config_def, config_value), 'C'},
    {"config_type", offsetof(sys_config_def, config_type), 'C'},
    {"create_by", offsetof(sys_config_def, create_by), 'C'},
    {"create_time", offsetof(sys_config_def, create_time), 'C'},
    {"update_by", offsetof(sys_config_def, update_by), 'C'},
    {"update_time", offsetof(sys_config_def, update_time), 'C'},
    {"remark", offsetof(sys_config_def, remark), 'C'},

};


static TABFUN SYS_CONFIG_INDMAP[MAXINDNUM]={
	{"config_id", offsetof(sys_config_def, config_id), 'I'},
};

int captchaImage(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "sys_config";
	char			sMsgSrcId[SRV_ID_LEN+1];
	char			sMethod[16];
	char			sUri[128];
	int				nRet;
	char			sSql[2048];
	char			where[256];
	char			set[2048];
	char			sTmpBuf[128];
	char			*IPC = sMsgBuf+ MSGHEADLEN;
	sys_config_def ptCurrentTbl;

	memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
	memcpy(sTmpBuf, IPC, 6);
	char			*json_string = sMsgBuf+MSGHEADLEN+6+atoi(sTmpBuf);
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s begin", sFuncName);
	showIpc(IPC);
	GetIpcValueExt(IPC, "$MSGSRCID", sMsgSrcId, sizeof(sMsgSrcId));
	GetIpcValueExt(IPC, "$METHOD", sMethod, sizeof(sMethod));
	GetIpcValueExt(IPC, "$PATH", sUri, sizeof(sUri));
	cJSON      *root,  *body, *item, *array;

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
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "\n%s", json_string);
		memset((char *)&ptCurrentTbl, 0x00, sizeof(ptCurrentTbl));
		J2S(root, &ptCurrentTbl, SYS_CONFIG_COLMAP);
		PrintTbl(&ptCurrentTbl, SYS_CONFIG_COLMAP);
		nRet = mysql_TBL_INSERT("sys_config", &ptCurrentTbl, SYS_CONFIG_COLMAP);
		if(nRet != 0)
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "insert error nRet=[%d]", nRet);
			return -1;
		}
		DbsCommit();
	}
	else if (strcmp(sMethod, "GET" ) == 0 )
	{
/*
		GetIpcValueExt(IPC, "_page", sTmpBuf, sizeof(sTmpBuf));
		int pageNum = atoi(sTmpBuf);
		GetIpcValueExt(IPC, "_limit", sTmpBuf, sizeof(sTmpBuf));
		int pageSize = atoi(sTmpBuf);

		if( pageNum == 0 || pageSize == 0 )
		{
			memset(sSql,  0x00, sizeof(sSql));
			sprintf(sSql, "select count(1) as total_rows from sys_config ");
			GetQueryWhere(IPC, sSql,  SYS_CONFIG_COLMAP);
			memset((char*)&ptTblCommon, 0x00, sizeof(ptTblCommon));
			nRet = mysql_ONE_SELECT(sSql, &ptTblCommon, TBL_COMMON_COLMAP);
			if(nRet ==0 ){ 
				HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "ptCurrentTbl.total_rows=%d", ptTblCommon.total_rows);
				root = cJSON_CreateObject();
				S2J(root, &ptTblCommon, TBL_COMMON_COLMAP);
				json_string = cJSON_Print(root);
			}
		}
		else
		if(GetIpcValueExt(IPC, "$reserve", sTmpBuf, sizeof(sTmpBuf)) > 0)
		{
			if(strcmp(sTmpBuf, "tbl_fld_tsf") == 0)
			{
				array = cJSON_CreateArray();
				memset(sSql,  0x00, sizeof(sSql));
				strcpy(sSql, "select * from sys_config a left join  tbl_fld_tsf b on a.i_pos_index = b.i_sour_fld_index ");
				GetQueryWhereSchema(IPC, sSql,  SYS_CONFIG_COLMAP, "a");
				GetQuerySortLimit(IPC, sSql );
				mysql_TBL_CURSOR(sSql, NULL);
				mysql_TBL_OPEN(sSql, NULL);
				while(1)
				{
					memset((char *)&ptCurrentTbl, 0x00, sizeof(ptCurrentTbl));
					nRet = mysql_TBL_FETCH(NULL, &ptCurrentTbl, SYS_CONFIG_COLMAP);
					if(nRet == 1403)
						break;
					else if(nRet == -1)
					{
						HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "fetch error [%s]", sSql);
						break;
					}
					else
					{
						item = cJSON_CreateObject();
						S2J(item, &ptCurrentTbl, SYS_CONFIG_COLMAP);
						cJSON_AddItemToArray(array, item);
					}
				}
				json_string = cJSON_Print(array);
			}
		}
		else
*/
		{
			memset(sSql,  0x00, sizeof(sSql));
			strcpy(sSql, "select config_value from sys_config where config_key='sys.account.captchaEnabled' ");
			memset((char*)&ptCurrentTbl, 0x00, sizeof(ptCurrentTbl));
			nRet = mysql_ONE_SELECT(sSql, &ptCurrentTbl, SYS_CONFIG_COLMAP);
			if(nRet ==0 ){ 
				HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "ptCurrentTbl.config_value=%s", ptCurrentTbl.config_value);
				root = cJSON_CreateObject();
				S2J(root, &ptCurrentTbl, SYS_CONFIG_COLMAP);
				cJSON_AddStringToObject(root, "msg", "操作成功");
				cJSON_AddNumberToObject(root, "code", 200);
				cJSON_AddBoolToObject(root, "captchaEnabled", 0);
				json_string = cJSON_Print(root);
			}
		}
		HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "\n%s", json_string);
	}
	else if (strcmp(sMethod, "PUT" ) == 0 )
    {
        root = cJSON_Parse(json_string);
        if (root == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "Error before: %s", error_ptr);
            }
            return -1;
        }
        json_string = cJSON_Print(root);
        HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "\n%s", json_string);
        memset((char *)&ptCurrentTbl, 0x00, sizeof(ptCurrentTbl));
        J2S(root, &ptCurrentTbl, SYS_CONFIG_COLMAP);
		PrintTbl(&ptCurrentTbl, SYS_CONFIG_COLMAP);
		memset(sSql,  0x00, sizeof(sSql));
		memset(set, 0x00, sizeof(set));
		memset(where, 0x00, sizeof(where));
		GetUpdateSetSql(root, set, SYS_CONFIG_COLMAP);
		GetUpdateWhereSql(root, where, SYS_CONFIG_INDMAP);
		sprintf(sSql, "update sys_config  set %s where %s;", set, where);
		HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "update sql=[%s]", sSql);
		if(strlen(set) > 0 && strlen(where) > 0)
		{
			nRet = mysql_TBL_UPDATE(sSql);
			if(nRet != 0)
			{
				HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "update error [%s]", sSql);
				return -1;
			}
			DbsCommit();
		}
    }
	else if (strcmp(sMethod, "DELETE" ) == 0 )
    {
        root = cJSON_Parse(json_string);
        if (root == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "Error before: %s", error_ptr);
            }
            return -1;
        }
        json_string = cJSON_Print(root);
        HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "\n%s", json_string);
        memset((char *)&ptCurrentTbl, 0x00, sizeof(ptCurrentTbl));
        J2S(root, &ptCurrentTbl, SYS_CONFIG_COLMAP);
		PrintTbl(&ptCurrentTbl, SYS_CONFIG_COLMAP);
		memset(sSql,  0x00, sizeof(sSql));
		memset(where, 0x00, sizeof(sSql));
		GetDeleteWhereSql(root, where, SYS_CONFIG_INDMAP);
		/*must delete depended  on unique index column */
		sprintf(sSql, "delete from  sys_config  where %s;",  where);
		if (GetDeleteWhereSql(root, where , SYS_CONFIG_INDMAP) < 0)
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "don't have needed column %s", json_string);
			return -1;
		}
		HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "delete sql=[%s]", sSql);
		nRet = mysql_TBL_DELETE(sSql);
		if(nRet != 0)
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "delete error [%s]", sSql);
			return -1;
		}
		DbsCommit();
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
