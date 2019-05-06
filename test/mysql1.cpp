/*************************************************************************
 * File:	mysql1.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-02 14:07
 ************************************************************************/
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdint.h>
#include <mysql/mysql.h>

using namespace std;

int main(){
    const char *host = "127.0.0.1";
    const char *user = "liuyongshuai";
    const char *passwd = "123456";
    const char *db = "db_runtofu";
    MYSQL mysql;
    mysql_init(&mysql);
    if (!mysql_real_connect(&mysql, host, user, passwd, db, 3306, NULL, 0)){
        fprintf(stderr, "connect MySQL failed:%s\n", mysql_error(&mysql));
    }
    mysql_set_character_set(&mysql, "utf8");
    char *sql = "select * from `tag`";
    if (mysql_real_query(&mysql, sql, strlen(sql)) != 0){
        fprintf(stderr, "%s\n", mysql_error(&mysql));
    }
    MYSQL_RES *list = mysql_store_result(&mysql);
    my_ulonglong field_num = mysql_field_count(&mysql);
    my_ulonglong num_rows = mysql_num_rows(list);
    fprintf(stderr, "mysql_field_count()=%lu\n", field_num);
    fprintf(stderr, "mysql_num_rows()=%lu\n", num_rows);

    MYSQL_FIELD *field_list = mysql_fetch_fields(list);
    MYSQL_ROW row;
    int i = 0, j = 0;
    for (; i < field_num; i++){
        fprintf(stderr, "field:%d=%s\n", i, field_list[i].name);
    }
    fprintf(stderr, "\n\n\n");
    for (j = 0; j < num_rows; j++){
        row = mysql_fetch_row(list);
        for (i = 0; i < field_num; i++){
            fprintf(stderr, "%s=%s\n", field_list[i].name, row[i]);
        }
        fprintf(stderr, "\n\n\n");
    }
    mysql_free_result(list);
}