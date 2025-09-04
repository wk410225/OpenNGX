#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iconv.h>
#include <fcntl.h>
#include <math.h>
#include <sys/stat.h>
#include <netdb.h>
#include <ctype.h>
#include <uuid/uuid.h>

#include "Common.h"
#include "HtLog.h"
#include "DbsTbl.h"

#define C_NULL_CHG	':'

#define M_IS_LOWERHEX(x)                ((x >= 'a') && (x <= 'f'))
#define M_IS_UPPERHEX(x)                ((x >= 'A') && (x <= 'F'))
#define IS_NUMBER(x)                    (x >= '0' && x <= '9')
#define IS_ALPHA(x)                     ((x >= 'a' && x <= 'z') || \
										(x >= 'A' && x <= 'Z'))
#define IS_SPACE(x)                      (x == ' ')

int istruealnum(char c);

/*****************************************************************************/
/* FUNC:   void CommonGetCurrentDate (char *sCurrentDate);                   */
/* INPUT:  <none>                                                            */
/* OUTPUT: sCurrentDate   -- the string of current date                      */
/* RETURN: <none>                                                            */
/* DESC:   Get the system date with the format (YYYYMMDD).                   */
/*****************************************************************************/
void  CommonGetCurrentDate(char *sCurrentDate)
{  
   time_t current;
   struct tm *tmCurrentTime;
   
   time(&current);
   tmCurrentTime = localtime(&current);
   sprintf(sCurrentDate, "%4d%02d%02d", tmCurrentTime->tm_year + 1900, 
           tmCurrentTime->tm_mon + 1, tmCurrentTime->tm_mday);
}

/*****************************************************************************/
/* FUNC:   void CommonGetCurrentTime (char *sCurrentTime);                   */
/* INPUT:  <none>                                                            */
/* OUTPUT: sCurrentTime   -- the string of current time                      */
/* RETURN: <none>                                                            */
/* DESC:   Get the system time with the format (YYYYMMDDhhmmss).             */
/*****************************************************************************/
void CommonGetCurrentTime(char *sCurrentTime)
{  
   time_t current;
   struct tm *tmCurrentTime;
   
   tzset();
   time(&current);
   tmCurrentTime = localtime(&current);
   sprintf(sCurrentTime, "%4d%02d%02d%02d%02d%02d", 
           tmCurrentTime->tm_year + 1900, tmCurrentTime->tm_mon + 1, 
           tmCurrentTime->tm_mday, tmCurrentTime->tm_hour,
           tmCurrentTime->tm_min, tmCurrentTime->tm_sec);
}

void GetCurrTime(char *sCurrentTime)
{  
   time_t current;
   struct tm *tmCurrentTime;
   
   tzset();
   time(&current);
   tmCurrentTime = localtime(&current);
   sprintf(sCurrentTime, "%4d-%02d-%02d %02d:%02d:%02d", 
           tmCurrentTime->tm_year + 1900, tmCurrentTime->tm_mon + 1, 
           tmCurrentTime->tm_mday, tmCurrentTime->tm_hour,
           tmCurrentTime->tm_min, tmCurrentTime->tm_sec);
}

int CommonRTrim(char *caDest)
{
	int i;
	for( i=strlen(caDest)-1 ; i>=0 ; i-- )
	{
		if( caDest[i] !=' ')
		{
			break;
		}
	}
	caDest[i+1] = 0;
	return 0;
}

int CommonLTrim( char *caDest )
{
    int i;
    char lsTmp[1024];
    memset(lsTmp, 0, sizeof(lsTmp));

    for( i = 0; i< strlen(caDest) ; i++ )
    {
        if( caDest[i] ==' ')
        {
            continue;
        }
        lsTmp[i] = caDest[i];
    }
    strcpy(caDest, lsTmp);
	return 0;
}

int CommonAllTrim( char *caDest ){
	CommonLTrim(caDest);
	CommonRTrim(caDest);
	return 0;
} 


void Hex2Str( char *sSrc,  char *sDest, int nSrcLen )
{
	int  i;
	char szTmp[3];

	for( i = 0; i < nSrcLen; i++ )
	{
		sprintf( szTmp, "%02X", (unsigned char) sSrc[i] );
		memcpy( &sDest[i * 2], szTmp, 2 );
	}
	return ;
}

void Str2Hex( char *sSrc, char *sDest, int nSrcLen )
{
    int i, nHighBits, nLowBits;

    for( i = 0; i < nSrcLen; i += 2 )
    {
        nHighBits = sSrc[i];
        nLowBits  = sSrc[i + 1];

        if( nHighBits > 0x39 )
            nHighBits -= 0x37;
        else
            nHighBits -= 0x30;

        if( i == nSrcLen - 1 )
            nLowBits = 0;
        else if( nLowBits > 0x39 )
            nLowBits -= 0x37;
        else
            nLowBits -= 0x30;

        sDest[i / 2] = (nHighBits << 4) | (nLowBits & 0x0f);
    }
	return ;
}

int DecodeNull (char *sStr, int nLen)
{

	int				i;
	
	for (i = 0; i < nLen; i++)
		if (sStr[i] == C_NULL_CHG)
			sStr[i] = 0x00;
	return 0;
}

int EncodeNull (char *sStr, int nLen)
{

	int				i;
	
	for (i = 0; i < nLen; i++)
		if (sStr[i] == 0x00)
			sStr[i] = C_NULL_CHG;
	return 0;
}

/********************************************************************
  Description : convert time from string (YYYYMMDDhhmmss)
		to time_t

  Input   : sTime -- time, YYYYMMDDhhmmss
  Return  : time in time_t
********************************************************************/
time_t GetTimeInLong (char *sTime ,int GmtFlag)
{
    char str[20];
    struct tm tmTmp;

	memset (&tmTmp, '\0', sizeof (tmTmp));

	/* get year, year start from 1900 in struct tm */
	memcpy (str, sTime, 4);
	str[4] = '\0';
	tmTmp.tm_year = atoi (str) - 1900;
	/* get month, month start from 0 in struct tm */
	memcpy (str, sTime + 4, 2);
	str[2] ='\0';
	tmTmp.tm_mon = atoi (str) - 1;
	/* get day */
	memcpy (str, sTime + 6, 2);
	str[2] = '\0';
	tmTmp.tm_mday = atoi (str);
	/* get hour */
	memcpy (str, sTime + 8, 2);
	str[2] = '\0';
	tmTmp.tm_hour = atoi (str);
	/* get minute */
	memcpy (str, sTime + 10, 2);
	str[2] = '\0';
	tmTmp.tm_min = atoi (str);
	/* get second */
	memcpy (str, sTime + 12, 2);
	str[2] = '\0';
	tmTmp.tm_sec = atoi (str);
	if (GmtFlag)
	   return mktime(&tmTmp) + 8*60*60;
    else
	   return mktime(&tmTmp);
}

void CommonGetF376( char *insert_time, char *buf )
{
	struct tm  *tmCurrent;
	time_t  lTime;
	char sDayOfYear[4];
 	char sRRN[7];

	memset(sDayOfYear, 0, sizeof(sDayOfYear));
	memset(sRRN, 0, sizeof(sRRN));
	lTime = GetTimeInLong (insert_time,0);
	time(&lTime);
	tmCurrent = gmtime(&lTime);
	sprintf(sDayOfYear, "%03d", tmCurrent->tm_yday + 1 );
	memcpy( sRRN, &insert_time[3], 1);
	memcpy( &sRRN[1], sDayOfYear, 3);
	memcpy( &sRRN[4], &insert_time[8], 2);
	memcpy( buf, sRRN, 6 ); buf[6]=0;
	return ;
}

