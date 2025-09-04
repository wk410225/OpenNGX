#ifndef _DBS_TBL_H
#define _DBS_TBL_H
#include <string.h>  
#include <stddef.h>

#define INS_INF_NUM_MAX 200

typedef struct
{
	long	usage_key;
	long	bmp_index;
	char	bmp_val[33];
} Tbl_bmp_inf_Def;

typedef struct
{
	char	srv_id[5];
	char	buf_chg_type[2];
	long	usage_key;
} Tbl_conv_type_Def;

typedef struct
{
	long	usage_key;
	long	txn_num;
	long	con_index;
	long	fld_index;
	long	begin_byte_pos;
	long	format_chg_need;
	char	val[36];
} Tbl_con_inf_Def;

typedef struct
{
	long	usage_key;
	long	fld_index;
	long	len_len;
	long	data_max_len;
	long	len_exp_val;
	long	ind_sym;
	long	data_format;
	long	len_char_set;
	long	data_char_set;
} Tbl_fld_inf_Def;

typedef struct
{
	long	usage_key;
	long	ipc_index;
	char	ipc_val[513];
} Tbl_ipc_inf_Def;

typedef struct
{
	long	usage_key;
	char	srv_id[5];
	char	line_index[3];
	char	line_state[3];
	char	line_max[3];
	char	line_dsp[65];
} Tbl_line_inf_Def;

typedef struct {
	long	usage_key;
	char	srv_id[5];
	short	line_index;
	char	local_addr[16];
	char	remote_addr[16];
	int	in_sock_num;
	int	out_sock_num;
} Tbl_line_cfg_Def;

typedef struct 
{
	char	srv_id[5];
	char	srv_id_bak[5];
} Tbl_line_bak_Def;

typedef struct
{  
	char    ins_id_cd[12];  
	char    ins_id_cd_32[12];  
	char    sys_state[2];  
	char    bf_stlm_date[9];  
	char    cur_stlm_date[9];  
	long    date_cut_time;  
	char    bf_txn_tbl[2];  
	char    cur_txn_tbl[2];
} Tbl_bat_cut_ctl_Def;

typedef struct
{    
	char   mcht_cd[16];    
	char   term_id[9];    
	char   term_sta[2];    
	char   term_sign_sta[2];    
	char   chk_sta[2];    
	char   term_mcc[5];    
	char   term_single_limit[13];    
	char   term_batch_nm[7];    
	char   term_stlm_dt[9];	
	char    psam_id[9];	
	char    term_place[2];	
	char    bind_tel[15];	
	char    run_main_id_2[14];	
	char    run_main_nm_2[51];
} Tbl_term_inf_Def;

typedef struct
{
	char	insert_time[15];
	char	update_time[15];
	char	sys_seq_num[7];
	char	msg_src_id[5];
	char	msg_dest_id[5];
	char	txn_num[5];
	char	trans_code[4];
	char	host_date[9];
	char	host_ssn[13];
	char	header_buf[47];
	char	msg_type[5];
	char	processing_code[7];
	char	trans_date_time[11];
	char	cup_ssn[13];
	char	time_local_trans[7];
	char	date_local_trans[5];
	char	date_settlmt[5];
	char	acq_inst_id_code[12];
	char	fwd_inst_id_code[12];
	char	resp_code[3];
	char	addtnl_data_len[4];
	char	addtnl_data[513];
	char	currcy_code_stlm[4];
	char	stlm_code[2];
	char	netwk_mgmt_code[4];
	char	crd_num[11];
	char	crd_revsal_num[11];
	char	dbt_num[11];
	char	dbt_revsal_num[11];
	char	xfer_num[11];
	char	xfer_revsal_num[11];
	char	inqury_num[11];
	char	authr_num[11];
	char	crd_proces_fee[13];
	char	dbt_proces_fee[13];
	char	crd_amt[17];
	char	crd_revsal_amt[17];
	char	dbt_amt[17];
	char	dbt_revsal_amt[17];
	char	amt_net_stlm[18];
	char	stlm_inst_id_len[3];
	char	stlm_inst_id[12];
	char	rcvg_code_len[3];
	char	rcvg_code[12];
	char	msq_type[17];
	char	misc_flag[33];
	char	misc_1[129];
} Tbl_mng_txn_Def;

typedef struct
{
	long	usage_key;
	long	txn_num;
	long	ipc_index;
	long	bmp_index;
	long	mand_bmp_index;
	char	msg_type[5];
	char	txn_code[4];
} Tbl_msg_inf_Def;

typedef struct
{
	char	msq_int_id[5];
	char	msq_key[17];
	char	msq_type[9];
} Tbl_msq_inf_Def;

typedef struct
{
	char	txn_num[5];
	char	msg_src_id[5];
	char	msg_to_id[5];
	char	check_card_flag[2];
} Tbl_route_inf_Def;

typedef struct
{
	char	src_id[5];
	char	dest_id[5];
	char	src_rsp_code[129];
	long	src_rsp_code_l;
	char	dest_rsp_code[129];
	long	dest_rsp_code_l;
	char	rsp_code_dsp[65];
} Tbl_rsp_code_map_Def;

typedef struct
{
	char    inst_date[9];
	char	inst_time[7];
	char	msg_src_id[5];
	char	txn_num[5];
	char	sys_seq_num[7];
	char	msg_dest_srv_id[5];
	char	send_count[3];
	char	center_stlm_date[9];
	char	host_stlm_date[9];
	char	msg_len[5];
	char	msg_ipc1[2049];
	char	msg_ipc2[2049];//add by Zhengzhou Kalian W at 2017-8-7, orig is : 2049 .
	char	msg_ipc3[2049];//add by  Zhengzhou Kalian W 20130918
	char	msg_ipc4[1025];//add by  Zhengzhou Kalian W 20130918
} Tbl_saf_msg_Def;

typedef struct
{
	long    usage_key;
	char	srv_id[5];
	char	srv_name[21];
	char	msq_int_id[5];
	char	srv_num[3];
	char	relate_srv_id[2049];
	char	srv_dsp[65];
} Tbl_srv_inf_Def;

typedef struct
{
	long    usage_key;
	char	srv_id[5];
	char	param_usage[2];
	char	param_index[3];
	char	param_data[65];
	char	param_dsp[65];
	long    total_rows;
} Tbl_srv_param_Def;

typedef struct
{
	char    ins_id_cd[12];
	char    acc1_offset[3];
	char    acc1_len[3];
	char    acc1_tnum[2];
	char    acc2_offset[3];
	char    acc2_len[3];
	char    acc2_tnum[2];
	char    bin_offset[3];
	char    bin_len[3];
	char    bin_sta_no[31];
	char    bin_end_no[31];
	char    bin_tnum[2];
	char    card_tp[3];
	char    card_dis[41]; /* add by  Zhengzhou Kalian W@20130718 */
} Tbl_bank_bin_inf_Def;

/* �⿨BIN��Ϣ�� */
typedef struct
{
	char    ins_id_cd[12];
	char    bin_sta_no[31];
	char    bin_end_no[31];
	char    bin_len[3];
	char    acc_len[3];
	char    card_tp[3];
} Tbl_forin_bin_inf_Def;

typedef struct
{
	char	ssn_type[2];
	char	ssn_value[7];
	char	ssn_min[7];
	char	ssn_max[7];
	char	buf_count[4];
} Tbl_ssn_Def;

typedef struct
{
	char	inst_date[15];
	char	updt_date[15];
	char	sys_seq_num[7];
	char	msg_src_id[5];
	char	msg_dest_id[5];
	char	txn_num[5];
	char	trans_code[4];
	char	trans_type[2];
	char	trans_state[2];
	char	revsal_flag[2];
	char	revsal_ssn[7];
	char	cancel_flag[2];
	char	cancel_ssn[7];
	char	host_date[9];
	char	host_ssn[13];
	char	term_ssn[13];
	char	order_no[41];
	char	inst_flag[9];
	char	key_rsp[49];
	char	key_revsal[49];
	char	key_cancel[49];
	char	header_buf[47];
	char	msg_type[5];
	char	pan_len[3];
	char	pan[20];
	char	processing_code[7];
	char	amt_trans[13];
	char	amt_settlmt[13];
	char	amt_cdhldr_bil[13];
	char	trans_date_time[11];
	char	conv_rate_stlm[9];
	char	conv_rate_cdhldr[9];
	char	cup_ssn[7];
	char	time_local_trans[7];
	char	date_local_trans[5];
	char	date_expr[5];
	char	date_settlmt[5];
	char	date_conv[5];
	char	mchnt_type[5];
	char	acq_cntry_code[4];
	char	pos_entry_mode[4];
	char	pos_cond_code[3];
	char	pos_pin_cap_code[3];
	char	amt_trans_fee[10];
	char	acq_inst_id_code[12];
	char	fwd_inst_id_code[12];
	char	track_2_data_len[3];
	char	track_2_data[38];
	char	track_3_data_len[4];
	char	track_3_data[105];
	char	retrivl_ref[13];
	char	authr_id_resp[7];
	char	resp_code[3];
	char	card_accp_term_id[9];
	char	card_accp_id[16];
	char	card_accp_name[61];
	char	addtnl_data_len[4];
	char	addtnl_data[513];
	char	track_1_data_len[3];
	char	track_1_data[80];
	char	currcy_code_trans[4];
	char	currcy_code_stlm[4];
	char	currcy_code_chldr[4];
	char	addtnl_amt_len[4];
	char	addtnl_amt[41];
	char	emv_len[4];
	char	emv_val[513];
	char	rsp_emv_len[4];
	char	rsp_emv_val[513];
	char	fld_reserved_len[4];
	char	fld_reserved[101];
	char	ch_auth_info_len[4];
	char	ch_auth_info[201];
	char	switch_data_len[4];
	char	switch_data[201];
	char	finacl_data_len[4];
	char	finacl_data[201];
	char	orig_data_elemts[43];
	char	replacement_amts[43];
	char	rcvg_code_len[3];
	char	rcvg_code[12];
	char	acct_id1_len[3];
	char	acct_id1[29];
	char	acct_id2_len[3];
	char	acct_id2[29];
	char	trans_descrpt_len[4];
	char	trans_descrpt[101];
	char	cup_swresved_len[4];
	char	cup_swresved[101];
	char	acq_swresved_len[4];
	char	acq_swresved[101];
	char	iss_swresved_len[4];
	char	iss_swresved[101];
	char	host_trans_fee1[13];
	char	host_trans_fee2[13];
	char	tlr_num[9];
	char	open_inst[16];
	char	stlm_inst[16];
	char	batch_flag[2];
	char	batch_date[9];
	char	msq_type[17];
	char	amt_return[13];
	char	authr_id_r[7];
	char	ips_mercht[16];
	char	misc_flag[33];
	char	misc_1[129];
	char	misc_2[129];
	char	card_type[2];
	char    extra_data[2048+1];
	char    extra_charge_type[20+1];
    char    extra_charge_value[10+1];
    char    cp_expect_fee[18+1];
    char    cp_fee[18+1];
	char	sy_card_accp_term_id[16+1];
	char	sy_card_accp_id[15+1];
	char	c_mcht_cd[32+1];
	char    cups_id[31];
    char    cups_terminal_id[16];
    char    cups_name[61];
    char    cups_orderid[33];
} Tbl_txn_Def;

