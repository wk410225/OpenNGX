#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <memory.h>
#include <string.h>
#include <mysql/mysql.h>
#include "HtLog.h"
#include "DbsDef.h"
#include "DbsTbl.h"
#include "MyDbsDef.h"

MYSQL mysql;
MYSQL_RES* result = NULL;   
MYSQL_FIELD *ptFields = NULL;
unsigned int num_fields = 0;
unsigned long long num_rows = 0;

MYSQL_RES* result1 = NULL;   
MYSQL_FIELD *ptFields1 = NULL;
unsigned int num_fields1 = 0;
unsigned long long num_rows1 = 0;

MYSQL_RES* result2 = NULL;   
MYSQL_FIELD *ptFields2 = NULL;
unsigned int num_fields2 = 0;
unsigned long long num_rows2 = 0;

MYSQL_RES* result3 = NULL;   
MYSQL_FIELD *ptFields3 = NULL;
unsigned int num_fields3 = 0;
unsigned long long num_rows3 = 0;

int InitConnect(MYSQL* pMysqlHandle, const char* const pIPAddr, int wPort, const char* const pUserName, const char* const pPasswd, const char* const pDbName,
                int iConnectTimeOut, int iReadTimeOut, int iWriteTimeOut)
{
    int ret = 0;

    mysql_init(pMysqlHandle);
    if (NULL == pMysqlHandle)
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__, "mysql_init is error!errno[%d],errmsg[%s]", mysql_errno(pMysqlHandle), mysql_error(pMysqlHandle));
        return -1;
    }

    if (iConnectTimeOut >= 0)
    {
        ret = mysql_options(pMysqlHandle, MYSQL_OPT_CONNECT_TIMEOUT, &iConnectTimeOut);
        if(ret)
        {
            HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"set connect timeout option is error!errno[%d],errmsg[%s].\n", mysql_errno(pMysqlHandle), mysql_error(pMysqlHandle));
            mysql_close(pMysqlHandle);
            return -2;
        }
    }

    if (iReadTimeOut >= 0)
    {
        ret = mysql_options(pMysqlHandle, MYSQL_OPT_READ_TIMEOUT, &iReadTimeOut);
        if(ret)
        {
            HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"set connect timeout option is error!errno[%d],errmsg[%s].\n", mysql_errno(pMysqlHandle), mysql_error(pMysqlHandle));
            mysql_close(pMysqlHandle);
            return -2;
        }
    }

    if (iWriteTimeOut >= 0)
    {
        ret = mysql_options(pMysqlHandle, MYSQL_OPT_WRITE_TIMEOUT, &iWriteTimeOut);
        if(ret)
        {
            HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"set connect timeout option is error!errno[%d],errmsg[%s].\n", mysql_errno(pMysqlHandle), mysql_error(pMysqlHandle));
            mysql_close(pMysqlHandle);
            return -2;
        }
    }
    
    if (NULL == mysql_real_connect(pMysqlHandle, pIPAddr, pUserName, pPasswd, pDbName, wPort, NULL, 0))
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"connect mysql is error! errno[%d], errmsg[%s], Host[%s:%d], userName[%s], passwd[%s], db[%s].\n",
               mysql_errno(pMysqlHandle), mysql_error(pMysqlHandle),
               pIPAddr, wPort, pUserName, pPasswd, (NULL != pDbName)?pDbName:"");
        mysql_close(pMysqlHandle);
        return -3;
    }
    mysql_query(pMysqlHandle,"set autocommit=0");
    mysql_set_character_set(pMysqlHandle, "utf8mb4");
#if 0
    // 设置事务隔离级别为读已提交
    if (mysql_query(pMysqlHandle, "SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED") != 0) 
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"set connect timeout option is error!errno[%d],errmsg[%s].\n", mysql_errno(pMysqlHandle), mysql_error(pMysqlHandle));
        mysql_close(pMysqlHandle);
        return 1;
    }

    // 禁用查询缓存
    if (mysql_query(pMysqlHandle, "SET SESSION query_cache_type = OFF") != 0) 
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"set connect timeout option is error!errno[%d],errmsg[%s].\n", mysql_errno(pMysqlHandle));
        mysql_close(pMysqlHandle);
        return 1;
    }
