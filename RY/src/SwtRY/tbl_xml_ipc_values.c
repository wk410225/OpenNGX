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
static TABFUN TBL_XML_IPC_VALUES_COLMAP[MAXCOLNUM]={
    {"l_usage_key",     offsetof(tbl_xml_ipc_values_def, l_usage_key), 'L'},
    {"i_buf_dsp_index", offsetof(tbl_xml_ipc_values_def, i_buf_dsp_index), 'L'},
    {"i_node_index",    offsetof(tbl_xml_ipc_values_def, i_node_index), 'L'},
    {"father_node_id",  offsetof(tbl_xml_ipc_values_def, father_node_id), 'L'},
    {"sp_node_name",    offsetof(tbl_xml_ipc_values_def, sp_node_name), 'C'},
    {"i_leaf_type",     offsetof(tbl_xml_ipc_values_def, i_leaf_type), 'L'},
    {"i_fld_buf",       offsetof(tbl_xml_ipc_values_def, i_fld_buf), 'C'},
    {"i_repeat_or",     offsetof(tbl_xml_ipc_values_def, i_repeat_or), 'L'},
    {"i_attribute_num", offsetof(tbl_xml_ipc_values_def, i_attribute_num), 'L'},
    {"sp_attribute_name1", offsetof(tbl_xml_ipc_values_def, sp_attribute_name1), 'C'},
    {"sp_attribute_test1", offsetof(tbl_xml_ipc_values_def, sp_attribute_test1), 'C'},
    {"sp_attribute_name2", offsetof(tbl_xml_ipc_values_def, sp_attribute_name2), 'C'},
    {"sp_attribute_test2", offsetof(tbl_xml_ipc_values_def, sp_attribute_test2), 'C'},
    {"sp_attribute_name3", offsetof(tbl_xml_ipc_values_def, sp_attribute_name3), 'C'},
    {"sp_attribute_test3", offsetof(tbl_xml_ipc_values_def, sp_attribute_test3), 'C'},
    {"i_hold1_usr",        offsetof(tbl_xml_ipc_values_def, i_hold1_usr), 'L'},
    {"sp_hold2_usr",       offsetof(tbl_xml_ipc_values_def, sp_hold2_usr), 'C'},
};

static TABFUN TBL_XML_IPC_VALUES_INDMAP[MAXINDNUM]={
    {"l_usage_key",			offsetof(tbl_xml_ipc_values_def, l_usage_key),		 'L'},
    {"i_buf_dsp_index",     offsetof(tbl_xml_ipc_values_def, i_buf_dsp_index),  'L'},
    {"i_node_index",		offsetof(tbl_xml_ipc_values_def, i_node_index),     'L'},
};

int tbl_xml_ipc_values(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "tbl_xml_ipc_values";
	char			sMsgSrcId[SRV_ID_LEN+1];
	char			sMethod[16];
	int				nRet;
	char			sSql[2048];
	char			where[256];
	char			set[2048];
	char			sTmpBuf[128];
	char			*IPC = sMsgBuf+ MSGHEADLEN;
	tbl_xml_ipc_values_def ptCurrentTbl;

	memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
	memcpy(sTmpBuf, IPC, 6);
	char		*json_string = sMsgBuf+MSGHEADLEN+6+atoi(sTmpBuf);
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s begin", sFuncName);
	showIpc(IPC);
	GetIpcValueExt(IPC, "$MSGSRCID", sMsgSrcId, sizeof(sMsgSrcId));
	GetIpcValueExt(IPC, "$METHOD", sMethod, sizeof(sMethod));
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
		J2S(root, &ptCurrentTbl, TBL_XML_IPC_VALUES_COLMAP);
		PrintTbl(&ptCurrentTbl, TBL_XML_IPC_VALUES_COLMAP);
		nRet = mysql_TBL_INSERT("tbl_xml_ipc_values", &ptCurrentTbl, TBL_XML_IPC_VALUES_COLMAP);
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
			sprintf(sSql, "select count(1) as total_rows from tbl_xml_ipc_values ");
			GetQueryWhere(IPC, sSql,  TBL_XML_IPC_VALUES_COLMAP);
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
*/
		{
			root= cJSON_CreateArray();
			memset(sSql,  0x00, sizeof(sSql));
			strcpy(sSql, "select * from tbl_xml_ipc_values ");
			GetQueryWhere(IPC, sSql,  TBL_XML_IPC_VALUES_COLMAP);
			GetQuerySortLimit(IPC, sSql );
			mysql_TBL_CURSOR(sSql, NULL);
			mysql_TBL_OPEN(sSql, NULL);
			while(1)
			{
				memset((char *)&ptCurrentTbl, 0x00, sizeof(ptCurrentTbl));
				nRet = mysql_TBL_FETCH(NULL, &ptCurrentTbl, TBL_XML_IPC_VALUES_COLMAP);
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
					S2J(item, &ptCurrentTbl, TBL_XML_IPC_VALUES_COLMAP);
					cJSON_AddItemToArray(root, item);
				}
			}
			json_string = cJSON_Print(root);
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
        J2S(root, &ptCurrentTbl, TBL_XML_IPC_VALUES_COLMAP);
		PrintTbl(&ptCurrentTbl, TBL_XML_IPC_VALUES_COLMAP);
		memset(sSql,  0x00, sizeof(sSql));
		memset(set, 0x00, sizeof(set));
		memset(where, 0x00, sizeof(where));
		GetUpdateSetSql(root, set, TBL_XML_IPC_VALUES_COLMAP);
		GetUpdateWhereSql(root, where, TBL_XML_IPC_VALUES_INDMAP);
		sprintf(sSql, "update tbl_xml_ipc_values  set %s where %s;", set, where);
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
        J2S(root, &ptCurrentTbl, TBL_XML_IPC_VALUES_COLMAP);
		PrintTbl(&ptCurrentTbl, TBL_XML_IPC_VALUES_COLMAP);
		memset(sSql,  0x00, sizeof(sSql));
		memset(where, 0x00, sizeof(sSql));
		/*must delete depended  on unique index column */
		if (GetDeleteWhereSql(root, where , TBL_XML_IPC_VALUES_INDMAP) < 0)
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "don't have needed column %s", json_string);
			return -1;
		}
		sprintf(sSql, "delete from  tbl_xml_ipc_values  where %s;",  where);
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
