#include <string.h>
#include "Daemon.h"
#include "MyDbsDef.h"

char			gsLogFile[LOG_NAME_LEN_MAX];

T_PcbDef		gpcbaList[ DM_MAX_PROC_NUM ];
T_ProcEnvDef	genvProcEnv[ DM_MAX_PROC_NUM ];

long	glUsageKey;
char	hsSrvId[5];
char	hsSrvId2[5];
char	hsSrvNum[5];
char	hsParamUsage[2];
char	hsParamIndex[3];
char	hsParamData[251];

Tbl_srv_inf_Def	atpTblDmSvrInf;

int	nExit = 0 ; /* 1: Exit ; 0: Run */
int	nChange = 0 ; /* 1: Change ; 0: Normal */
int nCldDeath = 0;
static int nConnDone = 0 ; /* 1: done ; 0: undo */

void vStopAllProcess();
void vStopAllProcessSafely();
void vStopAllProcessIgnoreSIGCLD(); 
void vHandleChildDeath( int n );
void vRestartTheseDeaths();

#define SRV_PARAM_USAGE_NAME "0"
#define SRV_PARAM_USAGE_ENV  "1"
#define SRV_PARAM_USAGE_ARG "2"

static int DbsConnectLast()
{
    int i;
    int nReturnCode = -1;

    for( i=0; i<5; i++)
    {
        nReturnCode = DbsConnect();
        if (nReturnCode)
        {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__,"DbsConnect [%d] error, [%d].", i, nReturnCode);
            sleep(1);
        }
        else
        {
            break;
        }
    }/*end for loop*/
    
    if (5 == i)
    {
        nConnDone = 1;
        return -1;
    }/* end if reach define max connect*/
    nConnDone = 0;
    return 0;
}

static int DbLineCheck()
{
	int nReturnCode = -1;
        
    if ( 0 == nConnDone )
    {
        if ( ( nReturnCode = DbsConnectTest() ) != 0)
        {
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__,"DbsConnectTest  error, [%d].", nReturnCode);
            if( ( nReturnCode =  DbsConnectLast() ) != 0 )
                return -1;
        }
    }
    else
    {
        if( ( nReturnCode =  DbsConnect() ) != 0 )
            return -1;
    }

	return 0;
}
/* modify end at 2015-02-06 */

