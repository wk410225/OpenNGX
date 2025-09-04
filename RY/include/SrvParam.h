#ifndef __SRV_PARAM_H
#define __SRV_PARAM_H

/* env var for Common/ToOpr.c */
#define TOCTL_WAIT_SECOND			"TOCTL_WAIT_SECOND"
#define TOCTL_WAIT_SECOND_DEFAULT	5

/* env var for Common/HtLog.c */
#define LOG_MODE					"LOG_MODE"
#define LOG_FILE_PATH				"LOG_FILE_PATH"
#define LOG_SWITCH_MODE				"LOG_SWITCH_MODE"
#define LOG_SIZE					"LOG_SIZE"
#define LOG_SIZE_DEFAULT			5

/* env var for Common/MsqOpr.c */
#define MSQ_NUM_MAX					"MSQ_NUM_MAX"
#define MSQ_NUM_MAX_DEFAULT			20

/* env var for ipc comress */
#define MSG_COMPRESS_FLAG			"MSG_COMPRESS_FLAG"

/* env var for HSM opr */
#define HSM_INDEX_CUP				"HSM_INDEX_CUP"

/* env var for SavFwd */
#define SF_CYCL_INTV_MIN			"SF_CYCL_INTV_MIN"
#define SF_GRP_MSG_NUM				"SF_GRP_MSG_NUM"
#define SF_GRP_INTV_SECOND			"SF_GRP_INTV_SECOND"

/* env var for Batch */
#define BT_AUTH_DAY					"BT_AUTH_DAY"
#define BT_BAK_FILE_PATH			"BT_BAK_FILE_PATH"
#define BT_GRP_REC_NUM				"BT_GRP_REC_NUM"
#define BT_HIS_TXN_DAY				"BT_HIS_TXN_DAY"
#define BT_LOG_DAY					"BT_LOG_DAY"

/* env var for Dbs, informix only */
#define DBNAME						"DBNAME"

/* env var for Bridge */
#define BDG_CHECK_CARD_FLAG			"BDG_CHECK_CARD_FLAG"

/* env var for Dumpmsg */
#define DM_CHECK_HOUR				"DM_CHECK_HOUR"
#define DM_SLEEP_MIN				"DM_SLEEP_MIN"

#define SRV_USAGE_KEY				"SRV_USAGE_KEY"

#endif
