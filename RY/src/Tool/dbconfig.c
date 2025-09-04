/**
* dbconfig.c
* 对连接数据库的用户名和密码加密
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "Common.h"

int
main(int argc, char *argv[])
{
    int choice;
    char tmp[2];
    int ch;
    while((ch=getopt(argc,argv,"c")) != -1)
    {
    	switch (ch) {
        case 'c':
        	DBconnect_sh();   //后续修改用于see脚本调用
        case ':':
        case '?':
        case 'h':
        default:
        	break;
        }
    }
    
    while (1) {
        choice = ShowMenu();
        switch (choice) {
        case 1:
            SetDBconfig();
            break;
        case 2:
            DBconnect();
            break;
        case 0:
            printf("退出\n");
            return 0;
        default:
            printf("请选择正确的功能!\n");
            getchar();
            break;
        }
    }


}



/**************************************************************
 ** 函数名      :  ShowMenu ()
 ** 功  能      :  将选择菜单显示
***************************************************************/
int ShowMenu()
{
    int choice = -1;
    char choicebuf[30];

    system("clear");

    printf("*************************数据库设置测试*************************\n");
    printf("	请选择功能：\n");
    printf("	[1]请输入用户密码\n");
    printf("	[2]平台数据库连接测试\n");
    printf("	[0]退出\n");
    printf("****************************************************************\n");
    printf("请选择：");
    fgets(choicebuf,sizeof(choicebuf),stdin);
    choice = atoi(choicebuf);
    printf("_________________________________________________________________\n");
    return choice;
}

/**************************************************************
 ** 函数名      :  SetDBconfig ()
 ** 功  能      :  设置连接数据库的用户名和密码
***************************************************************/
int SetDBconfig()
{
    char sUsername[128];
    char sPassword[128];
    char sEnpassword[128];
    char sFile[128];
    char sBuff[254];
    char buf[256];
    char tmp[128];
    size_t len;

    int iRet = -1;
    FILE *fp;

    memset(sUsername, 0, sizeof(sUsername));
    memset(sPassword, 0, sizeof(sPassword));
    memset(sEnpassword, 0, sizeof(sEnpassword));

    strcpy(sUsername,getenv("DBUSER"));
    do
    {
        printf("请输入数据库用户%s的密码(不能超过16位):",sUsername);
        fgets(sPassword,sizeof(sPassword),stdin);
    }
    while( strlen(sPassword) < 2 );
    sPassword[strlen(sPassword)-1] = '\x00';
    memset(sEnpassword, 0x00, sizeof(sEnpassword));
    len = strlen(sPassword);
    if (len >= 1 && sPassword[len - 1] == '\n')
        sPassword[len - 1] = '\0';

    sftEncDB(sPassword,sEnpassword);
    memset(sFile,0x00,sizeof(sFile));
    sprintf(sFile,"%s/ncup/config/dbconfig.ini",getenv("HOME"));

    fprintf(stdout,"加密后的密码密文[%s]\n",sEnpassword);

    if ((fp = fopen(sFile,"wb")) == NULL ) {
        fprintf(stdout,"S0000: 打开文件[%s]失败,errno=[%d](%s)\n",sFile,errno,strerror(errno));
        return -1;
    }

    memset(sBuff,0x00,sizeof(sBuff));
    sprintf(sBuff,"[CONFIG]\nDB_USER_PASSWORD=%s\n",sEnpassword);

    fwrite(sBuff,strlen(sBuff), 1,fp);
    fclose(fp);

    printf("设置连接数据库%s的密码成功!!\n",sUsername);
    printf("请键入任意键返回......");
    getchar();
    return 0;

}
/**************************************************************
 ** 函数名      :  DBconnetct ()
 ** 功  能      :  数据库连接测试
***************************************************************/
int DBconnect()
{
    char tmp[20];
    int iRet = -1;

    if ((iRet = DbsConnect()) != 0) {
        printf("打开数据库失败，错误码为iRet[%d]\n",iRet);
        DbsDisconnect();
        printf("按键入任意键返回......\n");
        getchar();
        return -1;
    }
    DbsDisconnect();
    printf("打开数据库成功!\n");
    printf("按键入任意键返回......\n");
    getchar();
}
/**************************************************************
 ** 函数名      :  DBconnect_sh()
 ** 功  能      :  数据库连接测试,提供给see中调用的
***************************************************************/
int DBconnect_sh()
{
    char tmp[20];
    int iRet = -1;
    char sFilePath[200];
    char sPassword[64];
    char sEnPassword[64];
    char softkey[64];

    memset(sFilePath,0x00,sizeof(sFilePath));
    memset(sPassword,0x00,sizeof(sPassword));
    memset(sEnPassword,0x00,sizeof(sEnPassword));
    memset(softkey,0x00,sizeof(softkey));

    strcat(sFilePath, getenv("FEHOME"));
    strcat(sFilePath, "/config/dbconfig.ini");
    if(GetProfileString("CONFIG","DB_USER_PASSWORD",sEnPassword,64,sFilePath) < 0)
    {
        printf("读取数据库配置:读取密码密文出错,err");
        return -1;
    }
    sftDecDB(sEnPassword,sPassword);
    printf("%s\n",sPassword);
  	exit(0);
}