int DaemonInit ( )
{
	int	i;
	int	nReturnCode;
	
	sprintf (gsLogFile, "%s.log", DM_LOG);

	if (getenv(SRV_USAGE_KEY))
		glUsageKey = atoi (getenv(SRV_USAGE_KEY));
	else
	{
		HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__, __LINE__, "Get SRV_USAGE_KEY error.");
		return -1;
	}

	/* connect to database */
    nReturnCode = DbsConnect ();
	if (nReturnCode)
	{
		HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__, __LINE__, "DbsConnect error [%d].",nReturnCode);
		return (nReturnCode);
	}
	HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__, __LINE__, "DbsConnect success");

    nReturnCode = InitMsq ();
	if (nReturnCode)
	{
		HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__, __LINE__, "InitMsqerror=[%d]", nReturnCode);
		return (nReturnCode);
	}

	memset ((char *)&atpTblDmSvrInf, 0, sizeof(atpTblDmSvrInf));
	atpTblDmSvrInf.usage_key = glUsageKey;
    nReturnCode = DbsSRVINF( DBS_CURSOR, &atpTblDmSvrInf);
   
	return 0;
}
/********************************************************************
*********input 		: void*******************************************
*********output		: void*******************************************
*********function	: read from database to delete the message queue
*********************which is already exist but will be confict after
*********************system boot*************************************
********************************************************************/ 
int InitMsq()
{
    int		nMsqKey;
    int		nMsqId;
    int		nReturnCode;
	Tbl_msq_inf_Def	tMsqInf;
	/* delete aveast 2012-02-02
    EXEC SQL DECLARE msq_cur CURSOR FOR
             select  msq_key
             from    TBL_MSQ_INF
             order by msq_int_id;

    EXEC SQL OPEN msq_cur;
    */
    memset(&tMsqInf, 0x00, sizeof(tMsqInf));
    nReturnCode = DbsMSQINF(DBS_CURSOR, &tMsqInf);
    if (nReturnCode != 0)
	{
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__, __LINE__, "FIRST CUROR msq_sur error: %d", nReturnCode);
        return -1;
    }
    
    nReturnCode = DbsMSQINF(DBS_OPEN, &tMsqInf);
    if (nReturnCode != 0){
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__, __LINE__, "OPEN CUROR msq_sur error: %d", nReturnCode);
        return -1;
    }

    while (1)
    {
        nReturnCode = DbsMSQINF(DBS_FETCH, &tMsqInf);
        if (nReturnCode == DBS_NOTFOUND)
            break;
        else if(nReturnCode != 0 && nReturnCode != DBS_FETCHNULL)
		{
            HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__, __LINE__, "FETCH CUROR msq_cur error: %d", nReturnCode);
			DbsMSQINF(DBS_CLOSE, &tMsqInf);
            return nReturnCode;
        }
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__, __LINE__, "tMsqInf.msq_int_id: [%s]", tMsqInf.msq_int_id);
        
        nMsqKey = atoi(tMsqInf.msq_key);

		nMsqId = msgget(nMsqKey, 0600);
		if (nMsqId >= 0)
		    msgctl(nMsqId, IPC_RMID, NULL);
    }
	DbsMSQINF(DBS_CLOSE, &tMsqInf);
	return 0;
}
/********************************************************************
*********input      : srvid******************************************
*********output     : the number of process *************************
*********function   : get the number of process which is related to srvid
*********************which is already exist but will be confict after
*********************system boot*************************************
********************************************************************/
int lGetProcNum( char *sSrvId )
{
	char	sFuncName[] = "lGetProcNum";
	int		i;
	int		llResult;

	HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s begin.", sFuncName);

	llResult = 0;
	for( i = 0; i < DM_MAX_PROC_NUM; i++ )
	{
		if(( gpcbaList[ i ].pid != -1 ) && ( !memcmp (gpcbaList[ i ].sSrvId, sSrvId, SRV_ID_LEN)) )
			llResult++;
	}

	HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s end.", sFuncName);
	return llResult;
}/* end of lGetProcNum */
/********************************************************************
*********input      : void ******************************************
*********output     : void ******************************************
*********function   : when terminal process signal is triggerd it should 
*********************call vHandleChange to kill the process only if it
*********************find this process has other uselessful process not
*********************not terminal***********************************/
void vHandleChange( int n )
{
	char	sFuncName[] = "vHandleChange";
	HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s begin.", sFuncName);
	nChange = 1;
	HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s end.", sFuncName);
}

