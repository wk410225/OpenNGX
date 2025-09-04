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
    {"user_id", offsetof(sys_user_def, user_id), 'I', "",  "userId",""},
    {"dept_id", offsetof(sys_user_def, dept_id), 'I', "", "deptId", ""},
    {"user_name", offsetof(sys_user_def, user_name), 'C', "userName", "userName", ""},
    {"nick_name", offsetof(sys_user_def, nick_name), 'C', "", "nickName", ""},
    {"user_type", offsetof(sys_user_def, user_type), 'C', "", "", ""},
    {"email", offsetof(sys_user_def, email), 'C', "", "email", ""},
    {"phonenumber", offsetof(sys_user_def, phonenumber), 'C', "", "phonenumber", ""},
    {"sex", offsetof(sys_user_def, sex), 'C', "", "sex", ""},
    {"avatar", offsetof(sys_user_def, avatar), 'C', "", "avatar", ""},
    {"password", offsetof(sys_user_def, password), 'C', "", "", ""},
    {"status", offsetof(sys_user_def, status), 'C', "", "status", ""},
    {"del_flag", offsetof(sys_user_def, del_flag), 'C', "", "delFlag", ""},
    {"login_ip", offsetof(sys_user_def, login_ip), 'C', "", "loginIp", ""},
    {"login_date", offsetof(sys_user_def, login_date), 'C', "", "loginDate", ""},
    {"pwd_update_date", offsetof(sys_user_def, pwd_update_date), 'C', "", "pwdUpdateDate", ""},
    {"create_by", offsetof(sys_user_def, create_by), 'C', "", "createBy", ""},
    {"create_time", offsetof(sys_user_def, create_time), 'C', "", "createTime", ""},
    {"update_by", offsetof(sys_user_def, update_by), 'C', "", "updateBy", ""},
    {"update_time", offsetof(sys_user_def, update_time), 'C', "", "updateTime", ""},
    {"remark", offsetof(sys_user_def, remark), 'C', "", "remark", ""},
};


static TABFUN SYS_USER_INDMAP[MAXINDNUM]={
    {"user_id", offsetof(sys_user_def, user_id), 'I'},
};

static TABFUN SYS_DEPT_COLMAP[MAXCOLNUM]={
    {"dept_id", offsetof(sys_dept_def, dept_id), 'I', "", "deptId", ""},
    {"parent_id", offsetof(sys_dept_def, parent_id), 'I', "", "parentId", ""},
    {"ancestors", offsetof(sys_dept_def, ancestors), 'C', "", "ancestors", ""},
    {"dept_name", offsetof(sys_dept_def, dept_name), 'C', "", "deptName", ""},
    {"order_num", offsetof(sys_dept_def, order_num), 'I', "", "orderNum", ""},
    {"leader", offsetof(sys_dept_def, leader), 'C', "", "leader", ""},
    {"phone", offsetof(sys_dept_def, phone), 'C', "", "phone", ""},
    {"email", offsetof(sys_dept_def, email), 'C', "", "email", ""},
    {"status", offsetof(sys_dept_def, status), 'C', "", "status", ""},
    {"del_flag", offsetof(sys_dept_def, del_flag), 'C', "", "delFlag", ""},
    {"create_by", offsetof(sys_dept_def, create_by), 'C', "", "createBy", ""},
    {"create_time", offsetof(sys_dept_def, create_time), 'C', "", "createTime", ""},
    {"update_by", offsetof(sys_dept_def, update_by), 'C', "", "updateBy", ""},
    {"update_time", offsetof(sys_dept_def, update_time), 'C', "", "updateTime", ""},
};


static TABFUN SYS_DEPT_INDMAP[MAXINDNUM]={
    {"dept_id", offsetof(sys_dept_def, dept_id), 'I'},
};

static TABFUN SYS_ROLE_COLMAP[MAXCOLNUM]={
    {"role_id", offsetof(sys_role_def, role_id), 'I', "", "roleId"},
    {"role_name", offsetof(sys_role_def, role_name), 'C', "","roleName"},
    {"role_key", offsetof(sys_role_def, role_key), 'C', "", "roleKey"},
    {"role_sort", offsetof(sys_role_def, role_sort), 'I', "", "roleSort"},
    {"data_scope", offsetof(sys_role_def, data_scope), 'C', "", "dataScope"},
    {"menu_check_strictly", offsetof(sys_role_def, menu_check_strictly), 'I', "", "menuCheckStrictly"},
    {"dept_check_strictly", offsetof(sys_role_def, dept_check_strictly), 'I', "", "deptCheckStrictly"},
    {"status", offsetof(sys_role_def, status), 'C', "", "status"},
    {"del_flag", offsetof(sys_role_def, del_flag), 'C', "", "delFlag"},
    {"create_by", offsetof(sys_role_def, create_by), 'C', "", "createBy"},
    {"create_time", offsetof(sys_role_def, create_time), 'C', "", "createTime"},
    {"update_by", offsetof(sys_role_def, update_by), 'C', "", "updateBy"},
    {"update_time", offsetof(sys_role_def, update_time), 'C', "", "updateTime"},
    {"remark", offsetof(sys_role_def, remark), 'C', "", "remark"},
};

