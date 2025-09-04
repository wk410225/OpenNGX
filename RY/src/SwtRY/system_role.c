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

static TABFUN SYS_ROLE_MENU_COLMAP[MAXCOLNUM]={
	{"role_id", offsetof(sys_role_menu_def, role_id), 'I',            "roleId", "roleId", ""},
	{"menu_id", offsetof(sys_role_menu_def, menu_id), 'I',            "menuId", "menuId", ""},
};
static TABFUN SYS_USER_ROLE_COLMAP[MAXCOLNUM]={
	{"user_id", offsetof(sys_user_role_def, user_id), 'I',            "userId", "userId", ""},
	{"role_id", offsetof(sys_user_role_def, role_id), 'I',            "roleId", "roleId", ""},
};
static TABFUN SYS_USER_ROLE_INDMAP[MAXCOLNUM]={
	{"user_id", offsetof(sys_user_role_def, user_id), 'I',            "userId", "userId", ""},
	{"role_id", offsetof(sys_user_role_def, role_id), 'I',            "roleId", "roleId", ""},
};

static TABFUN SYS_ROLE_COLMAP[MAXCOLNUM]={
    {"role_id", offsetof(sys_role_def, role_id), 'I', "roleId", "roleId"},
    {"role_name", offsetof(sys_role_def, role_name), 'C', "roleName","roleName"},
    {"role_key", offsetof(sys_role_def, role_key), 'C', "roleKey", "roleKey"},
    {"role_sort", offsetof(sys_role_def, role_sort), 'I', "roleSort", "roleSort"},
    {"data_scope", offsetof(sys_role_def, data_scope), 'C', "dataScope", "dataScope"},
    {"menu_check_strictly", offsetof(sys_role_def, menu_check_strictly), 'I', "menuCheckStrictly", "menuCheckStrictly", "B"},
    {"dept_check_strictly", offsetof(sys_role_def, dept_check_strictly), 'I', "deptCheckStrictly", "deptCheckStrictly", "B"},
    {"status", offsetof(sys_role_def, status), 'C', "status", "status"},
    {"del_flag", offsetof(sys_role_def, del_flag), 'C', "", "delFlag"},
    {"create_by", offsetof(sys_role_def, create_by), 'C', "", "createBy"},
    {"create_time", offsetof(sys_role_def, create_time), 'T', "params[beginTime]", "createTime", "params[endTime]"},
    {"update_by", offsetof(sys_role_def, update_by), 'C', "", "updateBy"},
    {"update_time", offsetof(sys_role_def, update_time), 'T', "", "updateTime"},
    {"remark", offsetof(sys_role_def, remark), 'C', "remark", "remark"},
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
static TABFUN SYS_MENU_COLMAP[MAXCOLNUM]={
    {"menu_id", offsetof(sys_menu_def, menu_id), 'I', "", "id"},
    {"menu_name", offsetof(sys_menu_def, menu_name), 'C', "", "label"},
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
    {"status", offsetof(sys_menu_def, status), 'C', "", "disabled", "B"},
    {"perms", offsetof(sys_menu_def, perms), 'C'},
    {"icon", offsetof(sys_menu_def, icon), 'C'},
    {"create_by", offsetof(sys_menu_def, create_by), 'C'},
    {"create_time", offsetof(sys_menu_def, create_time), 'C'},
    {"update_by", offsetof(sys_menu_def, update_by), 'C'},
    {"update_time", offsetof(sys_menu_def, update_time), 'C'},
    {"remark", offsetof(sys_menu_def, remark), 'C'},
};


int system_role(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "system_role";
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
		sprintf(sSql, "delete from sys_role where role_id= %d", atoi(param));
        HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", sSql);
		nRet = mysql_TBL_DELETE(sSql);
        if(nRet != 0)
        {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "delete error [%s]", sSql);
            return -1;
        }

		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "delete from sys_role_menu where role_id= %d", atoi(param));
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
	else if (strcmp(sMethod, "POST" ) == 0  && strcmp(sUri, "/system/role/export") == 0) //角色导出
	{
		xlsxiowriter handle;
		if ((handle = xlsxiowrite_open("role.xlsx", "ROLE")) == NULL) 
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "xlsxiowrite_open error ");
			return -1;
		}
		//set row height
		xlsxiowrite_set_row_height(handle, 1);
		//how many rows to buffer to detect column widths
		xlsxiowrite_set_detection_rows(handle, 6);
		//write column names
		xlsxiowrite_add_column( handle,"角色编号", 	0);
		xlsxiowrite_add_column( handle,"角色名称",  0);
		xlsxiowrite_add_column( handle,"权限字符",  0);
		xlsxiowrite_add_column( handle,"显示顺序",  0);
		xlsxiowrite_add_column( handle,"状态", 0);
		xlsxiowrite_add_column( handle,"创建时间", 0);
		xlsxiowrite_next_row(handle);


		sys_role_def	ptTblRole;

		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		memset(sSql,  0x00, sizeof(sSql));
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
				xlsxiowrite_add_cell_int(handle, ptTblRole.role_id);
				xlsxiowrite_add_cell_string(handle,   ptTblRole.role_name);
				xlsxiowrite_add_cell_string(handle,   ptTblRole.role_key);
				xlsxiowrite_add_cell_int(handle,   ptTblRole.role_sort);
				xlsxiowrite_add_cell_string(handle,   ptTblRole.status);
				xlsxiowrite_add_cell_string(handle,   ptTblRole.create_time);
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

		FILE *file = fopen("role.xlsx", "rb");
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
	else if (strcmp(sMethod, "POST" ) == 0  && strcmp(sUri, "/system/role") == 0) //新增角色
	{
		sys_role_def	ptTbl;
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
        NJ2S(root, &ptTbl, SYS_ROLE_COLMAP);
		strcpy(ptTbl.create_by, ptTblUser.user_name);
		strcpy(ptTbl.del_flag, "0");
		GetCurrTime(ptTbl.create_time);
        PrintTbl(&ptTbl, SYS_ROLE_COLMAP);
        nRet = mysql_TBL_INSERT("sys_role", &ptTbl, SYS_ROLE_COLMAP);
        if(nRet != 0)
        {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "insert error nRet=[%d]", nRet);
            return -1;
        }
		memset(sSql,  0x00, sizeof(sSql));
		sprintf(sSql, "select  * from sys_role where role_key= '%s'", ptTbl.role_key);
		nRet = mysql_ONE_SELECT(sSql, &ptTbl, SYS_ROLE_COLMAP);

		sys_role_menu_def	ptTblRoleMenu;
		cJSON *menuIds= cJSON_GetObjectItemCaseSensitive(root, "menuIds");
		if (cJSON_IsArray(menuIds)) 
		{
			cJSON *item = NULL;
			cJSON_ArrayForEach(item, menuIds) {
				memset((char *)&ptTblRoleMenu, 0x00, sizeof(ptTblRoleMenu));
				if (cJSON_IsNumber(item)) {
					ptTblRoleMenu.menu_id = item->valueint;
					ptTblRoleMenu.role_id = ptTbl.role_id;
					nRet = mysql_TBL_INSERT("sys_role_menu", &ptTblRoleMenu, SYS_ROLE_MENU_COLMAP);
				}
			}
		}
        DbsCommit();
	}
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(sUri, "/system/role/authUser/unallocatedList") == 0) 
	{
		sys_user_def	ptTblUser;
		char sTmpBuf[128];
		char where[512];
		GetIpcValueExt(IPC, "roleId", sTmpBuf, sizeof(sTmpBuf));
		root = cJSON_CreateObject();
		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;
		parray = cJSON_CreateArray();
		memset(where,  0x00, sizeof(where));
		NGetQueryWhere(IPC, where,  SYS_USER_COLMAP);
		sprintf(sSql, "select * from sys_user %s and user_id not  in ( select user_id from sys_user_role where role_id = %s ) ", where , sTmpBuf );
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
				cJSON_AddItemToArray(parray, pdata);
			}
		}
		cJSON_AddItemToObject(root, "rows", parray);

		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);

	}
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(sUri, "/system/role/authUser/allocatedList") == 0) 
	{
		sys_user_def	ptTblUser;
		char sTmpBuf[128];
		char where[512];
		GetIpcValueExt(IPC, "roleId", sTmpBuf, sizeof(sTmpBuf));
		root = cJSON_CreateObject();
		cJSON *pdata = NULL, *cdata = NULL;
		cJSON *parray = NULL, *carray = NULL;
		parray = cJSON_CreateArray();
		memset(where,  0x00, sizeof(where));
		NGetQueryWhere(IPC, where,  SYS_USER_COLMAP);
		sprintf(sSql, "select * from sys_user %s and user_id in ( select user_id from sys_user_role where role_id = %s ) ", where , sTmpBuf );
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
				cJSON_AddItemToArray(parray, pdata);
			}
		}
		cJSON_AddItemToObject(root, "rows", parray);

		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);

	}
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(sUri, "/system/role/list") == 0) //获取角色列表
	{
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
				cJSON_AddItemToArray(parray, pdata);
			}
		}
		cJSON_AddItemToObject(root, "rows", parray);
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);
	}
	else if (strcmp(sMethod, "PUT" ) == 0  && strcmp(sUri, "/system/role/authUser/cancel") == 0) //修改状态
	{
		sys_user_role_def	ptTbl;
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
        NJ2S(root, &ptTbl, SYS_USER_ROLE_COLMAP);
        PrintTbl(&ptTbl, SYS_USER_ROLE_COLMAP);
        memset(sSql,  0x00, sizeof(sSql));
        memset(where, 0x00, sizeof(where));
        NGetUpdateWhereSql(root, where, SYS_USER_ROLE_INDMAP);
        if(strlen(where) > 0)
        {
			sprintf(sSql, "delete from sys_user_role  where %s ;", where);
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "delete sql=[%s]", sSql);
            nRet = mysql_TBL_UPDATE(sSql);
            if(nRet != 0)
            {
                HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "update error [%s]", sSql);
                return -1;
            }
            DbsCommit();
			cJSON_AddStringToObject(root, "msg", "查询成功");
			cJSON_AddNumberToObject(root, "code", 200);
        }
	    else 	
		{
			cJSON_AddStringToObject(root, "msg", "操作失败");
			cJSON_AddNumberToObject(root, "code", 500);
		}
    }
	else if (strcmp(sMethod, "PUT" ) == 0  && strcmp(sUri, "/system/role/changeStatus") == 0) //修改状态
	{
		TABFUN SYS_ROLE_INDMAP[MAXCOLNUM]={
			{"role_id", offsetof(sys_role_def, role_id), 'I', "roleId",  "",""},
		};
		sys_role_def	ptTbl;
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
        NJ2S(root, &ptTbl, SYS_ROLE_COLMAP);
        PrintTbl(&ptTbl, SYS_ROLE_COLMAP);
        memset(sSql,  0x00, sizeof(sSql));
        memset(set, 0x00, sizeof(set));
        memset(where, 0x00, sizeof(where));
        NGetUpdateSetSql(root, set, SYS_ROLE_COLMAP);
        NGetUpdateWhereSql(root, where, SYS_ROLE_INDMAP);
        sprintf(sSql, "update sys_role  set %s where %s;", set, where);
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
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);
    }
	else if (strcmp(sMethod, "PUT" ) == 0  && strcmp(sUri, "/system/role/authUser/cancelAll") == 0) 
	{
		char	roleId[16];
		char	userIds[1024];
		char    delim[8];
		GetIpcValueExt(IPC, "roleId", roleId, sizeof(roleId));
		GetIpcValueExt(IPC, "userIds", userIds, sizeof(userIds));
		sprintf(delim, "%s", ",");
		char *token = NULL;
		char *rest = userIds;

		while ((token = strsep(&rest, ",")) != NULL)
		{
			sprintf(sSql, "delete from sys_user_role where user_id = %s and role_id = %s;", token ,roleId);
			nRet = mysql_TBL_EXECUTE(sSql);
			if(nRet != 0)
			{
				HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "execute error [%s]", sSql);
				return -1;
			}
		}
        DbsCommit();
		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);
    }
	else if (strcmp(sMethod, "PUT" ) == 0  && strcmp(sUri, "/system/role/authUser/selectAll") == 0) 
	{
		char	roleId[16];
		char	userIds[1024];
		char    delim[8];
		GetIpcValueExt(IPC, "roleId", roleId, sizeof(roleId));
		GetIpcValueExt(IPC, "userIds", userIds, sizeof(userIds));
		sprintf(delim, "%s", ",");
		char *token = NULL;
		char *rest = userIds;

		while ((token = strsep(&rest, ",")) != NULL)
		{
			sprintf(sSql, "insert into sys_user_role(user_id,role_id) values (%s,%s);", token ,roleId);
			nRet = mysql_TBL_EXECUTE(sSql);
			if(nRet != 0)
			{
				HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "execute error [%s]", sSql);
				return -1;
			}
		}
        DbsCommit();
		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);
    }
	else if (strcmp(sMethod, "PUT" ) == 0  && strcmp(sUri, "/system/role") == 0) //修改状态
	{
		TABFUN SYS_ROLE_INDMAP[MAXCOLNUM]={
			{"role_id", offsetof(sys_role_def, role_id), 'I', "roleId",  "",""},
		};
		sys_role_def	ptTbl;
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
        NJ2S(root, &ptTbl, SYS_ROLE_COLMAP);
        PrintTbl(&ptTbl, SYS_ROLE_COLMAP);
        memset(sSql,  0x00, sizeof(sSql));
        memset(set, 0x00, sizeof(set));
        memset(where, 0x00, sizeof(where));
        NGetUpdateSetSql(root, set, SYS_ROLE_COLMAP);
        NGetUpdateWhereSql(root, where, SYS_ROLE_INDMAP);
        sprintf(sSql, "update sys_role  set %s where %s;", set, where);
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
		sprintf(sSql, "delete from sys_role_menu where role_id= %d", ptTbl.role_id);
        HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", sSql);
		nRet = mysql_TBL_DELETE(sSql);
        if(nRet != 0)
        {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "delete error [%s]", sSql);
            return -1;
        }

		sys_role_menu_def	ptTblRoleMenu;
		cJSON *menuIds= cJSON_GetObjectItemCaseSensitive(root, "menuIds");
		if (cJSON_IsArray(menuIds)) 
		{
			cJSON *item = NULL;
			cJSON_ArrayForEach(item, menuIds) {
				memset((char *)&ptTblRoleMenu, 0x00, sizeof(ptTblRoleMenu));
				if (cJSON_IsNumber(item)) {
					ptTblRoleMenu.menu_id = item->valueint;
					ptTblRoleMenu.role_id = ptTbl.role_id;
					nRet = mysql_TBL_INSERT("sys_role_menu", &ptTblRoleMenu, SYS_ROLE_MENU_COLMAP);
				}
			}
		}
        DbsCommit();
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);
    }
	else if ( strcmp(sMethod, "GET" ) == 0 && atoi(param) > 0 ) //修改页面查询 被修改角色信息
	{
    	sys_role_def    ptTbl;
    	sys_user_role_def    ptTblUserRole;

		root = cJSON_CreateObject();

		memset(sSql,  0x00, sizeof(sSql));
        sprintf(sSql, "select *  from sys_role where role_id=%d", atoi(param));
        memset((char*)&ptTbl, 0x00, sizeof(ptTbl));
        nRet = mysql_ONE_SELECT(sSql, &ptTbl, SYS_ROLE_COLMAP);
        if(nRet != 0 ){ 
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "error [%s]", sSql);
            return -1; 
        }
		cJSON   *role = cJSON_CreateObject();
		//cJSON	*parray = cJSON_CreateArray();
        NS2J(role, &ptTbl, SYS_ROLE_COLMAP);
		//cJSON_AddItemToArray(parray, role);
        cJSON_AddStringToObject(root, "msg", "操作成功");
        cJSON_AddNumberToObject(root, "code", 200);
		cJSON_AddItemToObject(root, "data", role);

        json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s", json_string);
	}
	else if (strcmp(sMethod, "PUT" ) == 0  && strcmp(sUri, "/system/role/dataScope") == 0)  //个人中心修改当前登录人信息
	{
		TABFUN SYS_ROLE_INDMAP[MAXCOLNUM]={
			{"role_id", offsetof(sys_role_def, role_id), 'I', "roleId",  "",""},
		};
		sys_role_def	ptTbl;
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
        NJ2S(root, &ptTbl, SYS_ROLE_COLMAP);
        PrintTbl(&ptTbl, SYS_ROLE_COLMAP);
        memset(sSql,  0x00, sizeof(sSql));
        memset(set, 0x00, sizeof(set));
        memset(where, 0x00, sizeof(where));
        NGetUpdateSetSql(root, set, SYS_ROLE_COLMAP);
        NGetUpdateWhereSql(root, where, SYS_ROLE_INDMAP);
        sprintf(sSql, "update sys_role  set %s where %s;", set, where);
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
        DbsCommit();
		cJSON_AddStringToObject(root, "msg", "查询成功");
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