void vChange( )
{
	char	sFuncName[] = "vChange";
	int		llEnvNum;
	int		llSrvNum;
	int		i,j,k;
	int		llResult;
	pid_t	llPid;
	int		ln_proc_num;
    int     nReturnCode;

	HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s begin.", sFuncName);

	sighold( SIGUSR1 );

	if( nExit ) /* Daemon Has Recvice Termination Signal */
	{
		return ;
	}

	/* open cursor */
	nReturnCode = DbsSRVINF(DBS_OPEN, &atpTblDmSvrInf);
	if (nReturnCode)
	{
		HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Open cursor error, %d.", nReturnCode);
		return ;
	}
	
	llEnvNum = 0;
	for (;;)
	{
		/* get a srv inf */
		memset (hsSrvId, 0, sizeof (hsSrvId));
		memset (hsSrvNum, 0, sizeof (hsSrvNum));
		
		memset(&atpTblDmSvrInf, 0x00, sizeof(atpTblDmSvrInf));
		nReturnCode = DbsSRVINF(DBS_FETCH, &atpTblDmSvrInf);
		if (nReturnCode == DBS_NOTFOUND)
			break;
			
		if (nReturnCode)
		{
			HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Fetch error, %d.", nReturnCode);
			break;
		}
		memcpy(hsSrvId, atpTblDmSvrInf.srv_id, 4);
		llSrvNum = atoi (atpTblDmSvrInf.srv_num);
			
		memcpy (genvProcEnv[ llEnvNum ].sSrvId, hsSrvId, SRV_ID_LEN);
		llEnvNum++;
		ln_proc_num = lGetProcNum( hsSrvId );
		if( ln_proc_num == llSrvNum )
			continue;
		else if( ln_proc_num > llSrvNum )
		{
			i = ln_proc_num - llSrvNum;
			for( k = 0; k < DM_MAX_PROC_NUM; k++ )
			{
				if(( gpcbaList[ k ].pid != -1 ) && 
					( !memcmp (gpcbaList[ k ].sSrvId, hsSrvId, SRV_ID_LEN) ) )
				{
					llResult = kill( gpcbaList[ k ].pid, SIGTERM );
					if( llResult == -1 )
					{
						HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "kill server %s pid %d error, %d.", gpcbaList[ k ].sSrvId, gpcbaList[ k ].pid, errno);
					}
					gpcbaList[ k ].pid = -1;
					memset(gpcbaList[ k ].sSrvId, 0x00, sizeof( gpcbaList[ k ].sSrvId ) );
					i--;
					if( i <= 0 )
						break;
				}
			}
		}
		else
		{
			for( i = 0; i < llSrvNum - ln_proc_num; i++ )
			{
				llPid = lStartProcess( hsSrvId, ln_proc_num+i+1 );
				if( llPid != -1 )
				{
					llResult = lInsertPid( llPid, hsSrvId, ln_proc_num+i+1 );
					if( llResult )
					{
						HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "lInsertPid error, server id %s.", hsSrvId);
					}
				}
				else
				{
					HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "lStartProcess error, server id %s.", hsSrvId);
				}
			}
		}
	}

	DbsSRVINF(DBS_CLOSE, &atpTblDmSvrInf);

	sigrelse( SIGUSR1 );
	/*sigset( SIGUSR1, vHandleChange );*/
	
	HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s end.", sFuncName);
}/* end of vHandleChange */
/************************************************************************
*********input      : char array*****************************************
*********output     : delete the "" from the right end of char***********
************************************************************************/
int lRtrim( char *vspStr )
{
	char	sFuncName[] = "lRtrim";
	int     i, j;
	
	
	i = strlen( vspStr ) - 1;
	while( vspStr[ i ] == ' ' && i >= 0 )
		i--;
	vspStr[ i + 1 ] = '\0';

	return 0;
	
}/* end lRtrim */

/************************************************************************
*************************************************************************
*********input		:process id,server id,server sequence id*************
*********output		:wheather the Insert operation is successful*********
*********function	:select a process which is not initial to fill it with
*********************transmitted parameter*******************************/
int lInsertPid( pid_t vlPid, char *sSrvId, int nSrvSeqId)
{
	char	sFuncName[] = "lInsertPid";
	int	i;
	int	k;

	HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s begin.", sFuncName);

	k = -1;
	for( i = 0; i < DM_MAX_PROC_NUM; i++ )
	{
		if( gpcbaList[ i ].pid == vlPid )
		{	
			HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "insert pid fail, duplicate pid.");
			return -1;
		}
		if( gpcbaList[ i ].pid == -1 )
			k = i;
	}/* end for */
	if( k != -1 )
	{
		gpcbaList[ k ].pid = vlPid;
		memcpy (gpcbaList[ k ].sSrvId, sSrvId, SRV_ID_LEN);
		gpcbaList[ k ].nSrvSeqId = nSrvSeqId;
		HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s end.", sFuncName);
		return 0;
	}
	else
	{
		HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "insert pid fail, pcb is full.");
		return -1;
	}
	
}/* end of lInsertPid */

/************************************************************************
*************************************************************************
*********input      :void************************************************
*********output     :void************************************************
*********function   :stop all the process in the gpcbaList***************
*************************************************************************/
void vStopAllProcessSafely()
{
	vStopAllProcess();
	sigset( SIGCLD, vHandleChildDeath ); /*Still Proc SigCLD */
}

void vStopAllProcessIgnoreSIGCLD() /* Used It Before Exit */
{
	vStopAllProcess();
}

