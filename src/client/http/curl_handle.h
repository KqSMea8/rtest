/*************************************************************************
 * File:	curl_handle.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	Sat 30 Mar 2019 04:42:27 PM CST
 ************************************************************************/

#ifndef _CLIENT_HTTP_CURL_HANDLE_H_
#define _CLIENT_HTTP_CURL_HANDLE_H_

#include <iostream>
#include <curl/curl.h>
#include <vector>
#include <map>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <unistd.h>
#include <memory.h>

using namespace std;
namespace rtest{
    class HttpResponse{
    public:
        string rspStr;
        string headerStr;
        map <string, string> headers;
        long httpCode;

        int getHeader(const string &k, string &v);

        void printHeaders();
    };

    class curlHandle{
    private:
        typedef struct _upload_file_list{
            string fieldName;//上传文件时的字段名称
            string filePath;//上传的文件的路径
            string fileName;//上传的文件名称，若无则直接从路径里截取
        } uploadFiles;

        struct curl_slist *headerList;
        struct curl_httppost *formPost;
        struct curl_httppost *formLast;

        string url;//要请求的url
        map <string, string> headers;//请求的头信息
        map <string, vector< string > > vals;//要post的KV键值对
        vector <uploadFiles> uploadFileList;//上传的文件列表
        uint32_t timeout;//超时，毫秒
        bool keepalive;//保持连接
        string proxy;//要设置的代理地址
        bool isDebug;

        //初始化请求信息：成功返回0
        int initCurlConf(HttpResponse &rsp);

        //初始化post的数据
        int initPostData();

        //初始化头信息
        void initHeaderList();

    public:
        //该实例唯一标识
        string uniqueId;

        //curl句柄
        CURL *curlFP;

        //重试次数
        uint32_t retry;

        curlHandle();

        ~curlHandle();

        void setDebug(bool b);

        //设置整体超时时间，单位毫秒，默认3秒
        curlHandle &setTimeout(uint32_t t);

        //设置重试次数，默认3次
        curlHandle &setRetryTimes(uint32_t t);

        //添加要上传的文件
        curlHandle &addFile(const string &fieldName, const string &filePath, const string &fileName);

        //添加单条header信息
        curlHandle &addHeader(const string &k, const string &v);

        //批量设置头信息
        curlHandle &addHeaders(const map <string, string> &hs);

        //设置要请求的host（设置header的相应值）
        curlHandle &setHost(const string &host);

        //设置URL
        curlHandle &setUrl(const string &url);

        //设置内容类型（设置header的相应值）
        curlHandle &setContentType(const string &ct);

        //设置长连接选项
        curlHandle &setKeepAlive(bool b);

        //设置代理用的地址和端口
        curlHandle &setProxy(const string &proxyHost);

        //设置userAgent（设置header的相应值）
        curlHandle &setUserAgent(const string &ua);

        //设置cookie（设置header的相应值）
        curlHandle &setRawCookie(const string &ck);

        //添加单个cookie的键值
        curlHandle &addCookie(const string &k, const string &v);

        //设置referer（设置header的相应值）
        curlHandle &setReferer(const string &referer);

        //设置HTTP Basic Authentication the provided username and password（设置header的相应值）
        curlHandle &setBasicAuth(const string &username, const string &password);

        //批量添加字段，一般是 PostForm 用，在即上传文件又有字段时Post也用
        curlHandle &setFields(const map <string, string> &data);

        //添加单个字段，POST用
        curlHandle &setField(const string &k, const string &v);

        //添加单个字段，可以添加数组，如 ：
        //client.AddField("a","1") 、client.AddField("a","2")
        //在接收的时候将收到 a=[1,2]
        curlHandle &addField(const string &k, const string &v);

        //获取请求的句柄
        CURL *curlGet(HttpResponse &rsp);

        //获取请求的句柄
        CURL *curlPostRaw(const string &rawData, HttpResponse &rsp);

        //获取请求的句柄
        CURL *curlPost(HttpResponse &rsp);

        //获取请求的句柄
        CURL *curlHead(HttpResponse &rsp);
    };
} //namespacecpputils

#endif // _CLIENT_HTTP_CURL_HANDLE_H_
