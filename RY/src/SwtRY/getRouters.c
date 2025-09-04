#include "Switch.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <bcrypt/bcrypt.h>
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

static TABFUN SYS_USER_COLMAP[MAXCOLNUM]={
    {"user_id", offsetof(sys_user_def, user_id), 'I'},
    {"dept_id", offsetof(sys_user_def, dept_id), 'I'},
    {"user_name", offsetof(sys_user_def, user_name), 'C'},
    {"nick_name", offsetof(sys_user_def, nick_name), 'C'},
    {"user_type", offsetof(sys_user_def, user_type), 'C'},
    {"email", offsetof(sys_user_def, email), 'C'},
    {"phonenumber", offsetof(sys_user_def, phonenumber), 'C'},
    {"sex", offsetof(sys_user_def, sex), 'C'},
    {"avatar", offsetof(sys_user_def, avatar), 'C'},
    {"password", offsetof(sys_user_def, password), 'C'},
    {"status", offsetof(sys_user_def, status), 'C'},
    {"del_flag", offsetof(sys_user_def, del_flag), 'C'},
    {"login_ip", offsetof(sys_user_def, login_ip), 'C'},
    {"login_date", offsetof(sys_user_def, login_date), 'C'},
    {"pwd_update_date", offsetof(sys_user_def, pwd_update_date), 'C'},
    {"create_by", offsetof(sys_user_def, create_by), 'C'},
    {"create_time", offsetof(sys_user_def, create_time), 'C'},
    {"update_by", offsetof(sys_user_def, update_by), 'C'},
    {"update_time", offsetof(sys_user_def, update_time), 'C'},
    {"remark", offsetof(sys_user_def, remark), 'C'},
};

static TABFUN SYS_USER_INDMAP[MAXINDNUM]={
    {"user_id", offsetof(sys_user_def, user_id), 'I'},
};

static TABFUN SYS_ROLE_COLMAP[MAXCOLNUM]={
    {"role_id", offsetof(sys_role_def, role_id), 'I'},
    {"role_name", offsetof(sys_role_def, role_name), 'C'},
    {"role_key", offsetof(sys_role_def, role_key), 'C'},
    {"role_sort", offsetof(sys_role_def, role_sort), 'I'},
    {"data_scope", offsetof(sys_role_def, data_scope), 'C'},
    {"menu_check_strictly", offsetof(sys_role_def, menu_check_strictly), 'I'},
    {"dept_check_strictly", offsetof(sys_role_def, dept_check_strictly), 'I'},
    {"status", offsetof(sys_role_def, status), 'C'},
    {"del_flag", offsetof(sys_role_def, del_flag), 'C'},
    {"create_by", offsetof(sys_role_def, create_by), 'C'},
    {"create_time", offsetof(sys_role_def, create_time), 'C'},
    {"update_by", offsetof(sys_role_def, update_by), 'C'},
    {"update_time", offsetof(sys_role_def, update_time), 'C'},
    {"remark", offsetof(sys_role_def, remark), 'C'},
};

static TABFUN SYS_ROLE_INDMAP[MAXINDNUM]={
    {"role_id", offsetof(sys_role_def, role_id), 'I'},
};

static TABFUN SYS_MENU_COLMAP[MAXCOLNUM]={
    {"menu_id", offsetof(sys_menu_def, menu_id), 'I'},
    {"menu_name", offsetof(sys_menu_def, menu_name), 'C'},
    {"parent_id", offsetof(sys_menu_def, parent_id), 'I'},
    {"order_num", offsetof(sys_menu_def, order_num), 'I'},
    {"path", offsetof(sys_menu_def, path), 'C'},
    {"component", offsetof(sys_menu_def, component), 'C'},
    {"query", offsetof(sys_menu_def, query), 'C'},
    {"route_name", offsetof(sys_menu_def, route_name), 'C'},
    {"is_frame", offsetof(sys_menu_def, is_frame), 'I'},
    {"is_cache", offsetof(sys_menu_def, is_cache), 'I'},
    {"menu_type", offsetof(sys_menu_def, menu_type), 'C'},
    {"visible", offsetof(sys_menu_def, visible), 'C'},
    {"status", offsetof(sys_menu_def, status), 'C'},
    {"perms", offsetof(sys_menu_def, perms), 'C'},
    {"icon", offsetof(sys_menu_def, icon), 'C'},
    {"create_by", offsetof(sys_menu_def, create_by), 'C'},
    {"create_time", offsetof(sys_menu_def, create_time), 'C'},
    {"update_by", offsetof(sys_menu_def, update_by), 'C'},
    {"update_time", offsetof(sys_menu_def, update_time), 'C'},
    {"remark", offsetof(sys_menu_def, remark), 'C'},
};

