rm -f *.sql
myexp "select * from tbl_srv_inf;" > tbl_srv_inf.sql
#myexp "select * from tbl_srv_param where srv_id in ('1106','2810');" > tbl_srv_param.sql
#myexp "select * from tbl_srv_param where 1=1;" > tbl_srv_param.sql
#myexp "select * from tbl_msg_inf;" > tbl_msg_inf.sql
#myexp "select * from tbl_ipc_inf;" > tbl_ipc_inf.sql
#myexp "select * from tbl_msq_inf;" > tbl_msq_inf.sql
#myexp "select * from tbl_xml_ipc_values where l_usage_key in (1,2,3,4,5,6,7,8);" > tbl_xml_ipc_values.sql
#myexp "select * from tbl_xml_ipc_values where l_usage_key in (9);" > tbl_xml_ipc_values.sql
#myexp "select * from tbl_xml_ipc_dsp where usage_key in (5);" > tbl_xml_ipc_dsp.sql
#myexp "select * from tbl_txn_inf where 1=1;" > tbl_txn_inf.sql
#myexp "select * from tbl_user_inf;" > tbl_user_inf.sql
#myexp "select * from tbl_xml_ipc_dsp where usage_key=9;" > tbl_xml_ipc_dsp.sql
#myexp "select * from tbl_txn where trans_state='S' and inst_date='20231130';" > tbl_txn.sql
#myexp "select * from tbl_txn where  inst_date='20231130';" > tbl_txn.sql
#myexp "select * from tbl_mcht_chg where  1=1;" > tbl_mcht_chg.sql
#myexp "select * from tbl_inst_key_inf where  1=1;" > tbl_inst_key_inf.sql
#myexp "select * from tbl_pos_key_inf where  1=1;" > tbl_pos_key_inf.sql
#myexp "select * from tbl_card_bin_inf where  1=1;" > tbl_card_bin_inf.sql
#myexp "select * from tbl_saf_saf_ipc where  1=1;" > tbl_saf_saf_ipc.sql
#myexp "select * from tbl_saf_ipc where  1=1;" > tbl_saf_ipc.sql
#myexp "select * from tbl_mcht_settle_inf where  1=1;" > tbl_mcht_settle_inf.sql
#myexp "select * from tbl_inst_key_inf where  1=1;" > tbl_inst_key_inf.sql
#myexp "select * from tbl_pos_key_inf where  1=1;" > tbl_pos_key_inf.sql
#myexp "select * from tbl_rsa_key_inf where  1=1;" > tbl_rsa_key_inf.sql
#myexp "select * from tbl_conv_type where  1=1;" > tbl_conv_type.sql
#myexp "select * from tbl_fld_inf where  1=1;" > tbl_fld_inf.sql
#myexp "select * from tbl_bmp_inf where  1=1;" > tbl_bmp_inf.sql
#myexp "SELECT DISTINCT m.* FROM sys_menu m LEFT JOIN sys_role_menu rm ON m.menu_id = rm.menu_id LEFT JOIN sys_user_role ur ON rm.role_id = ur.role_id LEFT JOIN sys_role r ON ur.role_id = r.role_id WHERE ur.user_id = 1 AND m.menu_type IN ('M', 'C') AND m.status = 0 AND r.status = 0 ORDER BY m.parent_id, m.order_num"
#myexp "select * from sys_user;" >  sys_user.sql
