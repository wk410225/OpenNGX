INCLUDE=-I$(ONLINE_HOME)/include/ -I/usr/include/mysql
LIBS=-L$(ONLINE_HOME)/lib  -L/usr/lib64/mysql -lmysqlclient -pthread
mysql_c_api_test_src=myddl.c
mysql_c_stmt_test_src=myqry.c
mysql_c_qrysp_src=myexp.c
mysql_c_t2s_src=t2s.c
mysql_c_table2struct_src=table2struct.c

BIN: myqry myddl myexp  t2s table2struct

myqry:$(mysql_c_stmt_test_src)
	gcc -o $@ $^ -g -Wall $(LIBS) $(INCLUDE)
	cp $@ $(ONLINE_HOME)/bin
	chmod +x $@
myddl:$(mysql_c_api_test_src)
	gcc -o $@ $^ -g -Wall $(LIBS) $(INCLUDE)
	chmod +x $@
	cp $@ $(ONLINE_HOME)/bin
myexp:$(mysql_c_qrysp_src)
	gcc -o $@ $^ -g -Wall $(LIBS) $(INCLUDE)
	chmod +x $@
	cp $@ $(ONLINE_HOME)/bin
t2s:$(mysql_c_t2s_src)
	gcc -o $@ $^ -g -Wall $(LIBS) $(INCLUDE)
	chmod +x $@
	cp $@ $(ONLINE_HOME)/bin
table2struct:$(mysql_c_table2struct_src)
	gcc -o $@ $^ -g -Wall $(LIBS) $(INCLUDE)
	chmod +x $@
	cp $@ $(ONLINE_HOME)/bin
clean:
	rm -rf *.o
	rm -rf myqry
	rm -rf myexp
	rm -rf myddl
	rm -rf t2s
