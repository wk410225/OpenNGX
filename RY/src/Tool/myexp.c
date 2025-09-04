#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <stddef.h>
#include "mysql.h" 

#define DEFAULTHOST "192.168.0.180"
#define DEFAULTUSER "root"
#define DEFAULTPASSWD "1qaz!QAZ@2023"
#define DEFAULTPORT 3306
#define DEFAULTDB  "mysql"


MYSQL mysql; 
MYSQL_RES* result = NULL; 
MYSQL_ROW row;              
MYSQL_FIELD *ptFields = NULL;
unsigned int num_fields = 0;
unsigned long long num_rows = 0;

int InitConnect(MYSQL* pMysqlHandle, const char* const pIPAddr, int wPort,
		const char* const pUserName, const char* const pPasswd, const char* const pDbName,
		int iConnectTimeOut, int iReadTimeOut, int iWriteTimeOut)
{
	int ret = 0;

	mysql_init(pMysqlHandle);
	if (NULL == pMysqlHandle)
	{
		printf("mysql_init is error!errno[%d],errmsg[%s].\n", mysql_errno(pMysqlHandle), mysql_error(pMysqlHandle));
		return -1;
	}

	if (iConnectTimeOut >= 0)
	{
		ret = mysql_options(pMysqlHandle, MYSQL_OPT_CONNECT_TIMEOUT, &iConnectTimeOut);//设置超时选项
		if(ret)
		{
			printf("set connect timeout option is error!errno[%d],errmsg[%s].\n", mysql_errno(pMysqlHandle), mysql_error(pMysqlHandle));
			mysql_close(pMysqlHandle);
			return -2;
		}
	}

	if (iReadTimeOut >= 0)
	{
		ret = mysql_options(pMysqlHandle, MYSQL_OPT_READ_TIMEOUT, &iReadTimeOut);//设置超时选项
		if(ret)
		{
			printf("set connect timeout option is error!errno[%d],errmsg[%s].\n", mysql_errno(pMysqlHandle), mysql_error(pMysqlHandle));
			mysql_close(pMysqlHandle);
			return -2;
		}
	}

	if (iWriteTimeOut >= 0)
	{
		ret = mysql_options(pMysqlHandle, MYSQL_OPT_WRITE_TIMEOUT, &iWriteTimeOut);//设置超时选项
		if(ret)
		{
			printf("set connect timeout option is error!errno[%d],errmsg[%s].\n", mysql_errno(pMysqlHandle), mysql_error(pMysqlHandle));
			mysql_close(pMysqlHandle);
			return -2;
		}
	}

	if (NULL == mysql_real_connect(pMysqlHandle, pIPAddr, pUserName, pPasswd, pDbName, wPort, NULL, 0))
	{
		printf("connect mysql is error! errno[%d], errmsg[%s], Host[%s:%d], userName[%s], passwd[%s], db[%s].\n",
				mysql_errno(pMysqlHandle), mysql_error(pMysqlHandle),
				pIPAddr, wPort, pUserName, pPasswd, (NULL != pDbName)?pDbName:"");
		mysql_close(pMysqlHandle);
		return -3;
	}
	mysql_query(pMysqlHandle,"set autocommit=0");
	//mysql_set_character_set(pMysqlHandle, "GB18030");
	mysql_set_character_set(pMysqlHandle, "utf8mb4");

	return 0;
}