typedef struct
{
	char	inst_date[15];
	char	updt_date[15];
	char	sys_seq_num[7];
	char	msg_src_id[5];
	char	msg_dest_id[5];
	char	txn_num[5];
	char	trans_code[4];
	char	trans_type[2];
	char	trans_state[2];
	char	revsal_flag[2];
	char	revsal_ssn[7];
	char	cancel_flag[2];
	char	cancel_ssn[7];
	char	host_date[9];
	char	host_ssn[13];
	char	term_ssn[13];
	char	order_no[41];
	char	inst_flag[9];
	char	key_rsp[49];
	char	key_revsal[49];
	char	key_cancel[49];
	char	header_buf[47];
	char	msg_type[5];
	char	pan_len[3];
	char	pan[20];
	char	processing_code[7];
	char	amt_trans[13];
	char	amt_settlmt[13];
	char	amt_cdhldr_bil[13];
	char	trans_date_time[11];
	char	conv_rate_stlm[9];
	char	conv_rate_cdhldr[9];
	char	cup_ssn[7];
	char	time_local_trans[7];
	char	date_local_trans[5];
	char	date_expr[5];
	char	date_settlmt[5];
	char	date_conv[5];
	char	mchnt_type[5];
	char	acq_cntry_code[4];
	char	pos_entry_mode[4];
	char	pos_cond_code[3];
	char	pos_pin_cap_code[3];
	char	amt_trans_fee[9];
	char	acq_inst_id_code[12];
	char	fwd_inst_id_code[12];
	char	track_2_data_len[3];
	char	track_2_data[38];
	char	track_3_data_len[4];
	char	track_3_data[105];
	char	retrivl_ref[13];
	char	authr_id_resp[7];
	char	resp_code[3];
	char	card_accp_term_id[9];
	char	card_accp_id[16];
	char	card_accp_name[41];
	char	track_1_data_len[3];
	char	track_1_data[80];
	char	addtnl_data_len[4];
	char	addtnl_data[513];
	char	currcy_code_trans[4];
	char	currcy_code_stlm[4];
	char	currcy_code_chldr[4];
	char	addtnl_amt_len[4];
	char	addtnl_amt[41];
	char	fld_reserved_len[4];
	char	fld_reserved[101];
	char	ch_auth_info_len[4];
	char	ch_auth_info[201];
	char	switch_data_len[4];
	char	switch_data[201];
	char	orig_data_elemts[43];
	char	replacement_amts[43];
	char	rcvg_code_len[3];
	char	rcvg_code[12];
	char	acct_id1_len[3];
	char	acct_id1[29];
	char	acct_id2_len[3];
	char	acct_id2[29];
	char	trans_descrpt_len[4];
	char	trans_descrpt[101];
	char	cup_swresved_len[4];
	char	cup_swresved[101];
	char	host_trans_fee1[13];
	char	host_trans_fee2[13];
	char	tlr_num[9];
	char	open_inst[16];
	char	stlm_inst[16];
	char	batch_flag[2];
	char	batch_date[9];
	char	msq_type[17];
	char	amt_return[13];
	char	authr_id_r[7];
	char	ips_mercht[16];
	char	misc_flag[33];
	char	misc_1[129];
	char	misc_2[129];
} Tbl_txn_his_Def;

typedef struct
{
	char	txn_num[5];
	char	msg_src_id[5];
	char	support_flag[2];
	char	rsp_type[2];
	char	msg_to[4];
	char	to_txn_num[5];
	char	msg_dest1[5];
	char	saf_count1[3];
	char	msg_dest2[5];
	char	saf_count2[3];
	char	rsp_dest_srv_id[5];
	char	rsp_txn_num[5];
	char	pin_flag[2];
	char	misc_flag[9];
	char	txn_dsp[129];
} Tbl_txn_inf_Def;

typedef struct
{
	char	srv_id[5];
	char	param_id[5];
	char	param_name[33];
	char	param_data[257];
	char	param_dsp[257];
} Tbl_usr_param_Def;

typedef struct
{
	short	usage_key;
	short	buf_chg_index;
	short	sour_buf_index;
	short	dest_buf_index;
	char	buf_dsp[257];
} Tbl_buf_chg_Def;

typedef struct
{
	short	usage_key;
	short	buf_dsp_index;
	short	pos_index;
	short	fld_index;
	short	fld_id;
	short	fld_offset;
} Tbl_buf_dsp_Def;

typedef struct
{
	short	usage_key;
	short	buf_dsp_index;
	char	buf_dsp[257];
} Tbl_buf_dsp_dsp_Def;

typedef struct
{
	short	usage_key;
	short	fld_id;
	short	fld_l;
	short	fld_type;
	char	fld_dsp[257];
} Tbl_fld_dsp_Def;

typedef struct
{
	short	usage_key;
	short	buf_chg_index;
	short	sour_fld_index;
	short	dest_fld_index;
} Tbl_fld_tsf_Def;

typedef struct
{
	short	usage_key;
	char	comp_key[11];
	short	comp_key_len;
	short	ipc_dft_index;
	short	buf_dsp_index;
	char	txn_num[5];
	char	ipc_dft[257];
} Tbl_ipc_dft_dsp_Def;

typedef struct
{
	short	usage_key;
	short	ipc_dft_index;
	short	pos_index;
	short	fld_index;
	short	fld_len;
	char	fld_val[601];
} Tbl_ipc_dft_Def;

typedef struct
{
	short	usage_key;
	short	debug_ipc_index;
	short	debug_flag;
	char	debug_ipc_dsp[256];
} Tbl_debug_ipc_dsp_Def;

typedef struct
{
	short	usage_key;
	short	debug_ipc_index;
	short	pos_index;
	char	fld_name[21];
	short	fld_len;
	short	fld_type;
	short	debug_flag;
} Tbl_debug_ipc_Def;

typedef struct
{
	char	srv_id[5];
	char	msq_type[17];
	char	creation_time[15];
} Tbl_dumpmsg_Def;

/*typedef struct
  {
  char inst_code[14];
  char inst_stat[2];
  char srv_id[5];
  char slmt_date[5];
  char inst_app_code[3];
  char inst_key_index[5];
  char branch_code[4];
  char mac_flag[2];
  char pin_flag[2];
  } Tbl_ins_inf_Def; add by  Zhengzhou Kalian W 20120217*/

typedef struct
{
	char       inst_code[14];
	char       inst_name[81];
	char       inst_stat[2];
	char       srv_id[5];
	char       slmt_year[5];
	char       pre_slmt_year[5];
	char       slmt_date[5];
	char       pre_slmt_date[5];
	char       inst_key_index[5];
	char       newkey[33];
	char       inst_app_code[3];
	char       branch_code[4];
	char       mac_flag[2];
	char       pin_flag[2];
	char       insert_time[15];
	char       update_time[15];
}Tbl_ins_inf_Def;

typedef struct
{
	int 	InstNum;
	Tbl_ins_inf_Def taInsInf[INS_INF_NUM_MAX];
}InstInfo;

typedef struct 
{
	char	card_id[17];
	long	card_id_offset;
	long 	card_id_len;
	char	branch_no[17];
	long	branch_no_offset;
	long 	branch_no_len;
	char	inst_code[14]; 
	char	dest_srv_id[5];
	char	txn_num[5];
	char	card_type[3];
} Tbl_card_route_Def;

typedef struct 
{
	char	sa_card_no[20];
	char	sa_limit_amt[13];
	char	sa_action[2];
	char	sa_state[2];
	char    sa_mcht_no[16];
} Tbl_ctl_card_inf_Def;

typedef struct 
{
	char	sa_mer_no[16];
	char	sa_limit_amt[13];
	char	sa_action[2];
	char	sa_state[2];
} Tbl_ctl_mcht_inf_Def;

typedef struct 
{
	char	sa_branch_code[7];
	char	sa_model_kind[3];
	char    sa_model_name[64+1]; /* add by  Zhengzhou Kalian W @20130716 */
	char    sa_risk_level[1+1]; /* add by  Zhengzhou Kalian W @20130716 */
	char    sa_mail_flag[1+1]; /* add by  Zhengzhou Kalian W @20130716 */
	char	sa_be_use[2];
	char	sa_action[2];
	char	sa_days[4];
	char	sa_limit_num[9];
	char	sa_limit_amount[13];
	double  sa_warnlvt;/* add by  Zhengzhou Kalian W @20130716 */
} Tbl_risk_inf_Def;

typedef struct
{
	char	sa_step_key[49];
	char	sa_tpcs_key[49];
	char	sa_txn_card[20];
	char	sa_expr_date[5];
	char	sa_mcht_no[16];
	char	sa_term_no[9];
	char	sa_txn_num[5];
	char	sa_txn_date[9];
	char	sa_txn_time[7];
	char	sa_txn_amt[13];
	char	sa_msg_src_id[5];
	char	sa_msg_dest_id[5];
	char	sa_rsp_code[3];
	char	sa_clc_flag[2];
	char	sa_clc_rsn1[51];
	char	sa_clc_rsn2[51];
	char	sa_note_txt[65];	/* char	sa_note_txt[51];  add by Zhengzhou Kalian Wiang @ 20130814*/
	char	sa_open_inst[4];
	char	sa_stlm_inst[11];
	char	sa_updt_time[15];
	char	sa_inst_time[15];
	char 	sa_inst_id_code[9] ;
} Tbl_clc_mon_Def;

typedef struct
{
	char 	sa_inst_id_code[8+1] ;
	char	sa_txn_card[20];
	char	sa_mcht_no[16];
	char	sa_txn_date[9];
	char	sa_ins_flag[2];
	char	sa_txn_amt[13];
	char	sa_txn_num[9];
} Tbl_txn_sts_Def;

typedef struct
{
	char	sa_inst_id[14];
	char	sa_term_id[9];
	char	sa_cup_term_id[9];
	char	sa_jcb_mcht_id[16];
	char	sa_mcht_addr[41];
	char	sa_mcht_type[5];
	char	sa_term_lock[2];
	char	sa_term_stat[2];
} Tbl_atm_inf_Def;