int nMNumIn(
         int*                         vlpNumVal,
         void*                          vvpNumStr,
         int                          vnNumStrL) {

    unsigned char*                              lspNumStr;

    lspNumStr = vvpNumStr;
    *vlpNumVal = 0;
    for(; vnNumStrL > 0; vnNumStrL--, lspNumStr++) {
        if((vnNumStrL > 8) &&
           (*lspNumStr != '0'))
            return -1;
        if(IS_NUMBER(*lspNumStr))
            *vlpNumVal = *vlpNumVal * 10 + *lspNumStr - '0';
        else
            return -2;
    } /* end of for */
    return 0;
} /* end of nMNumIn */

int nMNumOut(
         int                          vlNumVal,
         void*                          vvpNumStr,
         int                          vnNumStrL) {

    unsigned char*                              lspNumStr;

    lspNumStr = vvpNumStr;
    if(vnNumStrL < 1)
        return -1;
    lspNumStr += (vnNumStrL - 1);
    for(; vnNumStrL > 0; vnNumStrL--, lspNumStr--) {
        *lspNumStr = vlNumVal % 10 + '0';
        vlNumVal = vlNumVal / 10;
    } /* end of for */
    if(vlNumVal)
        return -2;
    return 0;
} /* end of nMNumOut */

int nMHexAsciiToBin(
         void*                          vvpOrigData,
         int                          vnOrigDataL,
         void*                          vvpResult) {

    unsigned char*                              lspOrigData;
    unsigned char*                              lspResult;

    lspOrigData = vvpOrigData;
    lspResult = vvpResult;

    for(; vnOrigDataL > 0; vnOrigDataL -= 2, lspResult++) {
        if(IS_NUMBER(*lspOrigData))
            *lspResult = ((*lspOrigData++) - '0') << 4;
        else if(M_IS_LOWERHEX(*lspOrigData))
            *lspResult = ((*lspOrigData++) - 'a' + 10) << 4;
        else if(M_IS_UPPERHEX(*lspOrigData))
            *lspResult = ((*lspOrigData++) - 'A' + 10) << 4;
        else
            return -1;
        if(IS_NUMBER(*lspOrigData))
            *lspResult += ((*lspOrigData++) - '0');
        else if(M_IS_LOWERHEX(*lspOrigData))
            *lspResult += ((*lspOrigData++) - 'a' + 10);
        else if(M_IS_UPPERHEX(*lspOrigData))
            *lspResult += ((*lspOrigData++) - 'A' + 10);
        else
            return -2;
    } /* end of for */

    return 0;
} /* end of nMHexAsciiToBin */

void vMBinToHexAscii(
        void*                           vvpOrigData,
        int                           vnOrigDataL,
        void*                           vvpResult) {

    unsigned char*                              lspOrigData;
    unsigned char*                              lspResult;

    lspOrigData = vvpOrigData;
    lspResult = vvpResult;

    for(; vnOrigDataL > 0; vnOrigDataL--, lspOrigData++) {
        if((*lspOrigData >> 4) > 9)
            *lspResult++ = (*lspOrigData >> 4) - 10 + 'A';
        else
            *lspResult++ = (*lspOrigData >> 4) + '0';
        if((*lspOrigData & 0x0f) > 9)
            *lspResult++ = (*lspOrigData & 0x0f) - 10 + 'A';
        else
            *lspResult++ = (*lspOrigData & 0x0f) + '0';
    } /* end of for */
} /* end of vMBinToHexAscii */

int nMCalcIn(
         int*                         vlpNumVal,
         void*                          vvpNumStr,
         int                          vnNumStrL) {

    unsigned char*                              lspNumStr;

    lspNumStr = vvpNumStr;
    *vlpNumVal = 0;
    for(; (vnNumStrL > 0) && (*lspNumStr == 0); vnNumStrL--, lspNumStr++);
    if(vnNumStrL > sizeof(*vlpNumVal))
        return -1;
    for(; vnNumStrL > 0; vnNumStrL--, lspNumStr++)
        *vlpNumVal = *vlpNumVal * 256 + *lspNumStr;
    return 0;
} /* end of nMCalcIn */

int nMCalcOut(
         unsigned short                          vlNumVal,
         void*                          vvpNumStr,
         int                          vnNumStrL) {

    unsigned char*                              lspNumStr;

    lspNumStr = vvpNumStr;
    memset(
       vvpNumStr,
       0,
       vnNumStrL);
    lspNumStr += (vnNumStrL - 1);
    for(; (vnNumStrL > 0) && (vlNumVal > 0); vnNumStrL--, lspNumStr--) {
        *lspNumStr = vlNumVal % 256;
        vlNumVal /= 256;
    } /* end of for */
    if(vlNumVal > 0)
        return -1;
    return 0;
} /* end of nMCalcOut */

void vBin2Asc( unsigned char *vspAscStr, unsigned char* vspBcdStr, int vnConLen )
{
	int	nCount;

	nCount = 0;

	for( ; nCount < vnConLen; nCount ++, vspAscStr ++ ) 
	{
		* vspAscStr = ( ( nCount & 0x01 ) ? ( * vspBcdStr ++ & 0x0f ) : ( * vspBcdStr >> 4 ) );
		* vspAscStr += ( ( * vspAscStr > 9 ) ? ( 'A' - 10 ) : '0' );
	}
	return;
}

void vAsc2Bin( unsigned char *vspBcdStr, unsigned char* vspAscStr, int vnConLen )
{
	int         lnCount;
	char        lsCh, lsCh1;

       	lsCh1 = 0x55;

    	for ( lnCount = 0; lnCount < vnConLen; vspAscStr ++, lnCount ++ ) 
	{

        	if ( * vspAscStr >= 'a' )
            		lsCh = * vspAscStr - 'a' + 10;
        	else if ( *vspAscStr >= 'A' )
            		lsCh = * vspAscStr - 'A' + 10;
        	else if ( * vspAscStr >= '0' )
            		lsCh = * vspAscStr - '0';
        	else
            		lsCh = 0;

        	if ( lsCh1 == 0x55 )
        	        lsCh1 = lsCh;
		else {
            		* vspBcdStr ++ = ( lsCh1 << 4 ) | lsCh;
            		lsCh1 = 0x55;
		}
	}

    	if ( lsCh1 != 0x55 )
        	* vspBcdStr = lsCh1 << 4;

    	return;
}

