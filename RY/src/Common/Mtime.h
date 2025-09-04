#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <stdio.h> 
#include <stdlib.h>

void GetGMTCurrentTime(char *sCurrentTime);

time_t GetTimeInLong (char *sTime,int GmtFlag);

/*unsigned char *juliancal_to_suncal( char *insert_time ); */

int AddYearToDate(
                char *spInDate,
                char *spOutDate);