typedef struct 
{
	long	l_usage_key;
	char	sa_fc_auth_rsp_code[4];
	char	sa_app_m2[2];
	char	sa_app_m4[2];
} Tbl_auth_rsp_info_Def;

typedef struct 
{
	char	sa_bank_no[7];
	char	sa_misc[21];
} Tbl_bank_info_Def;

typedef struct
{
	char	sa_txn_num[5];
	char	sa_pan_len[3];
	char	sa_pan[20];
	char	sa_amt_trans[13];
	char	sa_date_local_trans[5];
	char	sa_revsal_flag[2];
	char	sa_resp_code[3];
}Tbl_f_txn_check;

/* CODING BY LINYUN AT 2009/12/20 */
typedef struct   															
{
	char mchnt_no[15+1];
	char mchnt_st[1+1];
	//  �̶����� 00  02������ȡ  03ָ�������µ�
	char fee_type[2+1] ;
	//   ����   fee_type Ϊ0ʱʹ��
	char txn_fee[10+1];
	//   fee_typeΪ2ʱ  ���ʽ��
	char txn_amt[10+1] ;
	// fee_type Ϊ0 ����3ʱ  �������
	char txn_max_amt[20+1] ;
	char txn_ori_fee[10+1] ;
	char txn_down_fee[10+1] ;
	char txn_down_amt[20+1] ;
	char txn_down_date[8+1];
	//const amount
	char fix_amt[10+1] ;
	char div_fee_type[1+1] ;
	char div_fee_rate[10+1] ;
	char div_max_amt[12+1] ;
	char div_amt[12+1] ;
} Get_FeeInf_Def;
/* CODING BY LINYUN AT 2009/12/20 */
/*for Pos*/

typedef struct 
{
	char mchnt_no[16];
	char mchnt_st[1+1];
	char mcc[5];
	char cn_nm[61];
	char sign_inst_id[13+1];
	char print_inst_id[13+1];
	char brh_id[12+1];
	char point[8+1] ;
	char open_time[6+1];
	char close_time[6+1] ;
	char ispoint[1+1];
} Tbl_mcht_inf_Def;

typedef struct  
{
	char    mcht_cd[16];
	char    term_id[9];
	char    term_sta[2];
	char 	term_type[3] ;
	char    term_sign_sta[2];
	char    chk_sta[2];
	char    term_mcc[5];
	char    term_single_limit[13];
	char    term_batch_nm[7];
	char    term_stlm_dt[9];
	char    psam_id[9];
	char    term_place[2];
	char    bind_tel[15];
	char    run_main_id_2[14];
	char    run_main_nm_2[51];
} TBL_term_inf_Def;

typedef struct  
{
	char mcc[5];
	char ret_fee[1+1];
}TBL_mcc_inf;

typedef struct 
{
	char mchnt_id[15+1] ;
	char installment[2+1] ;
	char product_id[20+1] ;
}TBL_installment_inf;

typedef struct 
{
	char	mcht_cd[16]         ;   /* �̻��� */
	char 	term_id[9]         ;    /* �ն˺� */
	char 	txn_num[5]       ;      /* ���״��� */
	char    card_type[3];
	char 	month[3]         ;      /* �·� */
	char 	day[3]         ;  	    /* ���� */
	/*    char  m_count[6]    ; */        /* ���ۼƱ��� */
	/*    char  d_count[6]    ; */        /* ���ۼƱ��� */
	int		m_count;
	int		d_count;
	double 		m_total      ; 	    /* ���ۼƽ�� */
	double 		d_total   ; 	      /* ���ۼƽ�� */
	char 	year[5]         ;  	    /* ��     add by  Zhengzhou Kalian W @ 20130819*/
}  CST_MERTRANS_CNT_DEF;

typedef struct
{
	char	MCHT_CD     [16]    ;   /* �̻��� */
	char	TXN_NUM     [5]     ;   /* ���״��� */
	char	CARD_TYPE   [3]     ;   /* ���� 00 ��� 01 ���� 02 ���п�*/
	char	CHANNEL     [3]     ;   /* �޶��־ 0-�ض� 1-Ĭ�� */
	char	DAY_NUM     [6]     ;  	/* ���ս��ױ������� */
	double 	DAY_AMT      		; 	/* �����ۼƽ������ */
	double 	DAY_SINGLE   		; 	/* ���յ��ʽ������ */
	char	MON_NUM     [7]	 ;   /* ���½��ױ������� */
	double 	MON_AMT      		; 	/* �����ۼƽ������ */
	char	SA_STATE    [2];
	char	SA_ACTION   [2];
	char	RESERVED    [31]  ;
	char	REC_CRT_TS  [15]         ;     /* ����ʱ��� */
	char	REC_UPD_TS  [15]          ;      /* �޸�ʱ��� */
	double	MON_AVR_AMT;/* add by  Zhengzhou Kalian W@ 20130725 */
	double	DAY_AVR_AMT;/* add by  Zhengzhou Kalian W@ 20130725 */
	double	AVR_SINGLE_AMT;	/* add by  Zhengzhou Kalian W@ 20130725 */
}  Cst_mcht_fee_inf_Def;

/* add by  Zhengzhou Kalian W 20120420 */
typedef struct
{
	char	mcht_cd     [16]    ;   /* �̻��� */
	char	term_id     [9];        /* �ն˺� */
	char	txn_num     [5]     ;   /* ���״��� */
	char	card_type   [3]     ;   /* ���� 00 ��� 01 ���� 02 ���п�*/
	char	channel     [3]     ;   /* �޶��־ 0-�ض� 1-Ĭ�� */
	char	day_num     [6]     ;  	/* ���ս��ױ������� */
	double 	day_amt  	; 	/* �����ۼƽ������ */
	double 	day_single  	; 	/* ���յ��ʽ������ */
	char	mon_num     [7]	 ;   /* ���½��ױ������� */
	double 	mon_amt      	; 	/* �����ۼƽ������ */
	char	sa_state    [2]; /* ���״̬ */
	char	sa_action[2];
	/*  char	reserved    [31] ;
		char	rec_crt_ts  [15] ; 
		char	rec_upd_ts  [15] ; */
}  Cst_term_fee_inf_Def;

typedef struct {
	char mchnt_no[15+1];
	char active_no[5+1] ;
	char card_flag[2+1];
	char current_date[8+1];
	char trans_amt[12+1];
	char expect_num[12+1];
	char least_amt[12+1];
	int award1_num;
	int award2_num;
	int award3_num;
	int award4_num;
	int award5_num;
	int award1_fin;
	int award2_fin;
	int award3_fin;
	int award4_fin;
	int award5_fin;
}TBL_award_inf;

/*
   lmk	add Tbl_Mng_Dtl_Def struct
   */
typedef struct
{
	char    txn_date[9];
	char    txn_time[7];
	char    tel_no[16];
	char    brh_id[11];
	char    txn_code[5];
	char    msg_type[3];
	char    syn_ran[9];
	int		syn_no;
	char    chk_data[3];
	char    menu_snd_flg[2];
	int		menu_snd_cnt;
	char    opr_snd_flg[2];
	int 	opr_snd_cnt;
	char    func_snd_flg[2];
	int		func_snd_cnt;
	char    err_snd_flg[2];
	int		err_snd_cnt;
	char    prt_snd_flg[2];
	int		prt_snd_cnt;
	char    app_snd_flg[2];
	int		app_snd_cnt;
	char    int_seq_num[8];
	char    term_seq_num[8];

	char    rec_crt_ts[27];
	char    rec_upd_ts[27];
}Tbl_Mng_Dtl_Def;
/*
   whx	add Tbl_term_txn_Def struct
   */
typedef struct
{
	char usage_key[2];
	char msg_src_id[5];
	char term_txn_code[5];
	char int_txn_code[5];
	char to_txn_code[5];
	char msg_dest1[5];
	char msg_dest2[5];
	char msg_to[4];
	char saf_count1[3];
	char rsp_dest_srv_id[5];
	char rsp_type[2];
	char rsp_txn_num[5];
	char txn_type[3];
	char pin_flag[2];
	char dsp[31];
}Tbl_term_txn_Def;
/* 
   whx add  Tbl_txn_dtl_Def
   */
typedef struct
{
	char inst_date[9];
	char sys_seq_num[7];
	char inst_time[7];
	char msg_src_id[5];
	char txn_num[5];
	char trans_code[4];
	char trans_type[2];
	char trans_state[2];
	char revsal_flag[2];
	char revsal_ssn[7];
	char cancel_flag[2];
	char cancel_ssn[7];
	char resp_code[3];
	char host_date[9];
	char host_ssn[13];
	char term_ssn[13];
	char key_rsp[33];
	char key_revsal[33];
	char key_cancel[33];
	char header_buf[47];
	char msg_type[5];
	char cti_id[7];
	char txn_type[3];
	char tel_no[16];
	char brh_id[11];
	char syn_ran[9];
	char syn_no[3];
	char tsam_no[33];
	char cent_id[17];
	char pan[20];
	char amt_trans[13];
	char amt_cdhldr_bil[13];
	char mchnt_type[5];
	char acq_inst_id_code[12];
	char fwd_inst_id_code[12];
	char retrivl_ref[13];
	char authr_id_resp[7];
	char card_accp_term_id[9];
	char card_accp_id[16];
	char card_accp_name[41];
	char addtnl_data_len[3];
	char addtnl_data[65];
	char addtnl_amt[41];
	char acct_id1_len[3];
	char acct_id1[29];
	char acct_id2_len[3];
	char acct_id2[29];
	char trans_fee1[13];
	char trans_fee2[13];
	char open_inst[16];
	char stlm_inst[16];
	char batch_flag[2];
	char batch_date[9];
	char amt_return[13];
	char authr_id_r[7];
	char misc[257];
}Tbl_txn_dtl_Def;
/*
 * wux add  Tbl_term_cfg_Def
 */

typedef struct
{
	char usage_key[2];
	char tsam_no[33];
	char term_id[9];
	char term_type[3];
	char bank_id[11];
	char cent_id[17];
	char enc_flg[2];
	char enc_idx[11];
	char enc_mk[49];
	char enc_wk[49];
	char in_mch_id[19];
	char tel_no[17];
	char acct_type[2];
	char acct_id[21];
	char brh_id[5];
	char teller_id[9];
	char card_flg[2];
	char ver_idx[2];
	char login_dt[9];
	char pre_login_dt[9];
}Tbl_term_cfg_Def;
/*
 * wux add Tbl_sys_stat_Def Tbl_mch_dtl_Def Tbl_rsp_msg_Def
 */