void vStopAllProcess()
{
	char	sFuncName[] = "vStopAllProcess";
	int		i;
	int		llResult;

	HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s begin.", sFuncName);

	sigset (SIGCLD, SIG_IGN);
	for( i = 0; i < DM_MAX_PROC_NUM; i++ )
	{
		if( gpcbaList[ i ].pid != -1 )
		{
			llResult = kill( gpcbaList[ i ].pid, SIGTERM );
			if( llResult == -1 )
			{
				HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "stop %s error, %d.", gpcbaList[ i ].sSrvId, errno);
			}
			HtLog(	gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "stop %s, pid %d.", gpcbaList[ i ].sSrvId, gpcbaList[ i ].pid);
			gpcbaList[ i ].pid = -1;
		}
	}
	
	HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s end.", sFuncName);

}/* end of vStopAllProcess */

/************************************************************************
*************************************************************************
*********input      :void************************************************
*********output     :void************************************************
*********function   :call vStopAllProcess and disconnect with database***
*************************************************************************/
void vHandleExit( int n )
{
	char	sFuncName[] = "vHandleExit";
	
	HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s begin.", sFuncName);

	nExit = 1;

	vStopAllProcessIgnoreSIGCLD();
	
	DbsDisconnect ();

	HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s end.", sFuncName);

	exit( 1 );
}/* end of vHandleExit */

/************************************************************************
*************************************************************************
*********input      :void************************************************
*********output     :void************************************************
*********function   :check that the process which not dead***************
*************************************************************************/
void vHandleChildDeath( int n )
{
	char	sFuncName[] = "vHandleChildDeath";
	int	i,j;
	int	llPid;
    int nReturnCode;

	HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s begin.", sFuncName);

	while( waitpid( -1, &llPid, WNOHANG ) >0 );

	nCldDeath = 1;

	HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s end.", sFuncName);
}/* end of vHandleChildDeath */

void vRestartTheseDeaths()
{
	int i;
	int nHasDead;
	int nBeKilled;

	HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s begin.", __FUNCTION__);

	sighold( SIGCLD );
	for( i = 0; i < DM_MAX_PROC_NUM; i++ )
	{
		nHasDead = 	( gpcbaList[ i ].pid != -1 ) && ( kill( gpcbaList[ i ].pid, 0 ) == -1 ) ;
		nBeKilled = ( gpcbaList[ i ].pid == -1 ) && ( strlen( gpcbaList[ i ].sSrvId ) > 0 ) ; 

		if( nHasDead || nBeKilled )
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__,
                   "server %s pid %d stopped, restart it.", gpcbaList[ i ].sSrvId, gpcbaList[ i ].pid);

			gpcbaList[ i ].pid = lStartProcess( gpcbaList[ i ].sSrvId, gpcbaList[ i ].nSrvSeqId );
		}
	}
	sigrelse( SIGCLD );
	sigset( SIGCLD, vHandleChildDeath );

	HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s end.", __FUNCTION__);
}

/************************************************************************
*************************************************************************
*********input      :server id*******************************************
*********output     :the index in genvProcenv which could be translate to server id
*********function   :return the index of server id in genvProcEnv********
*************************************************************************/

int lSearchEnvIndex( char *sSrvId )
{
	char	sFuncName[] = "lSearchEnvIndex";
	int		i;
	
	HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s begin.", sFuncName);
	
	for( i = 0; i < DM_MAX_PROC_NUM; i++ )
		if( !memcmp (genvProcEnv[ i ].sSrvId, sSrvId, SRV_ID_LEN) )
		{
			HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s end.", sFuncName);
			return i;
		}
		
	HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s end.", sFuncName);
	return -1;
}/* end of lSearchEnvIndex */

