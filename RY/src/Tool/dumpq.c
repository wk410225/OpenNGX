#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "msq.h"
#include <ctype.h>

typedef struct
{
    long    lMsgType;
    char    sMsg[4096];
} T_MsqMsgDef;


char gsLogFile[32] = "dumpq.log";


short DebugString( char *psBuf, int iLength, int iLine)
{
   int i,j=0;
   char s[100], temp[5];

   printf( "Debug Information from Line: %04d\n", iLine);

   for (i=0; i<iLength; i++)
   {
      if (j==0)
      {
         memset( s, ' ', 84);
         sprintf(temp,   " %03d:",i );
         memcpy( s, temp, 5);
         sprintf(temp,   ":%03d",i+15 );
         memcpy( &s[72], temp, 4);
      }
      sprintf( temp, "%02X ", (unsigned char)psBuf[i]);
      memcpy( &s[j*3+5+(j>7)], temp, 3);
      if ( isprint( psBuf[i]))
      {
         s[j+55+(j>7)]=psBuf[i];
      }
      else
      {
         s[j+55+(j>7)]='.';
      }
      j++;
      if ( j==16)
      {
         s[76]=0;
         printf( "%s\n", s);
         j=0;
      }
   }
   if ( j)
   {
      s[76]=0;
      printf( "%s\n", s);
   }
   return 0;
}

void MsgbufDump (FILE *fp, T_MsqMsgDef *pmsg, size_t nbytes) 
{
	fprintf (fp, "MsgLen = %d  MsgType = %d\n", (int)nbytes, (int)pmsg->lMsgType); 
	DebugString(pmsg->sMsg, nbytes, __LINE__);
}

int MsqGet (key_t key ) 
{
	return (msgget (key, 0660|IPC_CREAT));
}

int MsqRecvType (int msqid, T_MsqMsgDef *pmsg, size_t nbytes, long lMsgType) 
{
	return (msgrcv (msqid, (T_MsqMsgDef *) pmsg, nbytes, lMsgType, 0));
}

int MsqRecv (int msqid, T_MsqMsgDef *pmsg, size_t nbytes) 
{
	return (MsqRecvType (msqid, pmsg, nbytes, 0 ));
}

int MsqCount (int msqid) 
{
	struct msqid_ds qds;

	if (msgctl (msqid, IPC_STAT, &qds) == -1) return (-1);

	return (qds.msg_qnum);
}

int MsqDump (FILE *fp, int msqid, int count) 
{
	T_MsqMsgDef msgbuf;
	int cnmsg = 0;
	int cnbyte;

	while (MsqCount (msqid) > 0) 
	{
		if (cnmsg >= count && count != -1) break;
		memset (&msgbuf, 0, sizeof (msgbuf));
		if ((cnbyte = MsqRecv (msqid, (T_MsqMsgDef *) &msgbuf, 4096)) == -1)
      		return (-1);
		MsgbufDump (fp, (T_MsqMsgDef *) &msgbuf, cnbyte);
		fputc ('\n', fp);
		fflush (fp);
		cnmsg++;
	}

	return (cnmsg);
}

int main (int argc, char ** argv) 
{
	key_t msqkey;
	char *sCount;
	long  msqid;
	int  count;
	int  cnmsg;
	int  nTotal;
	int  i;


    setbuf (stdout, NULL);
    setbuf (stderr, NULL);

	if (argc < 2) 
	{
		fprintf (stderr, "Usage: %s{ <MsgQueKey|MsgID>[:Count] ... }\n"
				  "       Count(def.): -1 (all)\n", argv[0]);
		exit (1);
	}

	for( i = 1, nTotal = 0; i < argc; i++ ) 
	{ 
   		if( sscanf( argv[i], "%i", &msqkey ) != 1 ) 
		{
   			fprintf( stderr, "DUMPQ: Cannot get MsgQueKey from \"%s\"\n", 
							argv[i] );
   			exit(1);
      	}

		if( ( sCount = strchr( argv[i], ':' ) ) == NULL ) 
		   	count = -1; /* dump all msg from the queue */
		else if( sscanf( ++sCount, "%i", &count ) != 1 ) 
		{
   			fprintf( stderr, "DUMPQ: Cannot get Count from \"%s\"\n", argv[i] );
   			exit (1);
   		}

   		if(msqkey == IPC_PRIVATE || (msqid = MsqGet (msqkey)) == -1) 
		{
			msqid = msqkey;
   		}
   
   		if( (cnmsg = MsqDump( stdout, msqid, count ) ) == -1 ) 
		{
   			fprintf( stderr, 
					"DUMPQ: Cannot dump message from queue %d (0x%X) :: %s\n",
   				   	msqkey, msqkey, strerror (errno));
   			exit (1);
   		}

   		printf( "DUMPQ: %d message(s) dumped from queue %d (0x%X)\n",
   			   	cnmsg, msqkey, msqkey);

      	nTotal += cnmsg;
   }

	if( argc > 2 ) printf( "DUMPQ: total = %d\n", nTotal );

}
