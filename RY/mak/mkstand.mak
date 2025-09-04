include $(ONLINE_HOME)/mak/platform.mak
include $(ONLINE_HOME)/mak/database.mak
# environment variables must be define outside of makefile
#
# $ONLINE_HOME : home directory of application 
# $CCFLG  : compiler options for c source code
# $CXXFLG : compiler options for c++ source code
# $DLFLG  : link options for dynamic library
# $SLFLG  : link options for static library
# $EXFLG  : link options for excecuteable object
# $DBFLG  : debug mode options
# $OPFLG  : optimize mode options
# $DLFIX  : suffix of dynamic library
# $SLFIX  : suffix of static library

# $OBJS   : c object files list
# $LIBS   : libs required for executable object
# $DEFS   : define flag list
# $TARGET : target objetc name

# $TUXSVCS: tuxedo services name
# $TUXRSMN: tuxedo resource manager name

# $DBNM   : db name (for db2)

# commands
 
PC		=	proc
EC		=	esql
DB2		=	db2

JAVAC   =   javac
JAVAH   =   javah
RMIC    =   rmic

RM		=	rm -f
CP		=	cp
MV		=   mv
CD		=   cd
ECHO	=	@echo

# paths and flags

SYSINCP	=	/usr/include
SYSLIBP	=	/usr/lib64
JINCP	=	$(JAVA_HOME)/include
JMDINCP	=	$(JAVA_HOME)/include/$(JAVAMD)

SRCP	=	$(ONLINE_HOME)/source
BINP	= 	$(ONLINE_HOME)/bin
TUXBINP =	$(ONLINE_HOME)/bin/tux
LIBP	=	$(ONLINE_HOME)/lib
INCP	=	$(ONLINE_HOME)/include
CLSP    =   $(BINP)/classes
JLIBP   =   $(JAVA_HOME)/lib

INCPS	=	-I$(SYSINCP) -I$(INCP) -I$(DBINCP) -I $(INCP)/mysql -I$(TUXINCP) -I/usr/local/include -I$(JINCP) -I$(JMDINCP) -I.
LIBPS	=	-L$(SYSLIBP) -L$(XLIBP) -L$(LIBP) $(DBLIBP) -L$(TUXLIBP) -L/usr/lib64/mysql -L/usr/local/lib
 
CCFLGS	=	$(RLFLG) $(CCFLG) $(DEFS) $(TUXDEF) $(DBDEF) $(OSDEF) $(PROCDEF) $(BITDEF) $(INCPS)
CXXFLGS =   $(RLFLG) $(CXXFLG) $(DEFS) $(TUXDEF) $(DBDEF) $(OSDEF) $(PROCDEF) $(BITDEF) $(INCPS)
DLFLGS	=	$(DLFLG) $(LIBPS)
EXFLGS	=	$(EXFLG) $(LIBPS)
 
DLTARG	= $(LIBP)/lib$(TARGET)$(DLFIX)
SLTARG	= $(LIBP)/lib$(TARGET)$(SLFIX)
EXTARG	= $(BINP)/$(TARGET)
TUXTARG	= $(TUXBINP)/$(TARGET)

debugdynamic: setdebug $(DLTARG)
releasedynamic: setrelease $(DLTARG)
debugstatic: setdebug $(SLTARG)
releasestatic: setrelease $(SLTARG)
debugexec: setdebug $(EXTARG)
releaseexec : setrelease $(EXTARG)
debugtuxsvrexec: setdebug $(TUXTARG)
releasetuxsvrexec : setrelease $(TUXTARG)
javaclasses : $(OBJS)
	$(ECHO) "Finish java package [$(TARGET)]"
rmi :
	$(ECHO) "rmic [$(TARGET)] ..."
	@$(CD) `dirname $(TARGET)`; \
	$(RMIC) `basename $(TARGET)`

setdebug:
	$(ECHO) $(DBFLG) > $(ONLINE_HOME)/mak/.mak.tmp

setrelease:
	$(ECHO) $(OPFLG) > $(ONLINE_HOME)/mak/.mak.tmp

$(SLTARG): $(OBJS) 
	$(ECHO) "Linking   [$(SLTARG)] ..."
	@$(SLINK) $(SLFLG) $@ $(OBJS)
	@$(RM) $(ONLINE_HOME)/mak/.mak.tmp

$(DLTARG): $(OBJS) 
	$(ECHO) "Linking   [$(DLTARG)] ..."
#	$(ECHO) "$(DLFLGS)"
#	$(ECHO) "$(LIBS)"
#	$(ECHO) "$(UXLIBS)"
	@if [ $(OSTYPE) = aix ]; \
	then \
		$(DLINK) -o $@ $(DLFLGS) $(OBJS) $(LIBS) $(UXLIBS); \
	else \
		$(DLINK) -o `basename $@` $(DLFLGS) $(OBJS) $(LIBS); \
		$(MV) `basename $@` `dirname $@`; \
	fi
	@$(RM) $(ONLINE_HOME)/mak/.mak.tmp

$(EXTARG): $(OBJS)
	$(ECHO) "Linking   [$(EXTARG)] ..."
