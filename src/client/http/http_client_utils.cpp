/*************************************************************************
 * File:	http_client_utils.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	Tue 26 Mar 2019 01:43:58 PM CST
 ************************************************************************/

#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "http_client_utils.h"
#include "../../utils/strutils.h"
#include "../../utils/some.h"

using namespace std;
namespace cpputils{
    static const uint8_t _tmpTokens[] = {
            '!', '#', '$', '%', '&', '\'', '*', '+', '-', '.', '0', '1', '2', '3', '4',
            '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
            'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'W', 'V', 'X', 'Y', 'Z', '^',
            '_', '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
            'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '|', '~',
    };
    static const char *_tmpCommonHeaders[] = {
            "Accept", "Accept-Charset", "Accept-Encoding", "Accept-Language", "Accept-Ranges", "Cache-Control", "Cc", "Connection", "Content-Id", "Content-Language",
            "Content-Length", "Content-Transfer-Encoding", "Content-Type", "Cookie", "Date", "Dkim-Signature", "Etag", "Expires", "From", "Host", "If-Modified-Since",
            "If-None-Match", "In-Reply-To", "Last-Modified", "Location", "Message-Id", "Mime-Version", "Pragma", "Received", "Return-Path", "Server", "Set-Cookie", "Subject",
            "To", "User-Agent", "Via", "X-Forwarded-For", "X-Imforwards", "X-Powered-By",
    };
    static const char *ffVersionFormat = "%02d.%d";
    static const int versionMin = 37;
    static const int versionMax = 70;
    static const char *chromeVersionFormat = "%02d.0.%04d.%03d";
    static const char *chromeUaFormat = "Mozilla/5.0 (%s) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/%s Safari/537.36";
    static const char *ffUaFormat = "Mozilla/5.0 (%s; rv:%s) Gecko/20100101 Firefox/%s";
    static const char *osVersion[] = {
            "Macintosh; Intel Mac OS X 10_10", "Windows NT 10.0", "Windows NT 5.1", "Windows NT 6.1; WOW64",
            "Windows NT 6.1; Win64; x64", "X11; Linux x86_64", "Macintosh; Intel Mac OS X 10_14_3"
    };
    static const char *androidFormat = "Mozilla/5.0 (Linux; Android %d.%d.%d; zh-cn) AppleWebKit/%03d.%d.%02d (KHTML, like Gecko) Version/%d.0 Mobile Safari/534.30";
    static const char *iosFormat = "Mozilla/5.0 (iPhone; CPU iPhone OS %d_%d_%d like Mac OS X) AppleWebKit/%03d.%d.%02d (KHTML, like Gecko) Version/%d.0 Mobile/15G77";
    map< uint8_t, bool > HttpClientUtils::isTokenTable;
    map <string, string> HttpClientUtils::commonHeader;

    //一些初始化操作
    void HttpClientUtils::init(){
        int i;
        if (isTokenTable.size() <= 0){
            size_t tokenLen = sizeof(_tmpTokens) / sizeof(uint8_t);
            for (i = 0; i < tokenLen; i++){
                isTokenTable[_tmpTokens[i]] = true;
            }
        }
        if (commonHeader.size() <= 0){
            size_t headerLen = sizeof(_tmpCommonHeaders) / sizeof(char *);
            for (i = 0; i < headerLen; i++){
                commonHeader[_tmpCommonHeaders[i]] = _tmpCommonHeaders[i];
            }
        }
    }

    //是否为合法字符
    bool HttpClientUtils::validHeaderFieldByte(uint8_t c){
        if ((int) c < 127 && isTokenTable.find(c) != isTokenTable.end()){
            return true;
        }
        return false;
    }

    //规范化header的key
    void HttpClientUtils::formatHeaderKey(string &key){
        size_t klen = key.size();
        size_t i;
        for (i = 0; i < klen; i++){
            uint8_t c = key[i];
            if (validHeaderFieldByte(c)){
                continue;
            }
            return;
        }
        uint8_t c;
        bool upper = true;
        for (i = 0; i < klen; i++){
            c = key[i];
            if (upper && c >= 'a' && c <= 'z'){
                c = toupper(c);
            }
            else if (!upper && c >= 'A' && c <= 'Z'){
                c = tolower(c);
            }
            key[i] = c;
            upper = (c == '-');
        }
    }

    //删除指定的头部信息
    void HttpClientUtils::deleteCurlSlist(struct curl_slist **curl_headers, const char *data){
        if (curl_headers == NULL || data == NULL){
            return;
        }
        struct curl_slist *node;
        struct curl_slist *head = *curl_headers;
        struct curl_slist *node_prev = NULL;
        struct curl_slist *node_del = NULL;
        const char *colon;
        size_t head_name_len;

        colon = strchr(data, ':');
        if (colon == NULL){
            return;
        }
        head_name_len = colon - data + 1;

        node = head;
        while (node != NULL){
            if (strncmp(node->data, data, head_name_len) == 0){
                node_del = node;
                node = node->next;
                if (node_prev != NULL){
                    node_prev->next = node_del->next;
                }
                else{
                    *curl_headers = node_del->next;
                }

                node_del->next = NULL;
                curl_slist_free_all(node_del);
                continue;
            }

            node_prev = node;
            node = node->next;
        }

        return;
    }

    //规范化header的key
    void HttpClientUtils::canonicalHeaderKey(string &key){
        init();
        bool upper = true;
        size_t i;
        for (i = 0; i < key.size(); i++){
            uint8_t c = key[i];
            if (!validHeaderFieldByte(c)){
                return;
            }
            if (upper && c >= 'a' && c <= 'z'){
                formatHeaderKey(key);
                return;
            }
            if (!upper && c >= 'A' && c <= 'Z'){
                formatHeaderKey(key);
                return;
            }
            upper = (c == '-');
        }
        return;
    }