int main(int argc, char* argv[]) 
{ 
	int ret = 0, index = 0;
/*
	const char * host = DEFAULTHOST;
	const char * user = DEFAULTUSER; 
	const char * passwd = DEFAULTPASSWD; 
	const char * db = DEFAULTDB; 
	unsigned int port = DEFAULTPORT;  
*/
	char *ptr = NULL;
	char sCol[8192];
	char sVal[8192];
	char pSql[8192];

	if(argc != 2)
	{
		printf("param fault for example qry \"select * from tbl_srv_inf;\" \n");
		return -1;
	}

	char host[64];
	char user[64];
	char passwd[64];
	char db[64];
	int		port;
	memset(host, 0x00, sizeof(host));
	memset(user, 0x00, sizeof(user));
	memset(passwd, 0x00, sizeof(passwd));
	memset(db, 0x00, sizeof(db));

	strcpy(host, getenv("DEFAULTHOST"));
	strcpy(user, getenv("DEFAULTUSER"));
	strcpy(passwd, getenv("DEFAULTPASSWD"));
	strcpy(db, getenv("DEFAULTDB"));
	port = atoi(getenv("DEFAULTPORT"));


	ret = InitConnect(&mysql, host, port, user, passwd, db, -1, -1, -1);
	if (0 != ret)
	{
		return -1;
	}
	memset(pSql, 0x00, sizeof(pSql));
	strcpy(pSql, argv[1]);
	printf("--");
	printf("%s\n", pSql);
	ptr=strstr(pSql, "from");
	if(ptr == NULL)
	{
		printf("该%s语句不正常\n", pSql);
		return -1;
	}

	printf("delete %s\n", ptr);
	ret = mysql_query(&mysql, pSql);  
	if (0 != ret)
	{
		printf("Execute sql[%s] is error!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
		return -1;
	}
	if (NULL == (result = mysql_store_result(&mysql)))
	{
		printf("store result is error[%s]!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
		return -2;
	}

	if (NULL == result)
	{
		printf("result is NULL!\n");
		return -1;
	}
	num_rows = mysql_num_rows(result);          
	num_fields = mysql_num_fields(result); 
	ptFields = mysql_fetch_fields(result);

	memset(sCol, 0x00, sizeof(sCol));
	for (index = 0; index < num_fields; index++)
	{
		if(index ==0)
			sprintf(sCol+strlen(sCol), "insert into %s (%s," ,ptFields[index].table, ptFields[index].name);
		else if(num_fields != index+1)
			sprintf(sCol+strlen(sCol), " %s,", ptFields[index].name);
		else
			sprintf(sCol+strlen(sCol), " %s)", ptFields[index].name);
	}
	while (NULL != (row = mysql_fetch_row(result))) 
	{
		memset(sVal, 0x00, sizeof(sVal));
		for (index = 0; index < num_fields; index++)
		{
			//printf("table=[%s]\t", ptFields[index].table);
			//printf("name=[%s]\t", ptFields[index].name);
			//printf("data_type=[%d]\t", ptFields[index].type);
			//printf("data=[%s]\n", row[index]);
			if(index == 0)
			{
				if (IS_NUM(ptFields[index].type))
				{
					sprintf(sVal+strlen(sVal), " values (%s," ,row[index]);
				}
				else
				{
					if(row[index] != 0x00)
					{
						sprintf(sVal+strlen(sVal), " values ('%s'," ,row[index]);
					}
					else
						strcat(sVal, " values ('',");
						
				}
			}
			else if(num_fields != index+1)
			{
				if (IS_NUM(ptFields[index].type))
					sprintf(sVal+strlen(sVal), "%s,", row[index]);
				else
				{
					if(row[index] != 0x00)
					{
						sprintf(sVal+strlen(sVal), "'%s',", row[index]);
					}
					else
						strcat(sVal, "'',");
				}
			}
			else
			{
				if (IS_NUM(ptFields[index].type))
					sprintf(sVal+strlen(sVal), "%s);", row[index]);
				else
				{
					if(row[index] != 0x00)
					{
						sprintf(sVal+strlen(sVal), "'%s');", row[index]);
							
					}
					else
						strcat(sVal, "'');");
				}
			}
		}
		printf("%s%s\n", sCol,sVal);
	}
	mysql_free_result(result);

	return ret;
}
int MySqlDbsCommit ()
{
	return mysql_commit(&mysql); //zero for success
}

int MySqlDbsRollback ()
{
	return mysql_rollback(&mysql); //zero for success
}

int MySqlDbsGetErrCode()
{
	return mysql_errno(&mysql);
}