pid_t lStartProcess(  char *sSrvId, int nSrvSeqId )
{
	char	sFuncName[] = "lStartProcess";
	char	sArgvSrvId[SRV_ID_LEN+1];
	char	sArgvSrvSeqId[5];
	int		llResult;
	int		i, j, k;
	pid_t	llPid;
	char	lsaExeName[ DM_MAX_EXE_NAME_LEN ];
	char	*lsapExecArgv[ DM_MAX_ARGC+1 ];
	char	lsaArgv[ DM_MAX_ARGC ][ DM_MAX_ARGV_LEN ];
	int		nReturnCode;
	
	Tbl_srv_param_Def	ptSrvParam, ptSrvParam1;
	
	HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s begin.", sFuncName);
	
	memset (hsSrvId2, 0, sizeof (hsSrvId2));
	memcpy (hsSrvId2, sSrvId, SRV_ID_LEN);
	
	/* initialize */
	memset( lsaArgv, 0, sizeof( lsaArgv ) );
	for( i = 0; i < DM_MAX_ARGC+1; i++ )
		lsapExecArgv[ i ] = 0;

	/**********************
	 * 获取服务名称 
	 *********************/
	strcpy (hsParamUsage, SRV_PARAM_USAGE_NAME);
	memset (hsParamData, 0, sizeof (hsParamData));

	memset(&ptSrvParam, 0x00, sizeof(Tbl_srv_param_Def));
	ptSrvParam.usage_key = glUsageKey;
	memcpy(ptSrvParam.srv_id, hsSrvId2, 4);
	ptSrvParam.param_usage[0] = hsParamUsage[0];
	
	nReturnCode = DbsSrvParam(DBS_SELECT, &ptSrvParam);
	if (nReturnCode)
	{
		HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Select exe file name error, %d.", nReturnCode);
		return -1;
	}
	HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Select exe file name [%s]", ptSrvParam.param_data);

	memcpy(hsParamData, ptSrvParam.param_data, sizeof(hsParamData));
	
	lRtrim( hsParamData );
	memcpy( lsaExeName, hsParamData, DM_MAX_EXE_NAME_LEN );
	lsapExecArgv[ 0 ] = lsaExeName;
	

	strcpy (hsParamUsage, SRV_PARAM_USAGE_ENV);

	memset(&ptSrvParam, 0x00, sizeof(Tbl_srv_param_Def));
	ptSrvParam.usage_key = glUsageKey;
	memcpy(ptSrvParam.srv_id, hsSrvId2, 4);
	ptSrvParam.param_usage[0] = hsParamUsage[0];
	nReturnCode = DbsSrvParam(DBS_CURSOR, &ptSrvParam);
	HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Open cursor [%d][%s][%s].", ptSrvParam.usage_key,ptSrvParam.srv_id,ptSrvParam.param_usage);

	nReturnCode = DbsSrvParam(DBS_OPEN, &ptSrvParam);
	if (nReturnCode)
	{
		HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Open cursor error, %d.", nReturnCode);
		return -1;
	}
	
	k = lSearchEnvIndex( sSrvId );
	i = 0;
	for (;;)
	{
		/* get a srv param */
		memset (hsParamData, 0, sizeof (hsParamData));

		memset(&ptSrvParam, 0x00, sizeof(Tbl_srv_param_Def));
		nReturnCode = DbsSrvParam(DBS_FETCH, &ptSrvParam);
		if (nReturnCode == DBS_NOTFOUND)
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__, __LINE__, "Not found %d", nReturnCode);
			break;
		}
			
		if (nReturnCode)
		{
			HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Fetch error, %d.", nReturnCode);
			DbsSrvParam(DBS_CLOSE, &ptSrvParam);
			return -1;
		}
		memcpy(hsParamIndex, ptSrvParam.param_index, 2);
		memcpy(hsParamData, ptSrvParam.param_data, 64);

		/* set env param */
		lRtrim( hsParamData );
		sprintf( genvProcEnv[ k ].saEnv[ i ], "%s", hsParamData );
		llResult = putenv( genvProcEnv[ k ].saEnv[ i ] );
		if( llResult == -1 )
		{
			HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "putenv %s error.", genvProcEnv[ k ].saEnv[ i ]);
			DbsSrvParam(DBS_CLOSE, &ptSrvParam);
			return -1 ;
		}
		HtLog(gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "tbl_srv_param putenv %s.", genvProcEnv[ k ].saEnv[ i ]);
		i++;
		/* check number of env var */
		if (i == DM_MAX_PROC_ENV_NUM)
		{
			HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Reached max env number, will discard extra env variables.");
			break;
		}
	}
	nReturnCode = DbsSrvParam(DBS_CLOSE, &ptSrvParam);

	/*********************
	 * 获取二维参数
	 * aveast 2012-02-25
	 *********************/
	strcpy (hsParamUsage, SRV_PARAM_USAGE_ARG);
	memset(&ptSrvParam, 0x00, sizeof(Tbl_srv_param_Def));
	ptSrvParam.usage_key = glUsageKey;
	memcpy(ptSrvParam.srv_id, hsSrvId2, 4);
	ptSrvParam.param_usage[0] = hsParamUsage[0];
	nReturnCode = DbsSrvParam(DBS_CURSOR, &ptSrvParam);
	HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Open cursor [%d][%s][%s].", ptSrvParam.usage_key,ptSrvParam.srv_id,ptSrvParam.param_usage);

	nReturnCode = DbsSrvParam(DBS_OPEN, &ptSrvParam);
	if (nReturnCode)
	{
		HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Open cursor error, %d.", nReturnCode);
		return -1;
	}
	
	/* every process has two argv, srv id, srv seq id */
	i = 1;
	memset (sArgvSrvId, 0, sizeof (sArgvSrvId));
	memcpy (sArgvSrvId, sSrvId, SRV_ID_LEN);
	lsapExecArgv[ i ] = sArgvSrvId;
	i++;
	memset (sArgvSrvSeqId, 0, sizeof (sArgvSrvSeqId));
	sprintf (sArgvSrvSeqId, "%d", nSrvSeqId);
	lsapExecArgv[ i ] = sArgvSrvSeqId;
	i++;
	
	for (;;)
	{
		/* get a srv param */
		memset (hsParamData, 0, sizeof (hsParamData));
		/* delete aveast 2012-02-02 
		EXEC SQL fetch dm_srv_param_cur
			into :hsParamIndex, :hsParamData;
		*/
		memset(&ptSrvParam, 0x00, sizeof(Tbl_srv_param_Def));
		
		nReturnCode = DbsSrvParam(DBS_FETCH, &ptSrvParam);
		if (nReturnCode == DBS_NOTFOUND)
		{
			HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Not Found %d.", nReturnCode);
			break;
		}
			
		if (nReturnCode)
		{
			HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Fetch error, %d.", nReturnCode);
			DbsSrvParam(DBS_CLOSE, &ptSrvParam);
			return -1;
		}

		memcpy(hsParamData, ptSrvParam.param_data, sizeof(hsParamData));

		lRtrim( hsParamData );
		/* ??? why strcat, is strcpy ok? */
		strcpy( lsaArgv[ i ], hsParamData );
		lsapExecArgv[ i ] = lsaArgv[ i ];
		i++;
		
		/* check number of argv */
		if (i == DM_MAX_ARGC)
		{
			HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Reached max argv number, will discard extra argv.");
			break;
		}
	}
	DbsSrvParam(DBS_CLOSE, &ptSrvParam);

