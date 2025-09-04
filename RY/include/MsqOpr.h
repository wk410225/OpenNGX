#ifndef __MSQ_OPR_H
#define __MSQ_OPR_H

#define MSQ_RCV_MODE_BLOCK		1
#define MSQ_RCV_MODE_NONBLOCK	2

#define SRV_MSQ_NUM_MAX			200

#define MSQ_MSG_SIZE_MAX	 5*1024*1024 
#define SRV_ID_LEN  4

typedef struct
{
	char sSrvId[SRV_ID_LEN];
	int nMsqId;
	long lMsqType;
} T_SrvMsq;

/*****************************************************************************/
/* FUNC:   int MsqInit (char *sSrvId�� T_SrvMsq *ptSrvMsq);                  */
/* INPUT:  sSrvId: ����server��id                                            */
/*         ptSrvMsq: ���ڴ��server id��queue id                             */
/* OUTPUT: ptSrvMsq: ���ڴ��server id��queue id                             */
/* RETURN: 0: �ɹ�, ����: ʧ��                                               */
/* DESC:   ����tbl_srv_inf, tbl_msq_inf, �򿪻򴴽���sSrvId��ص���Ϣ����    */
/*         ����server id��queue id����ptSrvMsq��                             */
/*         ptSrvMsq�ڵ���ǰ���ʼ��,���                                     */
/*****************************************************************************/
int MsqInit (char *sSrvId, T_SrvMsq *ptSrvMsq);

/*****************************************************************************/
/* FUNC:   int MsqRcv (char *sSrvId, T_SrvMsq *ptSrvMsq, long lMsqType,      */
/*                     int *pnMsgSize, char *sMsgBuf);                       */
/* INPUT:  sSrvId: server id                                                 */
/*         ptSrvMsq: ���ڴ��server id��queue id                             */
/*         lMsqType: ������Ϣʱ��message type                                */
/*         nRcvMode: ������Ϣ�ķ�ʽ,                                         */
/*                   MSQ_RCV_MODE_BLOCK, MSQ_RCV_MODE_NONBLOCK               */
/*         pnMsgSize: Ԥ�ƽ��յ���Ϣ�ĳ���, ������message_type               */
/* OUTPUT: pnMsgSize: ʵ�ʽ��յ���Ϣ�ĳ���, ������message_type               */
/*         sMsgBuf: ���յ���Ϣ                                               */
/* RETURN: 0: �ɹ�, ����: ʧ��                                               */
/* DESC:   ���շ��͸�sSrvId��Ӧ����Ϣ���е���Ϣ                              */
/*         ���øú���֮ǰ,�����ȵ���MsqInit����ʼ��                          */
/*****************************************************************************/
int MsqRcv (char *sSrvId, T_SrvMsq *ptSrvMsq, long lMsqType, int nRcvMod, int *pnMsgSize, char *sMsgBuf);

/*****************************************************************************/
/* FUNC:   int MsqSnd (char *sToSrvId,                                       */
/*                     T_SrvMsq *ptSrvMsq, long lMsqType,                    */
/*                     int nMsgSize, char *sMsgBuf);                         */
/* INPUT:  sToSrvId: server id                                               */
/*         ptSrvMsq: ���ڴ��server id��queue id                             */
/*         lMsqType: ������Ϣʱ��message type                                */
/*         nMsgSize: ���͵���ϢsMsgBuf�ĳ���, ������message_type             */
/*         sMsgBuf: ���͵���Ϣ                                               */
/* OUTPUT: ��                                                                */
/* RETURN: 0: �ɹ�, ����: ʧ��                                               */
/* DESC:   ������Ϣ��sToSrvId��Ӧ����Ϣ����,                                 */
/*         ����Ϣ���������е���Ϣ������������MSQ_NUM_MAX��ֵʱ, ����ʧ��     */
/*         ����Ϣ���������е���Ϣ��Сռ���������İٷֱ�                      */
/*           ��������MSQ_NUM_MAX��ֵʱ, ����ʧ��                             */
/*         ���øú���֮ǰ,�����ȵ���MsqInit����ʼ��                          */
/*****************************************************************************/
int MsqSnd (char *sToSrvId, T_SrvMsq *ptSrvMsq, long lMsqType, int nMsgSize, char *sMsgBuf);

#endif
