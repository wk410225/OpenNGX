
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <mysql/mysql.h>
// MySQL连接信息配置
#define DB_HOST "localhost"
#define DB_USER "your_username"
#define DB_PASS "your_password"
#define DB_NAME "your_database"
// 将MySQL类型映射到C类型
const char* map_mysql_type_to_c(const char* mysql_type) {
    if (strstr(mysql_type, "int")) return "int";
    if (strstr(mysql_type, "tinyint(1)")) return "bool";
    if (strstr(mysql_type, "tinyint")) return "char";
    if (strstr(mysql_type, "smallint")) return "short";
    if (strstr(mysql_type, "bigint")) return "long long";
    if (strstr(mysql_type, "float")) return "float";
    if (strstr(mysql_type, "double") || strstr(mysql_type, "decimal")) return "double";
    if (strstr(mysql_type, "char") || strstr(mysql_type, "text")) return "char";
    if (strstr(mysql_type, "date") || strstr(mysql_type, "time") || strstr(mysql_type, "year")) return "char";
    if (strstr(mysql_type, "blob") || strstr(mysql_type, "binary")) return "unsigned char";
    return "void*"; // 默认类型
}

void str2lower(char *str) 
{
	int i ;
    for (i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}
// 生成结构体定义
void generate_struct_definition(MYSQL* conn, const char* table_name) {
    char query[256];
    MYSQL_RES* res;
    MYSQL_ROW row;
    
    // 获取表结构信息
    snprintf(query, sizeof(query), "DESCRIBE %s", table_name);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "查询失败: %s\n", mysql_error(conn));
        return;
    }
    
    res = mysql_store_result(conn);
    if (!res) {
        fprintf(stderr, "获取结果失败: %s\n", mysql_error(conn));
        return;
    }
    
    // 打印结构体开始
    printf("// 表 %s 对应的结构体定义\n", table_name);
    printf("typedef struct {\n");
    
    // 处理每一列
    while ((row = mysql_fetch_row(res))) {
        char* field_name = row[0];
		str2lower(field_name);
        const char* mysql_type = row[1];
        const char* c_type = map_mysql_type_to_c(mysql_type);
        
        // 处理字符串类型
        if (strcmp(c_type, "char") == 0) {
            // 尝试从类型中提取长度，如 varchar(255)
            int length = 256; // 默认长度
            const char* paren = strchr(mysql_type, '(');
            if (paren) {
                length = atoi(paren + 1) + 1; // +1 为终止符
                if (length < 1) length = 256;
            }
            //printf("    %s %s[%d]; // %s\n", c_type, field_name, length, mysql_type);
            printf("    %s %s[%d]; \n", c_type, field_name, length );
        } 
        // 处理其他类型
        else {
            //printf("    %s %s; // %s\n", c_type, field_name, mysql_type);
            printf("    %s %s; \n", c_type, field_name);
        }
    }
    
    // 打印结构体结束
    printf("} %s_def;\n\n", table_name);
    
    mysql_free_result(res);
}

int main(int argc, char** argv) {
    MYSQL* conn;
    MYSQL_RES* res;
    MYSQL_ROW row;
	char host[64];
    char user[64];
    char passwd[64];
    char db[64];
    int     port;
    memset(host, 0x00, sizeof(host));
    memset(user, 0x00, sizeof(user));
    memset(passwd, 0x00, sizeof(passwd));
    memset(db, 0x00, sizeof(db));

    strcpy(host, getenv("DEFAULTHOST"));
    strcpy(user, getenv("DEFAULTUSER"));
    strcpy(passwd, getenv("DEFAULTPASSWD"));
    strcpy(db, getenv("DEFAULTDB"));
    port = atoi(getenv("DEFAULTPORT"));

    
    // 初始化MySQL连接
    conn = mysql_init(NULL);
    if (!conn) {
        fprintf(stderr, "MySQL初始化失败\n");
        return 1;
    }
    
    // 连接到数据库
    if (!mysql_real_connect(conn, host, user, passwd, db, port, NULL, 0)) {
        fprintf(stderr, "连接失败: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    
#if 0
    // 获取数据库中所有表名
    if (mysql_query(conn, "SHOW TABLES")) {
        fprintf(stderr, "查询失败: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    
    res = mysql_store_result(conn);
    if (!res) {
        fprintf(stderr, "获取结果失败: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
#endif
    
    // 为每个表生成结构体定义
    printf("/**\n");
    printf(" * 自动生成的C结构体定义\n");
    printf(" * 根据数据库 %s 的表结构生成\n", DB_NAME);
    printf(" */\n\n");
    printf("#ifndef DB_STRUCTS_H\n");
    printf("#define DB_STRUCTS_H\n\n");
    printf("#include <stdbool.h>\n\n");
    
/*
    while ((row = mysql_fetch_row(res))) {
        generate_struct_definition(conn, row[0]);
    }
*/
    
    generate_struct_definition(conn, argv[1]);
    printf("#endif // DB_STRUCTS_H\n");
    
    //mysql_free_result(res);
    mysql_close(conn);
    
    return 0;
}