typedef struct
{
	char    record_id[9];
	char    status[2];
	char    cups_status[2];
	char    cups_stlm_date[5];
	char    l_cups_stlm_date[5];
	char    rec_updt_time[15];
} Tbl_sys_stat_Def;

typedef struct
{
	char    bank_id[10+1];
	char    in_mch_id[18+1];
	char    acct_no[19+1];
	char    mch_id[18+1];
	int     fee_id1;
	int     fee_id2;
	char    ent_acct_mod[1+1];
	char    mch_name[40+1];
	char    mch_addr[40+1];
	char    lic_id[30+1];
	char    brh_id[20+1];
	char    tax_type[1+1];
	char    tax_id[30+1];
	char    corp_name[20+1];
	char    idt_card[20+1];
	char    link_name[20+1];
	char    tel_no[15+1];
	char    mob_no[15+1];
	char    email[20+1];
	char    mch_sta[1+1];
	char    reg_date[8+1];
	char    reg_name[20+1];
	char    term_num[5+1];
	char    cli_mng[20+1];
	char    dept[20+1];
	char    dsp[40+1];
} Tbl_mch_dtl_Def;


typedef struct
{
	char    chg_no[5];
	char    err_cd[3];
	char    err_msg[65];
} Tbl_rsp_msg_Def;

/*
 * wux add Tbl_Card_Bin_Def Tbl_mch_type_Def
 */
typedef struct
{
	char    card_bin[11];
	int     bin_len;
	char    cb_code[12];
	char    card_name[41];
	int     offset_id;
	char    card_flg[2];
	char    card_type[3];
	char    used_flag[2];
}Tbl_Card_Bin_Def;

typedef struct
{
	char    mch_type[4+1];
	char    mch_id[18+1];
	int     fee_id;
	char    bank_id[10+1];
	char    dsp[40+1];
} Tbl_mch_type_Def;

/*
 * wux add Tbl_Remit_Acct_Def T_TxnCnt
 */
typedef struct
{
	char in_mch_id[19];
	char acct_id[31];
	char acct_nm[31];
	char open_inst[21];
	char reg_name[11];
	char cli_mng[21];
	char cust_nm[21];
	char open_name[61];
} Tbl_Remit_Acct_Def;

typedef struct
{
	int      SaleCnt;   
	double   SaleAmt;  
	int      PayCnt ; 
	double   PayAmt ;
	int      SavCnt ; 
	double   SavAmt ; 
} T_TxnCnt;



typedef struct
{
	char	city_code[9];
	int		comp_len_1;
	int		begin_pos_1;
	char	branch_code[9];
	int		comp_len_2;
	int		begin_pos_2;
	char	dsp[33];
} CST_Tbl_bank_zone_Def;

typedef struct
{
	char loc_code[5];	
} CST_Tbl_bank_LOC_Def;



typedef struct
{
	char         fee_type[2];
	int          fee_id;
	char         fee_flag[2];
	double       txn_fee;
	double       fee_rate;
	double       min_fee;
	double       max_fee;
	double       max_amt;
	double       min_amt;
	char         dsp[31];
} Cst_Tbl_Fee_Cfg_Def;

typedef struct{   
	char   tbl_id[5];    
	char   tbl_name[65];    
	char   ipc_key[11];    
	char   usg_flg[2];    
	char   max_record_num[9];
} Tbl_mem_db_inf_Def;

typedef struct{
	char brh_id           [10+1]; 
	char acct1	           [40+1]; 
	char acct2            [40+1]; 
	char resv1            [128+1];
	char last_upd_opr_id  [40+1]; 
	char last_upd_txn_id  [10+1]; 
	char last_upd_ts	   [14+1]; 
}Tbl_brh_acct_Def;

typedef struct {
	char mcht_no [ 16];
	char settle_type [ 2];
	char rate_flag [ 2];
	char settle_chn [ 3];	/*modify len from 2 to 3   by Zhengzhou Kalian W20131021**/
	char bat_time [ 5];
	char auto_stl_flg [ 2];
	char part_num [ 21];
	char fee_type [ 2];
	char fee_fixed [ 16];
	char fee_max_amt [ 16];
	char fee_min_amt [ 16];
	char fee_rate [ 9];
	char fee_div_1 [ 61];
	char fee_div_2 [ 61];
	char fee_div_3 [ 61];
	char settle_mode [ 2];
	char fee_cycle [ 3];
	char settle_rpt [ 2];
	char settle_bank_no [ 12];
	char settle_bank_nm [ 81];
	char settle_acct_nm [ 81];
	char settle_acct [ 41];
	char fee_acct_nm [ 81];
	char fee_acct [ 41];
	char group_flag [ 2];
	char open_stlno [ 21];
	char change_stlno [ 21];
    char spe_settle_tp[13];
	char spe_settle_lv[49];
	char spe_settle_ds[256];
	char fee_back_flg[2];
	char reserved [ 61];
	char rec_upd_ts [ 15];
	char rec_crt_ts [ 15];
} Tbl_mcht_settle_inf_Def;


typedef struct {
	char       mcht_no[16];
	char       settle_type[2];
	char       rate_flag[2];
	char       settle_chn[2];
	char       bat_time[5];
	char       auto_stl_flg[2];
	char       part_num[21];
	char       fee_type[2];
	char       fee_fixed[16];
	char       fee_max_amt[16];
	char       fee_min_amt[16];
	char       fee_rate[9];
	char       fee_div_1[61];
	char       fee_div_2[61];
	char       fee_div_3[61];
	char       settle_mode[2];
	char       fee_cycle[3];
	char       settle_rpt[2];
	char       settle_bank_no[12];
	char       settle_bank_nm[81];
	char       settle_acct_nm[81];
	char       settle_acct[41];
	char       fee_acct_nm[81];
	char       fee_acct[41];
	char       group_flag[2];
	char       open_stlno[21];
	char       change_stlno[21];
	char       spe_settle_tp[13];
	char       spe_settle_lv[49];
	char       spe_settle_ds[256];
	char       fee_back_flg[2];
	char       reserved[61];
	char       rec_upd_ts[15];
	char       rec_crt_ts[15];
} tbl_mcht_settle_inf_t;


typedef struct {
	char disc_id [ 9];
	int index_num;
	double min_fee;
	double max_fee;
	double floor_amount;
	double upper_amount;
	int flag;
	double fee_value;
	char txn_num [ 5];
	char rec_upd_usr_id [ 11];
	char rec_upd_ts [ 15];
	char rec_crt_ts [ 15];
} Tbl_his_disc_algo_Def;

typedef struct {
	char mcht_nm        [60 + 1];
	char mcht_no        [15 + 1];
	char mcht_status    [1  + 1];
	char bank_no        [6  + 1];
	char subbrh_no      [6  + 1];
	char orgn_no        [3  + 1];
	char area_no        [6  + 1];
	char settle_area_no [4  + 1];
	char mcc            [4  + 1];
	char tcc            [1  + 1];
	char conn_type      [1  + 1];
	char mcht_cn_abbr   [40 + 1];
	char mng_mcht_id    [15 + 1];
	char acq_inst_id    [13 + 1];
	char sign_inst_id   [13 + 1];
	char apply_date     [8  + 1];
	char enable_date    [8  + 1];
	char mcht_no_hx     [150 + 1];
	char open_time		[8  + 1];
	char close_time 	[8  + 1];
	char net_tel		[30 + 1];
} Mem_tbl_mcht_base_inf_t;

typedef struct {
	char mcht_nm        [60 + 1];
	char mcht_no        [15 + 1];
	char mcht_status    [1  + 1];
	char bank_no        [6  + 1];
	char subbrh_no      [6  + 1];
	char orgn_no        [3  + 1];
	char area_no        [6  + 1];
	char settle_area_no [4  + 1];
	char mcc            [4  + 1];
	char tcc            [1  + 1];
	char conn_type      [1  + 1];
	char mcht_cn_abbr   [40 + 1];
	char mng_mcht_id    [15 + 1];
	char acq_inst_id    [13 + 1];
	char sign_inst_id   [13 + 1];
	char apply_date     [8  + 1];
	char enable_date    [8  + 1];
	char open_time		[8  + 1];
	char close_time 	[8  + 1];
	char mcht_no_hx     [150 + 1];
}Tbl_mcht_base_inf_t;

typedef struct
{
	char mcht_no          [15 + 1];
	char settle_type      [1  + 1];
	char settle_acct      [40 + 1];
	char group_flag       [1  + 1];
	char fee_rate         [8  + 1];
	char fee_acct         [40 + 1];
	char fee_back_flg    [1  + 1];
	char auto_stl_flg     [1  + 1];
	char spe_settle_tp    [12 + 1];
	char spe_settle_lv    [48 + 1];
} Mem_tbl_mcht_settle_inf_t;

typedef struct
{
	char term_id           [12  + 1];
	char mcht_cd           [15  + 1];
	char term_batch_nm     [6   + 1];
	char term_sta          [1 	+ 1];
	char term_send_flag    [1   + 1];
	char term_i_card_flag  [1   + 1];
	char term_mcc          [4 	+ 1];
	char term_tp           [2   + 1];
	char term_ver_tp       [2   + 1];
	/*char term_para         [512 + 1];
	  char term_para_1       [512 + 1];*/
	char term_sign_sta     [1   + 1];
	char chk_sta           [1 	+ 1];
	char support_ic        [1   + 1];
	char term_branch        [4   + 1];
	char param_down_sign   [1   + 1];
	char param1_down_sign  [1   + 1];
	char ic_down_sign      [1   + 1];
	char key_down_sign     [1   + 1];
	char finance_card1     [16  + 1];
	char finance_card2     [16  + 1];
	char finance_card3     [20  + 1];
	char bind_tel1         [15  + 1];
} Mem_tbl_term_inf_t;

typedef struct
{
	char   mcht_cd    [15 + 1];
	char   txn_num    [4  + 1];
	char   card_type  [2  + 1];
	char   channel    [2  + 1];
	char   day_num    [5  + 1];
	double day_amt    ;
	double day_single ;
	char   mon_num    [6  + 1];
	double mon_amt    ;
	char   sa_state   [1  + 1];
	char   sa_action   [1  + 1];
	/*char   reserved   [30 + 1];
	  char   rec_crt_ts [14 + 1];
	  char   rec_upd_ts [14 + 1];*/
	double	mon_avr_amt;/* add by  Zhengzhou Kalian W@ 20130725 */
	double	day_avr_amt;/* add by  Zhengzhou Kalian W@ 20130725 */
	double	avr_single_amt;	/* add by  Zhengzhou Kalian W@ 20130725 */
} Mem_cst_mcht_fee_inf_t;