static unsigned char AsciiToEbcdicBuf[256] = 
{ 
         /* 0,8   1,9   2,A   3,B   4,C   5,D   6,E   7,F */

/* 0 */     0x00, 0x01, 0x02, 0x03, 0x37, 0x2D, 0x2E, 0x2F,
/* 0 */     0x16, 0x05, 0x25, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
/* 1 */     0x10, 0x11, 0x12, 0x13, 0x3C, 0x3D, 0x32, 0x26,
/* 1 */     0x18, 0x19, 0x3F, 0x27, 0x1C, 0x1D, 0x1E, 0x1F,
/* 2 */     0x40, 0x4F, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D,
/* 2 */     0x4D, 0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61,
/* 3 */     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
/* 3 */     0xF8, 0xF9, 0x7A, 0x5E, 0x4C, 0x7E, 0x6E, 0x6F,
/* 4 */     0x7C, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
/* 4 */     0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6,
/* 5 */     0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6,
/* 5 */     0xE7, 0xE8, 0xE9, 0x4A, 0xE0, 0x5A, 0x5F, 0x6D,
/* 6 */     0x79, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
/* 6 */     0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
/* 7 */     0x97, 0x98, 0x99, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
/* 7 */     0xA7, 0xA8, 0xA9, 0xC0, 0x6A, 0xD0, 0xA1, 0x07,
/* 8 */     0x20, 0x21, 0x22, 0x23, 0x24, 0x15, 0x06, 0x17,
/* 8 */     0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x09, 0x0A, 0x1B,
/* 9 */     0x30, 0x31, 0x1A, 0x33, 0x34, 0x35, 0x36, 0x08,
/* 9 */     0x38, 0x39, 0x3A, 0x3B, 0x04, 0x14, 0x3E, 0xE1,
/* A */     0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
/* A */     0x49, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
/* B */     0x58, 0x59, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
/* B */     0x68, 0x69, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75,
/* C */     0x76, 0x77, 0x78, 0x80, 0x8A, 0x8B, 0x8C, 0x8D,
/* C */     0x8E, 0x8F, 0x90, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E,
/* D */     0x9F, 0xA0, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
/* D */     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
/* E */     0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
/* E */     0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xDA, 0xDB,
/* F */     0xDC, 0xDD, 0xDE, 0xDF, 0xEA, 0xEB, 0xEC, 0xED,
/* F */     0xEE, 0xEF, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

static unsigned char EbcdicToAsciiBuf[256] =
{
         /* 0,8   1,9   2,A   3,B   4,C   5,D   6,E   7,F */

/* 0 */ 	0x00, 0x01, 0x02, 0x03, 0x9C, 0x09, 0x86, 0x7F,
/* 0 */     0x97, 0x8D, 0x8E, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
/* 1 */     0x10, 0x11, 0x12, 0x13, 0x9D, 0x85, 0x08, 0x87,
/* 1 */     0x18, 0x19, 0x92, 0x8F, 0x1C, 0x1D, 0x1E, 0x1F,
/* 2 */     0x80, 0x81, 0x82, 0x83, 0x84, 0x0A, 0x17, 0x1B,
/* 2 */     0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x05, 0x06, 0x07,
/* 3 */     0x90, 0x91, 0x16, 0x93, 0x94, 0x95, 0x96, 0x04,
/* 3 */     0x98, 0x99, 0x9A, 0x9B, 0x14, 0x15, 0x9E, 0x1A,
/* 4 */     0x20, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
/* 4 */     0xA7, 0xA8, 0x5B, 0x2E, 0x3C, 0x28, 0x2B, 0x21,
/* 5 */     0x26, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
/* 5 */     0xB0, 0xB1, 0x5D, 0x24, 0x2A, 0x29, 0x3B, 0x5E,
/* 6 */     0x2D, 0x2F, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
/* 6 */     0xB8, 0xB9, 0x7C, 0x2C, 0x25, 0x5F, 0x3E, 0x3F,
/* 7 */     0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1,
/* 7 */     0xC2, 0x60, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22,
/* 8 */     0xC3, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
/* 8 */     0x68, 0x69, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9,
/* 9 */     0xCA, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
/* 9 */     0x71, 0x72, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0,
/* A */     0xD1, 0x7E, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
/* A */     0x79, 0x7A, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
/* B */     0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
/* B */     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
/* C */     0x7B, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
/* C */     0x48, 0x49, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED,
/* D */     0x7D, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
/* D */     0x51, 0x52, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3,
/* E */     0x5C, 0x9F, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
/* E */     0x59, 0x5A, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
/* F */     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
/* F */     0x38, 0x39, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
	};

void vAsc2Ebcdic( char* sDest, char* sSrc, int nBytes )
{
	int i;

	for( i = 0; i < nBytes; i++ )
		sDest[i] = AsciiToEbcdicBuf[(unsigned char)sSrc[i]];

} /* end of vAsc2Ebcdic() */


void vEbcdic2Asc( char* sDest, char* sSrc, int nBytes )
{
	int i;

	for( i = 0; i < nBytes; i++ )
		sDest[i] = EbcdicToAsciiBuf[(unsigned char)sSrc[i]];

} /* end of vEbcdic2Asc() */


void vHex2Asc( char *sDest, char *sSource, int nBytes )
{
	char szTemp[3];
	int i;

	for ( i = 0; i < nBytes; i++ )
	{
		sprintf( szTemp, "%02X", (unsigned char) sSource[i] );
		memcpy( &sDest[i*2], szTemp, 2 );
	}

} /* End of vHex2Asc() */


int vAsc2Hex(char *sDest, char *sSource, int nBytes )
{
	int i;
	char cTemp;
	
	for ( i = 0; i < nBytes; i++ )
	{
		if( !isxdigit(sSource[i]) )
			return -1;
  
		if( sSource[i] < 'A' )
			cTemp = sSource[i] - '0';
		else if( sSource[i] < 'a' )
			cTemp = sSource[i] - 'A' + 10;
		else
			cTemp = sSource[i] - 'a' + 10;
		
		if( i % 2 )
			sDest[i/2] |= cTemp;
		else
			sDest[i/2] = cTemp << 4;
	}
  
	return 0;

} /* End of vAsc2Hex() */


int vBcd2Asc( char *sDest,  char *sSrc, int nLen, char cAlign )
{
	int  nSrcOffset, nDestOffset, nDigit;
  
	nSrcOffset = ( cAlign == 'R' ) && ( nLen % 2 );

	for( nDestOffset = 0; nDestOffset < nLen; nSrcOffset++, nDestOffset++ )
	{
		if( nSrcOffset % 2 )
			nDigit = sSrc[nSrcOffset/2] & 0x0f;
		else
			nDigit = ( sSrc[nSrcOffset/2] & 0xf0 ) >> 4;
	   
		if( nDigit > 9 || nDigit < 0 )
			return -1;

		sDest[nDestOffset] = nDigit + '0';
	}

	return 0;

} /* End of vBcd2Asc() */      


int vAsc2Bcd( char *sDest, char *sSrc, int nLen, char cAlign )
{
	int  nSrcOffset, nDestOffset, nDigit;
  
	nDestOffset = ( cAlign == 'R' ) && ( nLen % 2 );

	for( nSrcOffset = 0; nSrcOffset < nLen; nSrcOffset++, nDestOffset++ )
	{
		nDigit = sSrc[nSrcOffset] - '0';

		if( nDigit > 9 || nDigit < 0 )
			return -1;

		if( nDestOffset % 2 )
			sDest[nDestOffset/2] |= nDigit;
		else
			sDest[nDestOffset/2] = nDigit << 4;
	   
	}

	return (nLen + 1) / 2;
	   
} /* End of vAsc2Bcd() */      

void vBitXOR( char * InputString, char * Key, char * OutputString, short Length)
{
	short i, k = 0;

	for( i = 0; i < Length; i++)
	{
		OutputString[i] = InputString[i] ^ Key[k];
 		k = (k + 1) % 8;	
	}
}  


int istruealnum(char c)
{
	if ('a' <= c && c <= 'z') return 1;
	if ('A' <= c && c <= 'Z') return 1;
	if ('0' <= c && c <= '9') return 1;

	return 0;
}
int GenTlv(char *sData, int nDataLen, char *sTag, int *pnDataLen, char *sBuff)
{
    int  nLen = 0, pos = 0;
	char sTempBin[255 + 1];
    
	if (nDataLen > 65535 || nDataLen < 0)
		return -1;

	memset(sTempBin, 0, sizeof(sTempBin));
    nMHexAsciiToBin(sTag, strlen(sTag), sTempBin);
	memcpy(sBuff + pos, sTempBin, ((strlen(sTag)) >> 1));
	pos += ((strlen(sTag)) >> 1);
    
	if (nDataLen <= 127)
		nLen = 1;
    else if (nDataLen > 127 || nDataLen <= 255)
	    nLen = 2;
    else 
	    nLen = 3;
    
    switch (nLen) {
		case 1:
			sBuff[pos] = nDataLen;
			++pos;
			break;
        case 2:
			sBuff[pos] = 0x81;
			++pos;
			sBuff[pos] = nDataLen;
			++pos;
			break;
        case 3:
			sBuff[pos] = 0x82;
			++pos;
			memcpy(sBuff + pos, (char*)&nDataLen, 2);
			pos += 2;
			break;
    }
  
        if (nLen == 1 || nLen == 2 || nLen == 3) { 
	   memcpy(sBuff + pos, sData, nDataLen);
	   pos += nDataLen;
        } else {
	   nMHexAsciiToBin(sData, nDataLen, sTempBin);
	   memcpy(sBuff + pos, sTempBin, (nDataLen >> 1));
	   pos += (nDataLen >> 1);
        }

        *pnDataLen= pos;

	return 0;
}
void bcd_to_asc (ascii_buf , bcd_buf , conv_len , type)
unsigned char   *ascii_buf , *bcd_buf , type ;
int             conv_len ;
{
    int         cnt ;
    
    if (conv_len & 0x01 && type) {
        cnt = 1 ;
        conv_len ++ ;
    }
    else
        cnt = 0 ;
    for ( ; cnt < conv_len ; cnt ++ , ascii_buf ++ ) {
        *ascii_buf = ( ( cnt & 0x01 ) ? ( *bcd_buf ++ & 0x0f ) : ( *bcd_buf >> 4) ) ;
        *ascii_buf += ( ( *ascii_buf > 9 ) ? ( 'A' - 10 ) : '0' ) ;
    }

    return ;
}

void asc_to_bcd ( bcd_buf , ascii_buf , conv_len , type )
unsigned char   *bcd_buf , *ascii_buf ;
int             conv_len,  type  ;
{
	int         cnt ;
	char        ch , ch1 ;

	if ( conv_len & 0x01 && type )
		ch1 = 0 ;
	else
		ch1 = 0x55 ;

	for ( cnt = 0 ; cnt < conv_len ; ascii_buf ++ , cnt ++ ) {
		if ( *ascii_buf >= 'a' )
			ch = *ascii_buf - 'a' + 10 ;
		else if ( *ascii_buf >= 'A' )
			ch = *ascii_buf- 'A' + 10 ;
		else if ( *ascii_buf >= '0' )
			ch = *ascii_buf-'0' ;
		else
			ch = 0;
		if ( ch1 == 0x55 )
			ch1 = ch ;
		else {
			*bcd_buf ++ = (ch1 << 4) | ch ;
			ch1 = 0x55 ;
		}
	}
	if ( ch1 != 0x55 )
		*bcd_buf = ch1 << 4 ;

	return ;
}


long diffDays(char *date1, char *date2)
{
	struct tm tm1, tm2;
	time_t clock1, clock2;
	char buf[5];

	tm1.tm_sec = 0;
	tm1.tm_min = 0;
	tm1.tm_hour = 0;

	memcpy(buf, date1, 4);
	buf[4] = '\0';
	tm1.tm_year = atoi(buf) - 1900;

	memcpy(buf, date1 + 4, 2);
	buf[2] = '\0';
	tm1.tm_mon = atoi(buf) - 1;

	memcpy(buf, date1 + 6, 2);
	buf[2] = '\0';
	tm1.tm_mday = atoi(buf);

	clock1 = mktime(&tm1);

	tm2.tm_sec = 0;
	tm2.tm_min = 0;
	tm2.tm_hour = 0;

	memcpy(buf, date2, 4);
	buf[4] = '\0';
	tm2.tm_year = atoi(buf) - 1900;

	memcpy(buf, date2 + 4, 2);
	buf[2] = '\0';
	tm2.tm_mon = atoi(buf) - 1;

	memcpy(buf, date2 + 6, 2);
	buf[2] = '\0';
	tm2.tm_mday = atoi(buf);

	clock2 = mktime(&tm2);

	return (long)((clock2 - clock1) / 60L / 60L / 24L);
}

void CommonGetWeekDate(int *WeekDate)
{
   time_t current;
   struct tm *tmCurrentTime;
   
   time(&current);
   tmCurrentTime = localtime(&current);
   *WeekDate = tmCurrentTime->tm_wday; 
}

void CommonGetHour(int *iHour)
{
   time_t current;
   struct tm *tmCurrentTime;
   
   time(&current);
   tmCurrentTime = localtime(&current);
   *iHour = tmCurrentTime->tm_hour; 
}


int CommonIsCrtTimeBetweenTimes(char *sOpenTime,char *sCloseTime)
{
   time_t current;
   struct tm *tmCurrentTime;
   char tm[7];
   time(&current);
   tmCurrentTime = localtime(&current);
   memset(tm,0,7);
   sprintf(tm, "%02d%02d%02d", tmCurrentTime->tm_hour,tmCurrentTime->tm_min, tmCurrentTime->tm_sec);

   if(atoi(sOpenTime+8)==atoi(sCloseTime+8))
	   return 0;
   if(atoi(sOpenTime+8)>atoi(sCloseTime+8))
	   if(atoi(tm)<atoi(sOpenTime+8)&&atoi(tm)>atoi(sCloseTime+8))
		   return 1;
	   else
		   return 0;
   if(atoi(tm)<atoi(sOpenTime+8)||atoi(tm)>atoi(sCloseTime+8))
	   return 1;
   return 0;
}

int XorBuf(char *sMacBlock, int nMacBlockLen, char *outMacData)
{
	int     i,j, length;
	unsigned long outdata[8]={0,0,0,0,0,0,0,0};
	char    inverse_c[8];
	char    outdata1[8];

	unsigned short zero = 0x00;

	if (sMacBlock == NULL)
		return -1;

	length = nMacBlockLen;

	for (i=0; i<=7; i++)
		outdata1[i] = (char)outdata[i];

	length = length -1;
	for (i=0; i<=length; i=i+8) {
		inverse_c[0] = sMacBlock[i];
		inverse_c[1] = sMacBlock[i+1];
		inverse_c[2] = sMacBlock[i+2];
		inverse_c[3] = sMacBlock[i+3];
		inverse_c[4] = sMacBlock[i+4];
		inverse_c[5] = sMacBlock[i+5];
		inverse_c[6] = sMacBlock[i+6];
		inverse_c[7] = sMacBlock[i+7];

		if ((i+1) > length ) inverse_c[1] = (char)zero;
		if ((i+2) > length ) inverse_c[2] = (char)zero;
		if ((i+3) > length ) inverse_c[3] = (char)zero;
		if ((i+4) > length ) inverse_c[4] = (char)zero;
		if ((i+5) > length ) inverse_c[5] = (char)zero;
		if ((i+6) > length ) inverse_c[6] = (char)zero;
		if ((i+7) > length ) inverse_c[7] = (char)zero;

		for (j=0; j<8; j++)
			outdata1[j] = outdata1[j] ^ inverse_c[j];
	}

	memcpy(outMacData, outdata1, 8);

	return 0;
}
void SetIpcValueExt(char *buffer, char *name, char *value, int nValLen)
{
    if(nValLen <= 0)
    {
        return ;
    }

    char    sLen[50+1], sTmpBuf[128];
    int     nLen = 0, nLenTag = 0;  

    memset(sLen, 0x00, sizeof(sLen));
    memcpy(sLen, buffer, 6);
    nLen += 6;
    nLen += atoi(sLen);

    nLenTag = nValLen+6;
    if(nLenTag > 6959)
    {
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__, __LINE__, "SetIpcValue  name[%s], value[%s]", name, value);
        return ;
    }

    nLenTag = strlen(name);
    memset(sLen, 0x00, sizeof(sLen));
    sprintf(sLen, "%02d", nLenTag);
    memcpy(buffer+nLen, sLen, 2);
    nLen += 2;
    memcpy(buffer+nLen, name, nLenTag);
    nLen += nLenTag;

    memset(sLen, 0x00, sizeof(sLen));
    sprintf(sLen, "%04d", nValLen);
    nLenTag = atoi(sLen);
    memcpy(buffer+nLen, sLen, 4);
    nLen += 4;
    memcpy(buffer+nLen, value, nLenTag);
    nLen += nLenTag;

    memset(sLen, 0x00, sizeof(sLen));
    sprintf(sLen, "%06d", nLen-6);
    memcpy(buffer, sLen, 6);
	return ;
}