#endif

    return 0;
}
    

int DbsConnect ()
{
        int ret = 0;
        ret = InitConnect(&mysql, getenv("DEFAULTHOST"), atoi(getenv("DEFAULTPORT")), getenv("DEFAULTUSER"), getenv("DEFAULTPASSWD"), getenv("DEFAULTDB"), -1, -1, -1);
        if (0 != ret)
        {
            return  mysql_errno(&mysql);
        }
        return 0;
}

int DbsBegin ()
{
    return 0;
}

int DbsCommit ()
{
    return mysql_commit(&mysql);
}

int DbsRollback ()
{
    return mysql_rollback(&mysql); 
}

int DbsDisconnect ()
{
    mysql_close(&mysql);
    return 0;
}

int DbsConnectTest()
{
    int ret = 0;
    char *pSql="select 1 ;";
    MYSQL_RES* result = NULL;   

    ret = mysql_query(&mysql, pSql);  
    if (ret !=0)
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__, "Execute sql[%s] is error!errno[%d],errmsg[%s], ret = [%d]", pSql, mysql_errno(&mysql), mysql_error(&mysql),ret);
        return -1;
    }
    if (NULL == (result = mysql_store_result(&mysql)))
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"store result is error[%s]!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
        return -2;
    }
    if (NULL == result)
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"result is NULL!\n");
        return -1;
    }
    mysql_free_result(result);
    return 0;
}

