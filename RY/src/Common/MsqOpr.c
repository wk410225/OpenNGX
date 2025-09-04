#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <errno.h>
#include <sys/msg.h>
//#include "SrvDef.h"
#include "SrvParam.h"
#include "MsqOpr.h"
#include "DbsDef.h"
#include "DbsTbl.h"
#include "ErrCode.h"
#include "Common.h"
#include <stdio.h>
#include <unistd.h>

#define MSG_ONE_MAX	    1024	
#define MSQ_PCT_MAX		0.8

#define MSQ_GET_FLAG	0660
#define MSQ_SEPERATOR	','

typedef struct
{
	long	lMsgType;
	char	sMsg[MSQ_MSG_SIZE_MAX];
} T_MsqMsgDef;

/*****************************************************************************/
/* FUNC:   int MsqInit (char *sSrvId�� T_SrvMsq *ptSrvMsq);                  */
/* INPUT:  sSrvId: ����server��id                                            */
/*         ptSrvMsq: ���ڴ��server id��queue id                             */
/* OUTPUT: ptSrvMsq: ���ڴ��server id��queue id                             */
/* RETURN: 0: �ɹ�, ����: ʧ��                                               */
/* DESC:   ����tbl_srv_inf, tbl_msq_inf, �򿪻򴴽���sSrvId��ص���Ϣ����    */
/*         ����server id��queue id����ptSrvMsq��                             */
/*         ptSrvMsq�ڵ���ǰ���ʼ��, ���                                    */
/*****************************************************************************/
int MsqInit (char *sSrvId, T_SrvMsq *ptSrvMsq)
{
	char		*sToken;
	char		sTmpSrvId[5];
	char		sRelateSrvId[2048];
	int		nReturnCode;
	int		nMsqNum;
	long            lUsageKey;

	Tbl_srv_inf_Def	tTblSrvInf;
	Tbl_msq_inf_Def	tTblMsqInf;
	
	if (getenv(SRV_USAGE_KEY))
		lUsageKey=atoi (getenv(SRV_USAGE_KEY));
	else
		return -1;

	/* get related queue id for sSrvId */
	memset ((char *)&tTblSrvInf, 0x00, sizeof (tTblSrvInf));
	memcpy (tTblSrvInf.srv_id, sSrvId, SRV_ID_LEN);
	tTblSrvInf.usage_key = lUsageKey;
	nReturnCode = DbsSRVINF(DBS_SELECT, &tTblSrvInf);
	if (nReturnCode)
	{
		printf("MsqInit DbsSRVINF  %d error!\n",__LINE__);
		return (nReturnCode);
	}
	
	nMsqNum = 0;

	if (tTblSrvInf.msq_int_id[0] != ' ')
	{
		/* initialize receiving queue */
		/* get queue key from tbl_msq_inf */
		memset ((char *)&tTblMsqInf, 0x00, sizeof (tTblMsqInf));
		memcpy (tTblMsqInf.msq_int_id, tTblSrvInf.msq_int_id, sizeof (tTblMsqInf.msq_int_id));
		nReturnCode = DbsMSQINF(DBS_SELECT, &tTblMsqInf);
		if (nReturnCode)
		{
			printf("MsqInit DbsMSQINF error!\n");
			return (nReturnCode);
		}
		/* open msq */
		nReturnCode = msgget (atol (tTblMsqInf.msq_key), MSQ_GET_FLAG|IPC_CREAT);
		if (nReturnCode == -1)
			return (ERR_CODE_MSQ_BASE+errno);
		/* save in sSrvId */
		strcpy (ptSrvMsq[nMsqNum].sSrvId, sSrvId);
		ptSrvMsq[nMsqNum].nMsqId = nReturnCode;
		ptSrvMsq[nMsqNum].lMsqType = atol (tTblMsqInf.msq_type);
	
		nMsqNum++;
	}
	
	/* initialize sending queue */
	memset(sRelateSrvId, 0, sizeof(sRelateSrvId));
	CommonAllTrim(tTblSrvInf.relate_srv_id);
	strcpy(sRelateSrvId, tTblSrvInf.relate_srv_id);
	sToken = sRelateSrvId;
	while (sRelateSrvId[0] != '\0' && sRelateSrvId[0] != ' ')
	{
		/* check whether msq count exceeds SRV_MSQ_NUM_MAX */
		if (nMsqNum > SRV_MSQ_NUM_MAX)
			return (ERR_CODE_MSQ_INIT);

		/* get internal msq id from tbl_srv_inf */        	
		memset ((char *) &tTblSrvInf, 0x00, sizeof (tTblSrvInf));
		tTblSrvInf.usage_key = lUsageKey;
		memcpy (tTblSrvInf.srv_id, sToken, SRV_ID_LEN);
		nReturnCode = DbsSRVINF(DBS_SELECT, &tTblSrvInf);
		if (nReturnCode)
		{
			printf("MsqInit DbsSRVINF [%s]  %d error!\n",tTblSrvInf.srv_id,__LINE__);
			return (nReturnCode);
		}
	
		/* get queue key from tbl_msq_inf */
		memset ((char *)&tTblMsqInf, 0x00, sizeof (tTblMsqInf));
		strcpy (tTblMsqInf.msq_int_id, tTblSrvInf.msq_int_id);
		nReturnCode = DbsMSQINF(DBS_SELECT, &tTblMsqInf);
		if (nReturnCode)
		{
			printf("MsqInit DbsMSQINF  line %d  msqid %s error %d\n",__LINE__,tTblMsqInf.msq_int_id,nReturnCode);	
			return (nReturnCode);
		}	
		/* open msq */
		nReturnCode = msgget (atol (tTblMsqInf.msq_key), MSQ_GET_FLAG|IPC_CREAT);
		if (nReturnCode == -1)
		{
			printf("msgget error %d\n",__LINE__, ERR_CODE_MSQ_BASE+errno);	
			return (ERR_CODE_MSQ_BASE+errno);
		}
			
		/* save in sSrvId */
		memcpy(ptSrvMsq[nMsqNum].sSrvId, sToken, SRV_ID_LEN);
		ptSrvMsq[nMsqNum].nMsqId = nReturnCode;
		ptSrvMsq[nMsqNum].lMsqType = atol(tTblMsqInf.msq_type);

		nMsqNum++;
		
		/* get next sending queue */
		sToken = strchr (sToken, MSQ_SEPERATOR);
		if (sToken == NULL)
			break;
		sToken++;
	}
	
	return 0;
}

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
int MsqRcv (char *sSrvId, T_SrvMsq *ptSrvMsq, long lMsqType, int nRcvMod, int *pnMsgSize, char *sMsgBuf)
{
	int			nReturnCode;
	int			nMsqId;
	long		lMsgFlag;
	long		lActualMsqType;
	T_MsqMsgDef	tMsqMsg;

	/* get msg queue id, msg type in ptSrvMsq */
	nReturnCode = MsqGetId (ptSrvMsq, sSrvId, &nMsqId, &lActualMsqType);
	if (nReturnCode)
	{
		return nReturnCode;
	}
	if (lMsqType != 0)
		lActualMsqType = lMsqType;
	
	/* set msg flag */
	lMsgFlag = 0;
	if (nRcvMod == MSQ_RCV_MODE_NONBLOCK)
		lMsgFlag = lMsgFlag | IPC_NOWAIT;
	
	/* receive msg from queue */

	nReturnCode = msgrcv (nMsqId, &tMsqMsg, *pnMsgSize, lActualMsqType, lMsgFlag);
	if (nReturnCode == -1)
	{
        if (errno == ENOMSG)
			return (ERR_CODE_MSQ_NO_MSG);
		else
			return (ERR_CODE_MSQ_BASE+errno);
	}
#if 0
	else
	{
		*pnMsgSize = nReturnCode;
		memcpy(sMsgBuf, tMsqMsg.sMsg, *pnMsgSize);
		return (0);
	}
#endif
	else
	{
		if( nRcvMod != MSQ_RCV_MODE_NONBLOCK )
		{
			lActualMsqType = ((SYSHEAD *)tMsqMsg.sMsg)->shMsgType;
			int shSourceID=((SYSHEAD *)tMsqMsg.sMsg)->shSourceID;
			int i = 0;
			if (shSourceID > 1)
			{
				HtLog("msqrcv.log",1,__FILE__,__LINE__,"shMsqType =[%d] shSourceID=[%d]", lActualMsqType, shSourceID);
				HtLog("msqrcv.log",1,__FILE__,__LINE__,"msqrcv [%d] len=[%d] msg=[%s]", i, nReturnCode ,tMsqMsg.sMsg+MSGHEADLEN);
			}
			*pnMsgSize = nReturnCode;
			memcpy(sMsgBuf,tMsqMsg.sMsg,*pnMsgSize);
			if (lActualMsqType > 0)
			{
				for(i=1; i < shSourceID; i++ )
				{
					//memset(&tMsqMsg, 0, sizeof(tMsqMsg));
					//nReturnCode = msgrcv (nMsqId, &tMsqMsg,*pnMsgSize, lActualMsqType,lMsgFlag);
					nReturnCode = msgrcv (nMsqId, &tMsqMsg,*pnMsgSize, lActualMsqType, 0);
					if (nReturnCode  == -1)
					{
						if (errno == ENOMSG)
							return (ERR_CODE_MSQ_NO_MSG);
						else
							return (ERR_CODE_MSQ_BASE+errno);
					}
					else 
					{
						HtLog("msqrcv.log",1,__FILE__,__LINE__,"msqrcv [%d] len=[%d] msg=[%s]", i, nReturnCode ,tMsqMsg.sMsg);
						memcpy(sMsgBuf + *pnMsgSize,tMsqMsg.sMsg,nReturnCode);
						*pnMsgSize += nReturnCode;
					}
				}
			}
		}
	}
	return(0);
}