int SetIpcValue(char *buffer, char *name, char *value)
{
	if(value == NULL || strlen(value) == 0 )
        return -1;

    char    sLen[50+1], sTmpBuf[128];
    int     nLen = 0, nLenTag = 0;  

    memset(sLen, 0x00, sizeof(sLen));
    memcpy(sLen, buffer, 6);
    nLen += 6;
    nLen += atoi(sLen);

    nLenTag = strlen(value)+6;
    if(nLenTag > 6959)
    {
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__, __LINE__, "SetIpcValue  name[%s], value[%s]", name, value);
        return -1;
    }

    nLenTag = strlen(name);
    memset(sLen, 0x00, sizeof(sLen));
    sprintf(sLen, "%02d", nLenTag);
    memcpy(buffer+nLen, sLen, 2);
    nLen += 2;
    memcpy(buffer+nLen, name, nLenTag);
    nLen += nLenTag;

    memset(sLen, 0x00, sizeof(sLen));
    sprintf(sLen, "%04d", strlen(value));
    nLenTag = atoi(sLen);
    memcpy(buffer+nLen, sLen, 4);
    nLen += 4;
    memcpy(buffer+nLen, value, nLenTag);
    nLen += nLenTag;

    memset(sLen, 0x00, sizeof(sLen));
    sprintf(sLen, "%06d", nLen-6);
    memcpy(buffer, sLen, 6);

    return 0;
}
void SetIpcLongValue(char *buffer, char *name, long num )
{
    char    sLen[50+1], sTmpBuf[128];
    int     nLen = 0, nLenTag = 0;  
	static char value[32];
	memset(value, 0x00, sizeof(value));
	sprintf(value, "%ld", num);

    memset(sLen, 0x00, sizeof(sLen));
    memcpy(sLen, buffer, 6);
    nLen += 6;
    nLen += atoi(sLen);

    nLenTag = strlen(value)+6;
    if(nLenTag > 6959)
    {
        HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__, __LINE__, "SetIpcValue  name[%s], value[%s]", name, value);
        return ;
    }

    nLenTag = strlen(name);
    memset(sLen, 0x00, sizeof(sLen));
    sprintf(sLen, "%02d", nLenTag);
    memcpy(buffer+nLen, sLen, 2);
    nLen += 2;
    memcpy(buffer+nLen, name, nLenTag);
    nLen += nLenTag;

    memset(sLen, 0x00, sizeof(sLen));
    sprintf(sLen, "%04d", strlen(value));
    nLenTag = atoi(sLen);
    memcpy(buffer+nLen, sLen, 4);
    nLen += 4;
    memcpy(buffer+nLen, value, nLenTag);
    nLen += nLenTag;

    memset(sLen, 0x00, sizeof(sLen));
    sprintf(sLen, "%06d", nLen-6);
    memcpy(buffer, sLen, 6);

    return ;
}


