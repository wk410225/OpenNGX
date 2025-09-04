#ifndef __SWT_TXN_H
#define __SWT_TXN_H

TXNFUN gaTxns[MAXTXNS]=
{
    { "/topconfig/tblsrvparam",		"2810",	 tbl_srv_param		,"服务参数表" },
    { "/tbl_xml_ipc_dsp",	"2810", tbl_xml_ipc_dsp	,    "xml peizhi" },
    { "/tbl_xml_ipc_values", "2810", tbl_xml_ipc_values	,"xml peizhi" },
/*
    { "tbl_xml_trans_dsp",	"2810", tbl_xml_trans_dsp	,    "xml peizhi" },
    { "tbl_xml_values", "2810", tbl_xml_values	,"xml peizhi" },
*/
    { "/tbl_buf_dsp", "2810", tbl_buf_dsp,"xml peizhi" },
    { "/tbl_buf_dsp_dsp", "2810", tbl_buf_dsp_dsp,"xml peizhi" },
    { "/tbl_buf_chg", "2810", tbl_buf_chg,"xml peizhi" },
    { "/tbl_fld_tsf", "2810", tbl_fld_tsf,"xml peizhi" },
    { "/tbl_fld_dsp", "2810", tbl_fld_dsp,"xml peizhi" },
    { "/captchaImage", "2810", captchaImage,"ruoyi " },
    { "/login", "2810", login,"ruoyi " },
    { "/logout", "2810", logout,"ruoyi " },
    { "/getInfo", "2810", getInfo,"ruoyi " },
    { "/getRouters", "2810", getRouters,"ruoyi " },
    { "/system/dict/data/type", "2810", system_dict_data_type,"ruoyi " },
    { "/system/dict/type", "2810", system_dict_type,"ruoyi " },
    { "/system/dict/data", "2810", system_dict_data,"ruoyi " },
    { "/system/user", "2810", system_user,"ruoyi " },
    { "/system/role", "2810", system_role,"ruoyi " },
    { "/system/menu", "2810", system_menu,"ruoyi " },
    { "/system/dept", "2810", system_dept,"ruoyi " },
    { "/system/post", "2810", system_post,"ruoyi " },
    { "/system/config", "2810", system_config,"ruoyi " },
    { "/system/notice", "2810", system_notice,"ruoyi " },
    { "/monitor/operlog", "2810", monitor_operlog,"ruoyi " },
    { "/monitor/logininfor", "2810", monitor_logininfor,"ruoyi " },
    { "/monitor/online", "2810", monitor_online,"ruoyi " },
    { "/monitor/jobLog", "2810", monitor_jobLog,"ruoyi " },
    { "/monitor/job", "2810", monitor_job,"ruoyi " },
    { "/monitor/cache", "2810", monitor_cache,"ruoyi " },
    { "/monitor/server", "2810", monitor_server,"ruoyi " },
};

#endif

