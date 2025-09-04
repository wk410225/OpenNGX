#include "Switch.h"
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

static TABFUN TBL_SRV_PARAM_COLMAP[MAXCOLNUM]={
	{"usage_key",    offsetof(tbl_srv_param_def, usage_key   ), 'I', "usageKey",    "usageKey",    ""},
	{"srv_id",       offsetof(tbl_srv_param_def, srv_id      ), 'C', "srvId",       "srvId",       ""},
	{"param_usage",  offsetof(tbl_srv_param_def, param_usage ), 'C', "paramUsage",  "paramUsage",  ""},
	{"param_index",  offsetof(tbl_srv_param_def, param_index ), 'C', "paramIndex",  "paramIndex",  ""},
	{"param_data",   offsetof(tbl_srv_param_def, param_data  ), 'C', "paramData",   "paramData",   ""},
	{"param_dsp",    offsetof(tbl_srv_param_def, param_dsp   ), 'C', "paramDsp",    "paramDsp",    ""},
};


int tbl_srv_param(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "tbl_srv_param";
	char			sMsgSrcId[SRV_ID_LEN+1];
	char			sMethod[16];
	char			sUri[128];
	int				nRet;
	char			sSql[2048];
	char			where[256];
	char			set[2048];
	char			sTmpBuf[128];
	char			*IPC = sMsgBuf+ MSGHEADLEN;
	tbl_srv_param_def	ptTbl;

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
	if (strcmp(sMethod, "DELETE" ) == 0 ) // 删除
	{
		TABFUN TBL_SRV_PARAM_INDMAP[MAXCOLNUM]={
			{"usage_key",    offsetof(tbl_srv_param_def, usage_key   ), 'I', "usageKey",    "usageKey",    ""},
			{"srv_id",       offsetof(tbl_srv_param_def, srv_id      ), 'C', "srvId",       "srvId",       ""},
			{"param_usage",  offsetof(tbl_srv_param_def, param_usage ), 'C', "paramUsage",  "paramUsage",  ""},
			{"param_index",  offsetof(tbl_srv_param_def, param_index ), 'C', "paramIndex",  "paramIndex",  ""},
		};
        root = cJSON_Parse(json_string);
        if (root == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "Error before: %s", error_ptr);
            }
            return -1;
        }
        json_string = cJSON_Print(root);
        HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "req json: %s", json_string);
		if (cJSON_IsArray(root)) 
        {
            cJSON *item = NULL;
            cJSON_ArrayForEach(item, root) 
			{
				memset(sSql,  0x00, sizeof(sSql));
				sprintf(sSql, "delete from tbl_srv_param where ");
				nRet = NGetDeleteWhereSql(item, sSql, TBL_SRV_PARAM_INDMAP);
				HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", sSql);
				nRet = mysql_TBL_DELETE(sSql);
			}
			DbsCommit();
			cJSON_Delete(root);
			root = cJSON_CreateObject();
			cJSON_AddStringToObject(root, "msg", "操作成功");
			cJSON_AddNumberToObject(root, "code", 200);
		}
		else 
		{
			memset(sSql,  0x00, sizeof(sSql));
			sprintf(sSql, "delete from tbl_srv_param where ");
			nRet = NGetDeleteWhereSql(root, sSql, TBL_SRV_PARAM_INDMAP);
			cJSON_Delete(root);
			HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", sSql);
			if(nRet == 0)
			{
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
			}
			else
			{
				root = cJSON_CreateObject();
				cJSON_AddStringToObject(root, "msg", "操作失败");
				cJSON_AddNumberToObject(root, "code", 500);
			}
		}
        json_string = cJSON_Print(root);
	}
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(sUri, "/topconfig/tblsrvparam/list") == 0) //查询列表
	{
    	tbl_srv_param_def    ptTblMenu;

		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;

		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select count(1) as total_rows from tbl_srv_param ");
        NGetQueryWhere(IPC, sSql,  TBL_SRV_PARAM_COLMAP);
        memset((char*)&ptTblCommon, 0x00, sizeof(ptTblCommon));
        nRet = mysql_ONE_SELECT(sSql, &ptTblCommon, TBL_COMMON_COLMAP);
        if(nRet ==0 ){
            HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "ptCurrentTbl.total_rows=%d", ptTblCommon.total_rows);
            NS2J(root, &ptTblCommon, TBL_COMMON_COLMAP);
        }

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select * from tbl_srv_param ");
		NGetQueryWhere(IPC, sSql,  TBL_SRV_PARAM_COLMAP);
		strcat(sSql, " order by usage_key,srv_id,param_usage,param_index asc");
		NGetQuerySortLimit(IPC, sSql);
		HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", sSql);
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		parray= cJSON_CreateArray();
		while(1)
		{
			memset((char *)&ptTblMenu, 0x00, sizeof(ptTblMenu));
			nRet = mysql_TBL_FETCH(NULL, &ptTblMenu, TBL_SRV_PARAM_COLMAP);
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
                NS2J(pdata, &ptTblMenu, TBL_SRV_PARAM_COLMAP);
				cJSON_AddItemToArray(parray, pdata);
			}
		}
		cJSON_AddItemToObject(root, "rows", parray); 
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string[%s]", cJSON_Print(root));
	}
	else if (strcmp(sMethod, "PUT" ) == 0  && strcmp(sUri, "/topconfig/tblsrvparam") == 0) //修改状态
	{
		TABFUN TBL_SRV_PARAM_INDMAP[MAXCOLNUM]={
			{"usage_key",    offsetof(tbl_srv_param_def, usage_key   ), 'I', "usageKey",    "usageKey",    ""},
			{"srv_id",       offsetof(tbl_srv_param_def, srv_id      ), 'C', "srvId",       "srvId",       ""},
			{"param_usage",  offsetof(tbl_srv_param_def, param_usage ), 'C', "paramUsage",  "paramUsage",  ""},
			{"param_index",  offsetof(tbl_srv_param_def, param_index ), 'C', "paramIndex",  "paramIndex",  ""},
		};
		tbl_srv_param_def	ptTbl;
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
        memset((char *)&ptTbl, 0x00, sizeof(ptTbl));
        NJ2S(root, &ptTbl, TBL_SRV_PARAM_COLMAP);
		//GetCurrTime(ptTbl.update_time);
        PrintTbl(&ptTbl, TBL_SRV_PARAM_COLMAP);
        memset(sSql,  0x00, sizeof(sSql));
        memset(set, 0x00, sizeof(set));
        memset(where, 0x00, sizeof(where));
        NGetUpdateSetSql(root, set, TBL_SRV_PARAM_COLMAP);
        NGetUpdateWhereSql(root, where, TBL_SRV_PARAM_INDMAP);
        sprintf(sSql, "update tbl_srv_param  set %s where %s;", set, where);
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
			cJSON_AddStringToObject(root, "msg", "操作成功");
			cJSON_AddNumberToObject(root, "code", 200);
        }
		else 
		{
			cJSON_AddStringToObject(root, "msg", "操作失败");
			cJSON_AddNumberToObject(root, "code", 500);
		}
    }
	else if (strcmp(sMethod, "POST" ) == 0  && strcmp(sUri, "/topconfig/tblsrvparam") == 0) //新增状态
	{
		tbl_srv_param_def	ptTbl;
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
        memset((char *)&ptTbl, 0x00, sizeof(ptTbl));
        NJ2S(root, &ptTbl, TBL_SRV_PARAM_COLMAP);
        PrintTbl(&ptTbl, TBL_SRV_PARAM_COLMAP);
		nRet = mysql_TBL_INSERT("tbl_srv_param", &ptTbl, TBL_SRV_PARAM_COLMAP);
		if(nRet != 0)
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "insert error ");
			cJSON_AddStringToObject(root, "msg", "操作失败");
			cJSON_AddNumberToObject(root, "code", 500);
		}
		else 
		{
			DbsCommit();
			cJSON_AddStringToObject(root, "msg", "操作成功");
			cJSON_AddNumberToObject(root, "code", 200);
		}
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
