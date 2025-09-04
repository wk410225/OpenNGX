/**
* dbconfig.c
* ���������ݿ���û������������
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
        	DBconnect_sh();   //�����޸�����see�ű�����
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
            printf("�˳�\n");
            return 0;
        default:
            printf("��ѡ����ȷ�Ĺ���!\n");
            getchar();
            break;
        }
    }


}



/**************************************************************
 ** ������      :  ShowMenu ()
 ** ��  ��      :  ��ѡ��˵���ʾ
***************************************************************/
int ShowMenu()
{
    int choice = -1;
    char choicebuf[30];

    system("clear");

    printf("*************************���ݿ����ò���*************************\n");
    printf("	��ѡ���ܣ�\n");
    printf("	[1]�������û�����\n");
    printf("	[2]ƽ̨���ݿ����Ӳ���\n");
    printf("	[0]�˳�\n");
    printf("****************************************************************\n");
    printf("��ѡ��");
    fgets(choicebuf,sizeof(choicebuf),stdin);
    choice = atoi(choicebuf);
    printf("_________________________________________________________________\n");
    return choice;
}

/**************************************************************
 ** ������      :  SetDBconfig ()
 ** ��  ��      :  �����������ݿ���û���������
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
        printf("���������ݿ��û�%s������(���ܳ���16λ):",sUsername);
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

    fprintf(stdout,"���ܺ����������[%s]\n",sEnpassword);

    if ((fp = fopen(sFile,"wb")) == NULL ) {
        fprintf(stdout,"S0000: ���ļ�[%s]ʧ��,errno=[%d](%s)\n",sFile,errno,strerror(errno));
        return -1;
    }

    memset(sBuff,0x00,sizeof(sBuff));
    sprintf(sBuff,"[CONFIG]\nDB_USER_PASSWORD=%s\n",sEnpassword);

    fwrite(sBuff,strlen(sBuff), 1,fp);
    fclose(fp);

    printf("�����������ݿ�%s������ɹ�!!\n",sUsername);
    printf("��������������......");
    getchar();
    return 0;

}
/**************************************************************
 ** ������      :  DBconnetct ()
 ** ��  ��      :  ���ݿ����Ӳ���
***************************************************************/
int DBconnect()
{
    char tmp[20];
    int iRet = -1;

    if ((iRet = DbsConnect()) != 0) {
        printf("�����ݿ�ʧ�ܣ�������ΪiRet[%d]\n",iRet);
        DbsDisconnect();
        printf("���������������......\n");
        getchar();
        return -1;
    }
    DbsDisconnect();
    printf("�����ݿ�ɹ�!\n");
    printf("���������������......\n");
    getchar();
}
/**************************************************************
 ** ������      :  DBconnect_sh()
 ** ��  ��      :  ���ݿ����Ӳ���,�ṩ��see�е��õ�
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
        printf("��ȡ���ݿ�����:��ȡ�������ĳ���,err");
        return -1;
    }
    sftDecDB(sEnPassword,sPassword);
    printf("%s\n",sPassword);
  	exit(0);
}
