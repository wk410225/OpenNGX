#include "DbsDef.h"
#include "MyDbsDef.h"

struct tbl_srv_param_def
{
    long    usage_key;
    char    srv_id[5];
    char    param_usage[2];
    char    param_index[3];
    char    param_data[65];
    char    param_dsp[65];
} ltTblSrvParam;

TABFUN TBL_SRV_PARAM_COLMAP[MAXCOLNUM]={
    {"usage_key",     offsetof(struct tbl_srv_param_def, usage_key), 'L'},
    {"srv_id",        offsetof(struct tbl_srv_param_def, srv_id), 'C'},
    {"param_usage",   offsetof(struct tbl_srv_param_def, param_usage), 'C'},
    {"param_index",   offsetof(struct tbl_srv_param_def, param_index), 'C'},
    {"param_data",    offsetof(struct tbl_srv_param_def, param_data), 'C'},
    {"param_dsp",     offsetof(struct tbl_srv_param_def, param_dsp), 'C'},
};

int DbsSrvParam(int nFunc, struct tbl_srv_param_def *ptSrvParam)
{
    char    pSql[MAXSQLLEN];
    int     iRet = 0;
    memcpy((char *)&ltTblSrvParam, (char *)ptSrvParam, sizeof(ltTblSrvParam));

    switch(nFunc)
    {
	case DBS_INSERT:
        return mysql_TBL_INSERT("tbl_srv_param", &ltTblSrvParam, TBL_SRV_PARAM_COLMAP);
        break;

    case DBS_SELECT:
        memset(pSql, 0x00, sizeof(pSql));
        sprintf(pSql, "select param_data from tbl_srv_param where usage_key=%d and srv_id = '%s' and  param_usage='%s' and param_index='%s';", \
             ltTblSrvParam.usage_key,ltTblSrvParam.srv_id, ltTblSrvParam.param_usage, "01");

	    HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "pSql=[%s]", pSql);
        iRet = TBL_DEF_ONE_SELECT(pSql, &ltTblSrvParam, TBL_SRV_PARAM_COLMAP);
        if(iRet == 0)
        {
            memcpy((char *)ptSrvParam, (char *)&ltTblSrvParam, sizeof(ltTblSrvParam));
        }
        return iRet;

    case DBS_CURSOR:

        memset(pSql, 0x00, sizeof(pSql));
        sprintf(pSql, "select param_data from tbl_srv_param where usage_key=%d and srv_id='%s' and param_usage='%s' order by param_index ;", ltTblSrvParam.usage_key, ltTblSrvParam.srv_id, ltTblSrvParam.param_usage);
	    HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "pSql=[%s]", pSql);
        return mysql_TBL_CURSOR1(pSql, &ltTblSrvParam);

    case DBS_OPEN:
        return mysql_TBL_OPEN1(pSql, &ltTblSrvParam);

    case DBS_FETCH:
        iRet = mysql_TBL_FETCH1(NULL, &ltTblSrvParam, TBL_SRV_PARAM_COLMAP);
        if(iRet ==0 )
        {
        	memcpy((char *)ptSrvParam, (char *)&ltTblSrvParam, sizeof(ltTblSrvParam));
			//HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "ltTblSrvParam.param_data=[%s]", ltTblSrvParam.param_data);
        }
        return iRet;

    case DBS_CLOSE:
        return mysql_TBL_CLOSE1(pSql, NULL);

    default:
        return -1;
    }
}
