
/*==========================================================================*

	FILE		  		msq.h

	TARGET			UNIX message queue programming support

	HISTORY			DATE			DESCRIPTION
	~~~~~~~			~~~~			~~~~~~~~~~~
	Ray Hawkins		06/20/96		Initial version

*==========================================================================*/

#ifndef	_MSQ_H
#define	_MSQ_H

/* HEADER FILE(S) */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pwd.h>
#include <grp.h>
#include <memory.h>

/* GLOBAL DEFINE(S) */

#define	C_MsqR			0400 /* msg queue read */
#define	C_MsqW			0200 /* msg queue write */
#define	C_MsqRW			0600 /* msg queue read & write */
#define	C_MsqAny			0000 /* msg queue no read or write */

#define	C_MsqPerm		((mode_t) 0777) /* queue permission */

#define	C_MsqAnyT		0 /* of any message type: for MsqRecv() */
#define	C_MsqNone		-1 /* param not used: for MsqChown() */
#define	C_MsqAll			-1 /* dump all message: for MsqDump() */

#define	C_MsgbufS		1024 /* mtext size small */
#define	C_MsgbufM		2048 /* mtext size medium */
#define	C_MsgbufL		4096 /* mtext size large */

#define	C_MsgErr			-1 /* msg error */
#define	C_MsgEnd			-2 /* msg data end */
#define	C_MsgEOF			-3 /* msg data file EOF */
#define	C_MsgOVF			-4 /* msgbuf overflow */

#define	C_MsgCheck		1 /* check msg data */
#define	C_MsgNoCheck	0 /* do not check msg data */


#endif /* _MSQ_H */