    //切割cookie信息
    void HttpClientUtils::splitRawCookie(const string &rawCookie, map <string, string> &cookies){
        vector <string> kvs;
        StrUtils::strSplit(rawCookie, ';', kvs);
        if (kvs.size() <= 0){
            return;
        }
        int i;
        size_t pos;
        for (i = 0; i < kvs.size(); i++){
            string kv = kvs[i];
            pos = kv.find('=', 0);
            if (pos == string::npos){
                continue;
            }
            string k(kv, 0, pos);
            string v(kv, pos + 1, kv.size());
            StrUtils::trimSpace(k);
            StrUtils::trimSpace(v);
            cout << k << endl;
            if (k.size() <= 0){
                continue;
            }
            cookies[k] = v;
        }
    }

    //合并cookie
    void HttpClientUtils::joinRawCookie(const map <string, string> &cookie, string &rawCookie){
        map< string, string >::const_iterator iter;
        for (iter = cookie.begin(); iter != cookie.end(); iter++){
            rawCookie.append(iter->first).append("=").append(iter->second).append("; ");
        }
        StrUtils::trimSpace(rawCookie);
        StrUtils::trimChar(rawCookie, ';');
    }

    //切割原始的头信息
    void HttpClientUtils::splitRawHeaders(const string &header, map <string, string> &rspHeader){
        vector <string> sublist;
        StrUtils::strSplit(header, '\n', sublist);
        vector< string >::const_iterator iter;
        for (iter = sublist.begin(); iter != sublist.end(); iter++){
            string kv = *iter;
            StrUtils::trimSpace(kv);
            size_t pos = kv.find_first_of(":");
            if (pos == string::npos){
                continue;
            }
            string k = kv.substr(0, pos);
            StrUtils::trimSpace(k);
            if (k.size() <= 0){
                continue;
            }
            StrUtils::toLower(k);
            string v = kv.substr(pos + 1, kv.size());
            StrUtils::trimSpace(v);
            rspHeader.insert(pair< string, string >(k, v));
        }
    }

    //随机生成一个User-Agent
    void HttpClientUtils::genRandomPCUserAgent(string &ua){
        bool isChrome = true;
        int r1 = SomeUtils::getRandom();
        if (r1 % 2 == 0){
            isChrome = false;
        }
        int r2 = SomeUtils::getRandom();
        int majorVersion = versionMin + r2 % (versionMax - versionMin);
        char version[128] = {0};
        if (isChrome){
            int r3 = SomeUtils::getRandom();
            int v1 = 1000 + r3 % 8999;
            int r4 = SomeUtils::getRandom();
            int v2 = 100 + r4 % 899;
            sprintf(version, chromeVersionFormat, majorVersion, v1, v2);
        }
        else{
            int r5 = SomeUtils::getRandom();
            sprintf(version, ffVersionFormat, majorVersion, r5 % 10);
        }
        int osLen = sizeof(osVersion) / sizeof(char *);
        int r6 = SomeUtils::getRandom();
        const char *os = osVersion[r6 % osLen];
        char userAgentBuf[1024] = {0};
        if (isChrome){
            sprintf(userAgentBuf, chromeUaFormat, os, version);
        }
        else{
            sprintf(userAgentBuf, ffUaFormat, os, version, version);
        }
        ua = userAgentBuf;
    }

    //随机生成一个mobile端的User-Agent
    void HttpClientUtils::genRandomMobileUserAgent(string &ua){
        bool isAndroid = true;
        int r1 = SomeUtils::getRandom();
        if (r1 % 2 == 0){
            isAndroid = false;
        }
        int v1 = SomeUtils::getRandom();
        int v2 = SomeUtils::getRandom() % 100;
        int v3 = SomeUtils::getRandom() % 100;
        int v4 = SomeUtils::getRandom() % 100 + 531;
        int v5 = SomeUtils::getRandom() % 100;
        int v6 = SomeUtils::getRandom() % 100;
        const char *format;
        if (isAndroid){
            v1 = v1 % 4 + 5;
            format = androidFormat;
        }
        else{
            v1 = v1 % 4 + 9;
            format = iosFormat;
        }
        char buf[1024] = {0};
        sprintf(buf, format, v1, v2, v3, v4, v5, v6, v1);
        ua = buf;
    }

    //发起GET请求并返回数据
    int HttpClientUtils::Get(curlHandle &curl, HttpResponse &rsp){
        return perform(curl.curlGet(rsp), rsp);
    }

    //直接发送原始数据【不带文件、没有KV对】
    int HttpClientUtils::PostRaw(curlHandle &curl, const string &data, HttpResponse &rsp){
        return perform(curl.curlPostRaw(data, rsp), rsp);
    }

    //发起POST请求并返回数据，有字段、上传文件，或者raw post用的
    //一般，raw post body 不会和上传文件、其他from表单信息同时出现
    int HttpClientUtils::Post(curlHandle &curl, HttpResponse &rsp){
        return perform(curl.curlPost(rsp), rsp);
    }


    //发起head请求并返回数据
    int HttpClientUtils::Head(curlHandle &curl, HttpResponse &rsp){
        return perform(curl.curlHead(rsp), rsp);
    }

    //执行相关请求：成功返回0
    int HttpClientUtils::perform(CURL *c, HttpResponse &rsp){
        CURLcode res;
        res = curl_easy_perform(c);
        if (res != 0){
            cerr << "error occur....." << curl_easy_strerror(res) << endl;
            return res;
        }
        curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &(rsp.httpCode));
        splitRawHeaders(rsp.headerStr, rsp.headers);
        return 0;
    }
}
