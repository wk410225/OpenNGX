#include "DbsDef.h"
#include "MyDbsDef.h"
struct TBL_msq_inf_Def
{
	char	msq_int_id[5];
	char	msq_key[17];
	char	msq_type[9];
} ltTblMsqInf;

TABFUN TBL_MSQ_INF_COLMAP[MAXCOLNUM]={
    {"msq_int_id",  offsetof(struct TBL_msq_inf_Def, msq_int_id), 'C'},
    {"msq_key",     offsetof(struct TBL_msq_inf_Def, msq_key), 'C'},
    {"msq_type",    offsetof(struct TBL_msq_inf_Def, msq_type), 'C'},
};     

int DbsMSQINF( int vnOprType, struct TBL_msq_inf_Def *vtpTblMsqInf) 
{
    char pSql[MAXSQLLEN];
    int    iRet = 0;
    memcpy( &ltTblMsqInf, vtpTblMsqInf, sizeof(ltTblMsqInf));
    switch(vnOprType) {
    case DBS_SELECT:

        memset(pSql, 0x00, sizeof(pSql));
        sprintf(pSql, "select msq_key,msq_type from tbl_msq_inf where msq_int_id='%s' ;", ltTblMsqInf.msq_int_id);
        iRet = TBL_DEF_ONE_SELECT(pSql, &ltTblMsqInf, TBL_MSQ_INF_COLMAP);
        memcpy( vtpTblMsqInf, &ltTblMsqInf, sizeof(ltTblMsqInf));          
        return iRet;


    case DBS_CURSOR:

        memset(pSql, 0x00, sizeof(pSql));
        sprintf(pSql, "select * from tbl_msq_inf order by msq_int_id ;");
        return mysql_TBL_CURSOR(pSql, &ltTblMsqInf);
        
    case DBS_OPEN:
        return mysql_TBL_OPEN(pSql, &ltTblMsqInf);
            
    case DBS_FETCH:
        iRet = mysql_TBL_FETCH(NULL, &ltTblMsqInf, TBL_MSQ_INF_COLMAP);
        memcpy( vtpTblMsqInf, &ltTblMsqInf, sizeof(ltTblMsqInf));          
        return iRet;
    
    case DBS_CLOSE:
        return mysql_TBL_CLOSE(pSql, &ltTblMsqInf);

    default:
        return -1; 
    } /* end of switch */
} /* end of DbsMSQINF */