/*********************************************get param end***********************************************************/
/* start server parent process log the result of process ,at the same time child process start the exe filename with*/
/**************************************argv got by "get param" motion above***************************************/
	llPid = fork();
	if( llPid == -1 )
	{
		HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "fork error, %d.", errno);
		return 0;
	}
	else if( llPid > 0 )
	{
		HtLog(	gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "start %s %s success, pid %d.", sSrvId, lsaExeName, llPid);
		HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s end.", sFuncName);
		return llPid;
	}
	else
	{
		if( execvp( lsaExeName, lsapExecArgv ) == -1 )
		{
			HtLog(	gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "start %s fail at execvp, %d.", lsaExeName, errno);
		}
		/* execvp fail, child process exit */
		exit (1);
	}

	return -1;

}/* end of lStartProcess */

void vStartAllProcess()
{
	char	sFuncName[] = "vStartAllProcess";
	int		llEnvNum;
	int		i;
	int		llResult;
	int		llSrvNum;
	int		llProcNum;
	pid_t	llPid;

	HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s begin.", sFuncName);

	/* open cursor */

	HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "usage_key=[%d]", atpTblDmSvrInf.usage_key );
	llResult = DbsSRVINF(DBS_OPEN, &atpTblDmSvrInf);
	if (llResult)
	{
		HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Open cursor error, %d.", llResult);
		return ;
	}
	
	llEnvNum = 0;
	llProcNum = 0;
	for (;;)
	{
		memset (hsSrvId, 0, sizeof (hsSrvId));
		memset (hsSrvNum, 0, sizeof (hsSrvNum));
		
		memset(&atpTblDmSvrInf, 0x00, sizeof(Tbl_srv_inf_Def));
		llResult = DbsSRVINF(DBS_FETCH, &atpTblDmSvrInf);
		if (llResult == DBS_NOTFOUND)
			break;
			
		if (llResult)
		{
			HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Fetch error, %d.", llResult);
			DbsSRVINF(DBS_CLOSE, &atpTblDmSvrInf);
			return ;
		}
		HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "tbl_srv_inf.srv_id=[%s]", atpTblDmSvrInf.srv_id);
		HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "tbl_srv_inf.srv_name=[%s]", atpTblDmSvrInf.srv_name );
		HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "tbl_srv_inf.srv_num=[%s]", atpTblDmSvrInf.srv_num);
		memcpy(hsSrvId, atpTblDmSvrInf.srv_id, 4);
		memcpy(hsSrvNum, atpTblDmSvrInf.srv_num, 2);
		llSrvNum = atoi (hsSrvNum);
			
		memcpy (genvProcEnv[ llEnvNum ].sSrvId, hsSrvId, SRV_ID_LEN);
		HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "srv_id=%s", hsSrvId);
		llEnvNum++;
		for( i = 0; i < llSrvNum; i++ )
		{
			llPid = lStartProcess( hsSrvId, i+1 );
			if( llPid > 0 )
			{
				llResult = lInsertPid( llPid, hsSrvId, i+1 );
				if( llResult )
				{
					HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "lInsertPid error, server id %s.", hsSrvId);
				}
				else
				{
					llProcNum++;
					if (llProcNum == DM_MAX_PROC_NUM)
					{
						HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Reached max process number, will not start any process.");
						break;
					}
				}
			}
			else
			{
				HtLog(	gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "lStartProcess error, server id %s.", hsSrvId);
			}
		}
	}

	DbsSRVINF(DBS_CLOSE, &atpTblDmSvrInf);

	HtLog(	gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s end.", sFuncName);

}/* end of vStartAllProcess */