typedef struct
{
	char   mcht_no    [15 + 1];
	char   channel    [2  + 1];
	char   buss_type  [6  + 1];
	char   txn_num    [4  + 1];
	char   sa_state   [1  + 1];	
	char   reserved1  [12 + 1];
	char   reserved2  [12 + 1];
} Tbl_mcht_tran_ctl_Def;
typedef struct
{
	char   channel    [2  + 1];
	char   card_bin   [30 + 1];
	char   txn_num    [4  + 1];
	char   sa_state   [1  + 1];	
	char   reserved1  [12 + 1];
	char   reserved2  [12 + 1];
} Tbl_chl_tran_ctl_Def;

typedef struct {
	char	mcht_no[16];
	char	channel[3];
	char    card_type[3];
	char	buss_type[7];
	char	txn_num[5];
	char   sa_state   [1  + 1];	
	char	limit[3];
	char	reserved1[13];
	char	reserved2[13];
}TBL_MCHT_LIMIT;

typedef struct {
	char	term_id[9];
	char	channel[3];
	char    card_type[3];
	char	buss_type[7];
	char	txn_num[5];
	char    sa_state[1  + 1];	
	char	limit[3];
	char	reserved1[13];
	char	reserved2[13];
}TBL_TERM_LIMIT;

typedef struct {
	char	channel[3];
	char	card_bin[31];
	char	txn_num[5];
	char    sa_state[1  + 1];	
	char	limit[3];
	char	reserved1[13];
	char	reserved2[13];
}TBL_CHL_LIMIT;

typedef struct {
	char	rule_id[3];
	char	case_id[7];
	char	inst_id[7]; /* �ֹ�˾ */
	char	mcc[5];
	char	days[4];
	double	warnlvt;
	long	warncount;
	double	warnamt;
	char	sa_state[2];
}CST_AFTER_RULE_INF;

typedef struct cst_sys_param{
	char	owner[21];
	char	key[21];
	char	type[3];
	char	value[201];
}CST_SYS_PARAM;

typedef struct 
{
	char term_ins       [8  + 1]; 
	char mcht_mcc       [4  + 1]; 
	char mcht_id        [15 + 1];
	char mcht_term_id   [8  + 1];
	char ins_mcc        [4  + 1];
	char ins_mcht       [20 + 1];
	char ins_term       [15 + 1];                             
}T_ConvMchtInf;

typedef struct 
{
	char	merch_id[16];
	char	term_id[16];
	char	dest_id[5];
	char	ssn_value[7];
	char	min_value[7];
	char	max_value[7];
	char	reserved[33];
}TBL_ssn_bank_Def;

typedef struct 
{
	char	merch_id[16];
	char	term_id[16];
	char	dest_id[5];
	char	pan[29];
	char	bank_ssn_value[7];
	char	host_ssn_value[7];
	char	reserved[33];
}TBL_ssn_conv_bank_Def;

/* add by  Zhengzhou Kalian W 20130619 */
typedef struct
{
	char mcht_no[16];
	char card_no[20];
	char credits_rules[17];
	char change_rules[17];  /* add by  Zhengzhou Kalian W at 20130625  �һ������ֶ�  */
	char points[13];
	char member_name[17];
	char phone_no[33];
	char flag[2];
	char reserved1[33];
	char reserved2[33];
	char upd_opr_id[17];
	char crt_opr_id[17];
	char upd_ts[15];
	char crt_ts[15]; 
}TBL_member_points_inf_Def;
/* add end */

typedef struct
{
	char	mcht_no[16];
	char	map_flag[2];
	char	rule_id[11];
	char	reserved1[33];
	char	reserved2[33];
	char	reserved3[33];
	char	upd_opr_id[17];
	char	crt_opr_id[17];
	char	upd_ts[15];
	char	crt_ts[15];
} Tbl_bft_mcht_inf_Def;

typedef struct
{
	char	rule_id[11];
	char	rule_name[33];
	char	mcc[5];
	char	type_flag[2];
	char	max_value[13];
	char	min_value[13];
	char	reserved1[33];
	char	reserved2[33];
	char	upd_opr_id[17];
	char	crt_opr_id[17];
	char	upd_ts[15];
	char	crt_ts[15];
} Tbl_rule_mcc_inf_Def;

typedef struct
{
	char	mcht_no[16];
	char	mcc[5];
	char	maped_flag[2];
	char	date_amt_total[13];
	char	reserved1[33];
	char	reserved2[33];
	char	reserved3[33];
	char	upd_opr_id[17];
	char	crt_opr_id[17];
	char	upd_ts[15];
	char	crt_ts[15];
	char    inst_id[9];
} Tbl_maped_mcht_inf_Def;

typedef struct tbl_cup_coma_txn_table{
	char	date_settlmt[9];
	char	key_cup[49];
	char 	txn_type[4];
	char	txn_num[5];
	char	stlm_flag[2];
	char    acq_inst_id_code[12]; 
	char    fwd_inst_id_code[12];
	char    trade_sn[7];
	char    trans_date_time[11];
	char    pan[20];
	char    trade_amount[13];
	char    accept_amt[13];
	char    trade_fee[13];
	char    msg_type[5];
	char    trans_type[7];
	char    mchnt_type[5];
	char    pos_no[9];
	char    mcht_no[16];
	char    retrivl_ref[13];
	char    pos_cond_code[3];
	char    authr_id_resp[7];
	char    rcvg_code[12];
	char    ori_cup_ssn[7];
	char    resp_code[3];
	char    pos_entry_mode[4];
	char    scfee[13];
	char    sdfee[13];
	char    switch_fee[13];
	char    switch_flag[2];
	char    card_seq_id[4];
	char    pos_read_ability[2];
	char    card_cond_code[2];
	char    ori_trans_date_time[11];
	char    issuer_code[12];
	char    trans_area_flag[2];
	char    pos_type[3];
	char    ect_flag[3];
	char    add_fee[13];
	char    other_inf[15];
	char    fwd_stlm_inst_id[12];
	char    rcvg_stlm_inst_id[12];
	char    revsal_flag[2];
	char    cancel_flag[2];
	char    settle_date[5];
	char    settle_time[3];
	char    mchnt_name_addr[41];
	char    curr_type[4];
	char    cup_agio_fee[10];
	char    mchnt_fee[10];
	char    mchnt_settlmt_bank[12];
	char    mchnt_settlmt_bank_fee[10];
	char    srv_role1[12];
	char    srv_role1_fee[10];
	char    srv_role2[12];
	char    srv_role2_fee[10];
	char    srv_role3[12];
	char    srv_role3_fee[10];
	char    srv_role4[12];
	char    srv_role4_fee[10];
	char    srv_role5[12];
	char    srv_role5_fee[10];
	char    srv_role6[12];
	char    srv_role6_fee[10];
	char    srv_role7[12];
	char    srv_role7_fee[10];
	char    srv_role8[12];
	char    srv_role8_fee[10];
	char    srv_role9[12];
	char    srv_role10_fee[10];
	char    user_def_fee1[10];
	char    user_def_fee2[10];
	char    user_def_fee3[10];
	char    reserve[301];
	char	reserve1[65];
} tbl_cup_coma_def;

typedef struct tbl_cup_zpsum{
	char    mcht_code[16];
	char    mcht_type[5];
	char    mcht_nm[81];
	char    bank_nm[61];
	char    bank_sys_no[13];
	char    bank_settlmt_no[13];
	char    mcht_acct[33];
	char    mcht_acct_nm[81];
	char    mcht_area_code[7];
	char    normal_txn_time[9];
	char    normal_txn_amt[14];
	char    normal_txn_fee[14];
	char    error_txn_time[9];
	char    error_txn_amt[14];
	char    error_txn_fee[14];
	char    other_all_amt[14];
	char    settlmt_all_amt[14];
	char    in_ex_amt[14];
	char    in_ex_role[2];
	char    normal_release_amt[14];
	char    normal_hang_amt[14];
	char    fill_amt[14];
	char    enter_acc_amt[14];
	char    enter_acc_role[2]; 
	char    covering_type[2];
	char    reserve[201];
}tbl_cup_zpsum_def;


typedef struct tbl_cup_com_txn_table{
	char	date_settlmt[9];
	char	key_cup[49];
	char 	txn_type[4];
	char	txn_num[5];
	char	stlm_flag[2];
	char    acq_inst_id_code[12]; 
	char    fwd_inst_id_code[12];
	char    trade_sn[7];
	char    trans_date_time[11];
	char    pan[20];
	char    trade_amount[13];
	char    accept_amt[13];
	char    trade_fee[13];
	char    msg_type[5];
	char    trans_type[7];
	char    mchnt_type[5];
	char    pos_no[9];
	char    mcht_no[16];
	char    retrivl_ref[13];
	char    pos_cond_code[3];
	char    authr_id_resp[7];
	char    rcvg_code[12];
	char    ori_cup_ssn[7];
	char    resp_code[3];
	char    pos_entry_mode[4];
	char    scfee[13];
	char    sdfee[13];
	char    switch_fee[13];
	char    switch_flag[2];
	char    card_seq_id[4];
	char    pos_read_ability[2];
	char    card_cond_code[2];
	char    ori_trans_date_time[11];
	char    issuer_code[12];
	char    trans_area_flag[2];
	char    pos_type[3];
	char    ect_flag[3];
	char    add_fee[13];
	char    other_inf[15];
} tbl_cup_com_def;

/* add by  Zhengzhou Kalian W @20130725 */
#define RISK_INF_NUM_MAX 30
typedef struct
{
	int 	RiskNum;
	Tbl_risk_inf_Def taRiskInf[RISK_INF_NUM_MAX];
}RiskInf;

typedef struct
{
	char 	mcht_no[16];
	char	mcc[5];
	char	bus_amt[13];
	char	cost_fee[13];
	char	settle_type[3];
	char	acreage[11];
	char	emp_num[11];
	char	manage_years[5];
	char	single_amt[13];
	char	month_total_amt[13];
	char 	income[13];
	char 	risk_events_num[10];
	char	single_num[8];
	char	eval_way_flag[2];
	char	eval_level[4];
	char	eval_end_time[15];
	char	reserved1[33];
	char	reserved2[33];
	char	upd_opr_id[17];
	char	crt_opr_id[17];
} tbl_mcht_eval_inf_def;

typedef struct{
	char    id[6];
	char    item[21]; 
	char    onef[21];
	char    twof[21];
	char    threef[21];
	char    fourf[21];
	char    fivef[21];
	char    sixf[21];
	char    sevenf[21];
	char    eightf[21];
	char    ninef[21];
	char    tenf[21];
	char    power[11]; 
} tbl_evastandard_inf_def;