int GetIpcValue(char *buffer, char *name, char *value)
{
    char    sLen[50+1], sTmpBuf[5];
    int     nLen = 0, nLenTag = 0, nTmpLen = 0;

    memset(sLen, 0x00, sizeof(sLen));
    memcpy(sLen, buffer, 6);
    nLen = atoi(sLen);

    nLenTag = strlen(name);
    memset(sLen, 0x00, sizeof(sLen));
    sprintf(sLen, "%02d%s", nLenTag, name);
    nLenTag += 2;

    while(nTmpLen < nLen)
    {
        if(memcmp(buffer+nTmpLen+6, sLen, nLenTag) == 0)
        {
            memset(sTmpBuf, 0x00, sizeof(sTmpBuf)); 
            nTmpLen += nLenTag;
            memcpy(sTmpBuf, buffer+nTmpLen+6, 4);
            nTmpLen += 4;
            memcpy(value, buffer+nTmpLen+6, atoi(sTmpBuf));
            return atoi(sTmpBuf); 
        }
        else 
        {
            memset(sTmpBuf, 0x00, sizeof(sTmpBuf)); 
            memcpy(sTmpBuf, buffer+nTmpLen+6, 2);
            nTmpLen += 2;
            nTmpLen += atoi(sTmpBuf);

            memset(sTmpBuf, 0x00, sizeof(sTmpBuf)); 
            memcpy(sTmpBuf, buffer+nTmpLen+6, 4);
            nTmpLen += 4;
            nTmpLen += atoi(sTmpBuf);
        }
    }
    return 0;
}
char *GetIpcPtr(char *buffer, char *name)
{
	static char value[4096];
	char    *ptr = value;
    char    sLen[50+1], sTmpBuf[5];
    int     nLen = 0, nLenTag = 0, nTmpLen = 0;
	int		nValLen ;
	

    memset(sLen, 0x00, sizeof(sLen));
    memcpy(sLen, buffer, 6);
    nLen = atoi(sLen);

    nLenTag = strlen(name);
    memset(sLen, 0x00, sizeof(sLen));
    sprintf(sLen, "%02d%s", nLenTag, name);
    nLenTag += 2;

    while(nTmpLen < nLen)
    {
        if(memcmp(buffer+nTmpLen+6, sLen, nLenTag) == 0)
        {
            memset(sTmpBuf, 0x00, sizeof(sTmpBuf)); 
            nTmpLen += nLenTag;
            memcpy(sTmpBuf, buffer+nTmpLen+6, 4);
            nTmpLen += 4;
			nValLen = atoi(sTmpBuf);
            memcpy(ptr, buffer+nTmpLen+6, nValLen);
			value[nValLen] = 0x00;
            return  ptr; 
        }
        else 
        {
            memset(sTmpBuf, 0x00, sizeof(sTmpBuf)); 
            memcpy(sTmpBuf, buffer+nTmpLen+6, 2);
            nTmpLen += 2;
            nTmpLen += atoi(sTmpBuf);

            memset(sTmpBuf, 0x00, sizeof(sTmpBuf)); 
            memcpy(sTmpBuf, buffer+nTmpLen+6, 4);
            nTmpLen += 4;
            nTmpLen += atoi(sTmpBuf);
        }
    }
    return NULL;
}
long GetIpcLongValue(char *buffer, char *name)
{
	static char value[32];
	memset(value, 0x00, sizeof(value));
    char    sLen[50+1], sTmpBuf[5];
    int     nLen = 0, nLenTag = 0, nTmpLen = 0;
	int		nValLen ;
	

    memset(sLen, 0x00, sizeof(sLen));
    memcpy(sLen, buffer, 6);
    nLen = atoi(sLen);

    nLenTag = strlen(name);
    memset(sLen, 0x00, sizeof(sLen));
    sprintf(sLen, "%02d%s", nLenTag, name);
    nLenTag += 2;

    while(nTmpLen < nLen)
    {
        if(memcmp(buffer+nTmpLen+6, sLen, nLenTag) == 0)
        {
            memset(sTmpBuf, 0x00, sizeof(sTmpBuf)); 
            nTmpLen += nLenTag;
            memcpy(sTmpBuf, buffer+nTmpLen+6, 4);
            nTmpLen += 4;
			nValLen = atoi(sTmpBuf);
            memcpy(value, buffer+nTmpLen+6, nValLen);
            return  atol(value); 
        }
        else 
        {
            memset(sTmpBuf, 0x00, sizeof(sTmpBuf)); 
            memcpy(sTmpBuf, buffer+nTmpLen+6, 2);
            nTmpLen += 2;
            nTmpLen += atoi(sTmpBuf);

            memset(sTmpBuf, 0x00, sizeof(sTmpBuf)); 
            memcpy(sTmpBuf, buffer+nTmpLen+6, 4);
            nTmpLen += 4;
            nTmpLen += atoi(sTmpBuf);
        }
    }
    return -99999999;
}
int GetIpcValueExt(char *buffer, char *name, char *value,int value_len)
{
    char    sLen[50+1], sTmpBuf[5];
    int     nLen = 0, nLenTag = 0, nTmpLen = 0;
	memset(value, 0x00 ,value_len);

    memset(sLen, 0x00, sizeof(sLen));
    memcpy(sLen, buffer, 6);
    nLen = atoi(sLen);

    nLenTag = strlen(name);
    memset(sLen, 0x00, sizeof(sLen));
    sprintf(sLen, "%02d%s", nLenTag, name);
    nLenTag += 2;

    while(nTmpLen < nLen)
    {
        if(memcmp(buffer+nTmpLen+6, sLen, nLenTag) == 0)
        {
            memset(sTmpBuf, 0x00, sizeof(sTmpBuf)); 
            nTmpLen += nLenTag;
            memcpy(sTmpBuf, buffer+nTmpLen+6, 4);
            nTmpLen += 4;
            memcpy(value, buffer+nTmpLen+6, atoi(sTmpBuf));
            return atoi(sTmpBuf); 
        }
        else 
        {
            memset(sTmpBuf, 0x00, sizeof(sTmpBuf)); 
            memcpy(sTmpBuf, buffer+nTmpLen+6, 2);
            nTmpLen += 2;
            nTmpLen += atoi(sTmpBuf);

            memset(sTmpBuf, 0x00, sizeof(sTmpBuf)); 
            memcpy(sTmpBuf, buffer+nTmpLen+6, 4);
            nTmpLen += 4;
            nTmpLen += atoi(sTmpBuf);
        }
    }
    return 0;
}