#	$(ECHO) "[$(EXFLGS)...$(OBJS) $(LIBS) $(DBLIBS)]"
	@$(CC) -o $@ $(EXFLGS) $(OBJS) $(LIBS)
	@$(RM) $(ONLINE_HOME)/mak/.mak.tmp

$(TUXTARG): $(OBJS)
	$(ECHO) "buildserver [$(TUXTARG)] ..."
	@CFLAGS="$(EXFLGS)" \
	$(TUXBS) -o $@ -f "$(OBJS)" $(TUXSVCS) -f "$(LIBS)" $(TUXRSMN)

# implicit
 
.SUFFIXES:
.SUFFIXES: .java .class .xc .ec .pc .sqC .sqc .cp .cpp .c .o
 
.xc.o:
	$(ECHO) "Compiling [$@] ..."
	@$(RM) $*$(DBFIX)
	@$(CP) $*.xc $*$(DBFIX)
	@if [ $(DBTYPE) = ora ]; \
	then \
		$(PC) lines=yes define=$(DBD) include=$(TUXINCP) include=$(INCP) include=$(JINCP) include=$(JMDINCP) include=`dirname $@` code=ANSI_C iname=$*$(DBFIX); \
		$(CC) -o $@ `cat $(ONLINE_HOME)/mak/.mak.tmp` $(CCFLGS) -c $*.c; \
		$(RM) $*.lis; \
	else \
		$(EC) -e -ED$(DBD) $(INCPS) $*$(DBFIX); \
		$(CC) -o $@ `cat $(ONLINE_HOME)/mak/.mak.tmp` $(CCFLGS) -c `basename $*`.c; \
		$(RM) `basename $*`.c; \
	fi
	@$(RM) $*$(DBFIX) $*.c

.pc.o:
	$(ECHO) "Compiling [$@] ..."
	@$(PC) lines=yes define=$(DBD) include=$(TUXINCP) include=$(INCP) include=$(JINCP) include=$(JMDINCP) include=`dirname $@` code=ANSI_C iname=$*$(DBFIX)
	@$(CD) `dirname $@` ; \
	$(CC) -o $@ `cat $(ONLINE_HOME)/mak/.mak.tmp` $(CCFLGS) -c $*.c
	@$(RM) $*.lis $*.c

.ec.o:
	$(ECHO) "Compiling [$@] ..."
	@$(EC) -e -ED$(DBD) $(INCPS) $*$(DBFIX)
	@$(CD) `dirname $@` ; \
	$(CC) -o $@ `cat $(ONLINE_HOME)/mak/.mak.tmp` $(CCFLGS) -c `basename $*`.c; \
	$(RM) `basename $*`.c

.sqc.o:
	$(ECHO) "Compiling [$@] ..."
	@$(RM) $*.bnd
	@$(DB2) connect to $(DBNM)
	@DB2INCLUDE=$(INCP) \
	$(DB2) prep $< bindfile
	@$(DB2) bind $*.bnd
	@$(DB2) connect reset
	@$(DB2) terminate
	@$(CD) `dirname $@` ; \
	$(CC) -o $@ `cat $(ONLINE_HOME)/mak/.mak.tmp` $(CCFLGS) -c `basename $*`.c
	@$(RM) $*.bnd $*.c

.sqC.o:
	$(ECHO) "Compiling [$@] ..."
	@$(RM) $*.bnd
	@$(DB2) connect to $(DBNM)
	@DB2INCLUDE=$(INCP) \
	$(DB2) prep $< bindfile
	@$(DB2) bind $*.bnd
	@$(DB2) connect reset
	@$(DB2) terminate
	@$(CD) `dirname $@` ; \
	$(CXX) -o $@ `cat $(ONLINE_HOME)/mak/.mak.tmp` $(CCFLGS) -c `basename $*`.C
	@$(RM) $*.bnd $*.C

.cp.o:
	$(ECHO) "Compiling [$@] ..."
	$(CPRE) -y $(INCPS) -O $*.c $*.cp
	@$(CD) `dirname $@` ; \
	$(CC) -o $@ `cat $(ONLINE_HOME)/mak/.mak.tmp` $(CCFLGS) -c `basename $*`.c
	@$(RM) $*.c

.cpp.o:
	$(ECHO) "Compiling [$@] ..."
	@$(CD) `dirname $@` ; \
	$(CXX) -o $@ `cat $(ONLINE_HOME)/mak/.mak.tmp` $(CXXFLGS) -c `basename $<`

.c.o:
	$(ECHO) "Compiling [$@] ..."
	@$(CD) `dirname $@`; \
	$(CC) -o $@ `cat $(ONLINE_HOME)/mak/.mak.tmp` $(CCFLGS) -c `basename $<`
 
.java.class:
	$(ECHO) "Compiling [$@] ..."
	@$(CD) `dirname $@`; \
	$(JAVAC) `basename $<`

# clearing object codes
 
clean: cleanup
	@$(RM) $(SLTARG) $(DLTARG) $(EXTARG) $(TUXTARG)
 
cleanup:
	@$(RM) $(OBJS)
