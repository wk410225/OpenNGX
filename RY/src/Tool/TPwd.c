#include <stdio.h>
#include <stdlib.h>
//#include <curses.h>
#include <string.h>

#define SADBSUserPwdFile "TL_DBPWD_FILE"

int main(int argc,char **argv)
{
	char	sPutPwd[100], sNewPwd[100];
	char	sFileName[128];
	FILE    *fp;
	int		i;
	int     flag=0;/*��������־��0-��ȷ;1-����*/
	if(argc != 1)
	{
		printf ("Usage:%s\n", argv[0]);
		exit (-1);
	}
	memset(sPutPwd,0,sizeof(sPutPwd));
	memset(sNewPwd,0,sizeof(sNewPwd));
	memset(sFileName,0,sizeof(sFileName));
	printf("����������[a-z,0-9��'_']:\n");
	initscr();
	noecho();
	scanf("%s",sPutPwd);
	endwin();
	if(strlen(sPutPwd)>16)
	{
		printf("�������! ���볤�����16λ\n");
		return -1;
	}
	for(i=0;i<strlen(sPutPwd);i++)
	{
		if((sPutPwd[i]<'a' || sPutPwd[i]>'z') &&
		   (sPutPwd[i]<'0' || sPutPwd[i]>'9') &&
		   (sPutPwd[i] != '_'))
		{
			printf("�������! ������Сд��ĸ[a-z,0-9]\n");
			return;
		}
		if(sPutPwd[i]>='a' && sPutPwd[i]<='z')
			sNewPwd[i] = sPutPwd[i] - 47;
		else if(sPutPwd[i]>='0' && sPutPwd[i]<='9')
			sNewPwd[i] = 'a' - sPutPwd[i];
        else
			sNewPwd[i] = sPutPwd[i];

	}

	printf("newpin is %s\n",sNewPwd);
	sprintf(sFileName, "%s", getenv(SADBSUserPwdFile));

	fp = fopen (sFileName, "w");
	if(fp == NULL)
	{
		printf("open file[%s] error\n", sFileName);
		return -1;
	}

	if((fwrite(sNewPwd,i,1,fp)) == 0)
	{
		fclose(fp);
		printf("fwrite file[%s] error\n", sFileName);
		return -1;
	}

	fclose(fp);
	return 0;
}