int TBL_DEF_ONE_SELECT(char *pSql, void* stTestDb, TABFUN *TBL_COLMAP_DEF)
{
    int i;
    int ret = 0;
    int index;

    MYSQL_RES* result = NULL;   
    MYSQL_ROW row;              
    MYSQL_FIELD *ptFields = NULL;
    unsigned int num_fields = 0;
    unsigned long long num_rows = 0;

    ret = mysql_query(&mysql, pSql); 
    if (0 != ret)
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"Execute sql[%s] is error!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }
    if (NULL == (result = mysql_store_result(&mysql))) 
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"store result is error[%s]!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }

    num_fields = mysql_num_fields(result);  
    ptFields = mysql_fetch_fields(result);
    num_rows = mysql_num_rows(result);             
    //printf("num_rows=[%lld]\t\n", num_rows);
    if(num_rows > 1)
    {
        return -2112;
    }
    if(num_rows == 0)
    {
        return 1403;
    }
    while (NULL != (row = mysql_fetch_row(result))) 
    {
        for (index = 0; index < num_fields; index++)
        {
			//HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%s]", ptFields[index].name, row[index]);
			if(row[index] == NULL)
			{
				HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"null[%s]=[%s]", ptFields[index].name, row[index]);
				continue;
			}
            i = 0;
			//HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"TBL_COLMAP_DEF[%d].f_colname=[%s]", i, TBL_COLMAP_DEF[i].f_colname);
            while(TBL_COLMAP_DEF[i].f_colname[0] != 0x00)
            {
                if(strcasecmp(ptFields[index].name, TBL_COLMAP_DEF[i].f_colname) == 0)
                {
					HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%s]", ptFields[index].name, row[index]);
					switch(TBL_COLMAP_DEF[i].type)
                    {
                        case 'C':
                            strcpy((char *)stTestDb + TBL_COLMAP_DEF[i].offset, row[index]);
                            break;
                        case 'T':
                            strcpy((char *)stTestDb + TBL_COLMAP_DEF[i].offset, row[index]);
                            break;
                        case 'I':
                            *((int *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = atoi(row[index]);
                            break;
                        case 'F':
                            *((float *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = strtof(row[index], NULL);
                            break;
                        case 'L':
                            *((long *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = atol(row[index]);
                            break;
                        case 'D':
                            *((double *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = strtod(row[index], NULL);
                            break;
                        default:
							HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"bad type");
                    }
                }
                i++;
            }
        }
    }
    mysql_free_result(result);
    return 0;
}
int mysql_ONE_SELECT(char *pSql, void* stTestDb, TABFUN *TBL_COLMAP_DEF)
{
    int i;
    int ret = 0;
    int index;

    MYSQL_RES* result = NULL;   
    MYSQL_ROW row;              
    MYSQL_FIELD *ptFields = NULL;
    unsigned int num_fields = 0;
    unsigned long long num_rows = 0;

    ret = mysql_query(&mysql, pSql); 
    if (0 != ret)
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"Execute sql[%s] is error!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }
    if (NULL == (result = mysql_store_result(&mysql))) 
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"store result is error[%s]!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }

    num_fields = mysql_num_fields(result);  
    ptFields = mysql_fetch_fields(result);
    num_rows = mysql_num_rows(result);             
    //printf("num_rows=[%lld]\t\n", num_rows);
    if(num_rows > 1)
    {
        return -2112;
    }
    if(num_rows == 0)
    {
        return 1403;
    }
    while (NULL != (row = mysql_fetch_row(result))) 
    {
        for (index = 0; index < num_fields; index++)
        {
			//HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%s]", ptFields[index].name, row[index]);
			if(row[index] == NULL)
			{
				HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"null[%s]=[%s]", ptFields[index].name, row[index]);
				continue;
			}
            i = 0;
			//HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"TBL_COLMAP_DEF[%d].f_colname=[%s]", i, TBL_COLMAP_DEF[i].f_colname);
            while(TBL_COLMAP_DEF[i].f_colname[0] != 0x00)
            {
                if(strcasecmp(ptFields[index].name, TBL_COLMAP_DEF[i].f_colname) == 0)
                {
					HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%s]", ptFields[index].name, row[index]);
					switch(TBL_COLMAP_DEF[i].type)
                    {
                        case 'C':
                            strcpy((char *)stTestDb + TBL_COLMAP_DEF[i].offset, row[index]);
                            break;
                        case 'T':
                            strcpy((char *)stTestDb + TBL_COLMAP_DEF[i].offset, row[index]);
                            break;
                        case 'I':
                            *((int *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = atoi(row[index]);
                            break;
                        case 'F':
                            *((float *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = strtof(row[index], NULL);
                            break;
                        case 'L':
                            *((long *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = atol(row[index]);
                            break;
                        case 'D':
                            *((double *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = strtod(row[index], NULL);
                            break;
                        default:
							HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"bad type");
                    }
                }
                i++;
            }
        }
    }
    mysql_free_result(result);
    return 0;
}
int mysql_TBL_INSERT(char *tname, void *MYDBTABLE, TABFUN *TBL_COLMAP_DEF)
{
    int		ret = 0;
    int		index = 0;
    char	pSql[MAXSQLLEN];
    char	sql_col[2048];
    char	sql_val[8192];
    char    sTmpBuf1[MAXSQLLEN*2];
    char    sTmpBuf2[MAXSQLLEN*8];
    memset(pSql, 0x00,sizeof(pSql));
    memset(sTmpBuf1, 0x00, sizeof(sTmpBuf1));
    memset(sTmpBuf2, 0x00, sizeof(sTmpBuf2));

    while(strlen(TBL_COLMAP_DEF[index].f_colname) > 0 )
    {
        memset(sql_col, 0x00, sizeof(sql_col));
        memset(sql_val, 0x00, sizeof(sql_val));
        if(index == 0)
        {
           sprintf(sql_col, "%s", TBL_COLMAP_DEF[index].f_colname);
		   switch(TBL_COLMAP_DEF[index].type)
           {
               case 'C':
              	 	sprintf(sql_val, "'%s'", (char *)MYDBTABLE + TBL_COLMAP_DEF[index].offset);
	 HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%s]", TBL_COLMAP_DEF[index].f_colname, (char *)MYDBTABLE + TBL_COLMAP_DEF[index].offset);
                    break;
               case 'I':
              		sprintf(sql_val, "%d", *(int*)((char *)MYDBTABLE+ TBL_COLMAP_DEF[index].offset));
	 HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%d]", TBL_COLMAP_DEF[index].f_colname, *(int*)((char *)MYDBTABLE+ TBL_COLMAP_DEF[index].offset));
                  	break;
               case 'F':
                	sprintf(sql_val, "%f", *(float*)((char *)MYDBTABLE+ TBL_COLMAP_DEF[index].offset));
	 HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%f]", TBL_COLMAP_DEF[index].f_colname, *(float*)((char *)MYDBTABLE+ TBL_COLMAP_DEF[index].offset));
                    break;
               case 'L':
                	sprintf(sql_val, "%ld", *(long*)((char *)MYDBTABLE+ TBL_COLMAP_DEF[index].offset));
	 HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%ld]", TBL_COLMAP_DEF[index].f_colname, *(long*)((char *)MYDBTABLE+ TBL_COLMAP_DEF[index].offset));
                    break;
               case 'D':
                	sprintf(sql_val, "%lf", *(double*)((char *)MYDBTABLE+ TBL_COLMAP_DEF[index].offset));
	 HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%lf]", TBL_COLMAP_DEF[index].f_colname, *(double*)((char *)MYDBTABLE+ TBL_COLMAP_DEF[index].offset));
                    break;
               case 'T':
					if(strlen((char *)MYDBTABLE + TBL_COLMAP_DEF[index].offset) != 0)
						sprintf(sql_val, "'%s'", (char *)MYDBTABLE + TBL_COLMAP_DEF[index].offset);
					else 
						sprintf(sql_val, "NULL");
	 HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%s]", TBL_COLMAP_DEF[index].f_colname, (char *)MYDBTABLE + TBL_COLMAP_DEF[index].offset);
                    break;
               default:
					 HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"bad type");
            }
        }
		else
		{
			sprintf(sql_col, ",%s", TBL_COLMAP_DEF[index].f_colname);
			switch(TBL_COLMAP_DEF[index].type)
			{
				case 'C':
					sprintf(sql_val, ",'%s'", (char *)MYDBTABLE + TBL_COLMAP_DEF[index].offset);
	 HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%s]", TBL_COLMAP_DEF[index].f_colname, (char *)MYDBTABLE + TBL_COLMAP_DEF[index].offset);
					break;
				case 'I':
	 HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%d]", TBL_COLMAP_DEF[index].f_colname, *(int*)((char *)MYDBTABLE+ TBL_COLMAP_DEF[index].offset));
					sprintf(sql_val, ",%d", *(int*)((char *)MYDBTABLE+ TBL_COLMAP_DEF[index].offset));
					break;
				case 'F':
					sprintf(sql_val, ",%f", *(float*)((char *)MYDBTABLE+ TBL_COLMAP_DEF[index].offset));
	 HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%f]", TBL_COLMAP_DEF[index].f_colname, *(float*)((char *)MYDBTABLE+ TBL_COLMAP_DEF[index].offset));
					break;
				case 'L':
					sprintf(sql_val, ",%ld", *(long*)((char *)MYDBTABLE+ TBL_COLMAP_DEF[index].offset));
	 HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%ld]", TBL_COLMAP_DEF[index].f_colname, *(long*)((char *)MYDBTABLE+ TBL_COLMAP_DEF[index].offset));
					break;
				case 'D':
					sprintf(sql_val, ",%lf", *(double*)((char *)MYDBTABLE+ TBL_COLMAP_DEF[index].offset));
	 HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%lf]", TBL_COLMAP_DEF[index].f_colname, *(double*)((char *)MYDBTABLE+ TBL_COLMAP_DEF[index].offset));
					break;
               case 'T':
					if(strlen((char *)MYDBTABLE + TBL_COLMAP_DEF[index].offset) != 0)
						sprintf(sql_val, ",'%s'", (char *)MYDBTABLE + TBL_COLMAP_DEF[index].offset);
					else 
						sprintf(sql_val, ",NULL");
	 HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%s]", TBL_COLMAP_DEF[index].f_colname, (char *)MYDBTABLE + TBL_COLMAP_DEF[index].offset);
                    break;
				default:
					 HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"bad type");
			}
		}
		strcat(sTmpBuf1, sql_col);
		strcat(sTmpBuf2, sql_val);
		index++;
	}
	memset(pSql, 0x00, sizeof(pSql));
	sprintf(pSql, "insert into %s (%s) values (%s);", tname, sTmpBuf1,sTmpBuf2);
    HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"sql=[%s]\n", pSql);

    ret = mysql_query(&mysql, pSql);  
    if (0 != ret)
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"Execute sql[%s] is error!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }

    return 0;
}
int mysql_TBL_DELETE(char* pSql)
{
    int ret = 0;
    unsigned long long num_rows = 0;
    unsigned int warning_num = 0;

    ret = mysql_query(&mysql, pSql); 
    if (0 != ret)
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"Execute sql[%s] is error!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }
    num_rows = mysql_affected_rows(&mysql);
    warning_num = mysql_warning_count(&mysql);
    HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"sql[%s] affected rows[%llu], waring[%u]\n",pSql, num_rows, warning_num);
    HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"%s\n", mysql_info(&mysql)?mysql_info(&mysql):"");

    return 0;
}

