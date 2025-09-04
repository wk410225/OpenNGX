DBINCP = $(MYSQL_HOME)/include
DBLIBP = -L$(MYSQL_HOME)/lib 
DBLIBS = -lmysqlclient
DBD    = _DB_MYSQL
DBDEF  = -DMYSQL4 -D$(DBD)
DBTYPE = mysql
