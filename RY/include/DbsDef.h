#include <memory.h>
#ifndef _DBSDEF_H
#define _DBSDEF_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _DB_ORA
#define DBS_NOTFOUND            1403
#endif
#ifdef _DB_INFX
#define DBS_NOTFOUND            100
#endif
#ifdef _DB_DB2
#define DBS_NOTFOUND            100
#endif

#define DBS_INIT       0
#define DBS_SELECT     1
#define DBS_LOCK       2
#define DBS_UPDATE     3
#define DBS_DELETE     4
#define DBS_INSERT     5

#define DBS_CURSOR     11
#define DBS_OPEN       12
#define DBS_CLOSE      13
#define DBS_FETCH      14
#define	DBS_CURSOR1	   15
#define DBS_OPEN1      16
#define DBS_FETCH1     17
#define DBS_CLOSE1     18
#define DBS_INSERT1    19

#define	DBS_CURSOR2	  20
#define DBS_INSERT2   21
#define DBS_OPEN2     22
#define DBS_FETCH2    23
#define DBS_CLOSE2    24

#define	DBS_CURSOR3	  30
#define DBS_INSERT3   31
#define DBS_OPEN3     32
#define DBS_FETCH3    33
#define DBS_CLOSE3    34

#define DBS_SELECT1   40
#define DBS_LOCK1     41
#define DBS_UPDATE1   42
#define DBS_DELETE1	  43
#define DBS_SELECT2   44
#define DBS_LOCK2     45
#define DBS_UPDATE2   46
#define DBS_SELECT3   47
#define DBS_LOCK3     48
#define DBS_UPDATE3   49

#define DBS_UPDATE4   50
#define DBS_UPDATE5   51
#define DBS_SELECT4   52
#define DBS_SELECT5   53
#define DBS_UPDATE6   54
#define DBS_UPDATE7   55
#define DBS_UPDATE8   56

#define SELECT_STAT   57 
#define UPDATE_STAT   58
#define UPDATE_DATE   59
#define DBS_SELECT21  60
#define DBS_SELECT22  61
#define DBS_SELECT23  62
#define DBS_SELECT24  63

#define DBS_SELECT27  80
#define DBS_SELECT28  81
#define DBS_SELECT29  82
#define DBS_SELECT45  90

#define DBS_SELECT30  70
#define DBS_SELECT35  71
#define DBS_UPDATE11  72
#define DBS_UPDATE10  73

#define  DBS_SELECT_TXN_VALIDRET 110 
#define  DBS_SELECT_HIS_VALIDRET 111
#define DBS_SELECT43  149
#define DBS_SELECT44  150
#define DBS_SELECT25  151
#define DBS_SELECT26  152
#define DBS_UPDATE9   153
#define DBS_IND_NULL  154

#define DBS_SUCCESS   0
#define DBS_NULL      -1405
#define DBS_FETCHNULL -1405

#define DBS_COUNT    100 
#define DBS_FUNC_ERR -1

/* tbl_txn_inf.rsp_type value */
#define RSP_TYPE_NO_ACCOUNT         '1'
#define RSP_TYPE_RSP_BEFORE_ACCOUNT '2'
#define RSP_TYPE_RSP_AFTER_ACCOUNT  '3'

int DbsConnect ();
int DbsBegin ();
int DbsCommit ();
int DbsRollback ();
int DbsDisconnect ();
char* DbsError( );

#ifdef __cplusplus
}
#endif
#endif
