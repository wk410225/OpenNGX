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
static TABFUN SYS_USER_POST_COLMAP[MAXCOLNUM]={
	{"user_id", offsetof(sys_user_post_def, user_id), 'I',            "user_id", "user_id", ""},
	{"post_id", offsetof(sys_user_post_def, post_id), 'I',            "post_id", "post_id", ""},
};
static TABFUN SYS_USER_ROLE_COLMAP[MAXCOLNUM]={
	{"user_id", offsetof(sys_user_role_def, user_id), 'I',            "user_id", "user_id", ""},
	{"role_id", offsetof(sys_user_role_def, role_id), 'I',            "role_id", "role_id", ""},
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

static TABFUN DEPTTREE_SYS_DEPT_COLMAP[MAXCOLNUM]={
	{"dept_id", offsetof(sys_dept_def, dept_id), 'I', "", "id", ""},
	{"parent_id", offsetof(sys_dept_def, parent_id), 'I', "", "", ""},
	{"ancestors", offsetof(sys_dept_def, ancestors), 'C', "", "", ""},
	{"dept_name", offsetof(sys_dept_def, dept_name), 'C', "", "label", ""},
	{"order_num", offsetof(sys_dept_def, order_num), 'I', "", "", ""},
	{"leader", offsetof(sys_dept_def, leader), 'C', "", "", ""},
	{"phone", offsetof(sys_dept_def, phone), 'C', "", "", ""},
	{"email", offsetof(sys_dept_def, email), 'C', "", "", ""},
	{"status", offsetof(sys_dept_def, status), 'C', "", "disabled", "B"},
	{"del_flag", offsetof(sys_dept_def, del_flag), 'C', "", "", ""},
	{"create_by", offsetof(sys_dept_def, create_by), 'C', "", "", ""},
	{"create_time", offsetof(sys_dept_def, create_time), 'C', "", "", ""},
	{"update_by", offsetof(sys_dept_def, update_by), 'C', "", "", ""},
	{"update_time", offsetof(sys_dept_def, update_time), 'C', "", "", ""},
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

static TABFUN SYS_USER_COLMAP[MAXCOLNUM]={
	{"user_id", offsetof(sys_user_def, user_id), 'I', "",  "userId",""},
	{"dept_id", offsetof(sys_user_def, dept_id), 'I', "deptId", "deptId", ""},
	{"user_name", offsetof(sys_user_def, user_name), 'C', "userName", "userName", ""},
	{"nick_name", offsetof(sys_user_def, nick_name), 'C', "nickName", "nickName", ""},
	{"user_type", offsetof(sys_user_def, user_type), 'C', "", "", ""},
	{"email", offsetof(sys_user_def, email), 'C', "email", "email", ""},
	{"phonenumber", offsetof(sys_user_def, phonenumber), 'C', "phonenumber", "phonenumber", ""},
	{"sex", offsetof(sys_user_def, sex), 'C', "sex", "sex", ""},
	{"avatar", offsetof(sys_user_def, avatar), 'C', "", "avatar", ""},
	{"password", offsetof(sys_user_def, password), 'C', "", "", ""},
	{"status", offsetof(sys_user_def, status), 'C', "status", "status", ""},
	{"del_flag", offsetof(sys_user_def, del_flag), 'C', "", "delFlag", ""},
	{"login_ip", offsetof(sys_user_def, login_ip), 'C', "", "loginIp", ""},
	{"login_date", offsetof(sys_user_def, login_date), 'C', "", "loginDate", ""},
	{"pwd_update_date", offsetof(sys_user_def, pwd_update_date), 'C', "", "pwdUpdateDate", ""},
	{"create_by", offsetof(sys_user_def, create_by), 'C', "", "createBy", ""},
	{"create_time", offsetof(sys_user_def, create_time), 'C', "", "createTime", ""},
	{"update_by", offsetof(sys_user_def, update_by), 'C', "", "updateBy", ""},
	{"update_time", offsetof(sys_user_def, update_time), 'C', "", "updateTime", ""},
	{"remark", offsetof(sys_user_def, remark), 'C', "remark", "remark", ""},
};

static TABFUN SYS_POST_COLMAP[MAXCOLNUM]={
	{"post_id", offsetof(sys_post_def, post_id), 'I',            "post_id", "postId", ""},
	{"post_code", offsetof(sys_post_def, post_code), 'C',        "post_code", "postCode", ""},
	{"post_name", offsetof(sys_post_def, post_name), 'C',        "post_name", "postName", ""},
	{"post_sort", offsetof(sys_post_def, post_sort), 'I',        "post_sort", "postSort", ""},
	{"status", offsetof(sys_post_def, status), 'C',              "status", "status", ""},
	{"create_by", offsetof(sys_post_def, create_by), 'C',        "create_by", "createBy", ""},
	{"create_time", offsetof(sys_post_def, create_time), 'C',    "create_time", "createTime", ""},
	{"update_by", offsetof(sys_post_def, update_by), 'C',        "update_by", "updateBy", ""},
	{"update_time", offsetof(sys_post_def, update_time), 'C',    "update_time", "updateTime", ""},
	{"remark", offsetof(sys_post_def, remark), 'C',				 "remark", "remark", ""},
};

int system_user(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "system_user";
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
		sprintf(sSql, "delete from sys_user where user_id= %d", atoi(param));
        HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", sSql);
		nRet = mysql_TBL_DELETE(sSql);
        if(nRet != 0)
        {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "delete error [%s]", sSql);
            return -1;
        }

		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "delete from sys_user_post where user_id= %d", atoi(param));
        HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", sSql);
		nRet = mysql_TBL_DELETE(sSql);
        if(nRet != 0)
        {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "delete error [%s]", sSql);
            return -1;
        }
		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "delete from sys_user_role where user_id= %d", atoi(param));
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
	else if (strcmp(sMethod, "POST" ) == 0  && strcmp(sUri, "/system/user/profile/avatar") == 0) //头像图片
	{
		char boundary[128] = {0};
		if (!multipart_parse_boundary(json_string, boundary, sizeof(boundary))) {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error multipart_parse_boundary_from_header");
            return -1;
		}
		char header[128];
		GetIpcValueExt(IPC, "$json_len", header, sizeof(header)); 	
		int n = atoi(header);

		GetIpcValueExt(IPC, "content-type", header, sizeof(header)); 	
		char boundary2[128] = {0};	
		if (!multipart_parse_boundary_from_header(header, boundary2, sizeof(boundary2))) {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error multipart_parse_boundary_from_header");
            return -1;
		}
		if(strcmp(boundary, boundary2) != 0)
		{
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error multipart_parse_boundary_from_header");
            return -1;
		}
		MultipartForm form = {0};
		MultipartCode code;
		
		code = multipart_parse_form(json_string, n, boundary, &form);
		if (code != MULTIPART_OK) {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error multipart_parse_boundary_from_header");
            return -1;
		}
		FileHeader* file;
		file = multipart_get_file(&form, "avatarfile");
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "file->filename = [%s]",  file->filename);
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "file->mimetype = [%s]",  file->mimetype);
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "file->field_name= [%s]", file->field_name);
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "file->size= [%d]", file->size);
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "file->offset= [%d]", file->offset);
		
		size_t num_files = 0;
		size_t* indices = multipart_get_files(&form, "file", &num_files);
		free(indices);
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "num_files = [%d]", num_files);

		// Save the file
		bool saved = multipart_save_file(form.files[0], json_string, "user.png");
		// Free data allocated in the form.
		multipart_free_form(&form);

        root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "msg", "操作成功");
        cJSON_AddNumberToObject(root, "code", 200);
        json_string = cJSON_Print(root);

	}
	else if (strcmp(sMethod, "POST" ) == 0  && strcmp(sUri, "/system/user/importData") == 0) //用户导入
	{
		char boundary[128] = {0};
		if (!multipart_parse_boundary(json_string, boundary, sizeof(boundary))) {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error multipart_parse_boundary_from_header");
            return -1;
		}
		char header[128];
		GetIpcValueExt(IPC, "$json_len", header, sizeof(header)); 	
		int n = atoi(header);

		GetIpcValueExt(IPC, "content-type", header, sizeof(header)); 	
		char boundary2[128] = {0};	
		if (!multipart_parse_boundary_from_header(header, boundary2, sizeof(boundary2))) {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error multipart_parse_boundary_from_header");
            return -1;
		}
		if(strcmp(boundary, boundary2) != 0)
		{
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error multipart_parse_boundary_from_header");
            return -1;
		}
		MultipartForm form = {0};
		MultipartCode code;
		
		code = multipart_parse_form(json_string, n, boundary, &form);
		if (code != MULTIPART_OK) {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error multipart_parse_boundary_from_header");
            return -1;
		}
		FileHeader *file = multipart_get_file(&form, "avatarfile");
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "file->filename = [%s]",  file->filename);
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "file->mimetype = [%s]",  file->mimetype);
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "file->field_name= [%s]", file->field_name);
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "file->size= [%d]", file->size);
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "file->offset= [%d]", file->offset);
		
		size_t num_files = 0;
		size_t* indices = multipart_get_files(&form, "file", &num_files);
		free(indices);
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "num_files = [%d]", num_files);

		// Save the file
		bool saved = multipart_save_file(form.files[0], json_string, "upload_user.xlsx");
		// Free data allocated in the form.
		multipart_free_form(&form);