void SetAddValue(char *buffer, char *tag, char *tagvalue, char *delim)
{

    char key[128], value[128], sTmpBuf[128];
    char *ps = NULL, *pe = NULL, *p = NULL;
    int flag = 0;
    char  sumvalue[2048], tmpvalue[1024];

    //ps = tag;
    p = strchr(tag, '=');
    if(p == NULL)
    {
        SetIpcValue(buffer, tag, tagvalue);
        return ;
    }
    else
    {
        memset(sumvalue, 0x00, sizeof(sumvalue));
        memset(key, 0x00, sizeof(key));
        memcpy(key, tag, p-tag);
        memset(value, 0x00, sizeof(value));
        strcpy(value, p+1);
        ps = value;
        do
        {
            memset(tmpvalue, 0x00, sizeof(tmpvalue));
            p = NULL;
            p = strchr(ps, '+');
            if(p != NULL)
            {
                flag = 1;
                memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
                memcpy(sTmpBuf, ps, p-ps);
                GetIpcValue(buffer, sTmpBuf, tmpvalue);
                strcat(sumvalue, tmpvalue);
                strcat(sumvalue, delim);
                ps = p+1;
            }
            else
            {
                if(flag == 1)
                {
                    memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
                    strcpy(sTmpBuf, ps);
                    GetIpcValue(buffer, sTmpBuf, tmpvalue);
                    strcat(sumvalue, tmpvalue);
                    break;
                }
                else
                {
                    break;
                }
            }
        }while(1);
        if(flag == 1)
        {
            SetIpcValue(buffer, key, sumvalue);
        }
    }
    HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "key:[%s] sumvalue:[%s]", key, sumvalue);
    return ;
}
int DeleteIpcValue(char *buffer, char *name)
{
    char    sLen[50+1], sTmpBuf[5];
    int     nLen = 0, nLenTag = 0, nTmpLen = 0;

    memset(sLen, 0x00, sizeof(sLen));
    memcpy(sLen, buffer, 6);
    nLen = atoi(sLen);

    nLenTag = strlen(name);
    memset(sLen, 0x00, sizeof(sLen));
    sprintf(sLen, "%02d%s", nLenTag, name);
    nLenTag += 2;

    while(nTmpLen < nLen)
    {
        if(memcmp(buffer+nTmpLen+6, sLen, nLenTag) == 0)
        {
            memset(buffer+nTmpLen+6+2, 0x20, nLenTag-2);
            return 1;
        }
        else
        {
            memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
            memcpy(sTmpBuf, buffer+nTmpLen+6, 2);
            nTmpLen += 2;
            nTmpLen += atoi(sTmpBuf);

            memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
            memcpy(sTmpBuf, buffer+nTmpLen+6, 4);
            nTmpLen += 4;
            nTmpLen += atoi(sTmpBuf);
        }
    }
    return 0;
}
int DeleteIpcValueExt(char *buffer, char *name)
{
    char    sLen[50+1], sTmpBuf[5];
    int     nLen = 0, nLenTag = 0, nTmpLen = 0;

    memset(sLen, 0x00, sizeof(sLen));
    memcpy(sLen, buffer, 6);
    nLen = atoi(sLen);

    nLenTag = strlen(name);
    memset(sLen, 0x00, sizeof(sLen));
    sprintf(sLen, "%02d%s", nLenTag, name);
    nLenTag += 2;

    while(nTmpLen < nLen)
    {
        if(memcmp(buffer+nTmpLen+6, sLen, nLenTag) == 0)
        {
            memset(buffer+nTmpLen+6+2, 0x20, nLenTag-2);
            memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
			memcpy(sTmpBuf, buffer+nTmpLen+6+nLenTag, 4);
            memset(buffer+nTmpLen+6+nLenTag+4, 0x20, atoi(sTmpBuf));
            return 1;

        }
        else
        {
            memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
            memcpy(sTmpBuf, buffer+nTmpLen+6, 2);
            nTmpLen += 2;
            nTmpLen += atoi(sTmpBuf);

            memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
            memcpy(sTmpBuf, buffer+nTmpLen+6, 4);
            nTmpLen += 4;
            nTmpLen += atoi(sTmpBuf);
        }
    }
    return 0;
}
void CopyIpcValue(char *buffer, char *fromtag, char *totag)
{
    char   sTmpBuf[2048];
    int    len = 0;
    len = GetIpcValue(buffer, fromtag, sTmpBuf);
    SetIpcValueExt(buffer, totag, sTmpBuf, len);
    return ;
}
void ReSetIpcValue(char *buffer, char *fromtag, char *value)
{
    DeleteIpcValue(buffer, fromtag);
    SetIpcValue(buffer, fromtag, value);
    return ;
}
void ReSetIpcValueExt(char *buffer, char *fromtag, char *value, int value_len)
{
    DeleteIpcValue(buffer, fromtag);
    SetIpcValueExt(buffer, fromtag, value, value_len);
    return ;
}