typedef struct{
	char	mchtlevel[5];
	char	valueinfo[21];
	char	limitvalue[16]; /* add by Zhengzhou Kalian W 20130826 �޸Ľ��� */
	char	limittime[6];
	char 	mcc[5];
	char	rate[11];
} tbl_mcclevelinfo_def;

typedef struct{
	char    settle_date[8 + 1]; 
	char	branch_no[8 + 1];
	double	amt_trans;
	double	amt_profit;
	double	ins_profit;
	char    ins_level[2 + 1]; 
	char	settle_flag[1 + 1]; 
	char	reserved1[32 + 1];
	char	reserved2[32 + 1]; 
} tbl_ins_profit_inf_def;

/*zhaohao add 20110928*/
typedef struct{  
	char     date_settlmt      [  9];
	char     txn_num           [  5];
	char     pan               [ 20];
	char     trans_amt         [ 13];
	char     currcy_code_trans [  4];
	char     trans_date_time   [ 11];
	char     trans_ssn         [  7];
	char     authr_id_resp     [  7];
	char     authr_date        [  5];
	char     retrivl_ref       [ 13];
	char     acq_inst_id_code  [ 12];
	char     fwd_inst_id_code  [ 12];
	char     mcht_type         [  5];
	char     card_accp_term_id [  9];
	char     card_accp_id      [ 16];
	char     card_accp_name    [ 41];
	char     orig_trans_inf    [ 24];
	char     cup_ssn          [ 10];
	char     rcvg_code         [ 12];
	char     acq_code          [ 12];
	char     channel_num       [  3];
	char     txn_acq_type      [  2];
	char     cups_swresved     [  9];
	char     pos_cond_code     [ 13];
	char     fee_acq           [ 13];
	char     trans_dre_type    [  2];
	char     ECI_type          [  3];
	char     fee_spe_type      [  2];
	char     fee_spe_gra       [  2];
	char     trans_swresved    [ 12];
	char     stlm_flag         [  2];
	char     lst_upd_tlr       [ 12];
	char     create_time       [ 15];
	char     lst_upd_time      [ 15];
}bth_off_cups_txn_def;

/*zhaohao add 20110928*/
typedef struct{  
	char     date_settlmt      [  9];
	char     txn_num           [  5];
	char     pan               [ 20];
	char     trans_amt         [ 13];
	char     currcy_code_trans [  4];
	char     trans_date_time   [ 11];
	char     trans_ssn         [  7];
	char     authr_id_resp     [  7];
	char     authr_date        [  5];
	char     retrivl_ref       [ 13];
	char     acq_inst_id_code  [ 12];
	char     fwd_inst_id_code  [ 12];
	char     mcht_type         [  5];
	char     card_accp_term_id [  9];
	char     card_accp_id      [ 16];
	char     card_accp_name    [ 41];
	char     orig_trans_inf    [ 24];
	char     cup_ssn          [ 10];
	char     rcvg_code         [ 12];
	char     acq_code          [ 12];
	char     channel_num       [  3];
	char     txn_acq_type      [  2];
	char     cups_swresved     [  9];
	char     pos_cond_code     [ 13];
	char     fee_acq           [ 13];
	char     trans_dre_type    [  2];
	char     ECI_type          [  3];
	char     fee_spe_type      [  2];
	char     fee_spe_gra       [  2];
	char     trans_swresved    [ 12];
	char     stlm_flag         [  2];
	char     lst_upd_tlr       [ 12];
	char     create_time       [ 15];
	char     lst_upd_time      [ 15];
}bth_off_heart_txn_def;

typedef struct{  
	char     date_settlmt      [  9]; 
	char     mess_code         [  4]; 
	char     ref_code          [  3]; 
	char     file_name         [ 41];
	char     acq_inst_code_id  [ 12];
	char     misc_1            [ 31];
	char     txn_date          [  9]; 
	char     sys_seq_num       [  7]; 
	char     create_time       [ 15]; 
} bth_off_return_txn_def;


//20111107 zh
typedef struct {
	char        date_settlmt    [  9];
	char        txn_date        [  9];
	char        file_type       [  2];
	char        file_name       [ 21];
	char        file_sta        [  3];
	char        bat_id          [  6];
	char        bat_sta         [  2];
	char        misc_1          [129];
	char        update_time     [ 15];
	char        create_time     [ 15];
}tbl_on_line_ctl_def;

typedef struct {
	char mcc[4 + 1];
	char fee_type[1 + 1];
	double fee_rate;
	double max_fee;
	double fixed_value;
	char mcc_des[16 + 1];
}tbl_mcc_end_rate_def;

/*2013-8-22 add mjt*/
typedef struct {
	char	querytime	[9];
	char	ret_code	[2];
	char	err_msg		[201];
	char	acct_name	[41];
	char	org_code	[6];
	char	account_cur	[11];
	char	amount		[17];
	char	doc_flag	[3];
	char	summary		[105];
	char	check_num	[21];
	char	opteller	[4];
	char	transtime	[7];
	char	other_acct	[33];
	char	other_acct_name	[61];
	char	opr_id		[41];
	char	create_time	[15];
}tbl_xml_day_dtl_def;

/*���и�������ˮ*/
typedef struct {
	char date_settlmt[8 + 1];
	char mcht_no[15 + 1];
	char term_id[8 + 1];
	char batch_no[6 + 1];
	char sys_seq_num[6 + 1];
	char pan[19 + 1];
	char trans_date[8 + 1];
	char trans_time[6 + 1];
	char txn_type[10 + 1];
	char auth_id_resp[6 + 1];
	char trans_amt[12 + 1];
	char trans_fee[12 + 1];
	char stlm_flag[1 + 1];
	char reserved[32 + 1];
	char lst_upd_tlr[11 + 1];
	char create_time[14 + 1];
	char lst_upd_time[14 + 1];
}bth_sxf_txn_bdt_def;

/* add by  Zhengzhou Kalian W 20131206 Ѷ��������ˮ*/
typedef struct {
	char        date_settlmt   [ 8 + 1 ] ;
	char        trans_type     [32 + 1 ] ;
	char        trans_date     [ 8 + 1 ] ;
	char        trans_time     [10 + 1 ] ;
	char        inst_no        [ 8 + 1 ] ;
	char        mcht_no        [15 + 1 ] ;
	char        term_id        [ 8 + 1 ] ;
	char        trans_amt      [12 + 1 ] ;
	char        pan            [19 + 1 ] ;
	char        sys_seq_num    [ 6 + 1 ] ;
	char        trans_rsp_code [ 2 + 1 ] ;
	char        mcht_mcc       [ 4 + 1 ] ;
	char        xl_currency    [ 3 + 1 ] ;
	char        xl_amt         [12 + 1 ] ;
	char        xl_fee         [12 + 1 ] ;
	char        mcht_currency  [ 3 + 1 ] ;
	char        mcht_amt       [12 + 1 ] ;
	char        mcht_fee       [12 + 1 ] ;
	char        stlm_flag      [ 1 + 1 ] ;
	char        reserved       [32 + 1 ] ;
	char        lst_upd_tlr    [11 + 1 ] ;
	char        create_time    [14 + 1 ] ;
	char        lst_upd_time   [14 + 1 ] ;
}bth_xl_txn_bdt_def;

typedef struct {
	char    querytime   [9];
	char    ret_code    [2];
	char    err_msg     [201];
	char    acct_name   [24];
	char    org_code    [6];
	char    check_num   [21];
	char    pay_date    [9];
	char    summary     [5];
	char    amount      [17];
	char    doc_flag    [3];
	char    balance     [17];
	char    opteller    [4];
	char    other_acct  [33];
	char    other_acct_name [61];
	char	opr_id		[41];
	char	create_time	[15];
}tbl_xml_acct_his_dtl_def;

typedef struct {
	char	settle_date		[8 + 1];
	char	payenpname		[42 + 1];
	char	payacct			[23 + 1];
	char	recacct			[32 + 1];
	char	recacctname		[52 + 1];
	char	recbankname		[42 + 1];
	char	amount			[16 + 1];
	char	payuse			[52 + 1];
	char	remark			[300 + 1];
	char	transflag		[1 + 1];
	char	transmittype	[1 + 1];
	char	recdeptype		[1 + 1];
	char	trans_status	[1 + 1];
	char	reserved		[300 + 1];
	char	opr_id			[40 + 1];
	char	create_time		[14 + 1];
}tbl_xml_transfer_inf_def;

typedef struct {
	char mcht_no[15 + 1];         
	char province[8 + 1];         
	char city[8 + 1];             
	char area[8 + 1];             
	char depart[40 + 1];          
	char mcht_type_cause[2 + 1];  
	char country_code[10 + 1];    
	char nationality[10 + 1];     
	char bus_scope[200 + 1];      
	char bus_main[100 + 1];       
	char link_tel[30 + 1];        
	char fin_manager[30 + 1];     
	char fin_phone[30 + 1];       
	char fin_tel[30 + 1];         
	char fin_fax[30 + 1];         
	char fin_email[60 + 1];       
	char shop_num[4 + 1];         
	char bus_area[2 + 1];         
	char bus_zone[2 + 1];         
	char acreage[10 + 1];         
	char area_type[1 + 1];        
	char emp_num[10 + 1];         
	char cashier_num[10 + 1];     
	char cashier_desk_num[10 + 1];
	char turnover_before[10 + 1]; 
	char turnover[10 + 1];        
	char open_date[8 + 1];        
	char is_wineshop[1 + 1];      
	char wineshop_lvl[1 + 1];     
	char is_more_acq[1 + 1];      
	char is_app_outside[1 + 1];   
	char has_inner_pos_exp[1 + 1];
	char has_our_pos_exp[1 + 1];  
	char ser_type[1 + 1];         
	char ser_lvl[1 + 1];          
	char src[1 + 1];              
	char expander[30 + 1];        
	char referrer[30 + 1];        
	char mcht_no_old[20 + 1];     
	char mcht_mcc2[4 + 1];        
	char proxy[60 + 1];           
	char proxy_tel[30 + 1];           
}Tbl_mcht_supp1_Def;

typedef struct{
	char city_code[4 + 1];
	char city_des[120 + 1];
}Tbl_city_code_Def;

typedef struct{
	char acct_index[3 + 1];
	char payer_acct[32 + 1];
	char payer_acct_name[64 + 1];
	char payer_acct_bank[64 + 1];
	char payer_acct_bank_no[12 + 1];
	char acct_decript[64 + 1];
}Tbl_payfortune_acct_inf_Def;

