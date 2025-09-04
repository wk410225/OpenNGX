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
static TABFUN SYS_USER_ROLE_COLMAP[MAXCOLNUM]={
	{"user_id", offsetof(sys_user_role_def, user_id), 'I',            "user_id", "user_id", ""},
	{"role_id", offsetof(sys_user_role_def, role_id), 'I',            "role_id", "role_id", ""},
};
static TABFUN SYS_ROLE_MENU_COLMAP[MAXCOLNUM]={
    {"role_id", offsetof(sys_role_menu_def, role_id), 'I',            "roleId", "roleId", ""},
    {"menu_id", offsetof(sys_role_menu_def, menu_id), 'I',            "menuId", "menuId", ""},
};
static TABFUN SYS_MENU_COLMAP[MAXCOLNUM]={
	{"menu_id",      offsetof(sys_menu_def, menu_id        ), 'I', "menuId",      "menuId",      ""},
	{"menu_name",    offsetof(sys_menu_def, menu_name      ), 'C', "menuName",    "menuName",    ""},
	{"parent_id",    offsetof(sys_menu_def, parent_id      ), 'I', "parentId",    "parentId",    ""},
	{"order_num",    offsetof(sys_menu_def, order_num      ), 'I', "orderNum",    "orderNum",    ""},
	{"path",         offsetof(sys_menu_def, path           ), 'C', "path",         "path",         ""},
	{"component",    offsetof(sys_menu_def, component      ), 'C', "component",    "component",    ""},
	{"query",        offsetof(sys_menu_def, query          ), 'C', "query",        "query",        ""},
	{"route_name",   offsetof(sys_menu_def, route_name     ), 'C', "routeName",   "routeName",   ""},
	{"is_frame",     offsetof(sys_menu_def, is_frame       ), 'I', "isFrame",     "isFrame",     ""},
	{"is_cache",     offsetof(sys_menu_def, is_cache       ), 'I', "isCache",     "isCache",     ""},
	{"menu_type",    offsetof(sys_menu_def, menu_type      ), 'C', "menuType",    "menuType",    ""},
	{"visible",      offsetof(sys_menu_def, visible        ), 'C', "visible",      "visible",      ""},
	{"status",       offsetof(sys_menu_def, status         ), 'C', "status",       "status",       ""},
	{"perms",        offsetof(sys_menu_def, perms          ), 'C', "perms",        "perms",        ""},
	{"icon",         offsetof(sys_menu_def, icon           ), 'C', "icon",         "icon",         ""},
	{"create_by",    offsetof(sys_menu_def, create_by      ), 'C', "createBy",    "createBy",    ""},
	{"create_time",  offsetof(sys_menu_def, create_time    ), 'T', "createTime",  "createTime",  ""},
	{"update_by",    offsetof(sys_menu_def, update_by      ), 'C', "updateBy",    "updateBy",    ""},
	{"update_time",  offsetof(sys_menu_def, update_time    ), 'T', "updateTime",  "updateTime",  ""},
	{"remark",       offsetof(sys_menu_def, remark         ), 'C', "remark",       "remark",      ""},
};

static TABFUN SYS_ROLE_COLMAP[MAXCOLNUM]={
    {"role_id", offsetof(sys_role_def, role_id), 'I', "roleId", "roleId"},
    {"role_name", offsetof(sys_role_def, role_name), 'C', "roleName","roleName"},
    {"role_key", offsetof(sys_role_def, role_key), 'C', "roleKey", "roleKey"},
    {"role_sort", offsetof(sys_role_def, role_sort), 'I', "", "roleSort"},
    {"data_scope", offsetof(sys_role_def, data_scope), 'C', "", "dataScope"},
    {"menu_check_strictly", offsetof(sys_role_def, menu_check_strictly), 'I', "", "menuCheckStrictly"},
    {"dept_check_strictly", offsetof(sys_role_def, dept_check_strictly), 'I', "", "deptCheckStrictly"},
    {"status", offsetof(sys_role_def, status), 'C', "status", "status"},
    {"del_flag", offsetof(sys_role_def, del_flag), 'C', "", "delFlag"},
    {"create_by", offsetof(sys_role_def, create_by), 'C', "", "createBy"},
    {"create_time", offsetof(sys_role_def, create_time), 'T', "params[beginTime]", "createTime", "params[endTime]"},
    {"update_by", offsetof(sys_role_def, update_by), 'C', "", "updateBy"},
    {"update_time", offsetof(sys_role_def, update_time), 'C', "", "updateTime"},
    {"remark", offsetof(sys_role_def, remark), 'C', "", "remark"},
};

