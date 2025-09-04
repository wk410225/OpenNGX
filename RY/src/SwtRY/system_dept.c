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
    {"total_rows",    offsetof(struct tbl_common_def, total_rows), 'I', "", "total", ""},
};
static TABFUN SYS_DEPT_COLMAP[MAXCOLNUM]={
	{"dept_id",      offsetof(sys_dept_def, dept_id        ), 'I', "deptId",      "deptId",      ""},
	{"parent_id",    offsetof(sys_dept_def, parent_id      ), 'I', "parentId",    "parentId",    ""},
	{"ancestors",    offsetof(sys_dept_def, ancestors      ), 'C', "ancestors",    "ancestors",    ""},
	{"dept_name",    offsetof(sys_dept_def, dept_name      ), 'C', "deptName",    "deptName",    ""},
	{"order_num",    offsetof(sys_dept_def, order_num      ), 'I', "orderNum",    "orderNum",    ""},
	{"leader",       offsetof(sys_dept_def, leader         ), 'C', "leader",       "leader",       ""},
	{"phone",        offsetof(sys_dept_def, phone          ), 'C', "phone",        "phone",        ""},
	{"email",        offsetof(sys_dept_def, email          ), 'C', "email",        "email",        ""},
	{"status",       offsetof(sys_dept_def, status         ), 'C', "status",       "status",       ""},
	{"del_flag",     offsetof(sys_dept_def, del_flag       ), 'C', "delFlag",     "delFlag",     ""},
	{"create_by",    offsetof(sys_dept_def, create_by      ), 'C', "createBy",    "createBy",    ""},
	{"create_time",  offsetof(sys_dept_def, create_time    ), 'T', "createTime",  "createTime",  ""},
	{"update_by",    offsetof(sys_dept_def, update_by      ), 'C', "updateBy",    "updateBy",    ""},
	{"update_time",  offsetof(sys_dept_def, update_time    ), 'T', "updateTime",  "updateTime",  ""},
};

int system_dept(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "system_dept";
	char			sMsgSrcId[SRV_ID_LEN+1];
	char			sMethod[16];
	char			sUri[128];
	int				nRet;
	char			sSql[2048];
	char			where[256];
	char			set[2048];
	char			sTmpBuf[128];
	char			*IPC = sMsgBuf+ MSGHEADLEN;
	sys_dept_def	ptTbl;

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
	if (strcmp(sMethod, "DELETE" ) == 0  ) // 删除用户
	{
		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "delete from sys_dept where dept_id= %s", param);
        HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", sSql);
		nRet = mysql_TBL_DELETE(sSql);
        if(nRet != 0)
        {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "delete error [%s]", sSql);
            return -1;
        }
		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "delete from sys_role_dept where dept_id= %s", param);
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
	else if (strcmp(sMethod, "GET" ) == 0  && memcmp(sUri, "/system/dept/", strlen("/system/dept/")) == 0 && atoi(param) > 0) 
	{
    	sys_dept_def    ptTblMenu;

		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;

		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select * from sys_dept where dept_id = %s", param);
		HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", sSql);
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		//parray= cJSON_CreateArray();
		while(1)
		{
			memset((char *)&ptTblMenu, 0x00, sizeof(ptTblMenu));
			nRet = mysql_TBL_FETCH(NULL, &ptTblMenu, SYS_DEPT_COLMAP);
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
                NS2J(pdata, &ptTblMenu, SYS_DEPT_COLMAP);
				//cJSON_AddItemToArray(parray, pdata);
			}
		}
		//cJSON_AddItemToObject(root, "data", parray); 
		cJSON_AddItemToObject(root, "data", pdata); 
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string[%s]", cJSON_Print(root));
	}
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(sUri, "/system/dept/list") == 0) //菜单列表
	{
    	sys_dept_def    ptTblMenu;

		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;

		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select * from sys_dept ");
		NGetQueryWhere(IPC, sSql,  SYS_DEPT_COLMAP);
		strcat(sSql, " order by order_num asc");
		HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", sSql);
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		parray= cJSON_CreateArray();
		while(1)
		{
			memset((char *)&ptTblMenu, 0x00, sizeof(ptTblMenu));
			nRet = mysql_TBL_FETCH(NULL, &ptTblMenu, SYS_DEPT_COLMAP);
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
                NS2J(pdata, &ptTblMenu, SYS_DEPT_COLMAP);
				cJSON_AddItemToArray(parray, pdata);
			}
		}
		cJSON_AddItemToObject(root, "data", parray); 
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string[%s]", cJSON_Print(root));
	}
	else if (strcmp(sMethod, "PUT" ) == 0  && strcmp(sUri, "/system/dept") == 0) //修改状态
	{
		TABFUN SYS_DEPT_INDMAP[MAXCOLNUM]={
			{"dept_id", offsetof(sys_dept_def, dept_id), 'I', "deptId",  "",""},
		};
		sys_dept_def	ptTbl;
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
        NJ2S(root, &ptTbl, SYS_DEPT_COLMAP);
		GetCurrTime(ptTbl.update_time);
        PrintTbl(&ptTbl, SYS_DEPT_COLMAP);
        memset(sSql,  0x00, sizeof(sSql));
        memset(set, 0x00, sizeof(set));
        memset(where, 0x00, sizeof(where));
        NGetUpdateSetSql(root, set, SYS_DEPT_COLMAP);
        NGetUpdateWhereSql(root, where, SYS_DEPT_INDMAP);
        sprintf(sSql, "update sys_dept  set %s where %s;", set, where);
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
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
    }
	else if (strcmp(sMethod, "POST" ) == 0  && strcmp(sUri, "/system/dept") == 0) //新增状态
	{
		sys_dept_def	ptTbl;
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
        NJ2S(root, &ptTbl, SYS_DEPT_COLMAP);
        PrintTbl(&ptTbl, SYS_DEPT_COLMAP);
/*
        memset(sSql,  0x00, sizeof(sSql));
        memset(set, 0x00, sizeof(set));
        memset(where, 0x00, sizeof(where));
        NGetUpdateSetSql(root, set, SYS_DEPT_COLMAP);
        NGetUpdateWhereSql(root, where, SYS_DEPT_INDMAP);
        sprintf(sSql, "update sys_dept  set %s where %s;", set, where);
*/
		HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "update sql=[%s]", sSql);
		nRet = mysql_TBL_INSERT("sys_dept", &ptTbl, SYS_DEPT_COLMAP);
		if(nRet != 0)
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "update error [%s]", sSql);
			return -1;
		}
		DbsCommit();
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
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
