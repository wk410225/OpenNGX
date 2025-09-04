# OpenNGX
<br>第一 ：  nginx作为web服务器，如果能操作数据库就更好了，基于此愿景 ,依然采用LINUX C语言实现若依管理系统的服务端，前端保持不变，该框架我叫她OpenNGX。
<br>第二 ：  该框架采用libevent、 openssl、 redis 、cjson、 mysql、xlsxio、异步多进程、 消息队列通信等经典技术，高性能、低资源，适合开发轻量级、硬件资源少的后端系统，比如web管理系统、IM即时通讯。
<br>第三 ：  自研封装数据库操作：包括一键生成表的增删改查; JSON串和表结构体的自动互转； 自动生成sql语句等组件 使快速开发如虎添翼。
<br>第四 ：  OpenNGX 采用两层架构：通讯层和业务处理层；通讯层采用libevent 收发报文和读写消息队列，不存在阻塞，支持万级并发；业务处理层实现业务逻辑包括操作数据库、权限检查、redis交互、JWT处理、xlsx文件处理、bcrypt密码处理等， 两层模块根据具体业务场景、硬件资源等均可以开启多进程，并有守护进程Deamon管理，异常退出可重新拉起进程。
<br>第五 ：  实测性能：每次页面操作耗时约20毫秒。
<br>第六 ：  沟通交流微信: sunflowerformyself
<br>代码结构:
<br>一： RuoYi-Vue3-master.zip   前端代码和官方保持一致，也可从官网下载
<br>二： RY_server.zip           用OpenNGX实现的若依后端代码，解压后目录如下
<br>1： $HOME/.bash_profile  请用它，记得重启生效
<br>2： $HOME/thirdlib     运行服务又不想下载依赖库，可以将其复制到~/RY/lib
<br>3： $HOME/log          日志目录
<br>4： $HOME/RY/src       代码文件 
<br>5： $HOME/RY/include   头文件
<br>6： $HOME/RY/etc       配置文件，包括mysql的连接配置
<br>7： $HOME/RY/bin       编译好的可执行程序
<br>8： $HOME/RY/mak       makefile脚本
<br>9： $HOME/RY/sbin      脚本 包含启停服务，查看服务运行情况等命令
<br>10：$HOME/RY/sql       其中有三个sql文件，有两个是若依官方自带的，OpenNGX*.sql是项目运行所需的，三个都要入库
<br>11：$HOME/RY/lib       编译好的动态库

<br>HTTP    服务端口28093
<br>redis   必须是127.0.0.1 默认端口
<br>rundm   启动服务
<br>stopall 停服务 
<br>see     查看服务

    