void vHandleSignal( int n )
{
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "Receive Signal[%d]", n);	  
}

int main(short	argc, char **argv)
{
	int		i;
	int		nReturnCode;
	int		nWaitTime = 5;
	int		nTmp;

	if(argc >= 2) 
	{
		nTmp = atoi(argv[1]); /* argv[1]: the wait time */
		nWaitTime = (nTmp >= 1 && nTmp <= 600) ? nTmp : 5 ; /* Wait Time in [1, 600]s ,default 5s */
	}

	nReturnCode = DaemonInit ();
	if (nReturnCode)
	{
		printf("Daemon: DaemonInit error %d\n", nReturnCode);
		return -1;
	}
		
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "Daemon started.");	  

	signal( SIGHUP, SIG_IGN );

	/* initialize pcb and proc env block */
	for( i = 0; i < DM_MAX_PROC_NUM; i++ )
	{
		gpcbaList[ i ].pid = -1;
		memset (gpcbaList[ i].sSrvId, 0, sizeof (gpcbaList[ i].sSrvId));
		memset (genvProcEnv[ i].sSrvId, 0, sizeof (genvProcEnv[ i].sSrvId));
	}

	/* set signal handler */
	sighold( SIGCLD );
	sigset( SIGCLD, vHandleChildDeath );
	sighold( SIGUSR1 );
	sigset( SIGUSR1, vHandleChange );
	sighold( SIGTERM );
	sigset( SIGTERM, vHandleExit );
	
	/* start all processes */
	sighold(SIGCLD);
	vStartAllProcess();
	sigrelse(SIGCLD);