void CopyIpc2Ipc(char *frombuf, char *fromtag, char *tobuf, char *totag)
{
    char sTmpBuf[2048];
    int  len = 0;
    len =GetIpcValue(frombuf, fromtag, sTmpBuf);
    SetIpcValueExt(tobuf, totag, sTmpBuf, len);
    return ;
}

void showIpc(char *IPC)
{
    char            sFuncName[] = "showIpc";
    char    sTag[64], sValue[1024];
    char    sLen[7];
    int     n = 0, nAddLen = 0;

    HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s begin", sFuncName);

    memset(sLen, 0x00, sizeof(sLen));
    memcpy(sLen, IPC, 6);
    n = atoi(sLen);
 	while(nAddLen < n)
    {
        memset(sLen, 0x00, sizeof(sLen));
        memcpy(sLen, IPC+6+nAddLen, 2);
        nAddLen += 2;
        memset(sTag, 0x00, sizeof(sTag));
        memcpy(sTag, IPC+6+nAddLen, atoi(sLen));
        nAddLen += atoi(sLen);
        memset(sLen, 0x00, sizeof(sLen));
        memcpy(sLen, IPC+6+nAddLen, 4);
        nAddLen += 4;
        memset(sValue, 0x00, sizeof(sValue));
        memcpy(sValue, IPC+6+nAddLen, atoi(sLen));
        nAddLen += atoi(sLen);
        HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "[%-20s][%s]", sTag, sValue);
    }
    HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s end", sFuncName);
    return ;
}

int file_stat_ctime(const char* path, time_t* time)
{
	struct stat astat;
	int    statret = stat(path,&astat);

	if (0 != statret) {
		return statret;
	}
	*time = astat.st_ctime;
	return statret;
}
void SetTxnValue(char *column, char *IPC, char *name, int len)
{
	char sTmpBuf[1024] ;
	int  nTmpLen = 0;
	memset(sTmpBuf, 0x20, sizeof(sTmpBuf));
	GetIpcValue(IPC, name, sTmpBuf);
	memcpy(column, sTmpBuf, len);
	return ;
}

int parse_tag_tlvs(char *sTag, char *sBuf, int nBufLen, char *sTag_values)
{   
    int     i=0,iLen=0;
    char    offset[3+1];   
    
    if(nBufLen == 0)
    {    
        return -1;
    }    
    while(1)
    {    
        memset(offset, 0x00, sizeof(offset));
        if(memcmp(sBuf+i, sTag, 2) != 0)
        {    
            memcpy(offset, sBuf+i+2, 3);
            iLen=atoi(offset);
            i=i+2+3+iLen;   
            if(nBufLen-i <= 0)
            {   
                HtLog(gsLogFile, HT_LOG_MODE_ERROR, __FILE__, __LINE__, "%s",sBuf);
                return -1;
            }
        }
        else
        {                                                                                               
            memcpy(offset, sBuf+i+2, 3);                                                                
            iLen=atoi(offset);                                                                          
            memcpy(sTag_values, sBuf+i+2+3, iLen);                                                      
            i=i+2+3+iLen;                                                                               
            break;                                                                                      
        }                                                                                               
    }                                                                                                   
    return iLen;              
}


int t2i(char s[])
{
	int i, m;
	int n = 0;
	if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))
	{    
		i = 2;
	} 
	else     
	{ 
		i = 0;
	}
	for (m = i; (s[m] >= '0' && s[m] <= '9') || (s[m] >= 'a' && s[m] <= 'z') || (s[m] >='A' && s[m] <= 'Z');m++)
	{
		if(tolower(s[m]) > '9')
		{
			n = 16 * n + (10 + tolower(s[m]) - 'a');
		}
		else 
		{
			n = 16 * n + (tolower(s[m]) - '0');
		}
	}
	return n;
}
time_t convet_date(int year, int month, int day)
{
    struct tm info  = {0};
    info.tm_year = year - 1900;
    info.tm_mon = month - 1;
    info.tm_mday = day;

    return mktime(&info);
}
int GetOriTxnLeapSec(char *sOriTxnDate)
{
    char sFuncName[] = "GetOriTxnLeapSec";
    char sOriYear[4 + 1];
    char sOriMonth[2 + 1];
    char sOriDay[2 + 1];
    char sOriHour[2 + 1];
    char sOriMin[2 + 1];
    char sOriSec[2 + 1];

    char sCurrentDate[14 + 1];
    char sCurrYear[4 + 1];
    char sCurrMonth[2 + 1];
    char sCurrDay[2 + 1];
    char sCurrHour[2 + 1];
    char sCurrMin[2 + 1];
    char sCurrSec[2 + 1];
    int iOriSecCount = 0;
    int iCurrSecCount = 0;
    HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s begin.", sFuncName);
    memset(sOriYear, 0x00, sizeof(sOriYear));
    memset(sOriMonth , 0x00, sizeof(sOriMonth ));
    memset(sOriDay , 0x00, sizeof(sOriDay));
    memset(sOriHour, 0x00, sizeof(sOriHour));
    memset(sOriMin , 0x00, sizeof(sOriMin));
    memset(sOriSec , 0x00, sizeof(sOriSec));

    HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "oldtime=[%14.14s]", sOriTxnDate);
    memcpy(sOriYear, sOriTxnDate, 4);
    memcpy(sOriMonth, sOriTxnDate+4, 2);
    memcpy(sOriDay, sOriTxnDate+4+2, 2);
    memcpy(sOriHour, sOriTxnDate+4+2+2, 2);
    memcpy(sOriMin, sOriTxnDate+4+2+2+2, 2);
    memcpy(sOriSec, sOriTxnDate+4+2+2+2+2, 2);

    iOriSecCount = (int)convet_date(atoi(sOriYear), atoi(sOriMonth), atoi(sOriDay)) + atoi(sOriHour)*3600 + atoi(sOriMin)*60 + atoi(sOriSec);
	memset(sCurrentDate, 0x00, sizeof(sCurrentDate));
    memset(sCurrYear , 0x00, sizeof(sCurrYear));
    memset(sCurrMonth, 0x00, sizeof(sCurrMonth));
    memset(sCurrDay, 0x00, sizeof(sCurrDay));
    memset(sCurrHour , 0x00, sizeof(sCurrHour));
    memset(sCurrMin, 0x00, sizeof(sCurrMin));
    memset(sCurrSec, 0x00, sizeof(sCurrSec));

    CommonGetCurrentTime(sCurrentDate);
    HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "newtime=[%14.14s]", sCurrentDate);
    memcpy(sCurrYear, sCurrentDate, 4);
    memcpy(sCurrMonth, sCurrentDate+4, 2);
    memcpy(sCurrDay, sCurrentDate+4+2, 2);
    memcpy(sCurrHour, sCurrentDate+4+2+2, 2);
    memcpy(sCurrMin, sCurrentDate+4+2+2+2, 2);
    memcpy(sCurrSec, sCurrentDate+4+2+2+2+2, 2);
    iCurrSecCount = (int)convet_date(atoi(sCurrYear), atoi(sCurrMonth), atoi(sCurrDay)) + atoi(sCurrHour)*3600 + atoi(sCurrMin)*60 + atoi(sCurrSec);

    HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s end.", sFuncName);
    return (iCurrSecCount - iOriSecCount);
}

int getValueByDelim(char *sSrc, char *sDest, char *sDelim )
{
	char *sEnd;

	if( *sSrc == '\0')
	{
		return 1;
	}

	sEnd = strstr(sSrc, sDelim);
	if(sEnd == NULL)
	{
		strcpy(sDest, sSrc);
		return 2;
	}

	memcpy(sDest, sSrc, sEnd-sSrc);
	strcpy(sSrc, sEnd + (long)strlen(sDelim));
	return 0;
}


