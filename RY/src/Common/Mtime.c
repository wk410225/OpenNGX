#include "Mtime.h"
#include <string.h>

/*******************************************************************
*  Function: void *GetGMTCurrentTime(char *sCurrentDate)              *
*  Desc    : Get the current time with the format (YYYYMMDDHHMMSS) *
*      Input : None                                                *
*      Output: sCurrentTime                                        *
*  Return  : the string of the current time                        *
*  Calling :                                                       *
*******************************************************************/ 
void GetGMTCurrentTime(char *sCurrentTime)
{  
   time_t current;
   struct tm *tmCurrentTime;
   
   time(&current);
   tmCurrentTime = gmtime(&current);
   sprintf(sCurrentTime, "%4d%02d%02d%02d%02d%02d", 
				 tmCurrentTime->tm_year + 1900, tmCurrentTime->tm_mon + 1, 
				 tmCurrentTime->tm_mday, tmCurrentTime->tm_hour,
           tmCurrentTime->tm_min, tmCurrentTime->tm_sec);

}

void GetGMTInTime(char *sCurrentTime)
{  
   time_t current;
   struct tm *tmCurrentTime;
   
   current = GetTimeInLong(sCurrentTime,0); 
   tmCurrentTime = gmtime(&current);
   sprintf(sCurrentTime, "%4d%02d%02d%02d%02d%02d", 
				 tmCurrentTime->tm_year + 1900, tmCurrentTime->tm_mon + 1, 
				 tmCurrentTime->tm_mday, tmCurrentTime->tm_hour,
           tmCurrentTime->tm_min, tmCurrentTime->tm_sec);

}

/* convert local time in YYYYMMDDhhmmss to 
	sun calendar YDDDhhmmss00 */
/* close begin
unsigned char *juliancal_to_suncal( char *insert_time )
{
	struct tm  *tmCurrent;
	time_t  lTime;
	char szDayOfYear[5];
	char szHourTmp[3];
static 	char szAuRRN[13];      *add20040701*

	memset( szDayOfYear, 0, 5);
	memset(szHourTmp, 0, 3);
	memset(szAuRRN, 0, 13);
	lTime = GetTimeInLong (insert_time,0);
	tmCurrent = gmtime(&lTime);
	sprintf( szDayOfYear, "%03d", tmCurrent->tm_yday + 1 );
	sprintf(szHourTmp, "%02d", tmCurrent->tm_hour);
	memcpy( szAuRRN, &insert_time[3], 1);
	memcpy( &szAuRRN[1], szDayOfYear, 3);
	memcpy( &szAuRRN[4], &szHourTmp[0], 2);
	memcpy( &szAuRRN[6], &insert_time[8], 6);
	memcpy( &szAuRRN[10], "00", 2);
	szAuRRN[12] = 0; *add20040701*
	printf( "szAuRRN = [%s]\n", szAuRRN );
	return (unsigned char *)szAuRRN;
}
close end  by Zhengzhou Kalian W 2004-08-02 */
/* add begin */
void juliancal_to_suncal( char *insert_time, char *buf )
{
	struct tm  *tmCurrent;
	time_t  lTime;
	char szDayOfYear[5];
	char szHourTmp[3];
 	char szAuRRN[13];

	memset( szDayOfYear, 0, 5);
	memset(szHourTmp, 0, 3);
	memset(szAuRRN, 0, 13);
	lTime = GetTimeInLong (insert_time,0);
	tmCurrent = gmtime(&lTime);
	sprintf( szDayOfYear, "%03d", tmCurrent->tm_yday + 1 );
	sprintf(szHourTmp, "%02d", tmCurrent->tm_hour);
	memcpy( szAuRRN, &insert_time[3], 1);
	memcpy( &szAuRRN[1], szDayOfYear, 3);
	memcpy( &szAuRRN[4], &szHourTmp[0], 2);
	memcpy( &szAuRRN[6], &insert_time[8], 6);
	memcpy( &szAuRRN[10], "00", 2);
	szAuRRN[12] = 0;
        memcpy( buf, szAuRRN, 6 ); buf[6]=0;
	return ;
}
/* add by  Zhengzhou Kalian W 2004-08-02 */

/* add Year info to a Date without it
   date form is MMDDhhmmss
   return form is YYYYMMDDhhmmss*/