int mysql_TBL_UPDATE(char* pSql)
{
    int ret = 0;
    unsigned long long num_rows = 0;
    unsigned int warning_num = 0;

    ret = mysql_query(&mysql, pSql); 
    if (0 != ret)
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"Execute sql[%s] is error!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }
    num_rows = mysql_affected_rows(&mysql);
    warning_num = mysql_warning_count(&mysql);
    HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"sql[%s] affected rows[%llu], waring[%u]\n",pSql, num_rows, warning_num);
    HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"%s\n", mysql_info(&mysql)?mysql_info(&mysql):"");
	if (mysql_info(&mysql) && strstr(mysql_info(&mysql), "matched: 0")) {
        return 1403;  // 没有匹配行
    }
/*
	if(num_rows == 0)
		return 1403;
*/
    return 0;
}

int mysql_TBL_EXECUTE(char* pSql)
{
    int ret = 0;
    unsigned long long num_rows = 0;
    unsigned int warning_num = 0;

    ret = mysql_query(&mysql, pSql);
    if (0 != ret)
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"Execute sql[%s] is error!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }
    num_rows = mysql_affected_rows(&mysql);
    warning_num = mysql_warning_count(&mysql);
    HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"sql[%s] affected rows[%llu], waring[%u]\n",pSql, num_rows, warning_num);
    //HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"%s\n", mysql_info(&mysql)?mysql_info(&mysql):"");

    return 0;
}

