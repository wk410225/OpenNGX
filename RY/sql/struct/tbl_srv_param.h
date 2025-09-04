/**
 * 自动生成的C结构体定义
 * 根据数据库 your_database 的表结构生成
 */

#ifndef DB_STRUCTS_H
#define DB_STRUCTS_H

#include <stdbool.h>

// 表 tbl_srv_param 对应的结构体定义
typedef struct {
    int usage_key; 
    char srv_id[5]; 
    char param_usage[2]; 
    char param_index[3]; 
    char param_data[65]; 
    char param_dsp[65]; 
} tbl_srv_param_def;

#endif // DB_STRUCTS_H
