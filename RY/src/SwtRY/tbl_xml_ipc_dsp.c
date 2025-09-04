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
static TABFUN TBL_XML_IPC_DSP_COLMAP[MAXCOLNUM]={
    {"usage_key",     offsetof(tbl_xml_ipc_dsp_def, usage_key), 'L'},
    {"comp_key",      offsetof(tbl_xml_ipc_dsp_def, comp_key),  'C'},
    {"xml_index",     offsetof(tbl_xml_ipc_dsp_def, xml_index), 'L'},
    {"buf_index",     offsetof(tbl_xml_ipc_dsp_def, buf_index), 'L'},
    {"txn_number",    offsetof(tbl_xml_ipc_dsp_def, txn_number),'C'},
    {"xml_dsp",       offsetof(tbl_xml_ipc_dsp_def, xml_dsp),   'C'},
};
static TABFUN TBL_XML_IPC_DSP_INDMAP[MAXINDNUM]={
    {"usage_key",     offsetof(tbl_xml_ipc_dsp_def, usage_key), 'L'},
    {"comp_key",      offsetof(tbl_xml_ipc_dsp_def, comp_key),  'C'},
    {"xml_index",     offsetof(tbl_xml_ipc_dsp_def, xml_index), 'L'},
    {"txn_number",    offsetof(tbl_xml_ipc_dsp_def, txn_number),'C'}
};

int tbl_xml_ipc_dsp(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "tbl_xml_ipc_dsp";
	char			sMsgSrcId[SRV_ID_LEN+1];
	char			sMethod[16];
	char			sUri[128];
	int				nRet;
	char			sSql[2048];
	char			where[256];
	char			set[2048];
	char			sTmpBuf[128];
	char			*IPC = sMsgBuf+ MSGHEADLEN;
	tbl_xml_ipc_dsp_def ptCurrentTbl;

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
		J2S(root, &ptCurrentTbl, TBL_XML_IPC_DSP_COLMAP);
		PrintTbl(&ptCurrentTbl, TBL_XML_IPC_DSP_COLMAP);
		nRet = mysql_TBL_INSERT("tbl_xml_ipc_dsp", &ptCurrentTbl, TBL_XML_IPC_DSP_COLMAP);
		if(nRet != 0)
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "insert error nRet=[%d]", nRet);
			return -1;
		}
		DbsCommit();
	}
	else if (strcmp(sMethod, "GET" ) == 0 )
	{
		GetIpcValueExt(IPC, "_page", sTmpBuf, sizeof(sTmpBuf));
		int pageNum = atoi(sTmpBuf);
		GetIpcValueExt(IPC, "_limit", sTmpBuf, sizeof(sTmpBuf));
		int pageSize = atoi(sTmpBuf);

		if( pageNum == 0 || pageSize == 0 )
		{
			memset(sSql,  0x00, sizeof(sSql));
			sprintf(sSql, "select count(1) as total_rows from tbl_xml_ipc_dsp ");
			GetQueryWhere(IPC, sSql,  TBL_XML_IPC_DSP_COLMAP);
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
		{
			array = cJSON_CreateArray();
			memset(sSql,  0x00, sizeof(sSql));
			strcpy(sSql, "select * from tbl_xml_ipc_dsp ");
			GetQueryWhere(IPC, sSql,  TBL_XML_IPC_DSP_COLMAP);
			GetQuerySortLimit(IPC, sSql );
			mysql_TBL_CURSOR(sSql, NULL);
			mysql_TBL_OPEN(sSql, NULL);
			while(1)
			{
				memset((char *)&ptCurrentTbl, 0x00, sizeof(ptCurrentTbl));
				nRet = mysql_TBL_FETCH(NULL, &ptCurrentTbl, TBL_XML_IPC_DSP_COLMAP);
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
					S2J(item, &ptCurrentTbl, TBL_XML_IPC_DSP_COLMAP);
					cJSON_AddItemToArray(array, item);
				}
			}
			json_string = cJSON_Print(array);
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
        J2S(root, &ptCurrentTbl, TBL_XML_IPC_DSP_COLMAP);
		PrintTbl(&ptCurrentTbl, TBL_XML_IPC_DSP_COLMAP);
		memset(sSql,  0x00, sizeof(sSql));
		memset(set, 0x00, sizeof(set));
		memset(where, 0x00, sizeof(where));
		GetUpdateSetSql(root, set, TBL_XML_IPC_DSP_COLMAP);
		GetUpdateWhereSql(root, where, TBL_XML_IPC_DSP_INDMAP);
		sprintf(sSql, "update tbl_xml_ipc_dsp  set %s where %s;", set, where);
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
        J2S(root, &ptCurrentTbl, TBL_XML_IPC_DSP_COLMAP);
		PrintTbl(&ptCurrentTbl, TBL_XML_IPC_DSP_COLMAP);
		memset(sSql,  0x00, sizeof(sSql));
		memset(where, 0x00, sizeof(sSql));
		GetDeleteWhereSql(root, where, TBL_XML_IPC_DSP_INDMAP);
		/*must delete depended  on unique index column */
		sprintf(sSql, "delete from  tbl_xml_ipc_dsp  where %s;",  where);
		if (GetDeleteWhereSql(root, where , TBL_XML_IPC_DSP_INDMAP) < 0)
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
	strcpy(sMsgBuf+MSGHEADLEN, json_string);
	int nMsgLen = strlen(json_string)+MSGHEADLEN;
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