typedef struct{
	char term_no        [8 + 1]; 
	char mech_no        [15 + 1];
	char term_type      [1 + 1]; 
	char state          [1 + 1]; 
	char manufaturer    [30 + 1];
	char product_cd     [40 + 1];
	char terminal_type  [20 + 1];
	char batch_no       [12 + 1];
	char stor_opr_id    [40 + 1];
	char stor_date      [8 + 1]; 
	char reci_opr_id    [40 + 1];
	char reci_date      [8 + 1]; 
	char back_opr_id    [40 + 1];
	char bank_date      [8 + 1]; 
	char invalid_opr_id [40 + 1];
	char invalid_date   [8 + 1]; 
	char sign_opr_id    [40 + 1];
	char sign_date      [8 + 1]; 
	char misc           [40 + 1];
	char last_upd_opr_id[40 + 1];
	char last_upd_ts    [6 + 1]; 
	char pin_pad        [40 + 1];
}Tbl_term_management_Def;

typedef struct{
	char filename[32 + 1];
	char down_flag[1 + 1];
	char file_des[64 + 1];
}Tbl_cup_filename_inf_Def;

typedef struct{
        char       cardno  [19  + 1];
        char       exp_date[4   + 1];
        double     leg_balance_amount;
        double     avl_balance_amount;
        double     scale;
        char       reserve1[256 + 1];
        char       reserve2[256 + 1];
        char       reserve3[256 + 1];
        char       reserve4[256 + 1];
        char       reserve5[256 + 1];
}Tbl_base_cardno_inf_Def;

typedef struct {
	char txn_num[4+1];
	char need_flag[1+1];
	char json_key[50+1];
	char json_key_desc[100+1];
	char json_offset[4+1];
	char json_default[1024+1];
	char json_len[4+1];
	char ipc_key[50+1];
	char ipc_offset[4+1];
	char ipc_default[1024+1];
	char ipc_len[4+1];
} Tbl_cups_config_Def;

/* add by  Zhengzhou Kalian W 20160416*/

typedef struct {
char mcht_no [ 16];
char mcht_nm [ 61];
char related_mcht_no [ 16];
char related_mcht_nm [ 61];
} tbl_mcht_related_inf_def;

typedef struct 
{
	char a_mcht_cd      [15 + 1];
    char a_term_id      [8  + 1];
    char b_mcht_cd      [15 + 1];
    char b_term_id      [12  + 1];
    char inst_id        [20 + 1];
    char mcht_nm        [60 + 1];
    char swt_id         [5  + 1];
    char d_disc_id1     [10 + 1];
    char d_disc_id2     [10 + 1];
    char c_disc_id1     [10 + 1];
    char c_disc_id2     [10 + 1];
	int  id;
    char xy             [30 + 1];
    char distance       [15 + 1];
    char term_sn        [50 + 1];
    char term_supply    [8 + 1];
    char term_type      [8 + 1];
    char c_mcht_cd      [15 + 1 ];
    char c_term_key     [32 + 1 ];
    char agent_no       [16+1];
    char y_disc_id1     [32 + 1 ];
    char sn_flag        [8 + 1 ];
    char settle_cycle   [8 + 1 ];
    char ctrl_flag      [64+ 1 ];
    char entry_mode     [8 + 1 ];
    char enc_type       [8 + 1 ];
    char debit_extra_charge_type[20+1];
    char debit_extra_charge_rate[10+1];
    char debit_extra_charge_quota[10+1];
    char credit_extra_charge_type[20+1];
    char credit_extra_charge_rate[10+1];
    char credit_extra_charge_quota[10+1];
    char reserve1       [120 + 1];
    char reserve2       [120 + 1];
} tbl_mcht_chg_def;

typedef struct {
	char  otxnnum[9 + 1]; /* �ⲿ������*/
	char  itxnnum[4 + 1]; /* �ڲ������� */
} Fd_txn_num_map;
typedef struct
{
	char    inst_date[8+1];
	char    inst_time[6+1];
	char    count[2+1];
	char    trans_date_time[10+1];
	char    msg_src_id[4+1];
	char    msg_dest_id[4+1];
	char    txn_num[4+1];
	char    key_rsp[48+1];
	char    sys_seq_num[6+1];
	char    pan[34+1];
	char    amt_trans[19+1];
	char    date_settlmt[8+1];
	char    msg_len[6+1];
	char    msg_ipc1[4000+1];
	char    msg_ipc2[4000+1];
} Tbl_Saf_Saf_Def;
typedef struct
{
	char    inst_date[8+1];
	char    inst_time[6+1];
	char    trans_date_time[10+1];
	char    msg_src_id[4+1];
	char    msg_dest_id[4+1];
	char    txn_num[4+1];
	char    key_rsp[48+1];
	char    sys_seq_num[6+1];
	char    pan[34+1];
	char    amt_trans[19+1];
	char    amt_return[19+1];
	char    date_settlmt[8+1];
	char    orderid[40+1];
	char    seq_no[20+1];
	char    reserve1[128+1];
	char    reserve2[256+1];
	char    msg_len[6+1];
	char    msg_ipc1[4000+1];
	char    msg_ipc2[4000+1];
} Tbl_Saf_Ipc_Def;

typedef struct
{
	char   pan[19+1];
	char   issuer_code[11+1];
	char   issuer_name[30+1];
	char   card_no_len[2+1];
	char   bin_num[11+1];
	char   bin_len[2+1];
	char   cup_logo_flag[1+1];
	char   debit_credit_flag[1+1];
	char   add_delete_stat[1+1];                                                                        
	char   additional_data[30+1];                                                                       
	char   card_type[2+1];                                                                              
	char   bank_addt_data[30+1];                                                                        
	char   update_date[8+1];                                                                            
}Tbl_Card_Bin_Inf_Def;

typedef struct
{
    char    inst_date[8+1];
    char    inst_time[6+1];
    char    txn_num[4+1];
    int     count;
    char    key_rsp[48+1];
    char    pan[19+1];
    char    reserve1[60+1];
    char    reserve2[120+1];
} Tbl_Verify_Limit_Def;

typedef struct
{
	char       param_code[15+1];
	char       param_name[15+1];
	char       param_value[256+1];
	char       param_remark[128+1];
	char       upd_time[14+1];
	char       crt_time[14+1];
}Tbl_Param_Inf_Def;

typedef struct {
    char       chnl_name[65];
    char       chnl_no[5];
    char       encry_mode[9];
    char       encry_key[65];
    char       ip[129];
    int        port;
    char       path[129];
} tbl_notice_chnl_inf_t;


typedef struct {
    char       mcht_no[16];
    char       chnl_no[5];
    short      count;
} tbl_mcht_notice_inf_t;

typedef struct {
    char       merch_id[16];
    char       term_id[16];
    char       inst_id[9];
    char       inst_idx[5];
    char       pik[33];
    char       mak[33];
    char       zpik[33];
    char       zmak[33];
    char       zmk[33];
    char       reserved[33];
    char       ztmk[33];
    char       tpik[33];
    char       tmak[33];
    char       tmk[33];
    char       reserved1[513];
    char       reserved2[513];
} tbl_inst_key_inf_t;

typedef struct {
    char       merch_id[16];
    char       term_id[16];
    char       mpos_idx[5];
    char       pik[33];
    char       mak[33];
    char       trk[33];
    char       zpik[33];
    char       zmak[33];
    char       ztrk[33];
    char       tmk[33];
    char       reserved[33];
} tbl_pos_key_inf_t;

typedef struct {
    char       key_index[4+1];
    char       pub_key_len[4+1];
    char       pub_key[2048+1];
    char       pri_key_len[4+1];
    char       pri_key[2048+1];
} tbl_rsa_key_inf_t;

typedef struct {
    char       base_req_no[65];
    char       mch_no[16];
    char       term_no[13];
    char       activity_channel_fee[65];
    char       activity_channel_type[9];
    char       status[3];
    char       deduction_amount[65];
    char       activity_pay_order_no[33];
    char       channel_order_no[65];
    char       third_pay_order_no[65]; 
    char       post_term_batch_no[33];
    char       post_term_req_no[33];
    char       search_ref_no[33];
    char       system_no[33];
    char       activity_pay_type[9];
    char       activity_finish_time[33];
	char	   agt_no[33];
	char	   sv_agt_no[33];
	char	   org_agt_no[33];
	char	   channel_code[33];
	char	   dev_sn_no[65];
} tb_mch_term_activity_record_t;

typedef struct 
{
      long   usage_key ;
      char  comp_key[2];
      long   xml_index ;
      long  buf_index;
      char  txn_number[11];
      char xml_dsp[257];
}tbl_xml_ipc_dsp_def;

typedef struct 
{
  long l_usage_key ;
  long i_buf_dsp_index;
  long i_node_index ;
  long father_node_id ;
  char sp_node_name[41] ;
  long i_leaf_type ;
  char i_fld_buf[31];
  long i_repeat_or;
  long i_attribute_num ;
  char sp_attribute_name1[150+1];
  char sp_attribute_test1[4+1];
  char sp_attribute_name2[4+1];
  char sp_attribute_test2[4+1];
  char sp_attribute_name3[4+1];
  char sp_attribute_test3[4+1];
  long i_hold1_usr ;
  char sp_hold2_usr[200+1];
}tbl_xml_ipc_values_def;

typedef struct {
    int l_usage_key; 
    int i_buf_chg_index; 
    int i_sour_buf_index; 
    int i_dest_buf_index; 
    char sp_dsp[257]; 
} tbl_buf_chg_def;


typedef struct {
    int l_usage_key; 
    int i_buf_chg_index; 
    int i_sour_fld_index; 
    int i_dest_fld_index; 
} tbl_fld_tsf_def;

typedef struct {
    int l_usage_key; 
    int i_fld_id; 
    int n_fld_l; 
    int n_fld_type; 
    char sp_fld_dsp[251]; 
} tbl_fld_dsp_def;


typedef struct {
    int l_usage_key; 
    int i_buf_dsp_index; 
    char sp_dsp[257]; 
} tbl_buf_dsp_dsp_def;

typedef struct {
    int l_usage_key; 
    int i_buf_dsp_index; 
    int i_pos_index; 
    int i_fld_index; 
    int i_fld_id; 
    int i_fld_offset; 
} tbl_buf_dsp_def;



