/*************************************************************************
 * File:	httpRequest.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-04 14:38
 ************************************************************************/
#ifndef _CPPUTILS_HTTPREQUEST_H_201904041438_
#define _CPPUTILS_HTTPREQUEST_H_201904041438_

#include<string>
#include<vector>
#include<string.h>
#include<map>
#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include "../utils/byte_buffer.h"

#define HTTP_HEADER_BUF_SIZE 10240
#define NORMAL_POST_CONTENT_TYPE "application/x-www-form-urlencoded"
#define UPLOAD_POST_CONTENT_TYPE "multipart/form-data"

namespace runtofuServer{
    class httpRequest{
    private:
        //解析 multipart/form-data 请求的POST数据
        void parseMultiPartFormDataArgs(const char *boundary);

        void parseArgs(const string &str);

    public:
        httpRequest();

        //初始化方法
        int parseBody(const void *body, size_t len);

        //初始化
        void reset();

        ~httpRequest();

        //请求方法类型
        typedef enum{
            GET = 1, POST, PUT, HEAD, DELETE
        } HTTP_METHOD;

        //上传的文件类型
        typedef struct _upload_file{
            string fileName;
            string contentType;
            char *fileBuf;
            size_t fileLen;
        } UPLOAD_FILE;

        //请求的唯一标识
        std::string uniqueId;

        //请求的头信息
        std::map <std::string, std::string> headers;

        //请求时带的参数
        std::map <std::string, std::vector< std::string >> args;

        //原始的请求信息
        char *rawBody;
        size_t rawBodyLen;

        //请求时的host
        std::string host;

        //请求时的uri
        std::string uri;

        //请求时用的协议，如"HTTP/1.1"
        std::string protocol;

        //请求时的方法
        HTTP_METHOD method;

        //上传的文件列表
        std::map <std::string, UPLOAD_FILE> uploadFiles;

        //提取header信息
        void getHeader(const std::string &key, std::string &val);

        //提取请求时的参数
        void getArg(const std::string &key, std::vector <ByteBuffer> &vals);

        //提取cookie信息
        void getCookie(const std::string &key, std::string &val);

        //校验请求是否合法
        bool checkValid();

        //是否为ajax请求
        bool isAjax();

        //打印解析出来的请求信息
        void printReq();
    };
} //namespace runtofuServer


#endif //_CPPUTILS_HTTPREQUEST_H_201904041438_
