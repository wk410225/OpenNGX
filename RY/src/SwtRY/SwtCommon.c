#include "Switch.h"
#include <sys/time.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <bcrypt/bcrypt.h>
//#define  KEY  "abcdefghijklmnopqrstuvwxyz0123456789"
#define  KEY  "abcdefghijklmnopqrstuvwxyz"

extern redisContext    *c; // Redis连接上下文
extern redisReply      *reply; // Redis命令回复

static long get_current_time_millis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}   


#if 0
int SendMsg (T_IpcIntTxnDef *ptIpcIntTxn, Tbl_txn_Def *ptTxn, Tbl_txn_Def *ptOrigTxn)
{
	char	sFuncName[] = "SendMsg";
	char	sMsgBuf[MSQ_MSG_SIZE_MAX];
	int 	nMsgLen;
	int	nReturnCode;
	int	nLineStat;
	char            *IPC = (char *)ptIpcIntTxn+IPC_XML_OFFSET;
	int 	nRet;
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s begin.", sFuncName);
	HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__,"ptIpcIntTxn->sMsgDestId1 %4.4s",ptIpcIntTxn->sMsgDestId);
	HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__,"ptIpcIntTxn->sTxnNum %4.4s",ptIpcIntTxn->sTxnNum);
	if( memcmp(ptIpcIntTxn->sMsgDestId, "170", SRV_ID_LEN-1) == 0)
	{
		memset(ptIpcIntTxn->sMsqType, '0', FLD_MSQ_TYPE_LEN);
	}

	//HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__,"ptIpcIntTxn->sRespCode[%2.2s]",ptIpcIntTxn->sRespCode);
	char sLen[8];
	memset(sLen, 0x00,sizeof(sLen));
	memcpy(sLen, IPC, 6);

	//nMsgLen = sizeof (*ptIpcIntTxn);
	nMsgLen = atoi(sLen)+6+IPC_XML_OFFSET;
	memset(sMsgBuf, 0x20, sizeof(sMsgBuf));
	memcpy (sMsgBuf, (char *)ptIpcIntTxn, nMsgLen);

	nReturnCode = MsqSnd (SRV_ID_IPC2XML, gatSrvMsq, 0, nMsgLen, sMsgBuf);
	if (nReturnCode)
	{
		HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "MsqSnd error, %d. %d[%s] ", 
				nReturnCode,errno,strerror(errno));
		return -1;
	}
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s from %4.4s to %4.4s end.", 
			sFuncName,ptIpcIntTxn->sMsgSrcId,ptIpcIntTxn->sMsgDestId);
	return 0;
}

int InsertSafIpc (T_IpcIntTxnDef *ptSendIpcIntTxn, Tbl_txn_Def *ptTxn, char *sCount)
{
    char                sFuncName[] = "InsertSafIpc";
    int                 nCount;
    int                 nReturnCode;
    char                sTmpBuf[36];
    int                 nMsgLen;
    Tbl_Saf_Saf_Def     tSafMsg;
    char                *IPC = (char *)ptSendIpcIntTxn+IPC_XML_OFFSET;

    HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s begin.", sFuncName);

    HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "nCount = %d .", atoi(sCount) );

    nCount = atoi (sCount);
    if (nCount > 0)
    {
        DbsBegin ();
        memset ((char *)&tSafMsg, 0, sizeof (tSafMsg));
        memcpy (tSafMsg.inst_date, ptTxn->inst_date, 8);
        memcpy (tSafMsg.inst_time, ptTxn->inst_date+8, 6);
        memcpy (tSafMsg.msg_src_id, ptTxn->msg_src_id, SRV_ID_LEN);
        memcpy (tSafMsg.txn_num, ptTxn->txn_num, FLD_TXN_NUM_LEN);
        memcpy (tSafMsg.sys_seq_num, ptTxn->sys_seq_num, FLD_SYS_SEQ_NUM_LEN);
        memcpy (tSafMsg.msg_dest_id, ptSendIpcIntTxn->sMsgDestId, SRV_ID_LEN);
        memcpy (tSafMsg.key_rsp, ptSendIpcIntTxn->sKeyRsp, KEY_RSP_LEN);
        memcpy (tSafMsg.count, sCount, 2);
        SetTxnValue(tSafMsg.date_settlmt, IPC, "DATE_SETTLMT", sizeof(tSafMsg.date_settlmt) -1);
        SetTxnValue(tSafMsg.trans_date_time, IPC, "TRANS_DATE_TIME", sizeof(tSafMsg.trans_date_time) -1);
        SetTxnValue(tSafMsg.pan, IPC, "PAN", sizeof(tSafMsg.pan) -1);
        SetTxnValue(tSafMsg.amt_trans, IPC, "AMT_TRANS", sizeof(tSafMsg.amt_trans) -1);
        memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
        memcpy(sTmpBuf, IPC, 6);
        nMsgLen = atoi(sTmpBuf);
        if(nMsgLen > 7994)
        {
            HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "�ڲ�IPC TLV ����%d.", nMsgLen);
            return -1;
        }
        sprintf (tSafMsg.msg_len, "%06d", nMsgLen);
		if (nMsgLen > 3994)
        {
            memcpy (tSafMsg.msg_ipc1, IPC, 4000); //add by Zhengzhou Kalian W@20180808
            memcpy (tSafMsg.msg_ipc2, IPC + 4000, nMsgLen - 4000);
        }
        else
        {
            memcpy (tSafMsg.msg_ipc1, IPC, nMsgLen+6);
        }

        nReturnCode = DbsSafSafIpc (DBS_INSERT, &tSafMsg);
        if (nReturnCode)
        {
            HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "DbsSafMsg insert error, %d.", nReturnCode);
            DbsRollback ();
            return -1;
        }
        DbsCommit ();
    }
    HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s end.", sFuncName);
    return 0;
}