/*****************************************************************************/
/* FUNC:   int MsqSnd (char *sToSrvId, T_SrvMsq *ptSrvMsq, long lMsqType,    */
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
int MsqSnd (char *sToSrvId, T_SrvMsq *ptSrvMsq, long lMsqType, int nMsgSize, char *sMsgBuf)
{
	int			nReturnCode;
	int			nMsqId;
	int			nMsqNumMax;
	long		lMsgFlag;
	long		lActualMsqType;
	T_MsqMsgDef	tMsqMsg;
	struct msqid_ds	tMsqidDs;
	char		*lspTmp;

	/* get msg queue max num */
	if((lspTmp = getenv(MSQ_NUM_MAX)) == NULL)
	{
		nMsqNumMax = MSQ_NUM_MAX_DEFAULT;
	}
	else
	{
		if((nMsqNumMax = atoi(lspTmp)) <= 0)
		{
			nMsqNumMax = MSQ_NUM_MAX_DEFAULT;
		}
	}
	
	/* get msg queue id in ptSrvMsq */
	nReturnCode = MsqGetId (ptSrvMsq, sToSrvId, &nMsqId, &lActualMsqType);
	if (nReturnCode)
	{
		return nReturnCode;
	}
	
	/* check queue state */
	nReturnCode = msgctl (nMsqId, IPC_STAT, &tMsqidDs);
	if (nReturnCode	== -1)
	{
		return (ERR_CODE_MSQ_BASE+errno);
	}
	
	/* check number of msg in the queue */
	if (tMsqidDs.msg_qnum > nMsqNumMax)
	{
		return (ERR_CODE_MSQ_TOO_MANY_MSG);
	}
	
	/* check bytes in the queue */
	/*
	if (tMsqidDs.msg_qbytes * MSQ_PCT_MAX < tMsqidDs.msg_cbytes)
		return (ERR_CODE_MSQ_TOO_MANY_MSG);
	*/

	/* prepare buffer to be sent */
	if (lMsqType != 0)
		lActualMsqType = lMsqType;
	tMsqMsg.lMsgType = lActualMsqType;

#if 0
	memcpy(tMsqMsg.sMsg, sMsgBuf, nMsgSize);
	/* send msg into queue */
	nReturnCode	= msgsnd (nMsqId, &tMsqMsg, nMsgSize, IPC_NOWAIT);
	if (nReturnCode	== -1)
	{
		return (ERR_CODE_MSQ_BASE+errno);
	}
#endif 
	if(nMsgSize<=MSG_ONE_MAX)
	{
		((SYSHEAD *)sMsgBuf)->shSourceID = 1 ;
		memcpy(tMsqMsg.sMsg, sMsgBuf,nMsgSize);
		nReturnCode=msgsnd(nMsqId, &tMsqMsg, nMsgSize,0);
		if ( nReturnCode == -1)
			return (ERR_CODE_MSQ_BASE+errno);
	}
	else
	{
		int n ;
		if(nMsgSize%MSG_ONE_MAX == 0)
			n = nMsgSize/MSG_ONE_MAX ;
		else
			n = nMsgSize/MSG_ONE_MAX + 1 ;
		//HtLog("msq.log",1,__FILE__,__LINE__,"msqsnd [%d] ", n);
		int pid = getpid() ;
		int i=0 , offset=0, len =0;
		do
		{
			offset = i*MSG_ONE_MAX;
			if(nMsgSize-offset >= MSG_ONE_MAX)
				len=MSG_ONE_MAX ;
			else
				len = nMsgSize - offset;
			((SYSHEAD *)sMsgBuf)->shMsgType = pid ;
			((SYSHEAD *)sMsgBuf)->shSourceID = n ;
			memcpy(tMsqMsg.sMsg, sMsgBuf + offset , len );
			if (i == 0)
			{
				 HtLog("msqsnd.log",1,__FILE__,__LINE__,"shMsgType=[%d] shSourceID=[%d]", ((SYSHEAD *)sMsgBuf)->shMsgType, ((SYSHEAD *)sMsgBuf)->shSourceID);
				 HtLog("msqsnd.log",1,__FILE__,__LINE__,"msqsnd [%d] len=[%d] msg[%s]", i,len ,tMsqMsg.sMsg+MSGHEADLEN);
			}
			else
			{
				 HtLog("msqsnd.log",1,__FILE__,__LINE__,"msqsnd [%d] len=[%d] msg[%s]", i,len, tMsqMsg.sMsg);
			}
			nReturnCode = msgsnd(nMsqId, &tMsqMsg, len, 0);
			if ( nReturnCode  == -1 )
				return (ERR_CODE_MSQ_BASE + errno);
			tMsqMsg.lMsgType = pid;
			i++;
		} while( i < n);
	}
	return 0;
}


