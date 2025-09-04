include $(ONLINE_HOME)/mak/platform.mak
include $(ONLINE_HOME)/mak/database.mak

PRGOBJS = \
	$(ONLINE_HOME)/src/SwtRY/Switch.o \
	$(ONLINE_HOME)/src/SwtRY/multipart.o \
	$(ONLINE_HOME)/src/SwtRY/SwtCommon.o \
	$(ONLINE_HOME)/src/SwtRY/tbl_srv_param.o \
	$(ONLINE_HOME)/src/SwtRY/tbl_xml_ipc_dsp.o \
	$(ONLINE_HOME)/src/SwtRY/tbl_xml_ipc_values.o \
	$(ONLINE_HOME)/src/SwtRY/tbl_buf_chg.o \
	$(ONLINE_HOME)/src/SwtRY/tbl_fld_tsf.o \
	$(ONLINE_HOME)/src/SwtRY/tbl_fld_dsp.o \
	$(ONLINE_HOME)/src/SwtRY/tbl_buf_dsp.o \
	$(ONLINE_HOME)/src/SwtRY/tbl_buf_dsp_dsp.o \
	$(ONLINE_HOME)/src/SwtRY/captchaImage.o \
	$(ONLINE_HOME)/src/SwtRY/login.o \
	$(ONLINE_HOME)/src/SwtRY/getInfo.o \
	$(ONLINE_HOME)/src/SwtRY/logout.o \
	$(ONLINE_HOME)/src/SwtRY/getRouters.o \
	$(ONLINE_HOME)/src/SwtRY/system_dict_data_type.o \
	$(ONLINE_HOME)/src/SwtRY/system_dict_type.o \
	$(ONLINE_HOME)/src/SwtRY/system_dict_data.o \
	$(ONLINE_HOME)/src/SwtRY/system_user.o \
	$(ONLINE_HOME)/src/SwtRY/system_role.o \
	$(ONLINE_HOME)/src/SwtRY/system_menu.o \
	$(ONLINE_HOME)/src/SwtRY/system_config.o \
	$(ONLINE_HOME)/src/SwtRY/system_dept.o \
	$(ONLINE_HOME)/src/SwtRY/system_post.o \
	$(ONLINE_HOME)/src/SwtRY/system_notice.o \
	$(ONLINE_HOME)/src/SwtRY/monitor_operlog.o \
	$(ONLINE_HOME)/src/SwtRY/monitor_logininfor.o \
	$(ONLINE_HOME)/src/SwtRY/monitor_online.o \
	$(ONLINE_HOME)/src/SwtRY/monitor_job.o \
	$(ONLINE_HOME)/src/SwtRY/monitor_jobLog.o \
	$(ONLINE_HOME)/src/SwtRY/monitor_cache.o \
	$(ONLINE_HOME)/src/SwtRY/monitor_server.o \

PRGTARG = SwtRY
#PRGLIBS = -lCust -lCommon -lDbsComm -lCstDbs -lcjson -lcjson_utils -lssl -lcrypto  -lbcrypt -lsoftasym -lsoftsym -lxlsxio_read -lxlsxio_write -lhiredis -lm $(NETLIBS) $(UXLIBS) $(DBLIBS)
PRGLIBS =  -lCommon -lDbsComm -lcjson -lcjson_utils -lbcrypt -lxlsxio_read -lxlsxio_write -lhiredis -lssl -lcrypto -lm $(NETLIBS) $(UXLIBS) $(DBLIBS)
PRGDEFS =

# used for db2 database
PRGDBNM =

debug all: debugexec
release: releaseexec

# DO NOT modify any code below!!!

releasedynamic debugdynamic releasestatic debugstatic releaseexec debugexec clean:
	@make -f $(ONLINE_HOME)/mak/mkstand.mak $@ TARGET="$(PRGTARG)" OBJS="$(PRGOBJS)" LIBS="$(PRGLIBS)" DEFS="$(PRGDEFS)" DBNM="$(PRGDBNM)"
