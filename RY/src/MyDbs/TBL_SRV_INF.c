#include "DbsDef.h"
#include "MyDbsDef.h"

struct Tbl_Srv_Inf_Def
{ 
    long    usage_key;
    char    srv_id[5];
    char    srv_name[21];
    char    msq_int_id[5];
    char    srv_num[3];
    char    relate_srv_id[2049];
    char    srv_dsp[65];
} ltTblSrvInf;


TABFUN TBL_SRV_INF_COLMAP[MAXCOLNUM]={
    {"usage_key",     offsetof(struct Tbl_Srv_Inf_Def, usage_key), 'L'},
    {"srv_id",        offsetof(struct Tbl_Srv_Inf_Def, srv_id),    'C'},
    {"srv_name",      offsetof(struct Tbl_Srv_Inf_Def, srv_name),  'C'},
    {"msq_int_id",    offsetof(struct Tbl_Srv_Inf_Def, msq_int_id),'C'},
    {"srv_num",       offsetof(struct Tbl_Srv_Inf_Def, srv_num),   'C'},
    {"relate_srv_id", offsetof(struct Tbl_Srv_Inf_Def, relate_srv_id), 'C'},
    {"srv_dsp",       offsetof(struct Tbl_Srv_Inf_Def, srv_dsp), 'C'}
};

int DbsSRVINF( int vnOprType, struct Tbl_Srv_Inf_Def *vtpTblSrvInf) 
{
    char pSql[MAXSQLLEN];
    int    iRet = 0;
    memcpy( &ltTblSrvInf, vtpTblSrvInf, sizeof(ltTblSrvInf));
    switch(vnOprType) {
    case DBS_SELECT:
        memset(pSql, 0x00, sizeof(pSql));
        sprintf(pSql, "select * from tbl_srv_inf where usage_key=%d and srv_id = '%s';", ltTblSrvInf.usage_key,ltTblSrvInf.srv_id);
        iRet = TBL_DEF_ONE_SELECT(pSql, &ltTblSrvInf, TBL_SRV_INF_COLMAP);
        memcpy( vtpTblSrvInf, &ltTblSrvInf, sizeof(ltTblSrvInf));           
        return iRet;
    case DBS_CURSOR:
        memset(pSql, 0x00, sizeof(pSql));
        sprintf(pSql, "select * from tbl_srv_inf where usage_key=%d order by srv_id ;", ltTblSrvInf.usage_key);
        return mysql_TBL_CURSOR(pSql, &ltTblSrvInf);
    case DBS_OPEN:
        return mysql_TBL_OPEN(pSql, &ltTblSrvInf);
    case DBS_FETCH:
        iRet = mysql_TBL_FETCH(NULL,  &ltTblSrvInf,TBL_SRV_INF_COLMAP);
        memcpy( vtpTblSrvInf, &ltTblSrvInf, sizeof(ltTblSrvInf));           
        return iRet ;
    case DBS_CLOSE:
        return mysql_TBL_CLOSE(pSql, &ltTblSrvInf);
    default:
        return -1; 
    } 
} 
