#ifndef __DAEMON_H
#define __DAEMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include "SrvParam.h"
#include "DbsDef.h"
#include "DbsTbl.h"
#include "HtLog.h"
#include "ErrCode.h"

#define DM_LOG				"Daemon"
#define	DM_MAX_EXE_NAME_LEN	251
#define	DM_MAX_ARGC			20
#define	DM_MAX_ARGV_LEN		250
#define DM_MAX_ENV_LEN		250
#define	DM_MAX_PROC_NUM		2000
#define	DM_MAX_PROC_ENV_NUM	50
#define SRV_ID_LEN 5
char   gsLogFile[LOG_NAME_LEN_MAX];
typedef struct
{
	pid_t	pid;
	char	sSrvId[SRV_ID_LEN];
	int		nSrvSeqId;
} T_PcbDef;

typedef struct
{
	char	sSrvId[SRV_ID_LEN];
	char	saEnv[ DM_MAX_PROC_ENV_NUM ][ DM_MAX_ENV_LEN ];
}T_ProcEnvDef;

pid_t lStartProcess(  char *sSrvId, int nSrvSeqId );

#endif
