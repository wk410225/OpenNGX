#|----------------------------------------------------------------------------|
#|                           TOPMake 2.0                                      |
#| Copyright (c) 2000-2003 Shanghai Huateng Software Systems Co., Ltd.        |
#|    All Rights Reserved                                                     |
#|----------------------------------------------------------------------------|
#| FILE NAME    : sample.exec.mak                                             |
#| DESCRIPTIONS : Sample makefile for executable binary                       |
#|----------------------------------------------------------------------------|

include $(ONLINE_HOME)/mak/platform.mak
include $(ONLINE_HOME)/mak/database.mak
include $(ONLINE_HOME)/mak/tuxedo.mak

PRGOBJS = \
	$(ONLINE_HOME)/src/Tools/dbconfig.o

PRGTARG = dbconfig
PRGLIBS = -lCommon -lDbsComm -lCstDbs $(NETLIBS) $(UXLIBS)
PRGDEFS =

# used for db2 database
PRGDBNM =

debug all: debugexec
release: releaseexec

# DO NOT modify any code below!!!

releasedynamic debugdynamic releasestatic debugstatic releaseexec debugexec clean:
	@make -f $(ONLINE_HOME)/mak/mkstand.mak $@ TARGET="$(PRGTARG)" OBJS="$(PRGOBJS)" LIBS="$(PRGLIBS)" DEFS="$(PRGDEFS)" DBNM="$(PRGDBNM)"