static TABFUN SYS_ROLE_INDMAP[MAXINDNUM]={
    {"role_id", offsetof(sys_role_def, role_id), 'I'},
};

int getInfo(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "getInfo";
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
	sys_dept_def	ptTblDept;
	sys_role_def	ptTblRole;

	memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
	memcpy(sTmpBuf, IPC, 6);
	char			*json_string = sMsgBuf+MSGHEADLEN+6+atoi(sTmpBuf);
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s begin", sFuncName);
	showIpc(IPC);
	GetIpcValueExt(IPC, "$MSGSRCID", sMsgSrcId, sizeof(sMsgSrcId));
	GetIpcValueExt(IPC, "$METHOD", sMethod, sizeof(sMethod));
	GetIpcValueExt(IPC, "$PATH", sUri, sizeof(sUri));
	cJSON      *root,  *user, *item, *array;

	int				user_id;
	if (validate_jwt_token(IPC, &user_id) != 0 )
	{
		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "jwt验证不过");
		cJSON_AddNumberToObject(root, "code", 401);
		json_string = cJSON_Print(root);
		memset(sMethod, 0x00, sizeof(sMethod));
	}
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%d user_id", user_id);

	if (strcmp(sMethod, "GET" ) == 0 )
	{
		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);
		// 创建 permissions 数组
		cJSON *permissions = cJSON_CreateArray();
		cJSON_AddItemToArray(permissions, cJSON_CreateString("*:*:*"));
		cJSON_AddItemToObject(root, "permissions", permissions); // 
		// 创建 roles 数组
		cJSON *roles = cJSON_CreateArray();
		cJSON_AddItemToArray(roles, cJSON_CreateString("admin"));
		cJSON_AddItemToObject(root, "roles", roles);

		cJSON_AddBoolToObject(root, "isDefaultModifyPwd", 0);
		cJSON_AddBoolToObject(root, "isPasswordExpired", 0);

		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "select *  from sys_user where user_id=%d", user_id);
		memset((char*)&ptTbl, 0x00, sizeof(ptTbl));
		nRet = mysql_ONE_SELECT(sSql, &ptTbl, SYS_USER_COLMAP);
		if(nRet ==0 ){ 
			//HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "sSql=[%s] error ",sSql);
			user = cJSON_CreateObject();
			NS2J(user, &ptTbl, SYS_USER_COLMAP);
			//cJSON_AddItemToObject(root, "user", user);
		}

		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "select *  from sys_dept where dept_id=%d", ptTbl.dept_id);
		memset((char*)&ptTblDept, 0x00, sizeof(ptTblDept));
		nRet = mysql_ONE_SELECT(sSql, &ptTblDept, SYS_DEPT_COLMAP);
		if(nRet ==0 ){ 
			cJSON *dept= cJSON_CreateObject();
			NS2J(dept, &ptTblDept, SYS_DEPT_COLMAP);
			cJSON_AddItemToObject(user, "dept", dept);
		}
		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "select *  from sys_role  where role_id = (select role_id from sys_user_role  where user_id=%d)", ptTbl.user_id);
		memset((char*)&ptTblRole, 0x00, sizeof(ptTblRole));
		nRet = mysql_ONE_SELECT(sSql, &ptTblRole, SYS_ROLE_COLMAP);
		if(nRet ==0 ){ 
			cJSON *role= cJSON_CreateObject();
			NS2J(role, &ptTblRole, SYS_ROLE_COLMAP);
			array= cJSON_CreateArray();
			cJSON_AddItemToArray(array, role);
			cJSON_AddItemToObject(user, "roles", array);
		}
		cJSON_AddNullToObject(root, "roleIds");
		cJSON_AddNullToObject(root, "postIds");
		cJSON_AddNullToObject(root, "roleId");
		cJSON_AddBoolToObject(root, "admin", 1);

		cJSON_AddItemToObject(root, "user", user);
		json_string = cJSON_Print(root);
	}
	HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);
	//head=cJSON_PrintUnformatted(root);
	cJSON_Delete(root);
	SetIpcValue(sSql, "$HEADNUM", "1");
	SetIpcValue(sSql, "$KEY0", "Content-Type");
    SetIpcValue(sSql, "$VAL0", "application/json;charset=utf-8");

    memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
    memcpy(sTmpBuf, sSql, 6);
    int nIpcLen = 6+atoi(sTmpBuf);

    memcpy(sMsgBuf+MSGHEADLEN, sSql, nIpcLen);
    strcpy(sMsgBuf+MSGHEADLEN+nIpcLen, json_string);
    int nMsgLen = strlen(json_string)+MSGHEADLEN+nIpcLen;
	//strcpy(sMsgBuf+MSGHEADLEN, json_string);
	//int nMsgLen = strlen(json_string)+MSGHEADLEN;
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