/*
struct TBL_SRV_INF_DEF
{
    long    usage_key;
    char    srv_id[5];
    char    srv_name[21];
    char    msq_int_id[5];
    char    srv_num[5];
    char    relate_srv_id[2049];
    char    srv_dsp[65];
} stTBL_SRV_INF;


struct TBL_SRV_PARAM_DEF
{
    long    usage_key;
    char    srv_id[5];
    char    param_usage[2];
    char    param_index[3];
    char    param_data[65];
    char    param_dsp[65];
} stTBL_SRV_PARAM;

struct TBL_MSQ_INF_DEF
{
    char    msq_int_id[5];
    char    msq_key[17];
    char    msq_type[9];
} stTBL_MSQ_INF;

struct TBL_XML_IPC_DSP_DEF
{
    long   usage_key ;
    char   comp_key[2];
    long   xml_index ;
    long   buf_index;
    char   txn_number[11];
    char   xml_dsp[257];
}stTBL_XML_IPC_DSP;

struct TBL_XML_IPC_VALUES_DEF
{
  long l_usage_key ;
  long i_buf_dsp_index;
  long i_node_index ;
  long father_node_id ;
  char sp_node_name[41] ;
  long i_leaf_type ;
  char i_fld_buf[31];
  long i_repeat_or;
  long i_attribute_num ;
  char sp_attribute_name1[150+1];
  char sp_attribute_test1[4+1];
  char sp_attribute_name2[4+1];
  char sp_attribute_test2[4+1];
  char sp_attribute_name3[4+1];
  char sp_attribute_test3[4+1];
  long i_hold1_usr ;
  char sp_hold2_usr[200+1];
}stTBL_XML_IPC_VALUES;

typedef struct TBL_USER_INF_DEF
{   
    char    username[33];
    char    password[33];
    char    email[33];
    char    telephone[12];
    long    balance;
    char    crt_time[32];
    char    login_time[32];
    char    token[33];
} stTBL_USER_INF;

typedef struct TBL_N_TXN_DEF
{
    char    inst_date[9];
    char    sys_seq_num[7];
    char    inst_time[7];
    char    msg_src_id[5];
    char    txn_num[5];
    char    trans_code[4];
    char    trans_type[2];
    char    trans_state[2];
    char    revsal_flag[2];
    char    revsal_ssn[7];
    char    cancel_flag[2];
    char    cancel_ssn[7];
    char    host_date[9];
    char    host_ssn[13];
    char    term_ssn[13];
    char    term_date[9];
    char    card_org[3];
    char    txn_bank[12];
    char    tm_zone[4];
    char    key_rsp[49];
    char    key_revsal[49];
    char    key_cancel[49];
    char    header_buf[47];
    char    msg_type[5];
    char    pan_len[3];
    char    pan[20];
    char    processing_code[7];
    char    amt_trans[13];
    char    amt_settlmt[13];
    char    amt_cdhldr_bil[13];
    char    trans_date_time[11];
    char    conv_rate_stlm[9];
    char    conv_rate_cdhldr[9];
    char    cup_ssn[7];
    char    time_local_trans[7];
    char    date_local_trans[5];
    char    date_expr[5];
    char    date_settlmt[5];
    char    date_conv[5];
    char    mchnt_type[5];
    char    acq_cntry_code[4];
    char    pos_entry_mode[4];
    char    card_seqid[4];
    char    pos_cond_code[3];
    char    pos_pin_cap_code[3];
    char    amt_trans_fee[10];
    char    acq_inst_id_code[12];
    char    fwd_inst_id_code[12];
    char    retrivl_ref[13];
    char    authr_id_resp[7];
    char    resp_code[3];
    char    card_accp_term_id[9];
    char    card_accp_id[16];
    char    card_accp_name[41];
    char    addtnl_data_len[4];
    char    addtnl_data[513];
    char    currcy_code_trans[4];
    char    currcy_code_stlm[4];
    char    currcy_code_chldr[4];
    char    addtnl_amt_len[4];
    char    addtnl_amt[41];
    char    ic_data_len[4];
    char    ic_data[513];
    char    fld_reserved_len[4];
    char    fld_reserved[101];
    char    ch_auth_info_len[4];
    char    ch_auth_info[201];
    char    switch_data_len[4];
    char    switch_data[513];
    char    orig_data_elemts[57];
    char    replacement_amts[43];
    char    rcvg_code_len[3];
    char    rcvg_code[12];
    char    acct_id1_len[3];
    char    acct_id1[29];
    char    acct_id2_len[3];
    char    acct_id2[29];
    char    trans_descrpt_len[4];
    char    trans_descrpt[101];
    char    cup_swresved_len[4];
    char    cup_swresved[101];
    char    host_trans_fee1[13];
    char    host_trans_fee2[13];
    char    tlr_num[9];
    char    open_inst[16];
    char    stlm_inst[16];
    char    risk_flag[2];
    char    batch_flag[2];
    char    batch_date[11];
    char    msq_type[17];
    char    amt_return[13];
    char    authr_id_r[7];
    char    misc_flag[33];
    char    misc_1[129];
    char    misc_2[129];
    char    card_type[3];
    char    sys_tra[20];
} stTBLNTXN;

*/
typedef struct {
    char        mcht_cd [16];
    char        term_id [9];
    char        key_index[5];
    char        mac_key_len [3];
    char        mac_key [33];
    char        mac_key_chk[33];
    char        pin_key_len [3];
    char        pin_key [33];
    char        pin_key_chk[33];
    char        trk_key_len [3];
    char        trk_key [33];
    char        trk_key_chk[33];
    char        pos_bmk[41];
    char        pos_tmk[41];
    char        tmk_st[2];
    char        rec_opr_id[11];
    char        rec_upd_opr[41];
    char        rec_crt_ts[41];
    char        rec_upd_ts[41];
} tbl_term_key_def;

typedef struct {
    int config_id; 
    char config_name[101]; 
    char config_key[101]; 
    char config_value[501]; 
    char config_type[2]; 
    char create_by[65]; 
    char create_time[256]; 
    char update_by[65]; 
    char update_time[256]; 
    char remark[501]; 
} sys_config_def;

typedef struct {
    int user_id; 
    int dept_id; 
    char user_name[31]; 
    char nick_name[31]; 
    char user_type[3]; 
    char email[51]; 
    char phonenumber[12]; 
    char sex[2]; 
    char avatar[101]; 
    char password[101]; 
    char status[2]; 
    char del_flag[2]; 
    char login_ip[129]; 
    char login_date[256]; 
    char pwd_update_date[256]; 
    char create_by[65]; 
    char create_time[256]; 
    char update_by[65]; 
    char update_time[256]; 
    char remark[501]; 
} sys_user_def;

typedef struct {
    int dept_id; 
    int parent_id; 
    char ancestors[51]; 
    char dept_name[31]; 
    int order_num; 
    char leader[21]; 
    char phone[12]; 
    char email[51]; 
    char status[2]; 
    char del_flag[2]; 
    char create_by[65]; 
    char create_time[256]; 
    char update_by[65]; 
    char update_time[256]; 
} sys_dept_def;


typedef struct {
    int role_id; 
    char role_name[31]; 
    char role_key[101]; 
    int role_sort; 
    char data_scope[2]; 
    int menu_check_strictly; 
    int dept_check_strictly; 
    char status[2]; 
    char del_flag[2]; 
    char create_by[65]; 
    char create_time[256]; 
    char update_by[65]; 
    char update_time[256]; 
    char remark[501]; 
} sys_role_def;


typedef struct {
    int menu_id; 
    char menu_name[51]; 
    int parent_id; 
    int order_num; 
    char path[201]; 
    char component[256]; 
    char query[256]; 
    char route_name[51]; 
    int is_frame; 
    int is_cache; 
    char menu_type[2]; 
    char visible[2]; 
    char status[2]; 
    char perms[101]; 
    char icon[101]; 
    char create_by[65]; 
    char create_time[256]; 
    char update_by[65]; 
    char update_time[256]; 
    char remark[501]; 
} sys_menu_def;

typedef struct {
    int dict_id; 
    char dict_name[101]; 
    char dict_type[101]; 
    char status[2]; 
    char create_by[65]; 
    char create_time[256]; 
    char update_by[65]; 
    char update_time[256]; 
    char remark[501]; 
} sys_dict_type_def;

typedef struct {
    int dict_code; 
    int dict_sort; 
    char dict_label[101]; 
    char dict_value[101]; 
    char dict_type[101]; 
    char css_class[101]; 
    char list_class[101]; 
    char is_default[2]; 
    char status[2]; 
    char create_by[65]; 
    char create_time[256]; 
    char update_by[65]; 
    char update_time[256]; 
    char remark[501]; 
} sys_dict_data_def;

typedef struct {
    int post_id; 
    char post_code[65]; 
    char post_name[51]; 
    int post_sort; 
    char status[2]; 
    char create_by[65]; 
    char create_time[256]; 
    char update_by[65]; 
    char update_time[256]; 
    char remark[501]; 
} sys_post_def;

typedef struct {
    int user_id; 
    int post_id; 
} sys_user_post_def;

typedef struct {
    int user_id; 
    int role_id; 
} sys_user_role_def;

typedef struct {
    int role_id; 
    int menu_id; 
} sys_role_menu_def;

typedef struct {
    int notice_id; 
    char notice_title[51]; 
    char notice_type[2]; 
    char notice_content[4096]; 
    char status[2]; 
    char create_by[65]; 
    char create_time[256]; 
    char update_by[65]; 
    char update_time[256]; 
    char remark[256]; 
} sys_notice_def;

typedef struct {
    int oper_id; 
    char title[51]; 
    int business_type; 
    char method[201]; 
    char request_method[11]; 
    int operator_type; 
    char oper_name[51]; 
    char dept_name[51]; 
    char oper_url[256]; 
    char oper_ip[129]; 
    char oper_location[256]; 
    char oper_param[2001]; 
    char json_result[2001]; 
    int status; 
    char error_msg[2001]; 
    char oper_time[256]; 
    int cost_time; 
} sys_oper_log_def;

typedef struct {
    int info_id; 
    char user_name[51]; 
    char ipaddr[129]; 
    char login_location[256]; 
    char browser[51]; 
    char os[51]; 
    char status[2]; 
    char msg[256]; 
    char login_time[256]; 
} sys_logininfor_def;

typedef struct {
    int job_id; 
    char job_name[65]; 
    char job_group[65]; 
    char invoke_target[501]; 
    char cron_expression[256]; 
    char misfire_policy[21]; 
    char concurrent[2]; 
    char status[2]; 
    char create_by[65]; 
    char create_time[256]; 
    char update_by[65]; 
    char update_time[256]; 
    char remark[501]; 
} sys_job_def;

typedef struct {
    int job_log_id; 
    char job_name[65]; 
    char job_group[65]; 
    char invoke_target[501]; 
    char job_message[501]; 
    char status[2]; 
    char exception_info[2001]; 
    char create_time[256]; 
} sys_job_log_def;

typedef struct {
    int usage_key; 
    char srv_id[5]; 
    char param_usage[2]; 
    char param_index[3]; 
    char param_data[65]; 
    char param_dsp[65]; 
} tbl_srv_param_def;


#endif
