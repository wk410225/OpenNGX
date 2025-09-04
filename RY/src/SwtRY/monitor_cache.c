#include "Switch.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <bcrypt/bcrypt.h>
#include "multipart.h"
#include "xlsxio_read.h"
#include "xlsxio_write.h"
static struct tbl_common_def 
{
	long    page_num;
	long	page_size;
    long    total_rows;
	char	sort[128];
}ptTblCommon;

static TABFUN TBL_COMMON_COLMAP[MAXCOLNUM]={
    {"total_rows",    offsetof(struct tbl_common_def, total_rows), 'I', "", "total", ""},
};

typedef struct 
{
	char sKey[64];
	char sDsp[64];
}st_data_key_dsp;

int monitor_cache(char *sMsgBuf, int nIndex )
{
	char			sFuncName[] = "monitor_cache";
	char			sMsgSrcId[SRV_ID_LEN+1];
	char			sMethod[16];
	char			sUri[128];
	int				nRet;
	char			sSql[2048];
	char			where[256];
	char			set[2048];
	char			sTmpBuf[128];
	char			*IPC = sMsgBuf+ MSGHEADLEN;

	memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
	memcpy(sTmpBuf, IPC, 6);
	char			*json_string = sMsgBuf+MSGHEADLEN+6+atoi(sTmpBuf);
	HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%s begin", sFuncName);
	showIpc(IPC);
	GetIpcValueExt(IPC, "$MSGSRCID", sMsgSrcId, sizeof(sMsgSrcId));
	GetIpcValueExt(IPC, "$METHOD", sMethod, sizeof(sMethod));
	GetIpcValueExt(IPC, "$PATH", sUri, sizeof(sUri));
	cJSON      *root,  *body, *item, *array;
	char       *param= get_last_component(sUri);

	int         user_id;
    if (validate_jwt_token(IPC, &user_id) != 0 )
    {
        root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "msg", "jwt验证不过");
        cJSON_AddNumberToObject(root, "code", 401);
        json_string = cJSON_Print(root);
        memset(sMethod, 0x00, sizeof(sMethod));
    }
	if (strcmp(sMethod, "GET" ) == 0  && memcmp(sUri, "/monitor/cache/clearCacheName/", strlen("/monitor/cache/clearCacheName/")) == 0) //删除 
	{
		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string = %s", json_string);
	}
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(sUri, "/monitor/cache/clearCacheAll") == 0) //all delete
	{
		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string = %s", json_string);
	}
	else if (strcmp(sMethod, "GET" ) == 0  && memcmp(sUri, "/monitor/cache/getValue/", strlen("/monitor/cache/getValue/")) == 0) 
	{
		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		memset(sTmpBuf, 0x00, sizeof(sTmpBuf));
		strcpy(sTmpBuf, param);
		char *value = get_key_value(param);
		char *last = get_last(param, ':');
		char *first = get_last(sTmpBuf, ':');
		*first  = 0x00;

		item = cJSON_CreateObject();
		cJSON_AddStringToObject(item, "cacheName", sTmpBuf);
		cJSON_AddStringToObject(item, "cacheKey", last);
		cJSON_AddStringToObject(item, "cacheValue", value);
		cJSON_AddItemToObject(root, "data", item);
		free(value);
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string = %s", json_string);
	}
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(sUri, "/monitor/cache") == 0)  //redis信息
	{
		root = cJSON_Parse("{\"msg\":\"操作成功\",\"code\":200,\"data\":{\"commandStats\":[{\"name\":\"keys\",\"value\":\"322\"},{\"name\":\"type\",\"value\":\"744\"},{\"name\":\"get\",\"value\":\"954\"},{\"name\":\"ping\",\"value\":\"856\"},{\"name\":\"scan\",\"value\":\"144\"},{\"name\":\"command\",\"value\":\"6\"},{\"name\":\"dbsize\",\"value\":\"3\"},{\"name\":\"exists\",\"value\":\"57\"},{\"name\":\"hello\",\"value\":\"1\"},{\"name\":\"setex\",\"value\":\"9\"},{\"name\":\"config\",\"value\":\"4\"},{\"name\":\"set\",\"value\":\"113\"},{\"name\":\"select\",\"value\":\"321\"},{\"name\":\"expire\",\"value\":\"422\"},{\"name\":\"ttl\",\"value\":\"386\"},{\"name\":\"info\",\"value\":\"10\"},{\"name\":\"del\",\"value\":\"6\"},{\"name\":\"hlen\",\"value\":\"10\"}],\"info\":{\"io_threaded_reads_processed\":\"0\",\"tracking_clients\":\"0\",\"uptime_in_seconds\":\"147464\",\"cluster_connections\":\"0\",\"current_cow_size\":\"0\",\"maxmemory_human\":\"0B\",\"aof_last_cow_size\":\"0\",\"master_replid2\":\"0000000000000000000000000000000000000000\",\"mem_replication_backlog\":\"0\",\"aof_rewrite_scheduled\":\"0\",\"total_net_input_bytes\":\"311196\",\"rss_overhead_ratio\":\"1.68\",\"hz\":\"10\",\"current_cow_size_age\":\"0\",\"redis_build_id\":\"5e7d2a5c1d1fb236\",\"aof_last_bgrewrite_status\":\"ok\",\"multiplexing_api\":\"epoll\",\"client_recent_max_output_buffer\":\"0\",\"allocator_resident\":\"3801088\",\"mem_fragmentation_bytes\":\"5463496\",\"repl_backlog_first_byte_offset\":\"0\",\"tracking_total_prefixes\":\"0\",\"redis_mode\":\"standalone\",\"redis_git_dirty\":\"0\",\"allocator_rss_bytes\":\"2367488\",\"repl_backlog_histlen\":\"0\",\"io_threads_active\":\"0\",\"rss_overhead_bytes\":\"2592768\",\"total_system_memory\":\"1826521088\",\"loading\":\"0\",\"evicted_keys\":\"0\",\"maxclients\":\"10000\",\"cluster_enabled\":\"0\",\"redis_version\":\"6.2.17\",\"repl_backlog_active\":\"0\",\"mem_aof_buffer\":\"0\",\"allocator_frag_bytes\":\"371776\",\"io_threaded_writes_processed\":\"0\",\"instantaneous_ops_per_sec\":\"0\",\"used_memory_human\":\"910.32K\",\"total_error_replies\":\"7\",\"role\":\"master\",\"maxmemory\":\"0\",\"used_memory_lua\":\"30720\",\"rdb_current_bgsave_time_sec\":\"-1\",\"used_memory_startup\":\"812056\",\"used_cpu_sys_main_thread\":\"397.331205\",\"lazyfree_pending_objects\":\"0\",\"used_memory_dataset_perc\":\"47.78%\",\"allocator_frag_ratio\":\"1.35\",\"arch_bits\":\"64\",\"used_cpu_user_main_thread\":\"796.290565\",\"mem_clients_normal\":\"61496\",\"expired_time_cap_reached_count\":\"0\",\"unexpected_error_replies\":\"0\",\"mem_fragmentation_ratio\":\"6.87\",\"aof_last_rewrite_time_sec\":\"-1\",\"master_replid\":\"d5a71e695dfe6add026d9cfec2b9ae2dc1b9a317\",\"aof_rewrite_in_progress\":\"0\",\"config_file\":\"/etc/redis/redis.conf\",\"lru_clock\":\"11350006\",\"maxmemory_policy\":\"noeviction\",\"run_id\":\"7c709753f5abf9f8536cd2f783c25c1a31349a03\",\"latest_fork_usec\":\"3826\",\"tracking_total_items\":\"0\",\"total_commands_processed\":\"4367\",\"expired_keys\":\"90\",\"errorstat_ERR\":\"count=5\",\"used_memory\":\"932168\",\"module_fork_in_progress\":\"0\",\"dump_payload_sanitizations\":\"0\",\"mem_clients_slaves\":\"0\",\"keyspace_misses\":\"82\",\"server_time_usec\":\"1756180470481480\",\"executable\":\"/usr/bin/redis-server\",\"lazyfreed_objects\":\"0\",\"db0\":\"keys=23,expires=1,avg_ttl=334250\",\"used_memory_peak_human\":\"987.66K\",\"keyspace_hits\":\"2069\",\"rdb_last_cow_size\":\"483328\",\"used_memory_overhead\":\"874784\",\"active_defrag_hits\":\"0\",\"tcp_port\":\"6379\",\"uptime_in_days\":\"1\",\"used_memory_peak_perc\":\"92.17%\",\"current_save_keys_processed\":\"0\",\"blocked_clients\":\"0\",\"total_reads_processed\":\"4861\",\"expire_cycle_cpu_milliseconds\":\"84553\",\"sync_partial_err\":\"0\",\"used_memory_scripts_human\":\"0B\",\"aof_current_rewrite_time_sec\":\"-1\",\"aof_enabled\":\"0\",\"process_supervised\":\"systemd\",\"master_repl_offset\":\"0\",\"used_memory_dataset\":\"57384\",\"used_cpu_user\":\"796.571128\",\"rdb_last_bgsave_status\":\"ok\",\"tracking_total_keys\":\"0\",\"atomicvar_api\":\"c11-builtin\",\"allocator_rss_ratio\":\"2.65\",\"client_recent_max_input_buffer\":\"24\",\"clients_in_timeout_table\":\"0\",\"aof_last_write_status\":\"ok\",\"errorstat_WRONGTYPE\":\"count=2\",\"mem_allocator\":\"jemalloc-5.1.0\",\"used_memory_scripts\":\"0\",\"used_memory_peak\":\"1011360\",\"process_id\":\"787\",\"master_failover_state\":\"no-failover\",\"used_cpu_sys\":\"397.481232\",\"repl_backlog_size\":\"1048576\",\"connected_slaves\":\"0\",\"current_save_keys_total\":\"0\",\"gcc_version\":\"11.5.0\",\"total_system_memory_human\":\"1.70G\",\"sync_full\":\"0\",\"connected_clients\":\"3\",\"monotonic_clock\":\"POSIX clock_gettime\",\"module_fork_last_cow_size\":\"0\",\"total_writes_processed\":\"4368\",\"allocator_active\":\"1433600\",\"total_net_output_bytes\":\"1158444\",\"pubsub_channels\":\"0\",\"current_fork_perc\":\"0.00\",\"active_defrag_key_hits\":\"0\",\"rdb_changes_since_last_save\":\"3\",\"instantaneous_input_kbps\":\"0.04\",\"used_memory_rss_human\":\"6.10M\",\"configured_hz\":\"10\",\"expired_stale_perc\":\"0.00\",\"active_defrag_misses\":\"0\",\"used_cpu_sys_children\":\"0.214693\",\"number_of_cached_scripts\":\"0\",\"sync_partial_ok\":\"0\",\"used_memory_lua_human\":\"30.00K\",\"rdb_last_save_time\":\"1756177374\",\"pubsub_patterns\":\"0\",\"slave_expires_tracked_keys\":\"0\",\"redis_git_sha1\":\"00000000\",\"used_memory_rss\":\"6393856\",\"rdb_last_bgsave_time_sec\":\"0\",\"os\":\"Linux 5.14.0-503.14.1.el9_5.x86_64 x86_64\",\"mem_not_counted_for_evict\":\"0\",\"active_defrag_running\":\"0\",\"rejected_connections\":\"0\",\"total_forks\":\"32\",\"active_defrag_key_misses\":\"0\",\"allocator_allocated\":\"1061824\",\"instantaneous_output_kbps\":\"0.71\",\"second_repl_offset\":\"-1\",\"rdb_bgsave_in_progress\":\"0\",\"used_cpu_user_children\":\"0.296010\",\"total_connections_received\":\"495\",\"migrate_cached_sockets\":\"0\"},\"dbSize\":23}}");
		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string = %s", json_string);
	}
	else if (strcmp(sMethod, "GET" ) == 0  && memcmp(sUri, "/monitor/cache/getKeys/", strlen("/monitor/cache/getKeys/")) == 0) 
	{
		root = cJSON_CreateObject();
		array = cJSON_CreateArray();
		cJSON_AddStringToObject(root, "msg", "查询成功");
		cJSON_AddNumberToObject(root, "code", 200);
		int count;
		char **keys;
		//const char *pattern = "login_tokens:*";
		int max_keys = 100; // 限制最大数量
		sprintf(sTmpBuf, "%s*", param);

		get_keys_by_pattern(sTmpBuf, &count, &keys, max_keys);

		int i ;
		for (i = 0; i < (count < 10 ? count : 10); i++)
		{
			HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, "%d. %s", i + 1, keys[i]);
			char *value = NULL;
			value = get_key_value(keys[i]);
			if (value)
			{
				cJSON_AddItemToArray(array, cJSON_CreateString(keys[i]));
				free(value);
			}
			else
			{
				HtLog (gsLogFile, HT_LOG_MODE_NORMAL, __FILE__,__LINE__, " -> (nil)");
			}
			free(keys[i]);
		}
		free(keys);
		cJSON_AddItemToObject(root, "data" ,array);

		json_string = cJSON_Print(root);
		HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string = %s", json_string);
	}
	else if (strcmp(sMethod, "GET" ) == 0  && strcmp(sUri, "/monitor/cache/getNames") == 0) 
	{
		st_data_key_dsp redis_data[32]={
			{"login_tokens:", "用户信息"},
			{"sys_config:", "配置信息"},
			{"sys_dict:", "数据字典"},
			{"captcha_codes:", "验证码"},
			{"repeat_submit:", "防重提交"},
			{"rate_limit:", "限流处理"},
			{"pwd_err_cnt:", "密码错误次数"},
		};
		root = cJSON_CreateObject();
		array= cJSON_CreateArray();
        cJSON_AddStringToObject(root, "msg", "操作成功");
        cJSON_AddNumberToObject(root, "code", 200);
        //json_string = cJSON_Print(root);
		int i = 0;
		while (redis_data[i].sKey[0] != 0x00)
		{
			item = cJSON_CreateObject();
			cJSON_AddStringToObject(item, "cacheName", redis_data[i].sKey);
			cJSON_AddStringToObject(item, "cacheKey", "");
			cJSON_AddStringToObject(item, "cacheValue", "");
			cJSON_AddStringToObject(item, "remark", redis_data[i].sDsp);
			cJSON_AddItemToArray(array, item);
			i++;
		}
		cJSON_AddItemToObject(root,"data",array);
        json_string = cJSON_Print(root);
        HtLog(gsLogFile, HT_LOG_MODE_DEBUG, __FILE__,__LINE__, "json_string[%s]", json_string);
	}
	
	//head=cJSON_PrintUnformatted(root);
	cJSON_Delete(root);
	SetIpcValue(sSql, "$HEADNUM", "0");
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
