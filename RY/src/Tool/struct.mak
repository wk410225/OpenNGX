include $(ONLINE_HOME)/mak/platform.mak
include $(ONLINE_HOME)/mak/database.mak

PRGOBJS = \
	$(ONLINE_HOME)/src/Tool/struct.o

PRGTARG = struct
PRGLIBS = -lCommon -lDbsComm $(NETLIBS) $(UXLIBS)
PRGDEFS =

# used for db2 database
PRGDBNM =

debug all: debugexec
release: releaseexec

# DO NOT modify any code below!!!

releasedynamic debugdynamic releasestatic debugstatic releaseexec debugexec clean:
	@make -f $(ONLINE_HOME)/mak/mkstand.mak $@ TARGET="$(PRGTARG)" OBJS="$(PRGOBJS)" LIBS="$(PRGLIBS)" DEFS="$(PRGDEFS)" DBNM="$(PRGDBNM)"
