#include "Switch.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <bcrypt/bcrypt.h>

extern redisContext    *c; // Redis连接上下文
extern redisReply      *reply; // Redis命令回复

/*
void get_keys_by_pattern( const char *pattern, int *count, char ***keys, int max_keys) 
{
	redisReply *reply;
	int cursor = 0;
	*count = 0;
	*keys = NULL;

	do {
		reply = redisCommand(c, "SCAN %d MATCH %s COUNT 100", cursor, pattern );

		if (reply == NULL || reply->type != REDIS_REPLY_ARRAY) {
			printf("SCAN command failed\n");
			if (reply) freeReplyObject(reply);
			return;
		}

		cursor = atoi(reply->element[0]->str);
		int keys_in_batch = reply->element[1]->elements;

		*keys = realloc(*keys, (*count + keys_in_batch) * sizeof(char *));

		for (int i = 0; i < keys_in_batch && *count < max_keys; i++) {
			(*keys)[*count] = strdup(reply->element[1]->element[i]->str);
			(*count)++;
		}

		freeReplyObject(reply);

	} while (cursor != 0 && *count < max_keys);
}

char* get_key_value(const char *key) 
{
	redisReply *reply = redisCommand(c, "GET %s", key);

	if (reply == NULL || reply->type != REDIS_REPLY_STRING) {
		if (reply) freeReplyObject(reply);
		return NULL;
	}

	char *value = strdup(reply->str);
	freeReplyObject(reply);
	return value;
}

*/
int monitor_online(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "monitor_online";
	char			sMsgSrcId[SRV_ID_LEN+1];
	char			sMethod[16];
	char			sUri[128];
	int				nRet;
	char			sSql[2048];
	char			where[256];
	char			set[2048];
	char			sTmpBuf[128];
	char			sClientIp[24];
	char			sUserAgent[1024];
	char			*IPC = sMsgBuf+ MSGHEADLEN;

	memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
	memcpy(sTmpBuf, IPC, 6);
	char			*json_string = sMsgBuf+MSGHEADLEN+6+atoi(sTmpBuf);
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s begin", sFuncName);
	showIpc(IPC);
	GetIpcValueExt(IPC, "$MSGSRCID", sMsgSrcId, sizeof(sMsgSrcId));
	GetIpcValueExt(IPC, "$METHOD", sMethod, sizeof(sMethod));
	GetIpcValueExt(IPC, "$PATH", sUri, sizeof(sUri));
	//GetIpcValueExt(IPC, "$CLIENT_IP", sClientIp, sizeof(sClientIp));
	//GetIpcValueExt(IPC, "User-Agent", sUserAgent, sizeof(sUserAgent));
	cJSON  *myself,  *root,  *body, *item, *array;
	char       *param= get_last_component(sUri);

	if (validate_token(IPC, myself) !=0)
	{
	}

	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s", json_string);
	if (strcmp(sMethod, "DELETE" ) == 0 ) //强退
	{
		reply = redisCommand(c, "DEL login_tokens:%s", param);
		freeReplyObject(reply);
		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string = %s", json_string);
	}
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(sUri, "/monitor/online/list") == 0)
	{
		root = cJSON_CreateObject();
		array = cJSON_CreateArray();
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		int count;
		char **keys;
		const char *pattern = "login_tokens:*";
		int max_keys = 100; // 限制最大数量

		get_keys_by_pattern(pattern, &count, &keys, max_keys);
		cJSON_AddNumberToObject(root, "total", count);

		int i ;
		for (i = 0; i < (count < 10 ? count : 10); i++) 
		{
			HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%d. %s", i + 1, keys[i]);
			char *value = NULL;
			value = get_key_value(keys[i]);
			if (value) 
			{
				HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, " -> %s", value);
				body = cJSON_Parse(value);
				item = cJSON_GetObjectItemCaseSensitive(body, "userName");
				nRet = GetIpcValueExt(IPC, "userName", sTmpBuf, sizeof(sTmpBuf));
				if (nRet > 0 && strcmp(item->valuestring, sTmpBuf) != 0)
					continue;
				item = cJSON_GetObjectItemCaseSensitive(body, "ipaddr");
				nRet = GetIpcValueExt(IPC, "ipaddr", sTmpBuf, sizeof(sTmpBuf));
				if (nRet > 0 && strcmp(item->valuestring, sTmpBuf) != 0)
					continue;
				cJSON_DeleteItemFromObject(body, "user");
				cJSON_AddItemToArray(array, body);
				free(value);
			} 
			else 
			{
				HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, " -> (nil)");
			}
			free(keys[i]);
		}
		free(keys);
		cJSON_AddItemToObject(root, "rows" ,array);

		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string = %s", json_string);
	}
//head=cJSON_PrintUnformatted(root);
	cJSON_Delete(root);
	SetIpcValue(sSql, "$HEADNUM", "0000");
    memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
    memcpy(sTmpBuf, sSql, 6);
    int nIpcLen = 6+atoi(sTmpBuf);

    memcpy(sMsgBuf+MSGHEADLEN, sSql, nIpcLen);
    strcpy(sMsgBuf+MSGHEADLEN+nIpcLen, json_string);
    int nMsgLen = strlen(json_string)+MSGHEADLEN+nIpcLen;
	free(json_string);
	((SYSHEAD *)sMsgBuf)->shMsgLen = nMsgLen;
	HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "sMsgBuf->shMsgLen = %d", ((SYSHEAD *)sMsgBuf)->shMsgLen);
	nRet= MsqSnd (sMsgSrcId, gatSrvMsq, 0, nMsgLen, sMsgBuf);
    if (nRet)
    {
        HtLog (gsLogFile, HT_LOG_MODE_ERROR, __FILE__,__LINE__, "MsqSnd error, %d. %d[%s] ", nRet,errno,strerror(errno));
        return -1;
    }
	HtLog(	gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s end", sFuncName);
	return 0;
}
