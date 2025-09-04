include $(ONLINE_HOME)/mak/platform.mak
include $(ONLINE_HOME)/mak/database.mak

PRGOBJS = \
	$(ONLINE_HOME)/src/Common/Common.o \
	$(ONLINE_HOME)/src/Common/HtLog.o \
	$(ONLINE_HOME)/src/Common/MsqOpr.o \

PRGTARG = Common
PRGLIBS = -lm -luuid $(NETLIBS) $(UXLIBS) $(DBLIBS)
PRGDEFS =

debug all: debugdynamic
release: releasedynamic

# DO NOT modify any code below!!!

releasedynamic debugdynamic releasestatic debugstatic releaseexec debugexec clean:
	@make -f $(ONLINE_HOME)/mak/mkstand.mak $@ TARGET="$(PRGTARG)" OBJS="$(PRGOBJS)" LIBS="$(PRGLIBS)" DEFS="$(PRGDEFS)" DBNM="$(PRGDBNM)"
