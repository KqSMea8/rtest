/*************************************************************************
 * File:	http_client_utils.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	Tue 26 Mar 2019 01:43:49 PM CST
 ************************************************************************/

#ifndef _CLIENT_HTTP_CLIENT_HTTP_UTILS_H_
#define _CLIENT_HTTP_CLIENT_HTTP_UTILS_H_

#include <map>
#include <curl/curl.h>
#include <string>
#include <utility>
#include <stdint.h>
#include "curl_handle.h"

using namespace std;
namespace runtofuServer{
    class HttpClientUtils{
    protected:
        static map <string, string> commonHeader;
        static map< uint8_t, bool > isTokenTable;

        //内部方法一些
        static void init();

        //规范化header的key
        static void formatHeaderKey(string &key);

        static bool validHeaderFieldByte(uint8_t c);

        //执行相关请求：成功返回0
        static int perform(CURL *c, HttpResponse &rsp);

    public:
        //删除libcurl里相关头部，其实就是一个单链表管理
        static void deleteCurlSlist(struct curl_slist **curl_headers, const char *data);

        //规范化header的key
        static void canonicalHeaderKey(string &key);

        //切割cookie信息
        static void splitRawCookie(const string &rawCookie, map <string, string> &cookie);

        //合并cookie
        static void joinRawCookie(const map <string, string> &cookie, string &rawCookie);

        //切割原始的头信息
        static void splitRawHeaders(const string &header, map <string, string> &rspHeader);

        //随机生成一个PC端的User-Agent
        static void genRandomPCUserAgent(string &ua);

        //随机生成一个mobile端的User-Agent
        static void genRandomMobileUserAgent(string &ua);

        //发起GET请求并返回数据
        static int Get(curlHandle &curl, HttpResponse &rsp);

        //直接发送原始数据【不带文件、没有KV对】
        static int PostRaw(curlHandle &curl, const string &data, HttpResponse &rsp);

        //发起POST请求并返回数据，有字段、上传文件，或者raw post用的
        //一般，raw post body 不会和上传文件、其他from表单信息同时出现
        static int Post(curlHandle &curl, HttpResponse &rsp);

        //发起head请求并返回数据
        static int Head(curlHandle &curl, HttpResponse &rsp);
    };
}
#endif
