ONLINE_HOME=$HOME/RY; export ONLINE_HOME
PATH=/usr/local/bin:$PATH;export PATH
PATH=$ONLINE_HOME/bin:$ONLINE_HOME/sbin:$PATH;export PATH
LD_LIBRARY_PATH=$ONLINE_HOME/lib:/usr/lib:/usr/local/lib64:/usr/local/lib  export LD_LIBRARY_PATH
ulimit -c unlimited
ulimit -s unlimited
# mysql 
DEFAULTHOST=127.0.0.1; export DEFAULTHOST
DEFAULTUSER=bil; export DEFAULTUSER
DEFAULTPASSWD=bil; export DEFAULTPASSWD
DEFAULTPORT=3306; export DEFAULTPORT
DEFAULTDB=ry; export DEFAULTDB