int getRouters(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "getRouters";
	char			sMsgSrcId[SRV_ID_LEN+1];
	char			sMethod[16];
	char			sUri[128];
	int				nRet;
	char			sSql[2048];
	char			where[256];
	char			set[2048];
	char			sTmpBuf[128];
	char			*IPC = sMsgBuf+ MSGHEADLEN;
	sys_user_def	ptTbl;
	sys_role_def	ptTblRole;
	sys_menu_def	ptTblMenu, ptTblMenuChild, ptTblMCC;

	memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
	memcpy(sTmpBuf, IPC, 6);
	char			*json_string = sMsgBuf+MSGHEADLEN+6+atoi(sTmpBuf);
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s begin", sFuncName);
	showIpc(IPC);
	GetIpcValueExt(IPC, "$MSGSRCID", sMsgSrcId, sizeof(sMsgSrcId));
	GetIpcValueExt(IPC, "$METHOD", sMethod, sizeof(sMethod));
	GetIpcValueExt(IPC, "$PATH", sUri, sizeof(sUri));
	cJSON      *root,  *body, *item, *array, *imeta;

    int         user_id;
    if (validate_jwt_token(IPC, &user_id) != 0 )
    {
        root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "msg", "jwt验证不过");
        cJSON_AddNumberToObject(root, "code", 401);
        json_string = cJSON_Print(root);
        memset(sMethod, 0x00, sizeof(sMethod));
    }

	if (strcmp(sMethod, "GET" ) == 0 )
	{
		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);
		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "select *  from sys_user where user_id=%d", user_id);
		memset((char*)&ptTbl, 0x00, sizeof(ptTbl));
		nRet = mysql_ONE_SELECT(sSql, &ptTbl, SYS_USER_COLMAP);
		if(nRet !=0 ){ 
			HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "error %s", sSql);
			return -1;
		}
		memset(sSql,  0x00, sizeof(sSql));
		if(user_id == 1)
			sprintf(sSql, "select  * from sys_menu  where status = '0' and parent_id = 0");
		else 
			sprintf(sSql, "select  * from sys_menu  where status = '0' and parent_id = 0  and menu_id in  ( select menu_id from  sys_role_menu where  role_id in  (select group_concat(role_id) from sys_user_role where user_id=%d)) order by order_num ", ptTbl.user_id);
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
                //S2J(pdata, &ptTblMenu, SYS_MENU_COLMAP);
                
                memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
				strcpy(sTmpBuf, ptTblMenu.path);
				sTmpBuf[0] = toupper(sTmpBuf[0]);
                cJSON_AddStringToObject(pdata, "name", sTmpBuf);

                memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
				if(ptTblMenu.is_frame == 1)
					sprintf(sTmpBuf, "/%s",ptTblMenu.path);
				else 
					sprintf(sTmpBuf, "%s",ptTblMenu.path);

                cJSON_AddStringToObject(pdata, "path", sTmpBuf);
                cJSON_AddBoolToObject(pdata, "hidden", atoi(ptTblMenu.visible));

                memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
				if(ptTblMenu.is_frame == 1)
					cJSON_AddStringToObject(pdata, "redirect", "noRedirect");

				cJSON_AddStringToObject(pdata, "component", "Layout");
                cJSON_AddBoolToObject(pdata, "alwaysShow", 1);

				pmeta= cJSON_CreateObject();
				cJSON_AddStringToObject(pmeta, "title", ptTblMenu.menu_name);
				cJSON_AddStringToObject(pmeta, "icon", ptTblMenu.icon);
                cJSON_AddBoolToObject(pmeta, "noCache", ptTblMenu.is_cache);
				if(ptTblMenu.is_frame == 1)
					cJSON_AddNullToObject(pmeta, "link");
				else 
					cJSON_AddStringToObject(pmeta, "link", ptTblMenu.path);

				cJSON_AddItemToObject(pdata, "meta", pmeta); 

				carray= cJSON_CreateArray();

				memset(sSql,  0x00, sizeof(sSql));
				if(user_id == 1)
					sprintf(sSql, "select  * from sys_menu  where status = '0' and parent_id = %d", ptTblMenu.menu_id);
				else 
					sprintf(sSql, "select  * from sys_menu  where status = '0' and parent_id = %d  and menu_id in  ( select menu_id from  sys_role_menu where  role_id in  (select group_concat(role_id) from sys_user_role where user_id=%d)) order by order_num ", ptTblMenu.menu_id ,ptTbl.user_id);
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
						memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
						strcpy(sTmpBuf, ptTblMenuChild.path);
						sTmpBuf[0] = toupper(sTmpBuf[0]);
						cJSON_AddStringToObject(cdata, "name", sTmpBuf);

						cJSON_AddStringToObject(cdata, "path", ptTblMenuChild.path);
						cJSON_AddBoolToObject(cdata, "hidden", atoi(ptTblMenuChild.visible));
						cJSON_AddStringToObject(cdata, "component", ptTblMenuChild.component);

						cmeta= cJSON_CreateObject();
						cJSON_AddStringToObject(cmeta, "title", ptTblMenuChild.menu_name);
						cJSON_AddStringToObject(cmeta, "icon", ptTblMenuChild.icon);
						cJSON_AddBoolToObject(cmeta, "noCache", ptTblMenuChild.is_cache);
						if(ptTblMenuChild.is_frame == 1)
							cJSON_AddNullToObject(cmeta, "link");
						else 
							cJSON_AddStringToObject(cmeta, "link", ptTblMenuChild.path);
						cJSON_AddItemToObject(cdata, "meta", cmeta); 

						//cJSON_AddItemToArray(carray, cdata);


						array= cJSON_CreateArray();
						memset(sSql,  0x00, sizeof(sSql));
						if(user_id == 1)
							sprintf(sSql, "select  * from sys_menu  where status = '0' and parent_id = %d and menu_type in ('M', 'C')", ptTblMenuChild.menu_id);
						else 
							sprintf(sSql, "select  * from sys_menu  where status = '0' and parent_id = %d and menu_type in ('M', 'C') and menu_id in  ( select menu_id from  sys_role_menu where  role_id in  (select group_concat(role_id) from sys_user_role where user_id=%d)) order by order_num ", ptTblMenuChild.menu_id ,ptTbl.user_id);
						HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "[%s]", sSql);
						mysql_TBL_CURSOR2(sSql, NULL);
						mysql_TBL_OPEN2(sSql, NULL);
						while(1)
						{
							memset((char *)&ptTblMCC, 0x00, sizeof(ptTblMCC));
							nRet = mysql_TBL_FETCH2(NULL, &ptTblMCC, SYS_MENU_COLMAP);
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
								item= cJSON_CreateObject();
								memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
								strcpy(sTmpBuf, ptTblMCC.path);
								sTmpBuf[0] = toupper(sTmpBuf[0]);
								cJSON_AddStringToObject(item, "name", sTmpBuf);

								cJSON_AddStringToObject(item, "path", ptTblMCC.path);
								cJSON_AddBoolToObject(item, "hidden", atoi(ptTblMCC.visible));
								cJSON_AddStringToObject(item, "component", ptTblMCC.component);

								imeta= cJSON_CreateObject();
								cJSON_AddStringToObject(imeta, "title", ptTblMCC.menu_name);
								cJSON_AddStringToObject(imeta, "icon", ptTblMCC.icon);
								cJSON_AddBoolToObject(imeta, "noCache", ptTblMCC.is_cache);
								if(ptTblMCC.is_frame == 1)
									cJSON_AddNullToObject(imeta, "link");
								else 
									cJSON_AddStringToObject(imeta, "link", ptTblMCC.path);
								cJSON_AddItemToObject(item, "meta", imeta); 
								cJSON_AddItemToArray(array, item);
							}
						}
						cJSON_AddItemToObject(cdata, "children", array); 
					}
					cJSON_AddItemToArray(carray, cdata);
				}
				cJSON_AddItemToObject(pdata, "children", carray); 
				cJSON_AddItemToArray(parray, pdata);
				//HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "parray[%s]", cJSON_Print(parray));
			}
		}
		cJSON_AddItemToObject(root, "data", parray); 
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string[%s]", cJSON_Print(root));
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

//	strcpy(sMsgBuf+MSGHEADLEN, json_string);
//	int nMsgLen = strlen(json_string)+MSGHEADLEN;
	free(json_string);
	((SYSHEAD *)sMsgBuf)->shMsgLen = nMsgLen;
	//HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "sMsgBuf->shMsgLen = %d", ((SYSHEAD *)sMsgBuf)->shMsgLen);
	nRet= MsqSnd (sMsgSrcId, gatSrvMsq, 0, nMsgLen, sMsgBuf);
	if (nRet)
	{
		HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "MsqSnd error, %d. %d[%s] ", nRet,errno,strerror(errno));
		return -1;
	}
	HtLog(	gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s end", sFuncName);
	return 0;
}