int set_mcht_notice(T_IpcIntTxnDef *ptIpcIntTxn,Tbl_txn_Def *tTxn)
{
	HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "set_mcht_notice begin");
	T_IpcIntTxnDef ptIpc;
	memcpy((char*)&ptIpc, ptIpcIntTxn, sizeof(T_IpcIntTxnDef));
	char    *RSPIPC = (char *)&ptIpc+IPC_XML_OFFSET;
	int		nReturnCode = -1;
	char	sTmpBuf[256];

	GetIpcValueExt(RSPIPC, "FLD_39", tTxn->resp_code, F039_LEN);
	if(strcmp(tTxn->resp_code, "00") == 0 )
	{
		SetIpcValue(RSPIPC, "MISC_1", tTxn->misc_1);
		SetIpcValue(RSPIPC, "$DATE_TIME", tTxn->inst_date);
		SetIpcValue(RSPIPC, "CARD_TYPE", tTxn->card_type);
		HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "json֪ͨ");
		memcpy( ptIpc.sTxnNum, "A125", FLD_TXN_NUM_LEN );
		memcpy( ptIpc.sMsgSrcId, "2809", SRV_ID_LEN );
		memcpy( ptIpc.sMsgDestId, "1102", FLD_TXN_NUM_LEN );
		nReturnCode = SendMsgIpc(&ptIpc, tTxn, NULL);
		if (nReturnCode)
		{
			HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__,
					"SendMsg error, %d. Discard this message %s", nReturnCode,strerror(errno));
		}
	}
}
#endif
int num2hex(int num, char* hex_str) //int 100--->"96"
{
	int tmp, mask = 0xf;
	int i;
	char hex[16];
	memset(hex, 0x00, sizeof(hex));
	if(num % 16  == 0)
	{    
		sprintf(hex_str, "%d0", num/16);
		return 0;
	}   
	for(i= 9; i>=0; i--)
	{   
		tmp= num &mask;
		if(tmp == 0)
			break;
		hex[i] = (tmp>=10)?((tmp -10) +'a'):(tmp+'0');
		num = num >> 4;
	}
	if(strlen(hex+i+1)==1)
		sprintf(hex_str, "0%s", hex+i+1);
	else
		strcpy(hex_str, hex+i+1);
	return 0;
}



void J2S(cJSON *root , void *stdb, TABFUN *TBL_DB_DEF_COLMAP)
{
	int i = 0;	
	cJSON *item = NULL;
	while(TBL_DB_DEF_COLMAP[i].f_colname[0] != 0x00)
	{
		//HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "f_colname=[%s]", TBL_DB_DEF_COLMAP[i].f_colname);
		item = cJSON_GetObjectItemCaseSensitive(root, TBL_DB_DEF_COLMAP[i].f_colname);
		if(item != NULL)
		{
			switch(TBL_DB_DEF_COLMAP[i].type)
			{
				case 'C':
					if(item->type  == cJSON_String )
						strcpy((char *)stdb + TBL_DB_DEF_COLMAP[i].offset, item->valuestring );
					else 
						HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "%s  datatype not same", TBL_DB_DEF_COLMAP[i].f_colname);
					break;
				case 'I':
					if(item->type  == cJSON_Number )
						*((int *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)) = item->valueint;
					else if(item->type  == cJSON_String)
						*((long *)((char *)stdb + TBL_DB_DEF_COLMAP[i].offset)) = atoi(item->valuestring);
					else 
						HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "%s  datatype not same", TBL_DB_DEF_COLMAP[i].f_colname);
					break;
				case 'F':
					if(item->type  == cJSON_Number )
						*((float *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)) = (float)item->valuedouble;
					else 
						HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "%s  datatype not same", TBL_DB_DEF_COLMAP[i].f_colname);
					break;
				case 'L':
					if(item->type  == cJSON_Number )
						*((long *)((char *)stdb + TBL_DB_DEF_COLMAP[i].offset)) = (long )item->valuedouble;
					else if(item->type  == cJSON_String)
						*((long *)((char *)stdb + TBL_DB_DEF_COLMAP[i].offset)) = atol(item->valuestring);
					else 
						HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "%s  datatype not same", TBL_DB_DEF_COLMAP[i].f_colname);
					break;
				case 'D':
					if(item->type  == cJSON_Number )
						*((double *)((char *)stdb + TBL_DB_DEF_COLMAP[i].offset)) = item->valuedouble;
					else 
						HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "%s  datatype not same", TBL_DB_DEF_COLMAP[i].f_colname);
					break;
				default:
					HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"bad type");
			}
		}
		i++;
	}
}
void NJ2S(cJSON *root , void *stdb, TABFUN *TBL_DB_DEF_COLMAP)
{
	int i = 0;	
	cJSON *item = NULL;
	while(TBL_DB_DEF_COLMAP[i].f_colname[0] != 0x00)
	{
		if(TBL_DB_DEF_COLMAP[i].jreq[0] == 0x00)
		{
			i++;
			continue;
		}
		//HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "f_colname=[%s]", TBL_DB_DEF_COLMAP[i].f_colname);
		item = cJSON_GetObjectItemCaseSensitive(root, TBL_DB_DEF_COLMAP[i].jreq);
		if(item != NULL)
		{
			switch(TBL_DB_DEF_COLMAP[i].type)
			{
				case 'C':
					if(item->type  == cJSON_String )
						strcpy((char *)stdb + TBL_DB_DEF_COLMAP[i].offset, item->valuestring );
					else 
						HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "%s  datatype not same", TBL_DB_DEF_COLMAP[i].f_colname);
					break;
				case 'T':
					if(item->type  == cJSON_String )
						strcpy((char *)stdb + TBL_DB_DEF_COLMAP[i].offset, item->valuestring );
					else 
						HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "%s  datatype not same", TBL_DB_DEF_COLMAP[i].f_colname);
					break;
				case 'I':
					if(item->type  == cJSON_Number )
						*((int *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)) = item->valueint;
					else if(item->type  == cJSON_True )
						*((int *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)) = 1;
					else if(item->type  == cJSON_False )
						*((int *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)) = 0;
					else if(item->type  == cJSON_String)
						*((long *)((char *)stdb + TBL_DB_DEF_COLMAP[i].offset)) = atoi(item->valuestring);
					else 
						HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "%s  datatype not same", TBL_DB_DEF_COLMAP[i].f_colname);
					break;
				case 'F':
					if(item->type  == cJSON_Number )
						*((float *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)) = (float)item->valuedouble;
					else 
						HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "%s  datatype not same", TBL_DB_DEF_COLMAP[i].f_colname);
					break;
				case 'L':
					if(item->type  == cJSON_Number )
						*((long *)((char *)stdb + TBL_DB_DEF_COLMAP[i].offset)) = (long )item->valuedouble;
					else if(item->type  == cJSON_String)
						*((long *)((char *)stdb + TBL_DB_DEF_COLMAP[i].offset)) = atol(item->valuestring);
					else 
						HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "%s  datatype not same", TBL_DB_DEF_COLMAP[i].f_colname);
					break;
				case 'D':
					if(item->type  == cJSON_Number )
						*((double *)((char *)stdb + TBL_DB_DEF_COLMAP[i].offset)) = item->valuedouble;
					else 
						HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "%s  datatype not same", TBL_DB_DEF_COLMAP[i].f_colname);
					break;
				default:
					HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"bad type");
			}
		}
		i++;
	}
}
void S2J(cJSON *root , void *stdb, TABFUN *TBL_DB_DEF_COLMAP)
{
	int i = 0;	
	//cJSON *item = NULL;
	while(TBL_DB_DEF_COLMAP[i].f_colname[0] != 0x00)
	{
		//HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "f_colname=[%s]", TBL_DB_DEF_COLMAP[i].f_colname);
		switch(TBL_DB_DEF_COLMAP[i].type)
		{
			case 'C':
				cJSON_AddStringToObject(root, TBL_DB_DEF_COLMAP[i].f_colname, (char *)stdb + TBL_DB_DEF_COLMAP[i].offset);
				break;
			case 'I':
				cJSON_AddNumberToObject(root, TBL_DB_DEF_COLMAP[i].f_colname, *((int *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)));
				break;
			case 'F':
				cJSON_AddNumberToObject(root, TBL_DB_DEF_COLMAP[i].f_colname, *((float *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)));
				break;
			case 'L':
				cJSON_AddNumberToObject(root, TBL_DB_DEF_COLMAP[i].f_colname, *((long *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)));
				break;
			case 'D':
				cJSON_AddNumberToObject(root, TBL_DB_DEF_COLMAP[i].f_colname, *((double *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)));
				break;
			default:
				HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"unknow type");
		}
		i++;
	}
}
void NS2J(cJSON *root , void *stdb, TABFUN *TBL_DB_DEF_COLMAP)
{
    int i = 0;
    while(TBL_DB_DEF_COLMAP[i].f_colname[0] != 0x00 )
    {
		if (TBL_DB_DEF_COLMAP[i].jres[0] == 0x00)
		{
			i++;
			continue;
		}
			
        switch(TBL_DB_DEF_COLMAP[i].type )
        {
            case 'C':
				if(TBL_DB_DEF_COLMAP[i].jrtype[0] == 'B')
					cJSON_AddBoolToObject(root, TBL_DB_DEF_COLMAP[i].jres, atoi((char *)stdb + TBL_DB_DEF_COLMAP[i].offset));
				else 
					cJSON_AddStringToObject(root, TBL_DB_DEF_COLMAP[i].jres, (char *)stdb + TBL_DB_DEF_COLMAP[i].offset);
                break;
            case 'T':
				cJSON_AddStringToObject(root, TBL_DB_DEF_COLMAP[i].jres, (char *)stdb + TBL_DB_DEF_COLMAP[i].offset);
                break;
            case 'I':
				if(TBL_DB_DEF_COLMAP[i].jrtype[0] == 'B')
					cJSON_AddBoolToObject(root, TBL_DB_DEF_COLMAP[i].jres, *((int *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)));
				else
					cJSON_AddNumberToObject(root, TBL_DB_DEF_COLMAP[i].jres, *((int *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)));
                break;
            case 'F':
                cJSON_AddNumberToObject(root, TBL_DB_DEF_COLMAP[i].jres, *((float *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)));
                break;
            case 'L':
                cJSON_AddNumberToObject(root, TBL_DB_DEF_COLMAP[i].jres, *((long *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)));
                break;
            case 'D':
                cJSON_AddNumberToObject(root, TBL_DB_DEF_COLMAP[i].jres, *((double *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)));
                break;
            default:
                HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"unknow type");
        }
        i++;
    }
}