int mysql_TBL_CURSOR(char *pSql, void *MYDBTABLE)
{
    int ret = 0;

    ret = mysql_query(&mysql, pSql);  
    if (0 != ret)
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"Execute sql[%s] is error!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }
    if (NULL == (result = mysql_store_result(&mysql))) 
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"store result is error[%s]!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }
    return 0;
}


int mysql_TBL_OPEN(char *pSql, void *MYDBTABLE)
{
    num_rows = mysql_num_rows(result);             
    num_fields = mysql_num_fields(result);  
    ptFields = mysql_fetch_fields(result);
    return 0;
}

int mysql_TBL_FETCH(char *pSql, void *stTestDb, TABFUN *TBL_COLMAP_DEF)
{
    int i;
    int index;
    MYSQL_ROW row;            
    if(num_rows == 0)
    {
        return 1403;
    }
    if (NULL != (row = mysql_fetch_row(result))) 
    {
        for (index = 0; index < num_fields; index++)
        {
			//i = 0;
            i = index;
            while(TBL_COLMAP_DEF[i].f_colname[0] != 0x00 && row[index] != NULL)
            {
                if(strcasecmp(ptFields[index].name, TBL_COLMAP_DEF[i].f_colname) == 0)
                {
					//HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%s]", ptFields[index].name, row[index]);
					switch(TBL_COLMAP_DEF[i].type)
					{
						case 'C':
							strcpy((char *)stTestDb + TBL_COLMAP_DEF[i].offset, row[index]);
							break;
						case 'T':
							strcpy((char *)stTestDb + TBL_COLMAP_DEF[i].offset, row[index]);
							break;
						case 'I':
							*((int *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = atoi(row[index]);
							break;
						case 'F':
							*((float *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = strtof(row[index], NULL);
							break;
						case 'L':
							*((long *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = atol(row[index]);
							break;
						case 'D':
							*((double *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = strtod(row[index], NULL);
							break;
						default:
							 HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"bad type");
					}
                } 
                i++;
            }
        }
    }
    else
        return 1403;
    return 0;
}   

int mysql_TBL_CLOSE(char *pSql, void *stTestDb)
{
    mysql_free_result(result);
    return 0;
}

int mysql_TBL_CURSOR1(char *pSql, void *MYDBTABLE)
{
    int ret = 0;

    ret = mysql_query(&mysql, pSql);  
    if (0 != ret)
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"Execute sql[%s] is error!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }
    if (NULL == (result1 = mysql_store_result(&mysql))) 
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"store result is error[%s]!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }
    return 0;
}


int mysql_TBL_OPEN1(char *pSql, void *MYDBTABLE)
{
    num_rows1 = mysql_num_rows(result1);             
    num_fields1 = mysql_num_fields(result1);  
    ptFields1 = mysql_fetch_fields(result1);
    return 0;
}

int mysql_TBL_FETCH1(char *pSql, void *stTestDb, TABFUN *TBL_COLMAP_DEF)
{
    int i;
    int index;
    MYSQL_ROW row;            
    if(num_rows1 == 0)
    {
        return 1403;
    }
    if (NULL != (row = mysql_fetch_row(result1))) 
    {
        for (index = 0; index < num_fields1; index++)
        {
            //i = 0;
            i = index;
            while(TBL_COLMAP_DEF[i].f_colname[0] != 0x00 && row[index] != NULL)
            {
                if(strcasecmp(ptFields1[index].name, TBL_COLMAP_DEF[i].f_colname) == 0)
                {
					//HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%s]", ptFields[index].name, row[index]);
					switch(TBL_COLMAP_DEF[i].type)
					{
						case 'C':
							strcpy((char *)stTestDb + TBL_COLMAP_DEF[i].offset, row[index]);
							break;
						case 'T':
							strcpy((char *)stTestDb + TBL_COLMAP_DEF[i].offset, row[index]);
							break;
						case 'I':
							*((int *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = atoi(row[index]);
							break;
						case 'F':
							*((float *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = strtof(row[index], NULL);
							break;
						case 'L':
							*((long *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = atol(row[index]);
							break;
						case 'D':
							*((double *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = strtod(row[index], NULL);
							break;
						default:	
							 HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"bad type");
					}
                } 
                i++;
            }
        }
    }
    else
        return 1403;
    return 0;
}   

int mysql_TBL_CLOSE1(char *pSql, void *stTestDb)
{
    mysql_free_result(result1);
    return 0;
}

int mysql_TBL_CURSOR2(char *pSql, void *MYDBTABLE)
{
    int ret = 0;

    ret = mysql_query(&mysql, pSql);  
    if (0 != ret)
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"Execute sql[%s] is error!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }
    if (NULL == (result2 = mysql_store_result(&mysql))) 
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"store result is error[%s]!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }
    return 0;
}


int mysql_TBL_OPEN2(char *pSql, void *MYDBTABLE)
{
    num_rows2 = mysql_num_rows(result2);             
    num_fields2 = mysql_num_fields(result2);  
    ptFields2 = mysql_fetch_fields(result2);
    return 0;
}

int mysql_TBL_FETCH2(char *pSql, void *stTestDb, TABFUN *TBL_COLMAP_DEF)
{
    int i;
    int index;
    MYSQL_ROW row;            
    if(num_rows2 == 0)
    {
        return 1403;
    }
    if (NULL != (row = mysql_fetch_row(result2))) 
    {
        for (index = 0; index < num_fields2; index++)
        {
            //i = 0;
            i = index;
            while(TBL_COLMAP_DEF[i].f_colname[0] != 0x00 && row[index] != NULL)
            {
                if(strcasecmp(ptFields1[index].name, TBL_COLMAP_DEF[i].f_colname) == 0)
                {
					//HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%s]", ptFields[index].name, row[index]);
					switch(TBL_COLMAP_DEF[i].type)
					{
						case 'C':
							strcpy((char *)stTestDb + TBL_COLMAP_DEF[i].offset, row[index]);
							break;
						case 'T':
							strcpy((char *)stTestDb + TBL_COLMAP_DEF[i].offset, row[index]);
							break;
						case 'I':
							*((int *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = atoi(row[index]);
							break;
						case 'F':
							*((float *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = strtof(row[index], NULL);
							break;
						case 'L':
							*((long *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = atol(row[index]);
							break;
						case 'D':
							*((double *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = strtod(row[index], NULL);
							break;
						default:	
							 HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"bad type");
					}
                } 
                i++;
            }
        }
    }
    else
        return 1403;
    return 0;
}   

int mysql_TBL_CLOSE2(char *pSql, void *stTestDb)
{
    mysql_free_result(result2);
    return 0;
}
int mysql_TBL_CURSOR3(char *pSql, void *MYDBTABLE)
{
    int ret = 0;

    ret = mysql_query(&mysql, pSql);  
    if (0 != ret)
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"Execute sql[%s] is error!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }
    if (NULL == (result3 = mysql_store_result(&mysql))) 
    {
        HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"store result is error[%s]!errno[%d],errmsg[%s].\n", pSql, mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }
    return 0;
}


int mysql_TBL_OPEN3(char *pSql, void *MYDBTABLE)
{
    num_rows3 = mysql_num_rows(result3);             
    num_fields3 = mysql_num_fields(result3);  
    ptFields3 = mysql_fetch_fields(result3);
    return 0;
}

int mysql_TBL_FETCH3(char *pSql, void *stTestDb, TABFUN *TBL_COLMAP_DEF)
{
    int i;
    int index;
    MYSQL_ROW row;            
    if(num_rows3 == 0)
    {
        return 1403;
    }
    if (NULL != (row = mysql_fetch_row(result3))) 
    {
        for (index = 0; index < num_fields3; index++)
        {
            //i = 0;
            i = index;
            while(TBL_COLMAP_DEF[i].f_colname[0] != 0x00 && row[index] != NULL)
            {
                if(strcasecmp(ptFields1[index].name, TBL_COLMAP_DEF[i].f_colname) == 0)
                {
					//HtLog(gsLogFile,HT_LOG_MODE_DEBUG,__FILE__,__LINE__,"[%s]=[%s]", ptFields[index].name, row[index]);
					switch(TBL_COLMAP_DEF[i].type)
					{
						case 'C':
							strcpy((char *)stTestDb + TBL_COLMAP_DEF[i].offset, row[index]);
							break;
						case 'T':
							strcpy((char *)stTestDb + TBL_COLMAP_DEF[i].offset, row[index]);
							break;
						case 'I':
							*((int *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = atoi(row[index]);
							break;
						case 'F':
							*((float *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = strtof(row[index], NULL);
							break;
						case 'L':
							*((long *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = atol(row[index]);
							break;
						case 'D':
							*((double *)((char *)stTestDb+ TBL_COLMAP_DEF[i].offset)) = strtod(row[index], NULL);
							break;
						default:	
							 HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"bad type");
					}
                } 
                i++;
            }
        }
    }
    else
        return 1403;
    return 0;
}   

int mysql_TBL_CLOSE3(char *pSql, void *stTestDb)
{
    mysql_free_result(result3);
    return 0;
}