int XorBuf16(char *sMacBlock, int nMacBlockLen, char *outMacData)
{
    int     i,j, length;
    unsigned long outdata[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    char    inverse_c[16];
    char    outdata1[16];

    unsigned short zero = 0x00;

    if (sMacBlock == NULL)
        return -1;

    length = nMacBlockLen;

    for (i=0; i<=15; i++)
        outdata1[i] = (char)outdata[i];

    length = length -1;
    for (i=0; i<=length; i=i+16) {
        inverse_c[0] = sMacBlock[i];
        inverse_c[1] = sMacBlock[i+1];
        inverse_c[2] = sMacBlock[i+2];
        inverse_c[3] = sMacBlock[i+3];
        inverse_c[4] = sMacBlock[i+4];
        inverse_c[5] = sMacBlock[i+5];
        inverse_c[6] = sMacBlock[i+6];
        inverse_c[7] = sMacBlock[i+7];
        inverse_c[8] = sMacBlock[i+8];
        inverse_c[9] = sMacBlock[i+9];
        inverse_c[10] = sMacBlock[i+10];
        inverse_c[11] = sMacBlock[i+11];
        inverse_c[12] = sMacBlock[i+12];
        inverse_c[13] = sMacBlock[i+13];
        inverse_c[14] = sMacBlock[i+14];
        inverse_c[15] = sMacBlock[i+15];

        if ((i+1) > length ) inverse_c[1] = (char)zero;
        if ((i+2) > length ) inverse_c[2] = (char)zero;
        if ((i+3) > length ) inverse_c[3] = (char)zero;
        if ((i+4) > length ) inverse_c[4] = (char)zero;
        if ((i+5) > length ) inverse_c[5] = (char)zero;
        if ((i+6) > length ) inverse_c[6] = (char)zero;
        if ((i+7) > length ) inverse_c[7] = (char)zero;
        if ((i+8) > length ) inverse_c[8] = (char)zero;
        if ((i+9) > length ) inverse_c[9] = (char)zero;
        if ((i+10) > length ) inverse_c[10] = (char)zero;
        if ((i+11) > length ) inverse_c[11] = (char)zero;
        if ((i+12) > length ) inverse_c[12] = (char)zero;
        if ((i+13) > length ) inverse_c[13] = (char)zero;
        if ((i+14) > length ) inverse_c[14] = (char)zero;
        if ((i+15) > length ) inverse_c[15] = (char)zero;

        for (j=0; j<16; j++)
            outdata1[j] = outdata1[j] ^ inverse_c[j];
    }
    memcpy(outMacData, outdata1, 16);
    return 0;
}   
void GenDynamicCode(char *code, int len)
{
  int i,j;
  srand((int) time(0)); 
  for(i = 0; i < len;i++)
  {   j=(int)(10.0*rand()/(RAND_MAX+1.0));
    sprintf(code+i, "%d", j);
  }
  return ;
}

void showTLV(char * ptIpcIntTxn)
{
  char            sFuncName[] = "showTLV";
  char    sTag[64], sValue[1024];
  char    sLen[7];
  int     n = 0, nAddLen = 0;
  char    *IPC = ptIpcIntTxn+24;

  HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s begin", sFuncName);

  memset(sLen, 0x00, sizeof(sLen));
  memcpy(sLen, IPC, 6);
  n = atoi(sLen);
  while(nAddLen < n)
  {
    memset(sLen, 0x00, sizeof(sLen));
    memcpy(sLen, IPC+6+nAddLen, 2);
    nAddLen += 2;
    memset(sTag, 0x00, sizeof(sTag));
    memcpy(sTag, IPC+6+nAddLen, atoi(sLen));
    nAddLen += atoi(sLen);
    memset(sLen, 0x00, sizeof(sLen));
    memcpy(sLen, IPC+6+nAddLen, 4);
    nAddLen += 4;
    memset(sValue, 0x00, sizeof(sValue));
    memcpy(sValue, IPC+6+nAddLen, atoi(sLen));
    nAddLen += atoi(sLen);
    HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "[%-20s][%s]", sTag, sValue);
  }
  HtLog (gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "%s end", sFuncName);
  return ;
}
int dnsResolve(const char *hostname, char *ip)
{
    struct in_addr  ip_addr;
    struct hostent  *host;
    char           **pptr;
    host = gethostbyname(hostname);
    if(!host)
        return -1;
    pptr = host->h_addr_list;
    if(!inet_ntop(host->h_addrtype, *pptr, ip, 15))
    {   return -2;
    }
    return 0;
}

void timeFormat(char *input) 
{
    struct tm t;
	char output[32];
    // ʹԃ strptime ½㏶ˤɫؖ·�    strptime(input, "%Y%m%d%H%M%S", &t);
    // ʹԃ strftime ¸򊽻¯ΪѨҪµŊ䴶¸򊻍
	memset(output, 0x00, sizeof(output));
    strftime(output, sizeof(output), "%Y-%m-%d %H:%M:%S", &t);
	strcpy(input, output);
    return ;
}
void getDigitTime(char *output)
{
    time_t current_time;

    // »򈡵±ǰʱ¼䶄 Unix ʱ¼䵁
    current_time = time(NULL);
    // ´󓟠Unix ʱ¼䵁
    sprintf(output, "%ld", current_time);
    return ;
}

char generate_random_char() 
{
	struct timeval  tv;
	gettimeofday(&tv, NULL);
	unsigned int seed = tv.tv_sec ^ tv.tv_usec ^ getpid();
	srand( (int) seed);
    int rand_num = rand() % 16; // 62 = 10 (digits) + 26 (lowercase) + 26 (uppercase)
    if (rand_num < 10) 
	{
        return '0' + rand_num; // 0-9
    } 
	else if (rand_num < 16) 
	{
        return 'A' + (rand_num - 10); // a-z
    } 
}

void generate_random_string(char *str, int length) {
    int i;
    srand(time(NULL)); // Seed the random number generator
    for (i = 0; i < length; i++) {
        str[i] = generate_random_char();
    }
    str[length] = '\0'; // Null-terminate the string
}
void xor_blocks(unsigned char *block1, unsigned char *block2, unsigned char *result, size_t size) {
	int i;
    for (i = 0; i < size; i++) {
        result[i] = block1[i] ^ block2[i];
    }
}


void  A_xor_blocks(unsigned char *block1, unsigned char *block2, unsigned char *result, size_t size)
{
	char  BH1[256];
	char  BH2[256];
	char  BH3[256];
	Str2Hex(block1,  BH1, size);
	Str2Hex(block2,  BH2, size);
	xor_blocks(BH1, BH2, BH3, size/2);
	Hex2Str(BH3, result, size/2);
}

void split_with_strtok(char *str, char *delim) {
    char *token = strtok(str, delim);
    while (token != NULL) {
        printf("Token: %s\n", token);
        // 后续调用第一个参数为NULL
        token = strtok(NULL, delim);
    }
}

char *get_uuid()
{
    uuid_t uuid;                // 定义一个uuid_t类型的变量（通常是一个16字节的数组）
    static char uuid_str[37];          // 36个字符的UUID字符串 + '\0'
	char *ptr = uuid_str;
    // 生成一个UUID (推荐使用这个)
    uuid_generate(uuid);
    // 将二进制UUID转换为可读的字符串形式
    uuid_unparse(uuid, ptr);
	return ptr;
}
long get_current_time_millis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