void GetQueryWhere(char *IPC, char *where , TABFUN *TBL_DB_DEF_COLMAP)
{
	int i = 0;	
	int len = 0;
	char sTmpBuf[512];
	strcat(where , " where 1=1 ");
	while(TBL_DB_DEF_COLMAP[i].f_colname[0] != 0x00)
	{
		//HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "f_colname=[%s]", TBL_DB_DEF_COLMAP[i].f_colname);
		len = GetIpcValueExt(IPC, TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf, sizeof(sTmpBuf));
		if(len > 0 && strcmp(sTmpBuf, "undefined") != 0) 
		{
			switch(TBL_DB_DEF_COLMAP[i].type)
			{
				case 'C':
					sprintf(where+strlen(where), "and %s='%s' " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				case 'I':
					sprintf(where+strlen(where), "and %s=%s " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				case 'F':
					sprintf(where+strlen(where), "and %s=%s " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				case 'L':
					sprintf(where+strlen(where), "and %s=%s " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				case 'D':
					sprintf(where+strlen(where), "and %s=%s " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				default:
					HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"unknow type");
			}
		}
		i++;
	}
	HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"where =[%s]", where);
}
void NGetQueryWhere(char *IPC, char *where , TABFUN *TBL_DB_DEF_COLMAP)
{
	int i = 0;	
	int len = 0;
	char sTmpBuf[512];
	strcat(where , " where 1=1  ");
	while(TBL_DB_DEF_COLMAP[i].f_colname[0] != 0x00)
	{
		//HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "f_colname=[%s]", TBL_DB_DEF_COLMAP[i].f_colname);
		if(TBL_DB_DEF_COLMAP[i].jreq[0] != 0x00)
			len = GetIpcValueExt(IPC, TBL_DB_DEF_COLMAP[i].jreq, sTmpBuf, sizeof(sTmpBuf));
		else 
			len = GetIpcValueExt(IPC, TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf, sizeof(sTmpBuf));
		if(len > 0 && strcmp(sTmpBuf, "undefined") != 0) 
		{
			switch(TBL_DB_DEF_COLMAP[i].type)
			{
				case 'C':
					sprintf(where+strlen(where), "and %s='%s' " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				case 'T':
					if(TBL_DB_DEF_COLMAP[i].jrtype[0] == 0x00 )
						sprintf(where+strlen(where), "and %s='%s' " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					else
					{
						char sTmpStr[128];
						len = GetIpcValueExt(IPC, TBL_DB_DEF_COLMAP[i].jrtype, sTmpStr, sizeof(sTmpStr));
						if(len > 0)
							sprintf(where+strlen(where), "and %s >= '%s' and %s <= '%s'" , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf, TBL_DB_DEF_COLMAP[i].f_colname, sTmpStr);
					}
					break;
				case 'I':
					sprintf(where+strlen(where), "and %s=%s " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				case 'F':
					sprintf(where+strlen(where), "and %s=%s " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				case 'L':
					sprintf(where+strlen(where), "and %s=%s " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				case 'D':
					sprintf(where+strlen(where), "and %s=%s " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				default:
					HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"unknow type");
			}
		}
		i++;
	}
	HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"where =[%s]", where);
}
#if 0
void GetWhere(char *IPC, char *where , TABFUN *TBL_DB_DEF_COLMAP)
{
	int i = 0;	
	int len = 0;
	char sTmpBuf[512];
	char sTmpStr[128];
	flag = 0;
	while(TBL_DB_DEF_COLMAP[i].f_colname[0] != 0x00)
	{
		//HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "f_colname=[%s]", TBL_DB_DEF_COLMAP[i].f_colname);
		if(TBL_DB_DEF_COLMAP[i].jreq[0] != 0x00)
			len = GetIpcValueExt(IPC, TBL_DB_DEF_COLMAP[i].jreq, sTmpBuf, sizeof(sTmpBuf));
		else 
			len = GetIpcValueExt(IPC, TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf, sizeof(sTmpBuf));
		if(len > 0 && strcmp(sTmpBuf, "undefined") != 0) 
		{
			switch(TBL_DB_DEF_COLMAP[i].type)
			{
				case 'C':
					if(flag == 0)
					{
						sprintf(where+strlen(where), "%s='%s' " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
						flag = 1;
					}
					else 
						sprintf(where+strlen(where), "and %s='%s' " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				case 'T':
					if(flag == 0)
					{
						flag = 1;
						if(TBL_DB_DEF_COLMAP[i].jrtype[0] == 0x00 )
							sprintf(where+strlen(where), "%s='%s' " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
						else
						{
							len = GetIpcValueExt(IPC, TBL_DB_DEF_COLMAP[i].jrtype, sTmpStr, sizeof(sTmpStr));
							if(len > 0)
								sprintf(where+strlen(where), "%s >= '%s' and %s <= '%s'" , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf, TBL_DB_DEF_COLMAP[i].f_colname, sTmpStr);
						}
					}
					else
					{
						if(TBL_DB_DEF_COLMAP[i].jrtype[0] == 0x00 )
							sprintf(where+strlen(where), "and %s='%s' " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
						else
						{
							len = GetIpcValueExt(IPC, TBL_DB_DEF_COLMAP[i].jrtype, sTmpStr, sizeof(sTmpStr));
							if(len > 0)
								sprintf(where+strlen(where), "and %s >= '%s' and %s <= '%s'" , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf, TBL_DB_DEF_COLMAP[i].f_colname, sTmpStr);
						}

					}
					break;
				case 'I':
					if(flag == 0)
					{
						flag = 1;
						sprintf(where+strlen(where), "%s=%s " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					}
					else 
						sprintf(where+strlen(where), "and %s=%s " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				case 'F':
					if(flag == 0)
					{
						flag = 1;
						sprintf(where+strlen(where), "%s=%s " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					}
					else 
						sprintf(where+strlen(where), "and %s=%s " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				case 'L':
					if(flag == 0)
					{
						flag = 1;
						sprintf(where+strlen(where), "%s=%s " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					}
					else 
						sprintf(where+strlen(where), "and %s=%s " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				case 'D':
					if(flag == 0)
					{
						flag = 1;
						sprintf(where+strlen(where), "and %s=%s " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					}
					else 
						sprintf(where+strlen(where), "and %s=%s " , TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				default:
					HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"unknow type");
			}
		}
		i++;
	}
	HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"where =[%s]", where);
}
#endif
void GetQueryWhereSchema(char *IPC, char *where , TABFUN *TBL_DB_DEF_COLMAP, char *schema)
{
	int i = 0;	
	int len = 0;
	char sTmpBuf[512];
	strcat(where , " where 1=1 ");
	while(TBL_DB_DEF_COLMAP[i].f_colname[0] != 0x00)
	{
		//HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "f_colname=[%s]", TBL_DB_DEF_COLMAP[i].f_colname);
		len = GetIpcValueExt(IPC, TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf, sizeof(sTmpBuf));
		if(len > 0 && strcmp(sTmpBuf, "undefined") != 0) 
		{
			switch(TBL_DB_DEF_COLMAP[i].type)
			{
				case 'C':
					sprintf(where+strlen(where), "and %s.%s='%s' " , schema, TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				case 'I':
					sprintf(where+strlen(where), "and %s.%s=%s " , schema, TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				case 'F':
					sprintf(where+strlen(where), "and %s.%s=%s " , schema, TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				case 'L':
					sprintf(where+strlen(where), "and %s.%s=%s " , schema, TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				case 'D':
					sprintf(where+strlen(where), "and %s.%s=%s " , schema, TBL_DB_DEF_COLMAP[i].f_colname, sTmpBuf);
					break;
				default:
					HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"unknow type");
			}
		}
		i++;
	}
	HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"where =[%s]", where);
}
void GetQuerySortLimit(char *IPC, char *where )
{
	int i = 0;	
	int len = 0;
	char sTmpBuf[128];
	char sTmpLen[32];
	len = GetIpcValueExt(IPC, "sort", sTmpBuf, sizeof(sTmpBuf));
	if(len > 0) 
	{
		sprintf(where+strlen(where), "order by %s " , sTmpBuf);
	}
	if ( GetIpcValueExt(IPC, "_page", sTmpBuf, sizeof(sTmpBuf)) > 0 && GetIpcValueExt(IPC, "_limit", sTmpLen, sizeof(sTmpLen)) > 0)
	{
		sprintf(where+strlen(where), "limit %d, %d" , ((atoi(sTmpBuf)-1) * atoi(sTmpLen)), atoi(sTmpLen));
	}
	HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"where order by limit = [%s]", where);
}
void NGetQuerySortLimit(char *IPC, char *where )
{       
    int i = 0;  
    int len = 0;
    char sTmpBuf[128];
    char sTmpLen[32];
    len = GetIpcValueExt(IPC, "sort", sTmpBuf, sizeof(sTmpBuf));
    if(len > 0)
    {       
        sprintf(where+strlen(where), " order by %s " , sTmpBuf);
    }       
    if ( GetIpcValueExt(IPC, "pageNum", sTmpBuf, sizeof(sTmpBuf)) > 0 && GetIpcValueExt(IPC, "pageSize", sTmpLen, sizeof(sTmpLen)) > 0)
    {       
        sprintf(where+strlen(where), " limit %d, %d" , ((atoi(sTmpBuf)-1) * atoi(sTmpLen)), atoi(sTmpLen));
    }           
    HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"where order by limit = [%s]", where);
}           
void PrintTbl(void *stdb, TABFUN *TBL_DB_DEF_COLMAP)
{
	HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "------------------begin print table");
	int i = 0;	
	while(TBL_DB_DEF_COLMAP[i].f_colname[0] != 0x00)
	{
		switch(TBL_DB_DEF_COLMAP[i].type)
		{
			case 'C':
HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "[%s]=[%s]", TBL_DB_DEF_COLMAP[i].f_colname, (char *)stdb + TBL_DB_DEF_COLMAP[i].offset);
				break;
			case 'T':
HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "[%s]=[%s]", TBL_DB_DEF_COLMAP[i].f_colname, (char *)stdb + TBL_DB_DEF_COLMAP[i].offset);
				break;
			case 'I':
HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "[%s]=[%d]", TBL_DB_DEF_COLMAP[i].f_colname, *((int *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)));
				break;
			case 'F':
HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "[%s]=[%f]", TBL_DB_DEF_COLMAP[i].f_colname, *((float *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)));
				break;
			case 'L':
HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "[%s]=[%ld]", TBL_DB_DEF_COLMAP[i].f_colname, *((long *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)));
				break;
			case 'D':
HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "[%s]=[%lf]", TBL_DB_DEF_COLMAP[i].f_colname, *((double *)((char *)stdb+ TBL_DB_DEF_COLMAP[i].offset)));
				break;
			default:
				HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"unknow type");
		}
		i++;
	}
	HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "---------------------end print table");
}

void GetUpdateSetSql(cJSON *root, char *set, TABFUN *TBL_DB_DEF_COLMAP)
{
    int 	i = 0;
	cJSON *item = NULL;
	while(TBL_DB_DEF_COLMAP[i].f_colname[0] != 0x00)
	{
		item = cJSON_GetObjectItemCaseSensitive(root, TBL_DB_DEF_COLMAP[i].f_colname);
		if(item != NULL && item->type != cJSON_NULL)
		{
			switch(TBL_DB_DEF_COLMAP[i].type)
			{
				case 'C':
					sprintf(set+strlen(set), "%s='%s'," , item->string , item->valuestring);
					break;
				case 'I':
					if (item->type == cJSON_String)
						sprintf(set+strlen(set), "%s=%d," , item->string , atoi(item->valuestring));
					else 
						sprintf(set+strlen(set), "%s=%d," , item->string , item->valueint);
					break;
				case 'F':
					sprintf(set+strlen(set), "%s=%f," , item->string , item->valuedouble);
					break;
				case 'L':
					if (item->type == cJSON_String)
						sprintf(set+strlen(set), "%s=%d," , item->string , atoi(item->valuestring));
					else
						sprintf(set+strlen(set), "%s=%ld," , item->string , item->valueint);
					break;
				case 'D':
					sprintf(set+strlen(set), "%s=%lf," , item->string , item->valuedouble);
					break;
				default:
					HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"unknow type");
			}
		}
		i++;
	}
	if(strlen(set) > 0)
		set[strlen(set)-1] = 0x00 ;
	return ;
}
void NGetUpdateSetSql(cJSON *root, char *set, TABFUN *TBL_DB_DEF_COLMAP)
{
    int 	i = 0;
	cJSON *item = NULL;
	while(TBL_DB_DEF_COLMAP[i].f_colname[0] != 0x00)
	{
		if(TBL_DB_DEF_COLMAP[i].jreq[0] == 0x00)
		{
			i++;
			continue;
		}
		item = cJSON_GetObjectItemCaseSensitive(root, TBL_DB_DEF_COLMAP[i].jreq);
		if(item != NULL && item->type != cJSON_NULL )
		{
			switch(TBL_DB_DEF_COLMAP[i].type)
			{
				case 'C':
					sprintf(set+strlen(set), "%s='%s'," , TBL_DB_DEF_COLMAP[i].f_colname, item->valuestring);
					break;
				case 'T':
					if(strlen(item->valuestring) == 0)
						sprintf(set+strlen(set), "%s = NULL ," , TBL_DB_DEF_COLMAP[i].f_colname);
					else 
						sprintf(set+strlen(set), "%s='%s'," , TBL_DB_DEF_COLMAP[i].f_colname, item->valuestring);
					break;
				case 'I':
					if (item->type == cJSON_String)
						sprintf(set+strlen(set), "%s=%d," , TBL_DB_DEF_COLMAP[i].f_colname, atoi(item->valuestring));
					else 
						sprintf(set+strlen(set), "%s=%d," , TBL_DB_DEF_COLMAP[i].f_colname, item->valueint);
					break;
				case 'F':
					sprintf(set+strlen(set), "%s=%f," , TBL_DB_DEF_COLMAP[i].f_colname, item->valuedouble);
					break;
				case 'L':
					if (item->type == cJSON_String)
						sprintf(set+strlen(set), "%s=%d," , TBL_DB_DEF_COLMAP[i].f_colname, atoi(item->valuestring));
					else
						sprintf(set+strlen(set), "%s=%ld," , TBL_DB_DEF_COLMAP[i].f_colname, item->valueint);
					break;
				case 'D':
					sprintf(set+strlen(set), "%s=%lf," , TBL_DB_DEF_COLMAP[i].f_colname, item->valuedouble);
					break;
				default:
					HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"unknow type");
			}
		}
		i++;
	}
	if(strlen(set) > 0)
		set[strlen(set)-1] = 0x00 ;
	return ;
}
int GetDeleteWhereSql(cJSON *root, char *where , TABFUN *TBL_DB_DEF_COLMAP)
{
	int 	i = 0;
	cJSON *item = NULL;
	while(TBL_DB_DEF_COLMAP[i].f_colname[0] != 0x00)
	{
		item = cJSON_GetObjectItemCaseSensitive(root, TBL_DB_DEF_COLMAP[i].f_colname);
		//if(item == NULL || item->type == cJSON_NULL)
		if(item == NULL)
			return -1;
		switch(TBL_DB_DEF_COLMAP[i].type)
		{
			case 'C':
				sprintf(where+strlen(where), "%s='%s' and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valuestring);
				break;
			case 'T':
				sprintf(where+strlen(where), "%s='%s' and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valuestring);
				break;
			case 'I':
				if (item->type == cJSON_String)
					sprintf(where+strlen(where), "%s=%d and " , item->string , atoi(item->valuestring));
				else
					sprintf(where+strlen(where), "%s=%d and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valueint);
				break;
			case 'F':
				sprintf(where+strlen(where), "%s=%f and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valuedouble);
				break;
			case 'L':
				if (item->type == cJSON_String)
					sprintf(where+strlen(where), "%s=%ld and " , item->string , atol(item->valuestring));
				else
					sprintf(where+strlen(where), "%s=%ld and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valueint);
				break;
			case 'D':
				sprintf(where+strlen(where), "%s=%lf and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valuedouble);
				break;
			default:
				HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"unknow type");
				return -1;
		}
		i++;
	}
	if(strlen(where) > 0)
		where[strlen(where)-4] = 0x00 ;
	return 0;
}
int NGetDeleteWhereSql(cJSON *root, char *where , TABFUN *TBL_DB_DEF_COLMAP)
{
	int 	i = 0;
	cJSON *item = NULL;
	while(TBL_DB_DEF_COLMAP[i].f_colname[0] != 0x00)
	{
		if(TBL_DB_DEF_COLMAP[i].jreq[0] == 0x00)
		{
			i++;
			continue;
		}
		item = cJSON_GetObjectItemCaseSensitive(root, TBL_DB_DEF_COLMAP[i].jreq);
		if(item == NULL || item->type == cJSON_NULL) // 删除要谨慎，少一个数据都不行
			return -1;
		switch(TBL_DB_DEF_COLMAP[i].type)
		{
			case 'C':
				sprintf(where+strlen(where), "%s='%s' and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valuestring);
				break;
			case 'T':
				sprintf(where+strlen(where), "%s='%s' and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valuestring);
				break;
			case 'I':
				if (item->type == cJSON_String)
					sprintf(where+strlen(where), "%s=%d and " , item->string , atoi(item->valuestring));
				else
					sprintf(where+strlen(where), "%s=%d and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valueint);
				break;
			case 'F':
				sprintf(where+strlen(where), "%s=%f and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valuedouble);
				break;
			case 'L':
				if (item->type == cJSON_String)
					sprintf(where+strlen(where), "%s=%ld and " , item->string , atol(item->valuestring));
				else
					sprintf(where+strlen(where), "%s=%ld and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valueint);
				break;
			case 'D':
				sprintf(where+strlen(where), "%s=%lf and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valuedouble);
				break;
			default:
				HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"unknow type");
				return -1;
		}
		i++;
	}
	if(strlen(where) > 0)
		where[strlen(where)-4] = 0x00 ;
	return 0;
}
int GetUpdateWhereSql(cJSON *root, char *where , TABFUN *TBL_DB_DEF_COLMAP)
{
	int 	i = 0;
	cJSON *item = NULL;
	while(TBL_DB_DEF_COLMAP[i].f_colname[0] != 0x00)
	{
		item = cJSON_GetObjectItemCaseSensitive(root, TBL_DB_DEF_COLMAP[i].f_colname);
		//if(item == NULL || item->type == cJSON_NULL)
		if(item == NULL)
			return -1;
		switch(TBL_DB_DEF_COLMAP[i].type)
		{
			case 'C':
				sprintf(where+strlen(where), "%s='%s' and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valuestring);
				break;
			case 'T':
				sprintf(where+strlen(where), "%s='%s' and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valuestring);
				break;
			case 'I':
				if (item->type == cJSON_String)
					sprintf(where+strlen(where), "%s=%d and " , item->string , atoi(item->valuestring));
				else
					sprintf(where+strlen(where), "%s=%d and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valueint);
				break;
			case 'F':
				sprintf(where+strlen(where), "%s=%f and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valuedouble);
				break;
			case 'L':
				if (item->type == cJSON_String)
					sprintf(where+strlen(where), "%s=%ld and " , item->string , atol(item->valuestring));
				else
					sprintf(where+strlen(where), "%s=%ld and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valueint);
				break;
			case 'D':
				sprintf(where+strlen(where), "%s=%lf and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valuedouble);
				break;
			default:
				HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"unknow type");
				return -1;
		}
		i++;
	}
	if(strlen(where) > 0)
		where[strlen(where)-4] = 0x00 ;
	return 0;
}
int NGetUpdateWhereSql(cJSON *root, char *where , TABFUN *TBL_DB_DEF_COLMAP)
{
	int 	i = 0;
	cJSON *item = NULL;
	while(TBL_DB_DEF_COLMAP[i].f_colname[0] != 0x00)
	{
		if(TBL_DB_DEF_COLMAP[i].jreq[0] == 0x00)
		{
			i++;
			continue;
		}
		item = cJSON_GetObjectItemCaseSensitive(root, TBL_DB_DEF_COLMAP[i].jreq);
		if(item == NULL || item->type == cJSON_NULL)
		{
			i++;
			continue;
		}
		switch(TBL_DB_DEF_COLMAP[i].type)
		{
			case 'C':
				sprintf(where+strlen(where), "%s='%s' and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valuestring);
				break;
			case 'T':
				sprintf(where+strlen(where), "%s='%s' and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valuestring);
				break;
			case 'I':
				if (item->type == cJSON_String)
					sprintf(where+strlen(where), "%s=%d and " , TBL_DB_DEF_COLMAP[i].f_colname , atoi(item->valuestring));
				else
					sprintf(where+strlen(where), "%s=%d and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valueint);
				break;
			case 'F':
				sprintf(where+strlen(where), "%s=%f and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valuedouble);
				break;
			case 'L':
				if (item->type == cJSON_String)
					sprintf(where+strlen(where), "%s=%ld and " , item->string , atol(item->valuestring));
				else
					sprintf(where+strlen(where), "%s=%ld and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valueint);
				break;
			case 'D':
				sprintf(where+strlen(where), "%s=%lf and " , TBL_DB_DEF_COLMAP[i].f_colname, item->valuedouble);
				break;
			default:
				HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"unknow type");
				return -1;
		}
		i++;
	}
	if(strlen(where) > 0)
		where[strlen(where)-4] = 0x00 ;
	return 0;
}

void base64_encode(const unsigned char *input, int length, char *output) 
{
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO *mem = BIO_new(BIO_s_mem());
    BIO_push(b64, mem);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64, input, length);
    BIO_flush(b64);

    char *output1;
    long out_len = BIO_get_mem_data(mem, &output1);
    char *result = strndup(output1, out_len);

    BIO_free_all(b64);
    strcpy(output, result);
}

int base64_decode(char *input, char *output, int *output_length)
{
    BIO *b64 = NULL;
    BIO *mem = NULL;
    int length = strlen(input);
    int decoded_length = 0;

    // 创建BIO链: 内存源 -> base64解码
    b64 = BIO_new(BIO_f_base64());
    mem = BIO_new_mem_buf(input, length);
    BIO_push(b64, mem);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // 不处理换行符

    // 读取解码后的数据
    decoded_length = BIO_read(b64, output, length);
    if (decoded_length < 0) {
        BIO_free_all(b64);
        return -1; // 解码失败
    }

    if (output_length) {
        *output_length = decoded_length;
    }

    BIO_free_all(b64);
    return 0; // 成功
}

int validate_jwt_token(char *IPC, int *user_id ) 
{
	char token[4096];
	char output[1024];
	int outputlen;
	GetIpcValueExt(IPC, "$JWT", token, sizeof(token));

	char *head= strtok(token, ".");
    //printf("head:\n%s\n", head);
    char *payload= strtok(NULL, ".");
    //printf("payload:\n%s\n", payload);
    char *digest1= strtok(NULL, ".");

    char signing_input[2048];
    snprintf(signing_input, sizeof(signing_input), "%s.%s", head, payload);
    unsigned char digest[SHA256_DIGEST_LENGTH];
    HMAC(EVP_sha256(), KEY, strlen(KEY), (unsigned char*)signing_input, strlen(signing_input), digest, NULL);

    char encoded_signature[256];
    base64_encode(digest, SHA256_DIGEST_LENGTH, encoded_signature);
    //printf("encoded_signature:\n%s\n", encoded_signature);
    
	if(strcmp(digest1, encoded_signature) != 0)
		return 401;	

    base64_decode(payload, output, &outputlen);
    //printf("output:\n%s\n", output);
    
    cJSON *root = cJSON_Parse(output);
	cJSON *item = cJSON_GetObjectItemCaseSensitive(root, "exp");
	int curr = time(NULL);
	if ( ( curr - item->valueint ) > 0)
	{
		cJSON_Delete(root);
		return 405;	
	}

	item = cJSON_GetObjectItemCaseSensitive(root, "userId");
	*user_id = item->valueint;

	item = cJSON_GetObjectItemCaseSensitive(root, "login_user_key");
    reply = redisCommand(c, "GET login_tokens:%s", item->valuestring);
    if (reply->type == REDIS_REPLY_STRING) {
        //printf("GET mykey: %s\n", reply->str);
		freeReplyObject(reply);
		reply = redisCommand(c, "EXPIRE login_tokens %d", 600);
    }
	else if (reply->type == REDIS_REPLY_NIL) {
		freeReplyObject(reply);
		return 405;	
    }
    freeReplyObject(reply);
    cJSON_Delete(root);
    //char *header_str = cJSON_Print(root);
    //printf("header_str :\n%s\n", header_str);
		
    //printf("           digest:%s\n", digest1);
    //printf("encoded_signature:%s\n", encoded_signature);
    return 0;
}
int validate_token(char *IPC, cJSON *out) 
{
	char token[4096];
	char output[1024];
	int outputlen;
	GetIpcValueExt(IPC, "$JWT", token, sizeof(token));

	char *head= strtok(token, ".");
    char *payload= strtok(NULL, ".");
    char *digest1= strtok(NULL, ".");

    char signing_input[2048];
    snprintf(signing_input, sizeof(signing_input), "%s.%s", head, payload);
    unsigned char digest[SHA256_DIGEST_LENGTH];
    HMAC(EVP_sha256(), KEY, strlen(KEY), (unsigned char*)signing_input, strlen(signing_input), digest, NULL);

    char encoded_signature[256];
    base64_encode(digest, SHA256_DIGEST_LENGTH, encoded_signature);
    //printf("encoded_signature:\n%s\n", encoded_signature);
    
	if(strcmp(digest1, encoded_signature) != 0)
		return 401;	

    base64_decode(payload, output, &outputlen);
    //printf("output:\n%s\n", output);
    
    cJSON *root = cJSON_Parse(output);
	out = root;
	cJSON *item = cJSON_GetObjectItemCaseSensitive(root, "exp");
	int curr = time(NULL);
	if ( ( curr - item->valueint ) > 0)
	{
		cJSON_Delete(root);
		return 405;	
	}

/*
	item = cJSON_GetObjectItemCaseSensitive(root, "userId");
	*user_id = item->valueint;
*/

	item = cJSON_GetObjectItemCaseSensitive(root, "login_user_key");
    reply = redisCommand(c, "GET login_tokens:%s", item->valuestring);
    if (reply->type == REDIS_REPLY_STRING) {
        //printf("GET mykey: %s\n", reply->str);
		freeReplyObject(reply);
		reply = redisCommand(c, "EXPIRE login_tokens %d", 600);
    }
	else if (reply->type == REDIS_REPLY_NIL) {
		freeReplyObject(reply);
		return 405;	
    }
    freeReplyObject(reply);
    //cJSON_Delete(root);
    return 0;
}
int logout_jwt_token(char *IPC, int *user_id ) 
{
	char token[4096];
	char output[1024];
	int outputlen;
	GetIpcValueExt(IPC, "$JWT", token, sizeof(token));

	char *head= strtok(token, ".");
    char *payload= strtok(NULL, ".");
    char *digest1= strtok(NULL, ".");
    char signing_input[2048];
    snprintf(signing_input, sizeof(signing_input), "%s.%s", head, payload);
    unsigned char digest[SHA256_DIGEST_LENGTH];
    HMAC(EVP_sha256(), KEY, strlen(KEY), (unsigned char*)signing_input, strlen(signing_input), digest, NULL);
    char encoded_signature[256];
    base64_encode(digest, SHA256_DIGEST_LENGTH, encoded_signature);
    
	if(strcmp(digest1, encoded_signature) != 0)
		return 401;	

    base64_decode(payload, output, &outputlen);
    
    cJSON *root = cJSON_Parse(output);
	cJSON *item = cJSON_GetObjectItemCaseSensitive(root, "exp");
	int curr = time(NULL);
	if ( ( curr - item->valueint ) > 0)
	{
		cJSON_Delete(root);
		return 405;	
	}

	item = cJSON_GetObjectItemCaseSensitive(root, "userId");
	*user_id = item->valueint;

	item = cJSON_GetObjectItemCaseSensitive(root, "login_user_key");
    reply = redisCommand(c, "DEL login_tokens:%s", item->valuestring);
/*
	if (reply->type == REDIS_REPLY_NIL) {
		freeReplyObject(reply);
		return 405;	
    }
*/
    freeReplyObject(reply);
    cJSON_Delete(root);
    return 0;
}

// 生成 JWT Token
//char* generate_jwt_token(const char* username, int user_id, char *json_string) 
int generate_jwt_token(char *IPC) 
{
    // 1. 创建 Header
    cJSON *header = cJSON_CreateObject();
    cJSON_AddStringToObject(header, "alg", "HS256");
    cJSON_AddStringToObject(header, "typ", "JWT");
    char *header_str = cJSON_PrintUnformatted(header);
    printf("header_str :\n%s\n", header_str);

	char uuid[64];
	memset(uuid, 0x00, sizeof(uuid));
	strcpy(uuid, get_uuid());
	SetIpcValue(IPC, "$UUID", uuid);

    // 2. 创建 Payload
    cJSON *payload = cJSON_CreateObject();
    cJSON_AddStringToObject(payload, "sub", GetIpcPtr(IPC, "userName"));
    cJSON_AddStringToObject(payload, "login_user_key", uuid);
	
	//long createtime =  time(NULL)*1000;
	long createtime ;
	createtime = get_current_time_millis();

	HtLog(gsLogFile,HT_LOG_MODE_ERROR,__FILE__,__LINE__,"createtime=%ld", createtime);
	
	SetIpcLongValue(IPC, "$CREATE_TIME", createtime);
	
	long expiretime =  createtime+36000*1000;
	SetIpcLongValue(IPC, "$EXPIRE_TIME", expiretime);
	
    cJSON_AddNumberToObject(payload, "userId", GetIpcLongValue(IPC, "userId"));
    cJSON_AddNumberToObject(payload, "iat", createtime);
    cJSON_AddNumberToObject(payload, "exp", expiretime); // 1小时过期
    //cJSON_AddNumberToObject(payload, "exp", (double)time(NULL) + 300); // 1小时过期
    char *payload_str = cJSON_PrintUnformatted(payload);
    printf("PayLoad :\n%s\n", payload_str);

	    // 3. Base64 编码 Header 和 Payload
    char encoded_header[256];
    char encoded_payload[1024];
    base64_encode((unsigned char*)header_str, strlen(header_str), encoded_header);
    base64_encode((unsigned char*)payload_str, strlen(payload_str), encoded_payload);

    // 4. 准备签名内容
    char signing_input[2048];
    snprintf(signing_input, sizeof(signing_input), "%s.%s", encoded_header, encoded_payload);

    // 5. 使用 HMAC-SHA256 生成签名
    unsigned char digest[SHA256_DIGEST_LENGTH];
    HMAC(EVP_sha256(), KEY, strlen(KEY), (unsigned char*)signing_input, strlen(signing_input), digest, NULL);

    char encoded_signature[256];
    base64_encode(digest, SHA256_DIGEST_LENGTH, encoded_signature);

    // 6. 组合成完整 Token
    char token[4096];
    snprintf(token, sizeof(token), "%s.%s.%s", encoded_header, encoded_payload, encoded_signature);
	SetIpcValue(IPC, "$JWT", token);


    // 7. 清理内存
    cJSON_Delete(header);
    cJSON_Delete(payload);
    free(header_str);
    free(payload_str);

    return 0;
}
/* Note: when passing a NULL valuestring, cJSON_SetString treats this as an error and return NULL */
char* cJSON_SetString(cJSON *object, const char *string)
{
    char *copy = NULL;
    size_t v1_len;
    size_t v2_len;
    /* if object's type is not cJSON_String or is cJSON_IsReference, it should not set valuestring */
    if ((object == NULL) || !(object->type & cJSON_String) || (object->type & cJSON_IsReference))
    {    
        return NULL;
    }    
    if (object->string == NULL || string == NULL)
    {    
        return NULL;
    }    
    v1_len = strlen(string);
    v2_len = strlen(object->string);
    if (v1_len <= v2_len)
    {    
        /* strcpy does not handle overlapping string: [X1, X2] [Y1, Y2] => X2 < Y1 or Y2 < X1 */
        if (!( string + v1_len < object->string || object->string + v2_len < string ))
        {    
            return NULL;
        }    
        strcpy(object->string, string);
        return object->string;
    }    
    copy = (char*) strdup((const unsigned char*)string);
    if (copy == NULL)
    {    
        return NULL;
    }    
    if (object->string != NULL)
	{
        cJSON_free(object->string);
    }
    object->string = copy;
    return copy;
}

char * get_last_component(char* path) 
{
    char* last_slash = strrchr(path, '/');
    if (last_slash == NULL) {
        return path;  // 没有斜杠，返回整个字符串
    }
    return last_slash + 1;  // 跳过斜杠
}

char * get_last(char* path, char delim) 
{
    char* last_slash = strrchr(path, delim);
    if (last_slash == NULL) {
        return path;  // 没有斜杠，返回整个字符串
    }
    return last_slash + 1;  // 跳过斜杠
}

// 生成BCrypt哈希
void hash_password(char* password, char *hash) {
    char salt[BCRYPT_HASHSIZE];
    //char hash[BCRYPT_HASHSIZE];

    // 生成随机盐值，cost factor设为12
    if (bcrypt_gensalt(12, salt) != 0) {
        fprintf(stderr, "生成盐值失败\n");
        return ;
    }
    // 使用盐值哈希密码
    if (bcrypt_hashpw(password, salt, hash) != 0) {
        fprintf(stderr, "密码哈希失败\n");
        return ;
    }
    // 返回动态分配的哈希字符串
    return ;
}


void get_keys_by_pattern( const char *pattern, int *count, char ***keys, int max_keys)
{
    redisReply *reply;
    int cursor = 0;
    *count = 0;
    *keys = NULL;

    do {
        reply = redisCommand(c, "SCAN %d MATCH %s COUNT 100", cursor, pattern );

        if (reply == NULL || reply->type != REDIS_REPLY_ARRAY) {
            printf("SCAN command failed\n");
            if (reply) freeReplyObject(reply);
            return;
        }

        cursor = atoi(reply->element[0]->str);
        int keys_in_batch = reply->element[1]->elements;

        *keys = realloc(*keys, (*count + keys_in_batch) * sizeof(char *));

        for (int i = 0; i < keys_in_batch && *count < max_keys; i++) {
            (*keys)[*count] = strdup(reply->element[1]->element[i]->str);
            (*count)++;
        }

        freeReplyObject(reply);

    } while (cursor != 0 && *count < max_keys);
}

char* get_key_value(const char *key)
{
    redisReply *reply = redisCommand(c, "GET %s", key);

    if (reply == NULL || reply->type != REDIS_REPLY_STRING) {
        if (reply) freeReplyObject(reply);
        return NULL;
    }

    char *value = strdup(reply->str);
    freeReplyObject(reply);
    return value;
}