/*****************************************************************************/
/* FUNC:   int MsqGetId (T_SrvMsq *ptSrvMsq, char *sSrvId, int *pnMsqId      */
/*                       long *plMsqType);                                   */
/* INPUT:  ptSrvMsq: ���ڴ��server id��queue id                             */
/*         sSrvId: server id                                                 */
/* OUTPUT: pnMsqId: ��ptSrvMsq��sSrvId��Ӧ��queue id                         */
/*         plMsqType: ��ptSrvMsq��sSrvId��Ӧ��msg type                       */
/* RETURN: 0: �ɹ�, ����: ʧ��                                               */
/* DESC:   ��ptSrvMsq�и���sSrvId���Ҷ�Ӧ��queue id                          */
/*****************************************************************************/
int MsqGetId (T_SrvMsq *ptSrvMsq, char *sSrvId, int *pnMsqId, long *plMsqType)
{
	int i;
	
	for (i = 0; i < SRV_MSQ_NUM_MAX; i++)
	{
		if (!strncmp (ptSrvMsq[i].sSrvId, sSrvId, SRV_ID_LEN))
		{
			break;
		}
	}
	
	if (i < SRV_MSQ_NUM_MAX)
	{
		*pnMsqId = ptSrvMsq[i].nMsqId;
		*plMsqType = ptSrvMsq[i].lMsqType;
		return 0;
	}
	else
		return (ERR_CODE_MSQ_INIT);
}
