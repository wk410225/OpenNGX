# OpenNGX
nginx作为web服务器，如果能操作数据库就更好了，基于此愿景 ,依然采用LINUX C语言实现若依管理系统的服务端，前端保持不变，该项目我叫她OpenNGX。 OpenNGX采用libevent openssl redis cjson  mysql xlsxio 多进程 消息队列通信等经典技术，高性能 低资源，适合轻量级的web管理系统后端实现。 其中自研封装的数据库操作，一键生成表的增删改查,JSON和表结构体的自动转换并自动操作数据库等技术使页面开发效率极高。
沟通交易+微信: sunflowerformyself
代码结构：
RuoYi-Vue3-master.zip   前端代码和官方保持一致，可以直接下载官方的
RY_server.zip  OpenNGX的若依后端代码，解压后目录如下：
.bash_profile  请用它，记得重启生效
~/thirdlib     运行依赖的三方库，用时需复制到~/RY/lib，也可以自己下载安装后，编译项目
~/log          日志目录
~/RY           
~/RY/src       代码文件
~/RY/include   头文件
~/RY/etc       配置文件，包括mysql的连接配置
~/RY/bin       编译好的可执行程序
~/RY/mak       makefile脚本
~/RY/sbin      脚本 包含启停服务，查看服务运行情况等命令
~/RY/sql       其中有三个sql文件，有两个是若依官方自带的，OpenNGX*.sql是项目运行所需的，都要入库

OpenNGX 两层架构，通讯层和业务处理层  通讯层采用libevent 处理收发报文，写入消息队列，不存在阻塞，故并发高。 业务处理层负责操作数据库，处理业务逻辑等 两层模块根据并发情况都可以开启多进程。

    
