#ifndef __ERR_CODE_H
#define __ERR_CODE_H

#define ERR_CODE_BASE			0

#define ERR_CODE_CONV_BASE		10000

#define ERR_CODE_CONV_NO_RULE		ERR_CODE_CONV_BASE+2001

#define ERR_CODE_COMMON_BASE	20000

#define ERR_CODE_COMMON_SRV_ID_INV	ERR_CODE_COMMON_BASE+1001

#define ERR_CODE_MSQ_BASE		30000

#define ERR_CODE_MSQ_INIT			ERR_CODE_MSQ_BASE+1001
#define ERR_CODE_MSQ_RCV			ERR_CODE_MSQ_BASE+1002
#define ERR_CODE_MSQ_SND			ERR_CODE_MSQ_BASE+1003
#define ERR_CODE_MSQ_TOO_MANY_MSG	ERR_CODE_MSQ_BASE+1004
#define ERR_CODE_MSQ_NO_MSG			ERR_CODE_MSQ_BASE+1005

#define ERR_CODE_LOG_BASE		40000
#define ERR_CODE_LOG_FILE_OPEN		ERR_CODE_LOG_BASE+1001

#define ERR_CODE_DBS_BASE		50000
#define ERR_CODE_DBS_SELECT			ERR_CODE_DBS_BASE+1001
#define ERR_CODE_DBS_INSERT			ERR_CODE_DBS_BASE+1002
#define ERR_CODE_DBS_UPDATE			ERR_CODE_DBS_BASE+1003
#define ERR_CODE_DBS_DELETE			ERR_CODE_DBS_BASE+1004

#define ERR_CODE_BRG_BASE		60000
#define ERR_CODE_BRD_NO_ROUTE		ERR_CODE_BRG_BASE+1001

#define CONNECT_DB_ERROR              "01"
#define CONNECT_DB_ERROR_DSP          "���ݿ����ӳ���"

#define DB_SELECT_BAT_MAIN_ERROR      "02"
#define DB_SELECT_BAT_MAIN_ERROR_DSP  "��ѯ��������ʧ��"

#define BAT_CHECK_CLS_ERROR           "03"
#define BAT_CHECK_CLS_ERROR_DSP       "�����������ȼ����ʧ��"

#define BAT_CHECK_REL_ERROR           "03"
#define BAT_CHECK_REL_ERROR_DSP       "��������������ϵ���ʧ��"

#define BAT_REPEAT_RUN_ERROR          "04"
#define BAT_REPEAT_RUN_ERROR_DSP      "���������ظ�����"

#define BAT_PROC_NOT_EXIST            "05"
#define BAT_PROC_NOT_EXIST_DSP        "�������г��򲻴���"

#define BAT_TASK_ASSIGN_STA_ERROR     "06"
#define BAT_TASK_ASSIGN_STA_ERROR_DSP "�������������״̬����"

#define BAT_RUN_TASK_ERROR            "07"
#define BAT_RUN_TASK_ERROR_DSP        "��������ִ��ʧ��"

#define DB_BAT_INIT_ERROR             "08"
#define DB_BAT_INIT_ERROR_DSP         "������ʼ��ʧ��"


#endif
