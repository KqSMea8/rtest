/*************************************************************************
 * File:	mysql_client.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019年03月23日 星期六 18时08分17秒
 ************************************************************************/

#ifndef _CLIENT_MYSQL_CLIENT_H_
#define _CLIENT_MYSQL_CLIENT_H_

#include <iostream>
#include <vector>
#include <map>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <unistd.h>
#include <memory.h>
#include "../../utils/byte_buffer.h"

using namespace std;
namespace runtofuServer{

    class MySQLClient{
    public:
        //存储MySQL的连接账号信息
        typedef struct _mysql_conf{
            string host;            //连接地址
            unsigned int port;            //端口号，默认3306
            string user;            //用户名
            string passwd;          //密码
            string dbName;          //DB名称
            string charset;         //设置的字符编码，默认utf8
            size_t timeout;         //连接超时时间，单位秒，默认5秒
            bool autoCommit;        //是否自动提交，默认为true
            int maxIdleConns;       //允许最大空闲连接数，默认为2
            int maxOpenConns;       //最多允许打开多少个连接，默认0不限制
            size_t connMaxLiftTime; //连接的最大生存时间，默认0不限制，单位秒
        } MySQLConf;

        //提取单行的单个字段
        void fetchOne();

        //提取所有行的第一个字段的列表
        void fetchCols();

        //提取一行数据
        void fetchRow();

        //提取多行数据
        void fetchRows();

        //执行一条更新语句
        int execute();

        //删除一条数据，返回lastAffectedRows
        int deleteData();

        //写入一条数据，返回lastInsertId
        int insertData();

        //批量写入数据，返回影响行数
        int insertBatchData();

        //执行一条：INSERT INTO table (a,b,c) VALUES (1,2,3) ON DUPLICATE KEY UPDATE c=c+1 语句
        int insertUpdateData();

        //更新一条数据，返回lastAffectedRows
        int updateData();

        //执行一条select ... for update语句
        int fetchForUpdate();

        //开启事务
        int beginTransaction();

        //提交事务
        int commitTransaction();

        //回滚事务
        int rollBackTransaction();

        //执行ping命令
        int ping();

    private:
        void conn();

        //连接句柄
        MYSQL *mysql;
    };


} //namespacecpputils

#endif // _CLIENT_MYSQL_CLIENT_H_
