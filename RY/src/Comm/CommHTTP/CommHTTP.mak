include $(ONLINE_HOME)/mak/platform.mak
include $(ONLINE_HOME)/mak/database.mak
PRGOBJS = \
	$(ONLINE_HOME)/src/Comm/CommHTTP/CommHTTP.o \

PRGTARG = CommHTTP
PRGLIBS =  -lCommon -lDbsComm -levent  $(NETLIBS) $(UXLIBS) 
#PRGLIBS =  -lCommon -lDbsComm -levent -lssl -lcrypto $(NETLIBS) $(UXLIBS) 
#PRGDEFS = -DWITHOUT_OPENSSL
PRGDEFS = 

# used for db2 database
PRGDBNM =

debug all: debugexec
release: releaseexec

# DO NOT modify any code below!!!

releasedynamic debugdynamic releasestatic debugstatic releaseexec debugexec clean:
	@make -f $(ONLINE_HOME)/mak/mkstand.mak $@ TARGET="$(PRGTARG)" OBJS="$(PRGOBJS)" LIBS="$(PRGLIBS)" DEFS="$(PRGDEFS)" DBNM="$(PRGDBNM)"
