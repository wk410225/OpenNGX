#|----------------------------------------------------------------------------|
#|                           TOPMake 2.0                                      |
#| Copyright (c) 2000-2003 Shanghai Huateng Software Systems Co., Ltd.        |
#|    All Rights Reserved                                                     |
#|----------------------------------------------------------------------------|
#| FILE NAME    : sample.dynamiclib.mak                                       |
#| DESCRIPTIONS : Sample makefile for dynamic library                         |
#|----------------------------------------------------------------------------|

include $(ONLINE_HOME)/mak/platform.mak
include $(ONLINE_HOME)/mak/database.mak

PRGOBJS = \
	$(ONLINE_HOME)/src/MyDbs/DBS_COMMON.o \
    $(ONLINE_HOME)/src/MyDbs/TBL_SRV_INF.o \
    $(ONLINE_HOME)/src/MyDbs/TBL_MSQ_INF.o \
    $(ONLINE_HOME)/src/MyDbs/TBL_SRV_PARAM.o \


PRGTARG = DbsComm
PRGLIBS = $(DBLIBS)
PRGDEFS =

# used for db2 database
PRGDBNM = $(DBNAME)

debug all: debugdynamic
release: releasedynamic

# DO NOT modify any code below!!!

releasedynamic debugdynamic releasestatic debugstatic releaseexec debugexec clean:
	@make -f $(ONLINE_HOME)/mak/mkstand.mak $@ TARGET="$(PRGTARG)" OBJS="$(PRGOBJS)" LIBS="$(PRGLIBS)" DEFS="$(PRGDEFS)" DBNM="$(PRGDBNM)"
