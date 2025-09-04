#ifndef __COMMHTTP_H
#define __COMMHTTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <math.h>

//#include "SrvDef.h"
#include "DbsDef.h"
#include "DbsTbl.h"
#include "ErrCode.h"
#include "MsqOpr.h"
#include "HtLog.h"
#include "Common.h"

#define SRV_ID_LEN 4
extern char  gsSrvId[SRV_ID_LEN+1];
extern char  gsToSrvId[SRV_ID_LEN+1];

extern T_SrvMsq        gatSrvMsq[SRV_MSQ_NUM_MAX];

#endif
