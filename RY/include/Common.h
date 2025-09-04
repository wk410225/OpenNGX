#ifndef __COMMON_H
#define __COMMON_H

typedef struct SYSHEAD {
  int shMsgLen;
  int shMsgType;
  int iSender;
  int shSourceID;
}SYSHEAD;

#define SYSHEADLEN sizeof(SYSHEAD) 

typedef struct SESSIONHEAD {
  unsigned char  caSessionMsgLen[2];
  unsigned char  cSessionRule;
  unsigned char  cSessionMsgResult;
  unsigned char  pzSessionMsgReserve[4];
  unsigned char  caSrvStan[12];
  unsigned char  tv_sec[12];
  unsigned char  tv_usec[8];
  unsigned char  reserved[8];
}SESSIONHEAD;

#define SESSIONHEADLEN sizeof(SESSIONHEAD) 
#define MSGHEADLEN  sizeof(SYSHEAD)+ sizeof(SESSIONHEAD)


#define MAX_RUNNING_PROCESSES     800
typedef struct _st_running_total {
	char    srv_id[4];
	int	    pid;
	short   on_off;
}st_running_total;
#define FILE_STAT_CTIME(path, time)  file_stat_ctime(path, time)

/*****************************************************************************/
/* FUNC:   void CommonGetCurrentDate (char *sCurrentDate);                   */
/* INPUT:  <none>                                                            */
/* OUTPUT: sCurrentDate   -- the string of current date                      */
/* RETURN: <none>                                                            */
/* DESC:   Get the system date with the format (YYYYMMDD).                   */
/*         NULL is added at the end.                                         */
/*****************************************************************************/
void  CommonGetCurrentDate(char *sCurrentDate);

/*****************************************************************************/
/* FUNC:   void CommonGetCurrentTime (char *sCurrentTime);                   */
/* INPUT:  <none>                                                            */
/* OUTPUT: sCurrentTime   -- the string of current time                      */
/* RETURN: <none>                                                            */
/* DESC:   Get the system time with the format (YYYYMMDDhhmmss).             */
/*         NULL is added at the end.                                         */
/*****************************************************************************/
void CommonGetCurrentTime(char *sCurrentTime);

/*int    GetInsKeyIndex ( InstInfo *ptInstInfo , char *sSrvId , char *sKeyIndex );*/
#endif