int system_menu(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "system_menu";
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
	sys_dept_def	ptTblC;

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
		sprintf(sSql, "delete from sys_menu where menu_id= %s", param);
        HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", sSql);
		nRet = mysql_TBL_DELETE(sSql);
        if(nRet != 0)
        {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "delete error [%s]", sSql);
            return -1;
        }
		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "delete from sys_role_menu where menu_id= %s", param);
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
	else if (strcmp(sMethod, "GET" ) == 0  && memcmp(sUri, "/system/menu/", strlen("/system/menu/")) == 0 && atoi(param) > 0) 
	{
    	sys_menu_def    ptTblMenu;

		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;

		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select * from sys_menu where menu_id = %s", param);
		HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", sSql);
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		//parray= cJSON_CreateArray();
		while(1)
		{
			memset((char *)&ptTblMenu, 0x00, sizeof(ptTblMenu));
			nRet = mysql_TBL_FETCH(NULL, &ptTblMenu, SYS_MENU_COLMAP);
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
                NS2J(pdata, &ptTblMenu, SYS_MENU_COLMAP);
				//cJSON_AddItemToArray(parray, pdata);
			}
		}
		//cJSON_AddItemToObject(root, "data", parray); 
		cJSON_AddItemToObject(root, "data", pdata); 
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string[%s]", cJSON_Print(root));
	}
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(sUri, "/system/menu/list") == 0) //菜单列表
	{
    	sys_menu_def    ptTblMenu;

		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;

		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select * from sys_menu ");
		NGetQueryWhere(IPC, sSql,  SYS_MENU_COLMAP);
		strcat(sSql, " order by order_num asc");
		HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", sSql);
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		parray= cJSON_CreateArray();
		while(1)
		{
			memset((char *)&ptTblMenu, 0x00, sizeof(ptTblMenu));
			nRet = mysql_TBL_FETCH(NULL, &ptTblMenu, SYS_MENU_COLMAP);
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
                NS2J(pdata, &ptTblMenu, SYS_MENU_COLMAP);
				cJSON_AddItemToArray(parray, pdata);
			}
		}
		cJSON_AddItemToObject(root, "data", parray); 
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string[%s]", cJSON_Print(root));
	}
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(sUri, "/system/menu/treeselect") == 0) //获取全部菜单列表
	{
		TABFUN SYS_MENU_COLMAP[MAXCOLNUM]={
			{"menu_id", offsetof(sys_menu_def, menu_id), 'I', "", "id"},
			{"menu_name", offsetof(sys_menu_def, menu_name), 'C', "", "label"},
			{"status", offsetof(sys_menu_def, status), 'C', "", "disabled", "B"},
		};
		root = cJSON_CreateObject();
		
		sys_menu_def    ptTblMenu, ptTblMenuChild, ptTbl;
		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "select  * from sys_menu  where parent_id = 0");
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;
		cJSON *pmeta= NULL, *cmeta= NULL;
		parray= cJSON_CreateArray();
		while(1)
		{
			memset((char *)&ptTblMenu, 0x00, sizeof(ptTblMenu));
			nRet = mysql_TBL_FETCH(NULL, &ptTblMenu, SYS_MENU_COLMAP);
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
                NS2J(pdata, &ptTblMenu, SYS_MENU_COLMAP);

				carray= cJSON_CreateArray();
				memset(sSql,  0x00, sizeof(sSql));
				sprintf(sSql, "select  * from sys_menu  where parent_id = %d", ptTblMenu.menu_id);
				HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "[%s]", sSql);
				mysql_TBL_CURSOR1(sSql, NULL);
				mysql_TBL_OPEN1(sSql, NULL);
				while(1)
				{
					memset((char *)&ptTblMenuChild, 0x00, sizeof(ptTblMenuChild));
					nRet = mysql_TBL_FETCH1(NULL, &ptTblMenuChild, SYS_MENU_COLMAP);
					if(nRet == 1403)
					{	
						mysql_TBL_CLOSE1(sSql, NULL);
						break;
					}
					else if(nRet == -1)
					{
						HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "fetch error [%s]", sSql);
						mysql_TBL_CLOSE1(sSql, NULL);
						return -1;
					}
					else
					{
						cdata = cJSON_CreateObject();
						NS2J(cdata, &ptTblMenuChild, SYS_MENU_COLMAP);

						array= cJSON_CreateArray();
						memset(sSql,  0x00, sizeof(sSql));
						sprintf(sSql, "select  * from sys_menu  where parent_id = %d", ptTblMenuChild.menu_id);
						HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "[%s]", sSql);
						mysql_TBL_CURSOR2(sSql, NULL);
						mysql_TBL_OPEN2(sSql, NULL);
						while(1)
						{
							memset((char *)&ptTbl, 0x00, sizeof(ptTbl));
							nRet = mysql_TBL_FETCH2(NULL, &ptTbl, SYS_MENU_COLMAP);
							if(nRet == 1403)
							{	
								mysql_TBL_CLOSE2(sSql, NULL);
								break;
							}
							else if(nRet == -1)
							{
								HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "fetch error [%s]", sSql);
								mysql_TBL_CLOSE2(sSql, NULL);
								return -1;
							}
							else
							{
								cmeta = cJSON_CreateObject();
								NS2J(cmeta, &ptTbl, SYS_MENU_COLMAP);
								cJSON_AddItemToArray(array, cmeta);
							}
						}
						cJSON_AddItemToObject(cdata, "children", array); 
						cJSON_AddItemToArray(carray, cdata);
					}
				}
				cJSON_AddItemToObject(pdata, "children", carray); 
				cJSON_AddItemToArray(parray, pdata);
			}
		}
		cJSON_AddItemToObject(root, "data", parray); 
		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string[%s]", cJSON_Print(root));

	}
	//获取全部角色的菜单列表
	else if (strcmp(sMethod, "GET" ) == 0 && memcmp(sUri, "/system/menu/roleMenuTreeselect/", strlen("/system/menu/roleMenuTreeselect/")) == 0 )
	{
		TABFUN SYS_MENU_COLMAP[MAXCOLNUM]={
			{"menu_id", offsetof(sys_menu_def, menu_id), 'I', "", "id"},
			{"menu_name", offsetof(sys_menu_def, menu_name), 'C', "", "label"},
			{"status", offsetof(sys_menu_def, status), 'C', "", "disabled", "B"},
		};
		root = cJSON_CreateObject();
		sys_menu_def    ptTblMenu, ptTblMenuChild, ptTbl;
		sys_role_menu_def    ptTblRoleMenu;
		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "select menu_id from sys_role_menu where role_id =%d ", atoi(param));
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		array= cJSON_CreateArray();
		while(1)
		{
			memset((char *)&ptTblRoleMenu, 0x00, sizeof(ptTblRoleMenu));
			nRet = mysql_TBL_FETCH(NULL, &ptTblRoleMenu, SYS_ROLE_MENU_COLMAP);
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
				cJSON_AddItemToArray(array, cJSON_CreateNumber(ptTblRoleMenu.menu_id));
			}
		}
		cJSON_AddItemToObject(root, "checkedKeys", array); 

		sprintf(sSql, "select  * from sys_menu  where parent_id = 0");
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;
		cJSON *pmeta= NULL, *cmeta= NULL;
		parray= cJSON_CreateArray();
		while(1)
		{
			memset((char *)&ptTblMenu, 0x00, sizeof(ptTblMenu));
			nRet = mysql_TBL_FETCH(NULL, &ptTblMenu, SYS_MENU_COLMAP);
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
                NS2J(pdata, &ptTblMenu, SYS_MENU_COLMAP);

				carray= cJSON_CreateArray();
				memset(sSql,  0x00, sizeof(sSql));
				sprintf(sSql, "select  * from sys_menu  where parent_id = %d", ptTblMenu.menu_id);
				HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "[%s]", sSql);
				mysql_TBL_CURSOR1(sSql, NULL);
				mysql_TBL_OPEN1(sSql, NULL);
				while(1)
				{
					memset((char *)&ptTblMenuChild, 0x00, sizeof(ptTblMenuChild));
					nRet = mysql_TBL_FETCH1(NULL, &ptTblMenuChild, SYS_MENU_COLMAP);
					if(nRet == 1403)
					{	
						mysql_TBL_CLOSE1(sSql, NULL);
						break;
					}
					else if(nRet == -1)
					{
						HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "fetch error [%s]", sSql);
						mysql_TBL_CLOSE1(sSql, NULL);
						return -1;
					}
					else
					{
						cdata = cJSON_CreateObject();
						NS2J(cdata, &ptTblMenuChild, SYS_MENU_COLMAP);

						array= cJSON_CreateArray();
						memset(sSql,  0x00, sizeof(sSql));
						sprintf(sSql, "select  * from sys_menu  where parent_id = %d", ptTblMenuChild.menu_id);
						HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "[%s]", sSql);
						mysql_TBL_CURSOR2(sSql, NULL);
						mysql_TBL_OPEN2(sSql, NULL);
						while(1)
						{
							memset((char *)&ptTbl, 0x00, sizeof(ptTbl));
							nRet = mysql_TBL_FETCH2(NULL, &ptTbl, SYS_MENU_COLMAP);
							if(nRet == 1403)
							{	
								mysql_TBL_CLOSE2(sSql, NULL);
								break;
							}
							else if(nRet == -1)
							{
								HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "fetch error [%s]", sSql);
								mysql_TBL_CLOSE2(sSql, NULL);
								return -1;
							}
							else
							{
								cmeta = cJSON_CreateObject();
								NS2J(cmeta, &ptTbl, SYS_MENU_COLMAP);
								cJSON_AddItemToArray(array, cmeta);
							}
						}
						cJSON_AddItemToObject(cdata, "children", array); 
						cJSON_AddItemToArray(carray, cdata);
					}
				}
				cJSON_AddItemToObject(pdata, "children", carray); 
				cJSON_AddItemToArray(parray, pdata);
			}
		}
		cJSON_AddItemToObject(root, "menus", parray); 
		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string[%s]", cJSON_Print(root));

	}
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(sUri, "/system/role/list") == 0) //获取角色列表
	{
		sys_user_def	ptTblUser;
		sys_dept_def	ptTblDept;
		sys_role_def	ptTblRole;

		root = cJSON_CreateObject();
		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "select count(1) as total_rows from sys_role ");
		NGetQueryWhere(IPC, sSql,  SYS_ROLE_COLMAP);
		memset((char*)&ptTblCommon, 0x00, sizeof(ptTblCommon));
		nRet = mysql_ONE_SELECT(sSql, &ptTblCommon, TBL_COMMON_COLMAP);
		if(nRet ==0 ){
			HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "ptCurrentTbl.total_rows=%d", ptTblCommon.total_rows);
			NS2J(root, &ptTblCommon, TBL_COMMON_COLMAP);
		}

		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;
		parray = cJSON_CreateArray();
		memset(sSql,  0x00, sizeof(sSql));
		strcpy(sSql, "select * from sys_role ");
		NGetQueryWhere(IPC, sSql,  SYS_ROLE_COLMAP);
		NGetQuerySortLimit(IPC, sSql );
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		while(1)
		{
			memset((char *)&ptTblRole, 0x00, sizeof(ptTblRole));
			nRet = mysql_TBL_FETCH(NULL, &ptTblRole, SYS_ROLE_COLMAP);
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
				PrintTbl(&ptTblRole, SYS_ROLE_COLMAP);
				pdata = cJSON_CreateObject();
				NS2J(pdata, &ptTblRole, SYS_ROLE_COLMAP);
				
				if(strcmp(ptTblUser.user_name, "admin") == 0)
					cJSON_AddBoolToObject(pdata, "admin", 1);
				else
					cJSON_AddBoolToObject(pdata, "admin", 0);
				cJSON_AddItemToArray(parray, pdata);
			}
		}
		cJSON_AddItemToObject(root, "rows", parray);
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);
	}
	else if (strcmp(sMethod, "PUT" ) == 0  && strcmp(sUri, "/system/menu") == 0) //修改状态
	{
		TABFUN SYS_MENU_INDMAP[MAXCOLNUM]={
			{"menu_id", offsetof(sys_menu_def, menu_id), 'I', "menuId",  "",""},
		};
		sys_menu_def	ptTbl;
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
        NJ2S(root, &ptTbl, SYS_MENU_COLMAP);
		GetCurrTime(ptTbl.update_time);
        PrintTbl(&ptTbl, SYS_MENU_COLMAP);
        memset(sSql,  0x00, sizeof(sSql));
        memset(set, 0x00, sizeof(set));
        memset(where, 0x00, sizeof(where));
        NGetUpdateSetSql(root, set, SYS_MENU_COLMAP);
        NGetUpdateWhereSql(root, where, SYS_MENU_INDMAP);
        sprintf(sSql, "update sys_menu  set %s where %s;", set, where);
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
	else if (strcmp(sMethod, "POST" ) == 0  && strcmp(sUri, "/system/menu") == 0) //新增状态
	{
		sys_menu_def	ptTbl;
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
        NJ2S(root, &ptTbl, SYS_MENU_COLMAP);
		GetCurrTime(ptTbl.create_time);
        PrintTbl(&ptTbl, SYS_MENU_COLMAP);
/*
        memset(sSql,  0x00, sizeof(sSql));
        memset(set, 0x00, sizeof(set));
        memset(where, 0x00, sizeof(where));
        NGetUpdateSetSql(root, set, SYS_MENU_COLMAP);
        NGetUpdateWhereSql(root, where, SYS_MENU_INDMAP);
        sprintf(sSql, "update sys_menu  set %s where %s;", set, where);
*/
		HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "update sql=[%s]", sSql);
		nRet = mysql_TBL_INSERT("sys_menu", &ptTbl, SYS_MENU_COLMAP);
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
