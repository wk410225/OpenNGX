#ifndef _MYDBS_DEF_H
#define _MYDBS_DEF_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include  <mysql/mysql.h>
#include  "HtLog.h"

#define DBS_NOTFOUND        1403
#define DBS_FETCHNULL       -1405

extern char gsLogFile[LOG_NAME_LEN_MAX];
 
#define MAXCOLNUM  128 
#define MAXINDNUM  16
#define MAXSQLLEN  8192

typedef struct
{
    char    f_colname[64+1];
    int     offset;
    char    type; 
	char    jreq[64+1];
	char    jres[64+1];
	char    jrtype[64+1];
} TABFUN;
#endif 