int AddYearToDate(
                char *spInDate,
                char *spOutDate){

        int    liError;
    	long   nowtime;
        struct tm *tmCurrentTime;
        char   TmpDateMon[3];
        int    InDateMonth;
        int    DiffMon;
        
	time(&nowtime);
	tmCurrentTime = localtime(&nowtime);
	memcpy(
	     &TmpDateMon[0],
	     &spInDate[0],
	     2);
        TmpDateMon[2] = 0;
	InDateMonth = atoi(TmpDateMon);

	DiffMon = InDateMonth - tmCurrentTime->tm_mon;
        if(abs(DiffMon)<= 6){
            sprintf(
                spOutDate,
                "%04d",
                tmCurrentTime->tm_year + 1900);
        }
        else if (DiffMon > 6) {
            sprintf(
                spOutDate,
                "%04d",
                tmCurrentTime->tm_year - 1 + 1900);
        }
        else{
            sprintf(
                spOutDate,
                "%04d",
                tmCurrentTime->tm_year + 1 + 1900);
        }
        memcpy(
             spOutDate + 4,
             spInDate,
             10);
		return 0;
}/*end of AddYearToDate*/

int  ChgField7ToGmt(char *field7)
{
   char curtime[15];

   memset(curtime, 0,15);
   memcpy(curtime, field7, 10);
   AddYearToDate(field7, curtime);
   GetGMTInTime(curtime);
   memcpy(field7, &curtime[4], 10);
   return 0;
}

int  ChgGmtToField7(char *field7)
{
   time_t tfield7;
   struct tm *tmptm;
   char curtime[15];

   memset(curtime, 0, 15);
   AddYearToDate(field7, curtime);
   tfield7 = GetTimeInLong(curtime,1);
   tmptm = localtime(&tfield7); 
   sprintf(curtime, "%02d%02d%02d%02d%02d", 
				 tmptm->tm_mon + 1, 
				 tmptm->tm_mday, tmptm->tm_hour,
                 tmptm->tm_min, tmptm->tm_sec);
   memcpy(field7, curtime, 10);
   return 0;
}   

/*unsigned char *GetField37SunCal(char *field7) close  by Zhengzhou Kalian W 2004-08-02
{
   char curtime[15];

   memset(curtime, 0, 15);
   AddYearToDate(field7, curtime);
  * GetGMTInTime(curtime);*
   return (unsigned char *)(juliancal_to_suncal(curtime));
}*/
/* add begin */
void GetField37SunCal(char *field7, char * buf)
{
   char curtime[15];

   memset(curtime, 0, 15);
   AddYearToDate(field7, curtime);
  /* GetGMTInTime(curtime);*/
  juliancal_to_suncal(curtime,buf);
  return;
}
/* add by  Zhengzhou Kalian W 2004-08-02 */

int  *GetField37jul(char *field37)
{
   char curtime[15];
   struct tm tmptime;
   char tmpstr[5];

   memset(tmpstr, 0, 5);
   GetGMTCurrentTime(curtime);
   memcpy(tmpstr, curtime, 4);
   memcpy(&tmpstr[3], field37, 1);
   tmptime.tm_year = atoi(tmpstr) - 1900;

   memset(tmpstr, 0, 5);
   memcpy(tmpstr, field37 + 1, 3);
   tmptime.tm_yday = atoi(tmpstr) - 1;

   memset(tmpstr, 0, 5);
   memcpy(tmpstr, field37 + 4, 2);
   tmptime.tm_hour = atoi(tmpstr);
   tmptime.tm_min = 0;
   tmptime.tm_sec = 0;
   return 0;
}
void juliancal_to_suncal_local( char *insert_time, char *buf )
{
	struct tm  *tmCurrent;
	time_t  lTime;
	char szDayOfYear[5];
	char szHourTmp[3];
 	char szAuRRN[13];

	memset( szDayOfYear, 0, 5);
	memset(szHourTmp, 0, 3);
	memset(szAuRRN, 0, 13);
	lTime = GetTimeInLong (insert_time,0);
	tmCurrent = localtime(&lTime);
	sprintf( szDayOfYear, "%03d", tmCurrent->tm_yday + 1 );
	sprintf(szHourTmp, "%02d", tmCurrent->tm_hour);
	memcpy( szAuRRN, &insert_time[3], 1);
	memcpy( &szAuRRN[1], szDayOfYear, 3);
	memcpy( &szAuRRN[4], &szHourTmp[0], 2);
	memcpy( &szAuRRN[6], &insert_time[8], 6);
	memcpy( &szAuRRN[10], "00", 2);
	szAuRRN[12] = 0;
        memcpy( buf, szAuRRN, 6 ); buf[6]=0;
	return ;
}