#if 0
#define SIGHUP       1
#define SIGINT       2
#define SIGQUIT      3
#define SIGILL       4
#define SIGTRAP      5
#define SIGABRT      6
#define SIGIOT       6
#define SIGBUS       7
#define SIGFPE       8
#define SIGKILL      9
#define SIGUSR1     10
#define SIGSEGV     11
#define SIGUSR2     12
#define SIGPIPE     13
#define SIGALRM     14
#define SIGTERM     15
#define SIGSTKFLT   16
#define SIGCHLD     17
#define SIGCONT     18
#define SIGSTOP     19
#define SIGTSTP     20
#define SIGTTIN     21
#define SIGTTOU     22
#define SIGURG      23
#define SIGXCPU     24
#define SIGXFSZ     25
#define SIGVTALRM   26
#define SIGPROF     27
#define SIGWINCH    28
#define SIGIO       29
#define SIGPOLL     SIGIO
#define SIGPWR      30
#define SIGSYS      31
//#define SIGUNUSED   31
#define SIGRTMIN    32

#define SIGSYSERROR 48  /* kernel error */
#define SIGRECOVERY 49  /* kernel recovery occurring */

#endif

sigset(SIGHUP     , vHandleSignal);
sigset(SIGINT     , vHandleSignal);
sigset(SIGQUIT    , vHandleSignal);
sigset(SIGILL     , vHandleSignal);
sigset(SIGTRAP    , vHandleSignal);
sigset(SIGABRT    , vHandleSignal);
sigset(SIGIOT     , vHandleSignal);
sigset(SIGBUS     , vHandleSignal);
sigset(SIGFPE     , vHandleSignal);
sigset(SIGKILL    , vHandleSignal);
sigset(SIGSEGV    , vHandleSignal);
sigset(SIGSYS     , vHandleSignal);
sigset(SIGPIPE    , vHandleSignal);
sigset(SIGALRM    , vHandleSignal);
/*
sigset(SIGTERM    , vHandleSignal);
*/
sigset(SIGURG     , vHandleSignal);
sigset(SIGSTOP    , vHandleSignal);
sigset(SIGTSTP    , vHandleSignal);
sigset(SIGCONT    , vHandleSignal);
/*
sigset(SIGCHLD    , vHandleSignal);
*/
sigset(SIGTTIN    , vHandleSignal);
sigset(SIGTTOU    , vHandleSignal);
//sigset(SIGUNUSED  , vHandleSignal);
sigset(SIGRTMIN   , vHandleSignal);
sigset(SIGIO      , vHandleSignal);
sigset(SIGPOLL    , vHandleSignal);
sigset(SIGXCPU    , vHandleSignal);
sigset(SIGXFSZ    , vHandleSignal);
sigset(SIGWINCH   , vHandleSignal);
sigset(SIGPWR     , vHandleSignal);
/*
sigset(SIGUSR1    , vHandleSignal);
*/
sigset(SIGUSR2    , vHandleSignal);
sigset(SIGPROF    , vHandleSignal);
sigset(SIGVTALRM  , vHandleSignal);

	/* forever wait. whenever a child process changes state, vHandleChildDeath will restart it */
	while( 1 )
	{
    	if (0 != DbLineCheck())
    	{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Db Line Down, StopAllProcess.");
        	vStopAllProcessSafely();

			sleep(60);
			continue;
    	}

		if( nExit ) /* 1: Exit, 0: Run */
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Recv SIGTERM, Will Exit.");
			sleep(60);
			continue;
		}

		if( nChange )
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Recv SIGUSR1, Will Change.");
			nChange = 0;
			vChange();	
		}

#if 1
		if( nCldDeath )
		{
			HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "Recv SIGCLD, Will Restart.");
			nCldDeath = 0;
			vRestartTheseDeaths();
		}
#endif 

#if 0
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "Loop For Restart These Dead Process.");
		vRestartTheseDeaths();
#endif	
		sleep(nWaitTime);
//#endif	
    }

}

