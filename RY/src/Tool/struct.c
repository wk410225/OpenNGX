#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#define LOG_NAME_LEN_MAX	      64	
char				gsLogFile[LOG_NAME_LEN_MAX];
void str2upper(char *str) {
	int i;
    for (i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

int main(int argc, char *argv[])
{
    FILE	*fp = NULL;
    char 	filename[128];
    char 	struct_name[128];
    char 	sRecord[1024];
    char 	sTempBuf[1024];
    int		nfd = 0;
    char	sFdName[128][128];
    char	sFdType[128];
    char	*ptr = NULL;
    int		i;

    memset(filename, 0x00, sizeof(filename));
    memset(struct_name, 0x00, sizeof(struct_name));
    memset(&sFdName, 0x00, sizeof(sFdName));
    if(argc <= 1)
    {
        printf("没有文件不行的\n\n");
        return 0;
    }
    strcpy(filename, argv[1]);
    printf("Filename = %s\n", filename);
    fp = fopen(filename, "r");
	if (fp == NULL)
    {
        printf("open file %s error msg=%s", argv[1], strerror(errno));
        return -1;
    }
    memset(sRecord, 0x00, sizeof(sRecord));
    while (fgets(sRecord, 1024, fp) !=NULL)
    {
        ptr = strstr(sRecord, "}");
        if (ptr != NULL)
        {
            i = 0;
            ptr = ptr + 1;
            while (*ptr)
            {
                if (*ptr != 0x20 && *ptr != '\t' && *ptr != ';')
                {
                    struct_name[i++] = *ptr;
                }
                if (*ptr == ';')
                {
                    //printf("%s\n", struct_name);
                    break;
                }
                ptr++;
            }
            continue;
		}
        ptr = strstr(sRecord, "char ");
        if (ptr != NULL)
        {
			sFdType[nfd] = 'C';
            i = 0;
            ptr = ptr + 4;
            while (*ptr)
            {
                if (*ptr != ' ' && *ptr != '\t' && *ptr != '[')
                {
                    sFdName[nfd][i++] = *ptr;
                }
                if (*ptr == '[')
                {
                    //printf("%s\n", sFdName[nfd]);
                    break;
                }
                ptr++;
            }
            nfd++;
            continue;
        }
        ptr = strstr(sRecord, "int ");
        if (ptr != NULL)
        {
			sFdType[nfd] = 'I';
            i = 0;
            ptr = ptr + 3;
            while (*ptr)
            {
                if (*ptr != ' ' && *ptr != '\t' && *ptr != ';')
                {
                    sFdName[nfd][i++] = *ptr;
                }
                if (*ptr == ';')
                {
                    //printf("%s\n", sFdName[nfd]);
                    break;
                }
                ptr++;
            }
            nfd++;
            continue;
        }
        ptr = strstr(sRecord, "long ");
        if (ptr != NULL)
        {
			sFdType[nfd] = 'L';
            i = 0;
            i = 0;
            ptr = ptr + 4;
            while (*ptr)
            {
                if (*ptr != ' ' && *ptr != '\t' && *ptr != ';')
                {
                    sFdName[nfd][i++] = *ptr;
                }
                if (*ptr == ';')
                {
                    //printf("%s\n", sFdName[nfd]);
                    break;
                }
                ptr++;
            }
            nfd++;
            continue;
        }
        ptr = strstr(sRecord, "float ");
        if (ptr != NULL)
        {
			sFdType[nfd] = 'F';
            i = 0;
            ptr = ptr + 5;
            while (*ptr)
            {
                if (*ptr != ' ' && *ptr != '\t' && *ptr != ';')
                {
                    sFdName[nfd][i++] = *ptr;
                }
                if (*ptr == ';')
                {
                    //printf("%s\n", sFdName[nfd]);
                    break;
                }
                ptr++;
            }
            nfd++;
            continue;
        }
        ptr = strstr(sRecord, "double ");
        if (ptr != NULL)
        {
			sFdType[nfd] = 'D';
            i = 0;
            ptr = ptr + 6;
            while (*ptr)
            {
                if (*ptr != ' ' && *ptr != '\t' && *ptr != ';')
                {
                    sFdName[nfd][i++] = *ptr;
                }
                if (*ptr == ';')
                {
                    //printf("%s\n", sFdName[nfd]);
                    break;
                }
                ptr++;
            }
            nfd++;
            continue;
        }
        memset(sRecord, 0x00, sizeof(sRecord));
    }
    int loop;
    memset(sTempBuf, 0x00, sizeof(sTempBuf));
	strcpy(sTempBuf, struct_name);
	str2upper(struct_name);

	struct_name[strlen(struct_name)-4] = 0x00;
    printf("static TABFUN %s_COLMAP[MAXCOLNUM]={\n", struct_name);
	char buffer[128];
	for (loop = 0; loop < nfd; loop++)
    {
		
        memset(buffer, 0x00, sizeof(buffer));
		sprintf(buffer, "\"%s\",", sFdName[loop]);
        //printf("	{\"%s\", offsetof(%s, %-15s), '%c',    \"%-10s\", \"%-10s\", \"\"},\n", sFdName[loop], sTempBuf, sFdName[loop], sFdType[loop], sFdName[loop],sFdName[loop]);
        printf("	{%-15s offsetof(%s, %-15s), '%c', %-15s %-15s \"\"},\n", buffer, sTempBuf, sFdName[loop], sFdType[loop], buffer,buffer);
        //printf("%s\n", sTempBuf);

    }
    printf("};\n");
    fclose(fp);
    return 0;
}