/******************************************************************************************************/

		xlsxioreader xlsxioread;
		if ((xlsxioread = xlsxioread_open("upload_user.xlsx")) == NULL) {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error xlsxioreader ");
            return -1;
		}
		//list available sheets
		xlsxioreadersheetlist sheetlist;
		const XLSXIOCHAR* sheetname;
        //HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Available sheets");
		if ((sheetlist = xlsxioread_sheetlist_open(xlsxioread)) != NULL) {
			while ((sheetname = xlsxioread_sheetlist_next(sheetlist)) != NULL) {
				//XML_Char_printf(X(" - %s\n"), sheetname);
        		HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "%s", sheetname);
			}
			xlsxioread_sheetlist_close(sheetlist);
		}
		//read values from first sheet
		XLSXIOCHAR* value;
       	HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Contents of first sheet:");
		xlsxioreadersheet sheet = xlsxioread_sheet_open(xlsxioread, NULL, XLSXIOREAD_SKIP_EMPTY_ROWS);
		while (xlsxioread_sheet_next_row(sheet)) {
			while ((value = xlsxioread_sheet_next_cell(sheet)) != NULL) {
				//XML_Char_printf(X("%s\t"), value);
        		HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "%s\t", value);
				xlsxioread_free(value);
			}
        	HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "");
		}
		xlsxioread_sheet_close(sheet);
		xlsxioread_close(xlsxioread);

        root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "msg", "操作成功");
        cJSON_AddNumberToObject(root, "code", 200);
        json_string = cJSON_Print(root);
	}
	else if (strcmp(sMethod, "POST" ) == 0  && strcmp(sUri, "/system/user/export") == 0) //用户导出
	{
		xlsxiowriter handle;
		if ((handle = xlsxiowrite_open("user.xlsx", "USER")) == NULL) 
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "xlsxiowrite_open error ");
			return -1;
		}
		//set row height
		xlsxiowrite_set_row_height(handle, 1);
		//how many rows to buffer to detect column widths
		xlsxiowrite_set_detection_rows(handle, 11);
		//write column names
		xlsxiowrite_add_column( handle,"用户序号", 	0);
		xlsxiowrite_add_column( handle,"登录名称",  0);
		xlsxiowrite_add_column( handle,"用户名称",  0);
		xlsxiowrite_add_column( handle,"用户邮箱",  0);
		xlsxiowrite_add_column( handle,"手机号码",  0);
		xlsxiowrite_add_column( handle,"用户性别", 0);
		xlsxiowrite_add_column( handle,"账号状态", 0);
		xlsxiowrite_add_column( handle,"最后登录IP", 0);
		xlsxiowrite_add_column( handle,"最后登录时间", 0);
		xlsxiowrite_add_column( handle,"部门名称", 0);
		xlsxiowrite_add_column( handle, "部门负责人", 0);
		xlsxiowrite_next_row(handle);


		sys_user_def	ptTblUser;
		sys_dept_def	ptTblDept;

		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		memset(sSql,  0x00, sizeof(sSql));
		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;
		parray = cJSON_CreateArray();
		memset(sSql,  0x00, sizeof(sSql));
		strcpy(sSql, "select * from sys_user ");
		NGetQueryWhere(IPC, sSql,  SYS_USER_COLMAP);
		NGetQuerySortLimit(IPC, sSql );
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		int rownum = 1;
		int colnum = 1;
		while(1)
		{
			memset((char *)&ptTblUser, 0x00, sizeof(ptTblUser));
			nRet = mysql_TBL_FETCH(NULL, &ptTblUser, SYS_USER_COLMAP);
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
				PrintTbl(&ptTblUser, SYS_USER_COLMAP);
				//pdata = cJSON_CreateObject();
				//NS2J(pdata, &ptTblUser, SYS_USER_COLMAP);
				memset(sSql,  0x00, sizeof(sSql));
				sprintf(sSql, "select *  from sys_dept  where dept_id= %d ", ptTblUser.dept_id);
				nRet = mysql_ONE_SELECT(sSql, &ptTblDept, SYS_DEPT_COLMAP);
				if(nRet ==0 ){
					PrintTbl(&ptTblDept, SYS_DEPT_COLMAP);
				}
				xlsxiowrite_add_cell_int(handle, ptTblUser.user_id);
				xlsxiowrite_add_cell_string(handle, 	 ptTblUser.user_name);
				xlsxiowrite_add_cell_string(handle,   ptTblUser.nick_name);
				xlsxiowrite_add_cell_string(handle,   ptTblUser.email);
				xlsxiowrite_add_cell_string(handle,   ptTblUser.phonenumber);
				xlsxiowrite_add_cell_string(handle,   ptTblUser.sex);
				xlsxiowrite_add_cell_string(handle,   ptTblUser.status);
				xlsxiowrite_add_cell_string(handle,   ptTblUser.login_ip);
				xlsxiowrite_add_cell_string(handle,   ptTblUser.login_date);
				xlsxiowrite_add_cell_string(handle,   ptTblDept.dept_name);
				xlsxiowrite_add_cell_string(handle,    ptTblDept.leader);
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
		SetIpcValue(sSql, "$VAL1", "attachment; filename=user.xlsx");


		memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
		memcpy(sTmpBuf, sSql, 6);
		int nIpcLen = 6+atoi(sTmpBuf);
		memcpy(sMsgBuf+MSGHEADLEN, sSql, nIpcLen);

		FILE *file = fopen("user.xlsx", "rb");
		int nFileLen = fread(sMsgBuf+MSGHEADLEN+nIpcLen, 1, 8192, file); 
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
	else if (strcmp(sMethod, "POST" ) == 0  && strcmp(sUri, "/system/user") == 0) //新增用户
	{
		TABFUN I_SYS_USER_COLMAP[MAXCOLNUM]={
			{"user_id",		offsetof(sys_user_def, user_id), 'I', "",  "userId",""},
			{"dept_id",		offsetof(sys_user_def, dept_id), 'I', "deptId", "dept_id", ""},
			{"user_name", offsetof(sys_user_def, user_name), 'C', "userName", "userName", ""},
			{"nick_name", offsetof(sys_user_def, nick_name), 'C', "nickName", "nickName", ""},
			{"user_type", offsetof(sys_user_def, user_type), 'C', "", "", ""},
			{"email", offsetof(sys_user_def, email), 'C', "email", "email", ""},
			{"phonenumber", offsetof(sys_user_def, phonenumber), 'C', "phonenumber", "phonenumber", ""},
			{"sex", offsetof(sys_user_def, sex), 'C', "sex", "sex", ""},
			{"avatar", offsetof(sys_user_def, avatar), 'C', "", "avatar", ""},
			{"password", offsetof(sys_user_def, password), 'C', "", "", ""},
			{"status", offsetof(sys_user_def, status), 'C', "status", "status", ""},
			{"del_flag", offsetof(sys_user_def, del_flag), 'C', "", "delFlag", ""},
			{"login_ip", offsetof(sys_user_def, login_ip), 'C', "", "loginIp", ""},
			{"login_date", offsetof(sys_user_def, login_date), 'T', "", "loginDate", ""},
			{"pwd_update_date", offsetof(sys_user_def, pwd_update_date), 'T', "", "pwdUpdateDate", ""},
			{"create_by", offsetof(sys_user_def, create_by), 'C', "", "createBy", ""},
			{"create_time", offsetof(sys_user_def, create_time), 'T', "", "createTime", ""},
			{"update_by", offsetof(sys_user_def, update_by), 'C', "", "updateBy", ""},
			{"update_time", offsetof(sys_user_def, update_time), 'T', "", "updateTime", ""},
			{"remark", offsetof(sys_user_def, remark), 'C', "remark", "remark", ""},
		};
		sys_user_def	ptTbl;
		sys_user_def	ptTblUser;
		root = cJSON_Parse(json_string);
        if (root == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "Error before: %s", error_ptr);
            }
            return -1;
        }
        json_string = cJSON_Print(root);

		sprintf(sSql, "select  * from sys_user where user_id= %d", user_id);
		
        memset((char *)&ptTblUser, 0x00, sizeof(ptTblUser));
		nRet = mysql_ONE_SELECT(sSql, &ptTblUser, SYS_USER_COLMAP);

		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);
        HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "\n%s", json_string);
        memset((char *)&ptTbl, 0x00, sizeof(ptTbl));
        NJ2S(root, &ptTbl, SYS_USER_COLMAP);
		strcpy(ptTbl.user_type, "00");
		strcpy(ptTbl.login_ip, "127.0.0.1");
		strcpy(ptTbl.create_by, ptTblUser.user_name);
		GetCurrTime(ptTbl.create_time);
		item = cJSON_GetObjectItemCaseSensitive(root, "password");
		hash_password(item->valuestring, ptTbl.password);
        PrintTbl(&ptTbl, SYS_USER_COLMAP);
        nRet = mysql_TBL_INSERT("sys_user", &ptTbl, I_SYS_USER_COLMAP);
        if(nRet != 0)
        {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "insert error nRet=[%d]", nRet);
            return -1;
        }
		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "select  * from sys_user where user_name = '%s'", ptTbl.user_name);
		nRet = mysql_ONE_SELECT(sSql, &ptTbl, SYS_USER_COLMAP);

		sys_user_post_def	ptTblUserPost;
		cJSON *postIds = cJSON_GetObjectItemCaseSensitive(root, "postIds");
		if (cJSON_IsArray(postIds)) 
		{
			cJSON *item = NULL;
			cJSON_ArrayForEach(item, postIds) {
				memset((char *)&ptTblUserPost, 0x00, sizeof(ptTblUserPost));
				if (cJSON_IsNumber(item)) {
					ptTblUserPost.post_id = item->valueint;
					ptTblUserPost.user_id = ptTbl.user_id;
					nRet = mysql_TBL_INSERT("sys_user_post", &ptTblUserPost, SYS_USER_POST_COLMAP);
				}
			}
		}
		sys_user_role_def	ptTblUserRole;
		cJSON *roleIds = cJSON_GetObjectItemCaseSensitive(root, "roleIds");
		if (cJSON_IsArray(roleIds)) 
		{
			cJSON *item = NULL;
			cJSON_ArrayForEach(item, roleIds) {
				memset((char *)&ptTblUserRole, 0x00, sizeof(ptTblUserRole));
				if (cJSON_IsNumber(item)) {
					ptTblUserRole.role_id = item->valueint;
					ptTblUserRole.user_id = ptTbl.user_id;
					nRet = mysql_TBL_INSERT("sys_user_role", &ptTblUserPost, SYS_USER_ROLE_COLMAP);
				}
			}
		}
        DbsCommit();
	}
	else if (strcmp(sMethod, "GET" ) == 0 && memcmp(sUri, "/system/user/authRole/", strlen("/system/user/authRole/")) == 0 ) //分配角色
	{
		HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", sUri);
    	sys_user_def    ptTbl;
    	sys_dept_def    ptTblDept;
    	sys_role_def    ptTblRole;

		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;

		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select *  from sys_user where user_id=%d", atoi(param));
        memset((char*)&ptTbl, 0x00, sizeof(ptTbl));
        nRet = mysql_ONE_SELECT(sSql, &ptTbl, SYS_USER_COLMAP);
        if(nRet !=0 ){
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error [%s]", sSql);
			return -1;
        }
        cJSON *user = cJSON_CreateObject();
        NS2J(user, &ptTbl, SYS_USER_COLMAP);

        memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select *  from sys_dept where dept_id=%d", ptTbl.dept_id);
        memset((char*)&ptTblDept, 0x00, sizeof(ptTblDept));
        nRet = mysql_ONE_SELECT(sSql, &ptTblDept, SYS_DEPT_COLMAP);
        if(nRet !=0 ){
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error [%s]", sSql);
			return -1;
        }
        cJSON *dept= cJSON_CreateObject();
        NS2J(dept, &ptTblDept, SYS_DEPT_COLMAP);
        cJSON_AddItemToObject(user, "dept", dept);

        memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select *  from sys_role  where status ='0' and role_id in (select  GROUP_CONCAT(role_id) from sys_user_role  where user_id=%d )", ptTbl.user_id);
		HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", sSql);
		mysql_TBL_CURSOR(sSql, NULL);
        mysql_TBL_OPEN(sSql, NULL);
        parray= cJSON_CreateArray();
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
                break;
            }
            else
            {
                pdata = cJSON_CreateObject();
                NS2J(pdata, &ptTblRole, SYS_ROLE_COLMAP);
                cJSON_AddItemToArray(parray, pdata);
            }
        }
		cJSON_AddItemToObject(user, "roles", parray); 


		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select * from sys_role  where role_id != 1 and status='0'");//剔除admin
		HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", sSql);
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		parray= cJSON_CreateArray();
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
				break;
			}
			else
			{
				pdata = cJSON_CreateObject();
                NS2J(pdata, &ptTblRole, SYS_ROLE_COLMAP);
				cJSON_AddItemToArray(parray, pdata);
			}
		}
		cJSON_AddItemToObject(root, "roles", parray); 
		cJSON_AddItemToObject(root, "user", user); 
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string[%s]", cJSON_Print(root));
	}
	else if (strcmp(sMethod, "GET" ) == 0  && strlen(param) == 0) //获取  ??
	{
    	sys_role_def    ptTblRole;
    	sys_post_def    ptTblPost;

		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;

		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select * from sys_role  where role_id != 1 and status='0'");//剔除admin
		HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", sSql);
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		parray= cJSON_CreateArray();
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
				break;
			}
			else
			{
				pdata = cJSON_CreateObject();
                NS2J(pdata, &ptTblRole, SYS_ROLE_COLMAP);
				cJSON_AddItemToArray(parray, pdata);
			}
		}
		cJSON_AddItemToObject(root, "roles", parray); 

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select * from sys_post  where status = '0' order by post_sort");
		HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", sSql);
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		parray= cJSON_CreateArray();
		while(1)
		{
			memset((char *)&ptTblPost, 0x00, sizeof(ptTblPost));
			nRet = mysql_TBL_FETCH(NULL, &ptTblPost, SYS_POST_COLMAP);
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
                NS2J(pdata, &ptTblPost, SYS_POST_COLMAP);
				cJSON_AddItemToArray(parray, pdata);
			}
		}
		cJSON_AddItemToObject(root, "posts", parray); 
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string[%s]", cJSON_Print(root));
	}
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(param, "deptTree") == 0) //获取部men
	{
		sys_dept_def	ptTbl;
		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);
		if(user_id == 1) //admin
		{
			memset(sSql,  0x00, sizeof(sSql));
			sprintf(sSql, "select  * from sys_dept where parent_id=0 and status = '0'");
			memset((char*)&ptTbl, 0x00, sizeof(ptTbl));
			nRet = mysql_ONE_SELECT(sSql, &ptTbl, DEPTTREE_SYS_DEPT_COLMAP);
			if(nRet !=0 ){ 
				HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "error %s", sSql);
				return -1;
			}
			cJSON *data = cJSON_CreateObject();
			array= cJSON_CreateArray();
			NS2J(data, &ptTbl, DEPTTREE_SYS_DEPT_COLMAP);

			memset(sSql,  0x00, sizeof(sSql));
			sprintf(sSql, "select *  from sys_dept  where status='0' and  parent_id= %d order by order_num asc", ptTbl.dept_id);
			HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", sSql);
			mysql_TBL_CURSOR(sSql, NULL);
			mysql_TBL_OPEN(sSql, NULL);
			cJSON *pdata = NULL, *cdata = NULL;
			cJSON *parray = NULL, *carray = NULL;
			cJSON *pmeta= NULL, *cmeta= NULL;
			parray= cJSON_CreateArray();
			while(1)
			{
				memset((char *)&ptTbl, 0x00, sizeof(ptTbl));
				nRet = mysql_TBL_FETCH(NULL, &ptTbl, DEPTTREE_SYS_DEPT_COLMAP);
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
					NS2J(pdata, &ptTbl, DEPTTREE_SYS_DEPT_COLMAP);

					carray= cJSON_CreateArray();

					memset(sSql,  0x00, sizeof(sSql));
					sprintf(sSql, "select *  from sys_dept  where status='0' and  parent_id= %d order by order_num asc", ptTbl.dept_id);
					mysql_TBL_CURSOR1(sSql, NULL);
					mysql_TBL_OPEN1(sSql, NULL);
					while(1)
					{
						memset((char *)&ptTblC, 0x00, sizeof(ptTblC));
						nRet = mysql_TBL_FETCH1(NULL, &ptTblC, DEPTTREE_SYS_DEPT_COLMAP);
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
							NS2J(cdata, &ptTblC, DEPTTREE_SYS_DEPT_COLMAP);
							cJSON_AddItemToArray(carray, cdata);
							//HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "[%s]", cJSON_Print(carray));
						}
					}
					cJSON_AddItemToObject(pdata, "children", carray); 
					cJSON_AddItemToArray(parray, pdata);
					//HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "parray[%s]", cJSON_Print(parray));
				}
				//HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "array[%s]", cJSON_Print(array));
			}
			cJSON_AddItemToObject(data, "children", parray); 
			cJSON_AddItemToArray(array, data);
			cJSON_AddItemToObject(root, "data", array); 
		}
		else
		{
			sys_dept_def	ptTblDept;
			cJSON *pdata = NULL, *cdata = NULL;
			cJSON *parray = NULL, *carray = NULL;
		

			memset(sSql,  0x00, sizeof(sSql));
			sprintf(sSql, "select  * from sys_dept where parent_id=0 and status = '0'"); //最顶级部门
			memset((char*)&ptTbl, 0x00, sizeof(ptTbl));
			nRet = mysql_ONE_SELECT(sSql, &ptTbl, DEPTTREE_SYS_DEPT_COLMAP);
			if(nRet !=0 ){ 
				HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "error %s", sSql);
				return -1;
			}
			cJSON *data = cJSON_CreateObject();
			array= cJSON_CreateArray();
			NS2J(data, &ptTbl, DEPTTREE_SYS_DEPT_COLMAP);

			memset(sSql,  0x00, sizeof(sSql));
			sprintf(sSql, "select * from sys_dept where status='0' and  dept_id in (select  dept_id from sys_user where  user_id = %d )", user_id); //当前用户部门
			memset((char*)&ptTbl, 0x00, sizeof(ptTbl));
			nRet = mysql_ONE_SELECT(sSql, &ptTbl, DEPTTREE_SYS_DEPT_COLMAP);
			if(nRet !=0 ){ 
				HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "error %s", sSql);
				return -1;
			}
			cdata = cJSON_CreateObject();
			carray= cJSON_CreateArray();
			NS2J(cdata, &ptTbl, DEPTTREE_SYS_DEPT_COLMAP);
			cJSON_AddItemToArray(carray, cdata);

			memset(sSql,  0x00, sizeof(sSql));
			sprintf(sSql, "select *  from sys_dept  where status='0' and  dept_id= %d ", ptTbl.parent_id); //当前用户父部门
			memset((char*)&ptTbl, 0x00, sizeof(ptTbl));
			nRet = mysql_ONE_SELECT(sSql, &ptTbl, DEPTTREE_SYS_DEPT_COLMAP);
			if(nRet !=0 ){ 
				HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "error %s", sSql);
				return -1;
			}
			pdata = cJSON_CreateObject();
			parray= cJSON_CreateArray();
			NS2J(pdata, &ptTbl, DEPTTREE_SYS_DEPT_COLMAP);

			cJSON_AddItemToObject(pdata, "children", carray); 
			cJSON_AddItemToArray(parray, pdata);
			cJSON_AddItemToObject(data, "children", parray); 
			cJSON_AddItemToArray(array, data);
			cJSON_AddItemToObject(root, "data", array); 
		}
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string[%s]", cJSON_Print(root));
	}
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(param, "list") == 0) //获取用户列表
	{
		sys_user_def	ptTblUser;
		sys_dept_def	ptTblDept;

		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		memset(sSql,  0x00, sizeof(sSql));
		//sprintf(sSql, "select count(1) as total_rows from sys_user where status='0'");
		sprintf(sSql, "select count(1) as total_rows from sys_user ");
		NGetQueryWhere(IPC, sSql,  SYS_USER_COLMAP);
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
		//strcpy(sSql, "select * from sys_user where status='0' ");
		strcpy(sSql, "select * from sys_user ");
		NGetQueryWhere(IPC, sSql,  SYS_USER_COLMAP);
		NGetQuerySortLimit(IPC, sSql );
		mysql_TBL_CURSOR(sSql, NULL);
		mysql_TBL_OPEN(sSql, NULL);
		while(1)
		{
			memset((char *)&ptTblUser, 0x00, sizeof(ptTblUser));
			nRet = mysql_TBL_FETCH(NULL, &ptTblUser, SYS_USER_COLMAP);
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
				PrintTbl(&ptTblUser, SYS_USER_COLMAP);
				pdata = cJSON_CreateObject();
				NS2J(pdata, &ptTblUser, SYS_USER_COLMAP);
				memset(sSql,  0x00, sizeof(sSql));
				sprintf(sSql, "select *  from sys_dept  where dept_id= %d ", ptTblUser.dept_id);
				nRet = mysql_ONE_SELECT(sSql, &ptTblDept, SYS_DEPT_COLMAP);
				if(nRet ==0 ){
					PrintTbl(&ptTblDept, SYS_DEPT_COLMAP);
					cdata = cJSON_CreateObject();
					NS2J(cdata, &ptTblDept, SYS_DEPT_COLMAP);
					cJSON_AddItemToObject(pdata, "dept", cdata);
				}
				
				if(strcmp(ptTblUser.user_name, "admin") == 0)
					cJSON_AddBoolToObject(pdata, "admin", 1);
				else
					cJSON_AddBoolToObject(pdata, "admin", 0);
				cJSON_AddNullToObject(pdata, "roleIds");
				cJSON_AddNullToObject(pdata, "postIds");
				cJSON_AddNullToObject(pdata, "roleId");
				cJSON_AddItemToArray(parray, pdata);
			}
		}
		cJSON_AddItemToObject(root, "rows", parray);
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);
	}
	else if (strcmp(sMethod, "PUT" ) == 0  && strcmp(param, "changeStatus") == 0) //修改用户状态
	{
		TABFUN SYS_USER_COLMAP[MAXCOLNUM]={
			{"user_id", offsetof(sys_user_def, user_id), 'I', "userId",  "",""},
			{"status", offsetof(sys_user_def, status), 'C', "status", "", ""},
		};
		TABFUN SYS_USER_INDMAP[MAXCOLNUM]={
			{"user_id", offsetof(sys_user_def, user_id), 'I', "userId",  "",""},
		};
		sys_user_def	ptTbl;
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
        NJ2S(root, &ptTbl, SYS_USER_COLMAP);
        PrintTbl(&ptTbl, SYS_USER_COLMAP);
        memset(sSql,  0x00, sizeof(sSql));
        memset(set, 0x00, sizeof(set));
        memset(where, 0x00, sizeof(where));
        NGetUpdateSetSql(root, set, SYS_USER_COLMAP);
        NGetUpdateWhereSql(root, where, SYS_USER_INDMAP);
        sprintf(sSql, "update sys_user  set %s where %s;", set, where);
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
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(param, "profile") == 0)   //个人中心 当前用户的信息
	{
		TABFUN SYS_POST_COLMAP[MAXCOLNUM]={
			{"post_id", offsetof(sys_post_def, post_id), 'I',         "post_id", "post_id", ""},
			{"post_code", offsetof(sys_post_def, post_code), 'C',     "post_code", "post_code", ""},
			{"post_name", offsetof(sys_post_def, post_name), 'C',     "post_name", "post_name", ""},
			{"post_sort", offsetof(sys_post_def, post_sort), 'I',      "post_sort", "post_sort", ""},
			{"status", offsetof(sys_post_def, status), 'C',            "status", "status", ""},
			{"create_by", offsetof(sys_post_def, create_by), 'C',      "create_by", "create_by", ""},
			{"create_time", offsetof(sys_post_def, create_time), 'C',  "create_time", "create_time", ""},
			{"update_by", offsetof(sys_post_def, update_by), 'C',      "update_by", "update_by", ""},
			{"update_time", offsetof(sys_post_def, update_time), 'C',  "update_time", "update_time", ""},
			{"remark", offsetof(sys_post_def, remark), 'C',            "remark", "remark", ""},
		};

		cJSON      *user, *dept, *role;
		sys_user_def    ptTbl;
    	sys_dept_def    ptTblDept;
    	sys_role_def    ptTblRole;
    	sys_post_def    ptTblPost;

		root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "msg", "操作成功");
        cJSON_AddNumberToObject(root, "code", 200);

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select *  from sys_user where user_id=%d", user_id);
        memset((char*)&ptTbl, 0x00, sizeof(ptTbl));
        nRet = mysql_ONE_SELECT(sSql, &ptTbl, SYS_USER_COLMAP);
        if(nRet ==0 ){ 
            user = cJSON_CreateObject();
            NS2J(user, &ptTbl, SYS_USER_COLMAP);
        }

        memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select *  from sys_dept where dept_id=%d", ptTbl.dept_id);
        memset((char*)&ptTblDept, 0x00, sizeof(ptTblDept));
        nRet = mysql_ONE_SELECT(sSql, &ptTblDept, SYS_DEPT_COLMAP);
        if(nRet ==0 ){
            dept= cJSON_CreateObject();
            NS2J(dept, &ptTblDept, SYS_DEPT_COLMAP);
            cJSON_AddItemToObject(user, "dept", dept);
        }
        memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select *  from sys_role  where role_id = (select role_id from sys_user_role  where user_id=%d)", ptTbl.user_id);
        memset((char*)&ptTblRole, 0x00, sizeof(ptTblRole));
        nRet = mysql_ONE_SELECT(sSql, &ptTblRole, SYS_ROLE_COLMAP);
        if(nRet ==0 ){
            role= cJSON_CreateObject();
            NS2J(role, &ptTblRole, SYS_ROLE_COLMAP);
            array= cJSON_CreateArray();
            cJSON_AddItemToArray(array, role);
            cJSON_AddItemToObject(user, "roles", array);
        }
        memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select post_name  from sys_post  where post_id= (select post_id from sys_user_post  where user_id=%d)", ptTbl.user_id);
        memset((char*)&ptTblPost, 0x00, sizeof(ptTblPost));
        nRet = mysql_ONE_SELECT(sSql, &ptTblPost, SYS_POST_COLMAP);
        if(nRet ==0 ){
			cJSON_AddStringToObject(root, "postGroup", ptTblPost.post_name);
		}
		cJSON_AddItemToObject(root, "data", user);
		cJSON_AddNullToObject(root, "roleIds");
        cJSON_AddNullToObject(root, "postIds");
        cJSON_AddNullToObject(root, "roleId");
        cJSON_AddStringToObject(root, "roleGroup", ptTblRole.role_name);
		if (ptTbl.user_id == 1)
			cJSON_AddBoolToObject(root, "admin", 1);
		else 
			cJSON_AddBoolToObject(root, "admin", 0);
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);
	}
	else if ( strcmp(sMethod, "GET" ) == 0 && atoi(param) > 0 ) //修改页面查询 被修改用户信息
	{
		cJSON      *user, *dept, *role;
		sys_user_def    ptTbl;
    	sys_dept_def    ptTblDept;
    	sys_role_def    ptTblRole;
    	sys_user_role_def    ptTblUserRole;
    	sys_post_def    ptTblPost;
    	sys_user_post_def    ptTblUserPost;

		root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "msg", "操作成功");
        cJSON_AddNumberToObject(root, "code", 200);

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select *  from sys_user where user_id=%d", atoi(param));
        memset((char*)&ptTbl, 0x00, sizeof(ptTbl));
        nRet = mysql_ONE_SELECT(sSql, &ptTbl, SYS_USER_COLMAP);
        if(nRet != 0 ){ 
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error [%s]", sSql);
            return -1; 
        }
        user = cJSON_CreateObject();
        NS2J(user, &ptTbl, SYS_USER_COLMAP);

        memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select *  from sys_dept where dept_id=%d", ptTbl.dept_id);
        memset((char*)&ptTblDept, 0x00, sizeof(ptTblDept));
        nRet = mysql_ONE_SELECT(sSql, &ptTblDept, SYS_DEPT_COLMAP);
        if(nRet != 0 ){
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error [%s]", sSql);
            return -1; 
        }
        dept= cJSON_CreateObject();
        NS2J(dept, &ptTblDept, SYS_DEPT_COLMAP);
        cJSON_AddItemToObject(user, "dept", dept);

        memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select role_id from sys_user_role  where user_id=%d", ptTbl.user_id);
		mysql_TBL_CURSOR(sSql, NULL);
        mysql_TBL_OPEN(sSql, NULL);
		cJSON *roleIds = cJSON_CreateArray();
		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;
		parray= cJSON_CreateArray();
        while(1)
        {   
			memset((char*)&ptTblUserRole, 0x00, sizeof(ptTblUserRole));
            nRet = mysql_TBL_FETCH(NULL, &ptTblUserRole, SYS_USER_ROLE_COLMAP);
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
				cJSON_AddItemToArray(roleIds, cJSON_CreateNumber(ptTblUserRole.role_id));
				memset(sSql,  0x00, sizeof(sSql));
				sprintf(sSql, "select * from sys_role  where role_id = %d  and status='0'", ptTblUserRole.role_id);
				HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", sSql);
				
				nRet = mysql_ONE_SELECT(sSql, &ptTblRole, SYS_ROLE_COLMAP);
				if(nRet != 0 ){ 
					HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error [%s]", sSql);
					return -1; 
				}
				pdata = cJSON_CreateObject();
				NS2J(pdata, &ptTblRole, SYS_ROLE_COLMAP);
				cJSON_AddItemToArray(parray, pdata);
            }   
        }   
		cJSON_AddItemToObject(user, "roles", parray);
		cJSON_AddItemToObject(root, "roleIds", roleIds);

        memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select post_id from sys_user_post  where user_id=%d", ptTbl.user_id);
		mysql_TBL_CURSOR(sSql, NULL);
        mysql_TBL_OPEN(sSql, NULL);
		cJSON *postIds = cJSON_CreateArray();
		parray = cJSON_CreateArray();
        while(1)
        {   
			memset((char*)&ptTblUserPost, 0x00, sizeof(ptTblUserPost));
            nRet = mysql_TBL_FETCH(NULL, &ptTblUserPost, SYS_USER_POST_COLMAP);
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
				cJSON_AddItemToArray(postIds, cJSON_CreateNumber(ptTblUserPost.post_id));
				memset(sSql,  0x00, sizeof(sSql));
				sprintf(sSql, "select * from sys_post  where post_id =%d and status='0'", ptTblUserPost.post_id);
				HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", sSql);
				nRet = mysql_ONE_SELECT(sSql, &ptTblPost, SYS_POST_COLMAP);
				if(nRet != 0 ){ 
					HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error [%s]", sSql);
					return -1; 
				}
				pdata = cJSON_CreateObject();
				NS2J(pdata, &ptTblPost, SYS_POST_COLMAP);
				cJSON_AddItemToArray(parray, pdata);
            }   
        }   
		cJSON_AddItemToObject(user, "posts", parray);
		cJSON_AddItemToObject(root, "postIds", postIds);
		cJSON_AddItemToObject(root, "data", user);

        memset(sSql,  0x00, sizeof(sSql));
		parray = cJSON_CreateArray();
        sprintf(sSql, "select * from sys_role  where status='0' and role_id > 1 order by role_sort");
		mysql_TBL_CURSOR(sSql, NULL);
        mysql_TBL_OPEN(sSql, NULL);
        while(1)
        {   
			memset((char*)&ptTblRole, 0x00, sizeof(ptTblRole));
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
				pdata = cJSON_CreateObject();
				NS2J(pdata, &ptTblRole, SYS_ROLE_COLMAP);
				cJSON_AddItemToArray(parray, pdata);
			}
		}
		cJSON_AddItemToObject(root, "roles", parray);

        memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select * from sys_post  where status='0' order by post_sort");
		mysql_TBL_CURSOR(sSql, NULL);
        mysql_TBL_OPEN(sSql, NULL);
		parray = cJSON_CreateArray();
        while(1)
        {   
			memset((char*)&ptTblPost, 0x00, sizeof(ptTblPost));
            nRet = mysql_TBL_FETCH(NULL, &ptTblPost, SYS_POST_COLMAP);
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
				pdata = cJSON_CreateObject();
				NS2J(pdata, &ptTblPost, SYS_POST_COLMAP);
				cJSON_AddItemToArray(parray, pdata);
			}
		}
		cJSON_AddItemToObject(root, "posts", parray);
        json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);
	}
	else if (strcmp(sMethod, "PUT" ) == 0  && strcmp(param, "profile") == 0)  //个人中心修改当前登录人信息
	{
		TABFUN SYS_USER_COLMAP[MAXCOLNUM]={
			{"user_name", offsetof(sys_user_def, user_name), 'C', "userName", "userName", ""},
			{"nick_name", offsetof(sys_user_def, nick_name), 'C', "nickName", "nickName", ""},
			{"email", offsetof(sys_user_def, email), 'C', "email", "email", ""},
			{"phonenumber", offsetof(sys_user_def, phonenumber), 'C', "phonenumber", "phonenumber", ""},
			{"sex", offsetof(sys_user_def, sex), 'C', "sex", "sex", ""},
		};
		sys_user_def	ptTbl;
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
        NJ2S(root, &ptTbl, SYS_USER_COLMAP);
        PrintTbl(&ptTbl, SYS_USER_COLMAP);
        memset(sSql,  0x00, sizeof(sSql));
        memset(set, 0x00, sizeof(set));
        memset(where, 0x00, sizeof(where));
        NGetUpdateSetSql(root, set, SYS_USER_COLMAP);
        //NGetUpdateWhereSql(root, where, SYS_USER_INDMAP);
        sprintf(sSql, "update sys_user  set %s where user_id=%d;", set, user_id);
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "update sql=[%s]", sSql);
        if(strlen(set) > 0)
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
	else if (strcmp(sMethod, "PUT" ) == 0 && strcmp(sUri, "/system/user") == 0 ) //修改其他用户信息 
	{
		TABFUN SYS_USER_COLMAP[MAXCOLNUM]={
			{"user_id", offsetof(sys_user_def, user_id), 'I', "userId",  "",""},
			{"user_name", offsetof(sys_user_def, user_name), 'C', "userName", "userName", ""},
			{"dept_id", offsetof(sys_user_def, user_name), 'I', "deptId", "deptId", ""},
			{"nick_name", offsetof(sys_user_def, nick_name), 'C', "nickName", "nickName", ""},
			{"status", offsetof(sys_user_def, nick_name), 'C', "status", "status", ""},
			{"email", offsetof(sys_user_def, email), 'C', "email", "email", ""},
			{"phonenumber", offsetof(sys_user_def, phonenumber), 'C', "phonenumber", "phonenumber", ""},
			{"sex", offsetof(sys_user_def, sex), 'C', "sex", "sex", ""},
			{"remark", offsetof(sys_user_def, sex), 'C', "remark", "remark", ""},
		};
		TABFUN SYS_USER_INDMAP[MAXCOLNUM]={
			{"user_id", offsetof(sys_user_def, user_id), 'I', "userId",  "",""},
		};
		sys_user_def	ptTbl;
		sys_user_post_def	ptTblUserPost;
		sys_user_role_def	ptTblUserRole;
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
        NJ2S(root, &ptTbl, SYS_USER_COLMAP);
        PrintTbl(&ptTbl, SYS_USER_COLMAP);
        memset(sSql,  0x00, sizeof(sSql));
        memset(set, 0x00, sizeof(set));
        memset(where, 0x00, sizeof(where));
        NGetUpdateSetSql(root, set, SYS_USER_COLMAP);
        NGetUpdateWhereSql(root, where, SYS_USER_INDMAP);
        sprintf(sSql, "update sys_user  set %s where %s;", set, where);
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "update sql=[%s]", sSql);
        if(strlen(set) > 0 && strlen(where) > 0)
        {
            nRet = mysql_TBL_UPDATE(sSql);
            if(nRet != 0)
            {
                HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "update error [%s]", sSql);
                return -1;
            }
        }
		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "delete from sys_user_post where user_id= %d", ptTbl.user_id);
        HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", sSql);
		nRet = mysql_TBL_DELETE(sSql);
        if(nRet != 0)
        {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "delete error [%s]", sSql);
            return -1;
        }
		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "delete from sys_user_role where user_id= %d", ptTbl.user_id);
        HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", sSql);
		nRet = mysql_TBL_DELETE(sSql);
        if(nRet != 0)
        {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "delete error [%s]", sSql);
            return -1;
        }

		cJSON *postIds = cJSON_GetObjectItemCaseSensitive(root, "postIds");
		if (cJSON_IsArray(postIds)) 
		{
			cJSON *item = NULL;
			cJSON_ArrayForEach(item, postIds) {
				memset((char *)&ptTblUserPost, 0x00, sizeof(ptTblUserPost));
				if (cJSON_IsNumber(item)) {
					ptTblUserPost.post_id = item->valueint;
					ptTblUserPost.user_id = ptTbl.user_id;
					nRet = mysql_TBL_INSERT("sys_user_post", &ptTblUserPost, SYS_USER_POST_COLMAP);
				}
			}
		}
		cJSON *roleIds = cJSON_GetObjectItemCaseSensitive(root, "roleIds");
		if (cJSON_IsArray(roleIds)) 
		{
			cJSON *item = NULL;
			cJSON_ArrayForEach(item, roleIds) {
				memset((char *)&ptTblUserRole, 0x00, sizeof(ptTblUserRole));
				if (cJSON_IsNumber(item)) {
					ptTblUserRole.role_id = item->valueint;
					ptTblUserRole.user_id = ptTbl.user_id;
					nRet = mysql_TBL_INSERT("sys_user_role", &ptTblUserRole, SYS_USER_ROLE_COLMAP);
				}
			}
		}
        DbsCommit();
    }
	else if (strcmp(sMethod, "PUT" ) == 0 && strcmp(sUri, "/system/user/authRole") == 0 ) 
	{
		sys_user_role_def	ptTblUserRole;

		root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "msg", "操作成功");
        cJSON_AddNumberToObject(root, "code", 200);
        json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);

		GetIpcValueExt(IPC, "userId", sTmpBuf, sizeof(sTmpBuf));

		user_id = atoi(sTmpBuf);

		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "delete from sys_user_role where user_id= %d", user_id);
        HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", sSql);
		nRet = mysql_TBL_DELETE(sSql);
        if(nRet != 0)
        {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "delete error [%s]", sSql);
            return -1;
        }
		GetIpcValueExt(IPC, "roleIds", sTmpBuf, sizeof(sTmpBuf));
		char *token = strtok(sTmpBuf, ",");
    
		while (token != NULL) {
			ptTblUserRole.role_id = atoi(token);
			ptTblUserRole.user_id = user_id;
			nRet = mysql_TBL_INSERT("sys_user_role", &ptTblUserRole, SYS_USER_ROLE_COLMAP);
			token = strtok(NULL, ",");
		}

        DbsCommit();
    }
	else if (strcmp(sMethod, "PUT" ) == 0  && strcmp(param, "updatePwd") == 0) //个人中心 修改当前用户的密码
	{
        HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "\n%s", json_string);
		//void hash_password(char* password, char *hash);
	
        root = cJSON_Parse(json_string);
        if (root == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "Error before: %s", error_ptr);
            }
            return -1;
        }
        json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);
		free(json_string);

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select *  from sys_user where user_id=%d", user_id);
		sys_user_def	ptTbl;
        memset((char*)&ptTbl, 0x00, sizeof(ptTbl));
        nRet = mysql_ONE_SELECT(sSql, &ptTbl, SYS_USER_COLMAP);
        if(nRet ==0 ){ 
		
        }

		item = cJSON_GetObjectItemCaseSensitive(root, "oldPassword");
        HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s=%s", item->valuestring, ptTbl.password);
		body = cJSON_CreateObject();
        if (bcrypt_checkpw(item->valuestring, ptTbl.password) != 0 )
		{
			cJSON_AddStringToObject(body, "msg", "旧密码错误");
            cJSON_AddNumberToObject(body, "code", 500);
		}
		else
		{
			memset(ptTbl.password, 0x00, sizeof(ptTbl.password));	
			item = cJSON_GetObjectItemCaseSensitive(root, "newPassword");
			hash_password(item->valuestring, ptTbl.password);
			memset(sSql,  0x00, sizeof(sSql));
			sprintf(sSql, "update sys_user  set password = '%s' where user_id=%d;", ptTbl.password, user_id);
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "update sql=[%s]", sSql);
			if(strlen(set) > 0)
			{
				nRet = mysql_TBL_UPDATE(sSql);
				if(nRet != 0)
				{
					HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "update error [%s]", sSql);
					return -1;
				}
				DbsCommit();
			}
			cJSON_AddStringToObject(body, "msg", "操作成功");
            cJSON_AddNumberToObject(body, "code", 200);
		}
        json_string = cJSON_Print(body);
		cJSON_Delete(body);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);
	}
	else if (strcmp(sMethod, "PUT" ) == 0  && strcmp(param, "resetPwd") == 0) //个人中心 修改当前用户的密码
	{
        //HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "\n%s", json_string);
        sys_user_def ptTbl;
        root = cJSON_Parse(json_string);
        if (root == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "Error before: %s", error_ptr);
            }
            return -1;
        }
        json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);
		free(json_string);

		memset(ptTbl.password, 0x00, sizeof(ptTbl.password));	
		item = cJSON_GetObjectItemCaseSensitive(root, "password");
		hash_password(item->valuestring, ptTbl.password);
		item = cJSON_GetObjectItemCaseSensitive(root, "userId");
		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "update sys_user  set password = '%s' where user_id=%d;", ptTbl.password, item->valueint);
		HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "update sql=[%s]", sSql);
		if(strlen(set) > 0)
		{
			nRet = mysql_TBL_UPDATE(sSql);
			if(nRet != 0)
			{
				HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "update error [%s]", sSql);
				return -1;
			}
			DbsCommit();
		}
		cJSON_AddStringToObject(root, "msg", "操作成功");
		cJSON_AddNumberToObject(root, "code", 200);
        json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);
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
