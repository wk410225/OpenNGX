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
int MySqlDbsCommit ();

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

int TestMysqlDDL(MYSQL* pMysqlHandle, const char* pSql)
{   
    int ret = 0;
    
    ret = mysql_query(pMysqlHandle, pSql);  
    if (0 != ret)
    {   
        printf("Execute sql[%s] is error!errno[%d],errmsg[%s].\n", pSql, mysql_errno(pMysqlHandle), mysql_error(pMysqlHandle));   
        return -1;
    }
    
    return 0;
}

int main(int argc, char* argv[]) 
{ 
    int ret = 0;
/*
    const char * host = DEFAULTHOST;
    const char * user = DEFAULTUSER; 
    const char * passwd = DEFAULTPASSWD; 
    const char * db = DEFAULTDB; 
    unsigned int port = DEFAULTPORT;  
*/
	FILE *fp = NULL;
	char sTmpBuf[4096];
	char sFileRecord[4096];
	char sSql[4096];

	if(argc != 2)
	{
		printf("param fault for example \"ddl test.sql\" \n");
		return -1;
	}


	memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
	sprintf(sTmpBuf, "%s/sql/%s", getenv("FEHOME"), argv[1]);
   	printf("THE SQL FILE [%s] \n", sTmpBuf);
	fp = fopen(sTmpBuf, "r");
	if(fp == NULL)
	{
   		printf("fopen failure [%s] \n", sTmpBuf);
		return -1;
	}
	char host[64];
    char user[64];
    char passwd[64];
    char db[64];
    int     port;
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
	memset(sSql, 0x00, sizeof(sSql));
	int flag = 0;

	memset(sFileRecord, 0x00, sizeof(sFileRecord));

	while (fgets (sFileRecord, 4096, fp) != NULL)
	{
		if(sFileRecord[0] == '\n')
		{
			continue;
			if(sFileRecord[0] == '\r' && sFileRecord[1] == '\n')
			{
				continue;
			}
		}
		if(strchr(sFileRecord, ';') && flag == 0)
		{
			if(memcmp(sFileRecord, "--", 2) != 0)
			{
				ret = TestMysqlDDL(&mysql, sFileRecord);
				if(!ret )
				{
					sFileRecord[strlen(sFileRecord)-1] = 0x00;
					printf("SUCCESS [%s] \n", sFileRecord);
				}
			}
		}
		else if(strchr(sFileRecord, ';') && flag == 1)
		{
			strcat(sSql, sFileRecord);
			flag = 0;
			if(memcmp(sSql, "--", 2) != 0)
			{
				ret = TestMysqlDDL(&mysql, sSql);
				if(!ret )
				{
					sSql[strlen(sSql)-1] = 0x00;
					printf("SUCCESS [%s] \n", sSql);
				}
			}
			memset(sSql, 0x00, sizeof(sSql));
		}
		else
		{
			flag = 1;
			strcat(sSql, sFileRecord);
		}
		memset(sFileRecord, 0x00, sizeof(sFileRecord));
	}
	MySqlDbsCommit ();
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
