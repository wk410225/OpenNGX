#ifndef __SWITCH_H
#define __SWITCH_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <memory.h>
#include <ctype.h> 
#include <signal.h>
#include <unistd.h>
#include <stdint.h>

//#include "SrvDef.h"
#include "SrvParam.h"
#include "MsqOpr.h"
#include "HtLog.h"
#include "ErrCode.h"
#include "TxnNum.h"
#include "DbsDef.h"
#include "DbsTbl.h"
#include "Common.h"
#include <cjson/cJSON_Utils.h>
#include <cjson/cJSON.h>
#include <hiredis/hiredis.h>


#define SRV_SEQ_ID_LEN 2
#define MAXTXNS 400
#define MSG_SRC_ID_ANY      "9999"

//#define LINE_SEPARATOR "============================================="
typedef struct st_TXN_FUN
{
    char    caTxnNum[64+1];
    char	caMsgSrcId[SRV_ID_LEN+1];
    int     (*pfTxnFun)(char *, int );
    char    caTxnDsp[64];
} TXNFUN;

extern TXNFUN gaTxns[MAXTXNS];

typedef struct
{         
    char    f_colname[64+1];
    int     offset; 
    char    type;  
	char	jreq[64+1];
	char	jres[64+1];
	char	jrtype[64+1];
} TABFUN;  


#define MAXCOLNUM  128 
#define MAXINDNUM  16
#define MAXSQLLEN  8192

#define TXN_INF_NUM_MAX  200
//#define FLAG_YES "Y"
//#define FLAG_NO "N"
//#define FLAG_YES_C 'Y'
//#define FLAG_NO_C 'N'
extern char		gsSrvId[SRV_ID_LEN+1];
extern char		gsSrvSeqId[SRV_SEQ_ID_LEN+1];
extern char		gsLogFile[LOG_NAME_LEN_MAX];
//extern int		gnTxnInfNum;
//extern Tbl_txn_inf_Def		gatTxnInf[TXN_INF_NUM_MAX];
extern T_SrvMsq				gatSrvMsq[SRV_MSQ_NUM_MAX];

int SwitchInit (short argc, char **argv);
void HandleExit (int n);
int DbsConnectTest();

int DbsSRVINF (int , Tbl_srv_inf_Def *);

void J2S(cJSON *root , void *stdb, TABFUN *);
void NJ2S(cJSON *root , void *stdb, TABFUN *);
void S2J(cJSON *root , void *stdb, TABFUN *);
void NS2J(cJSON *root , void *stdb, TABFUN *);
void GetQuerySortLimit(char *IPC, char *where );
void NGetQuerySortLimit(char *IPC, char *where );
void GetQueryWhere(char *IPC, char *where , TABFUN *);
void NGetQueryWhere(char *IPC, char *where , TABFUN *);
void GetQueryWhereSchema(char *IPC, char *where , TABFUN *TBL_DB_DEF_COLMAP, char *schema);
int  NGetDeleteWhereSql(cJSON *, char *, TABFUN *TBL_DB_DEF_COLMAP);


int tbl_srv_param(char *, int nIndex);
int tbl_xml_ipc_dsp(char *, int nIndex);
int tbl_xml_ipc_values(char *, int nIndex);
int tbl_buf_chg(char *, int nIndex);
int tbl_fld_tsf(char *, int nIndex);
int tbl_fld_dsp(char *, int nIndex);
int tbl_buf_dsp_dsp(char *, int nIndex);
int tbl_buf_dsp(char *, int nIndex);
int GetIpcValueExt(char *, char *, char *, int );
int SetIpcValue(char *, char *, char *);
int mysql_TBL_INSERT(char *, void *, TABFUN * );
int mysql_ONE_SELECT(char *, void *, TABFUN * );

void mysql_TBL_CURSOR(char *, char *);
void mysql_TBL_OPEN(char *, char *);
int  mysql_TBL_FETCH(char *, void *, TABFUN *);
void mysql_TBL_CLOSE(char *, char *);

void mysql_TBL_CURSOR1(char *, char *);
void mysql_TBL_OPEN1(char *, char *);
int  mysql_TBL_FETCH1(char *, void *, TABFUN *);
void mysql_TBL_CLOSE1(char *, char *);

void mysql_TBL_CURSOR2(char *, char *);
void mysql_TBL_OPEN2(char *, char *);
int  mysql_TBL_FETCH2(char *, void *, TABFUN *);
void mysql_TBL_CLOSE2(char *, char *);

int  mysql_TBL_UPDATE(char *);
int  mysql_TBL_DELETE(char *);
int  mysql_TBL_EXECUTE(char *);

void PrintTbl(void *stdb, TABFUN *TBL_DB_DEF_COLMAP);

void  GetUpdateSetSql(cJSON *, char *, TABFUN *);
void  NGetUpdateSetSql(cJSON *, char *, TABFUN *);
int GetUpdateWhereSql(cJSON *, char *, TABFUN *);
int NGetUpdateWhereSql(cJSON *, char *, TABFUN *);
int GetDeleteWhereSql(cJSON *, char *, TABFUN *);

int generate_jwt_token(char *);
int  validate_jwt_token(char *, int *);
char* cJSON_SetString(cJSON *object, const char *string);

int captchaImage(char *, int nIndex);
int login(char *, int nIndex);
int logout(char *, int nIndex);
int getInfo(char *, int nIndex);
int getRouters(char *, int nIndex);
int system_dict_data_type(char *, int nIndex);
int system_dict_type(char *, int nIndex);
int system_dict_data(char *, int nIndex);
int system_user(char *, int nIndex);
int system_role(char *, int nIndex);
int system_config(char *, int nIndex);
int system_menu(char *, int nIndex);
int system_dept(char *, int nIndex);
int system_post(char *, int nIndex);
int system_notice(char *, int nIndex);
int monitor_operlog(char *, int nIndex);
int monitor_logininfor(char *, int nIndex);
int monitor_online(char *, int nIndex);
int monitor_job(char *, int nIndex);
int monitor_jobLog(char *, int nIndex);
int monitor_cache(char *, int nIndex);
int monitor_server(char *, int nIndex);
char * get_last_component(char* path);
void hash_password(char *, char *);
void GetCurrTime(char *);
char *get_uuid();
char *GetIpcPtr(char *,char *);
int SetIpcLongValue(char *, char *, long );
long GetIpcLongValue(char *, char *);
void get_keys_by_pattern( const char *pattern, int *count, char ***keys, int max_keys);
char* get_key_value(const char *key);
int validate_token(char *IPC, cJSON *out);
char* get_last(char *, char);




void showIpc(char *);


#endif
