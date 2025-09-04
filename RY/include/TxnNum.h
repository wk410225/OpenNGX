#ifndef __TXN_NUM_H
#define __TXN_NUM

#define	INDEX_TXN_NUM_TYPE		0
#define INDEX_TXN_NUM			1
#define INDEX_TXN_NUM_REQ_RSP	3

#define TXN_TYPE_LEN				2

#define TXN_REQ         '1'
#define TXN_RSP         '2'
/* tbl_txn.txn_num[3] */
#define TXN_NUM_BDT_REQ				'1'
#define TXN_NUM_BDT_RSP				'2'
#define TXN_NUM_TDB_REQ				'3'
#define TXN_NUM_TDB_RSP				'4'
#define TXN_NUM_BDB_REQ				'5'
#define TXN_NUM_BDB_RSP				'6'
#define TXN_NUM_CUST_REQ			'7'
#define TXN_NUM_CUST_RSP			'8'
#define TXN_NUM_TERM_RSP            '9'

/* tbl_txn.txn_num[0] */
#define TXN_NUM_NORMAL				'1'
#define TXN_NUM_REVSAL				'2'
#define TXN_NUM_CANCEL				'3'
#define TXN_NUM_CANCEL_REVSAL		'4'
#define TXN_NUM_NOTICE				'5'
#define TXN_NUM_MANAGE				'6'
#define TXN_NUM_POS_MANAGE			'8'

/* tbl_txn.txn_num[1..2] */
#define TXN_PRE_AUTH				41

/* define txn num */
#define TXN_NUM_PCT_BDT_REQ			"1101"
#define TXN_NUM_PCT_BDB_REQ			"1105"
#define TXN_NUM_TRT_TDB_RSP			"1054"
#define TXN_NUM_TVT_TDB_RSP			"1064"
#define TXN_NUM_AUT_REQ 			"1011"
#define TXN_NUM_AUT_RSP 			"1012"
#define TXN_NUM_AUT1_RSP 			"1091"
#define TXN_NUM_AUT2_RSP 			"3091"
#define TXN_NUM_AUT_CANCEL_REQ	    "3011"
#define TXN_NUM_CONS_CANCEL_REQ	    "3101"
#define TXN_NUM_POS_FUND_REQ	    "5151"
#define TXN_NUM_AUT_CANCEL_RSP	 	"3012"
#define TXN_NUM_AUT_CANL_REVL_REQ	"4011"
#define TXN_NUM_AUT_CANL_REVL_RSP	"4012"
#define TXN_NUM_ADV_REQ             "5095"
#define TXN_NUM_ADV_RSP             "5096"
#define TXN_RCV_END_MSG             "6065"
#define TXN_SND_END_MSG             "6032"
#define TXN_ENT_NOTICE              "XXX"
#endif
