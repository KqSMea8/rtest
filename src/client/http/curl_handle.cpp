/*************************************************************************
 * File:	curl_handle.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	Sat 30 Mar 2019 04:42:32 PM CST
 ************************************************************************/

#include <iostream>
#include "curl_handle.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "../../utils/some.h"
#include "../../utils/regexp.h"
#include "../../utils/strutils.h"
#include "http_client_utils.h"
#include "../../utils/color.h"
#include "../../utils/terminal_table.h"

#define HTTP_CLIENT_HTTP_BUF_SIZE 1024

using namespace std;

namespace cpputils{
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    static bool isInitGlobalCurl = false;

    //请求返回时的回调函数
    static size_t receive_data_callback(void *contents, size_t size, size_t nmemb, void *stream){
        string *str = (string *) stream;
        size_t i;
        size_t l = size * nmemb;
        char *cnt = (char *) contents;
        str->append(cnt);
        return l;
    }

    //信息记录header
    static size_t header_callback(char *contents, size_t size, size_t nmemb, void *stream){
        string *str = (string *) stream;
        size_t l = size * nmemb;
        char *cnt = (char *) contents;
        str->append(cnt);
        return l;
    }

    int HttpResponse::getHeader(const string &k, string &v){
        if (headerStr.size() > 0 && headers.size() <= 0){
            HttpClientUtils::splitRawHeaders(this->headerStr, this->headers);
        }
        map< string, string >::iterator iter;
        if ((iter = headers.find(k)) == headers.end()){
            return -1;
        }
        v = iter->second;
        return 0;
    }

    void HttpResponse::printHeaders(){
        if (headerStr.size() > 0 && headers.size() <= 0){
            HttpClientUtils::splitRawHeaders(this->headerStr, this->headers);
        }
        map< string, string >::const_iterator iter;
        string sepline;
        cout << endl;

        TerminalTable t;
        t.addHeadData(2, "key", "value");
        Color::Green("------------------", 0, 0, sepline);
        string start, end;
        Color::Red(" start print header ", 0, 0, start);
        Color::Red(" end print header ", 0, 0, end);
        cout << sepline << start << sepline << endl;
        for (iter = headers.begin(); iter != headers.end(); iter++){
            t.addRowData(2, iter->first.c_str(), iter->second.c_str());
        }
        string tstr;
        t.renderToString(tstr);
        cout << tstr;
        cout << sepline << end << sepline << endl;
        cout << endl;
    }

    curlHandle::curlHandle(){
        //只需要初始化一次即可
        if (!isInitGlobalCurl){
            pthread_mutex_lock(&mutex);
            if (isInitGlobalCurl){
                pthread_mutex_unlock(&mutex);
            }
            else{
                CURLcode code = curl_global_init(CURL_GLOBAL_ALL);
                if (code != 0){
                    const char *errmsg = curl_easy_strerror(code);
                    char buf[1024] = {0};
                    sprintf(buf, "curl_global_init failed,errmsg=%s", errmsg);
                    pthread_mutex_unlock(&mutex);
                    return;
                }
                isInitGlobalCurl = true;
                pthread_mutex_unlock(&mutex);
            }
        }

        this->headerList = NULL;
        this->isDebug = false;
        this->curlFP = NULL;
        this->formPost = NULL;
        this->formLast = NULL;
        this->timeout = 1000;//默认1秒
        this->retry = 1;//默认只试一次
        this->keepalive = false;//默认不用长链接

        //唯一标识设定
        string randomStr;
        SomeUtils::getDeviceRandom(randomStr, 30);
        uint64_t curNanoT;
        SomeUtils::getNanoSeconds(curNanoT);
        char hBuf[512] = {0};
        sprintf(hBuf, "%lx", curNanoT);
        this->uniqueId = hBuf + randomStr;
    }

    //设置debug模式
    void curlHandle::setDebug(bool b){
        isDebug = b;
    }

    //初始化头信息
    void curlHandle::initHeaderList(){
        if (this->headerList != NULL){
            curl_slist_free_all(this->headerList);
        }
        string ua;
        HttpClientUtils::genRandomPCUserAgent(ua);
        char uaBuf[HTTP_CLIENT_HTTP_BUF_SIZE] = {0};
        sprintf(uaBuf, "User-Agent:%s", ua.c_str());
        this->headerList = curl_slist_append(NULL, uaBuf);
    }

    //设置整体超时时间，单位毫秒，默认3秒
    curlHandle &curlHandle::setTimeout(uint32_t t){
        if (t > 0){
            this->timeout = t;
        }
        return *this;
    }

    //设置重试次数，默认3次
    curlHandle &curlHandle::setRetryTimes(uint32_t t){
        if (t > 0){
            this->retry = t;
        }
        return *this;
    }

    //添加要上传的文件
    curlHandle &curlHandle::addFile(const string &fieldName, const string &filePath, const string &fileName){
        if (!SomeUtils::isFileExist(filePath)){
            fprintf(stderr, "file[%s] not exist\n", filePath.c_str());
            return *this;
        }
        if (fieldName.size() <= 0){
            fprintf(stderr, "fieldName is empty,file[%s]\n", filePath.c_str());
            return *this;
        }
        string fname = fileName;
        if (fname.size() <= 0){
            SomeUtils::fileBaseName(filePath, fname);
        }
        uploadFiles uf = {fieldName:fieldName, filePath:filePath, fileName:fname};
        uploadFileList.push_back(uf);
        return *this;
    }

    //添加单条header信息
    curlHandle &curlHandle::addHeader(const string &k, const string &v){
        string key = k;
        HttpClientUtils::canonicalHeaderKey(key);
        if (key.size() <= 0){
            return *this;
        }
        headers[key] = v;
        return *this;
    }

    //批量设置头信息
    curlHandle &curlHandle::addHeaders(const map <string, string> &hs){
        map< string, string >::const_iterator iter;
        for (iter = hs.begin(); iter != hs.end(); iter++){
            string k = iter->first;
            HttpClientUtils::canonicalHeaderKey(k);
            if (k.size() <= 0){
                continue;
            }
            headers[k] = iter->second;
        }
        return *this;
    }

    //设置要请求的host（设置header的相应值）
    curlHandle &curlHandle::setHost(const string &host){
        addHeader("Host", host);
        return *this;
    }

    //设置URL
    curlHandle &curlHandle::setUrl(const string &url){
        this->url = url;
        return *this;
    }

    //设置内容类型（设置header的相应值）
    curlHandle &curlHandle::setContentType(const string &ct){
        addHeader("Content-Type", ct);
        return *this;
    }

    //设置长连接选项
    curlHandle &curlHandle::setKeepAlive(bool b){
        keepalive = b;
        return *this;
    }

    //设置代理用的地址和端口
    curlHandle &curlHandle::setProxy(const string &proxyHost){
        this->proxy = proxyHost;
        return *this;
    }

    //设置userAgent（设置header的相应值）
    curlHandle &curlHandle::setUserAgent(const string &ua){
        addHeader("User-Agent", ua);
        return *this;
    }

    //设置cookie（设置header的相应值）
    curlHandle &curlHandle::setRawCookie(const string &ck){
        addHeader("Cookie", ck);
        return *this;
    }

    //添加单个cookie的键值
    curlHandle &curlHandle::addCookie(const string &k, const string &v){
        map< string, string >::iterator iter;
        iter = headers.find("Cookie");
        string rawCK;
        if (iter == headers.end()){
            rawCK.append(k).append("=").append(v);
            setRawCookie(rawCK);
            return *this;
        }
        rawCK = iter->second;
        map <string, string> tmpCK;
        HttpClientUtils::splitRawCookie(rawCK, tmpCK);
        tmpCK[k] = v;
        rawCK.clear();
        HttpClientUtils::joinRawCookie(tmpCK, rawCK);
        setRawCookie(rawCK);
        return *this;
    }

    //设置referer（设置header的相应值）
    curlHandle &curlHandle::setReferer(const string &referer){
        addHeader("Referer", referer);
        return *this;
    }

    //设置HTTP Basic Authentication the provided username and password（设置header的相应值）
    curlHandle &curlHandle::setBasicAuth(const string &username, const string &password){
        string auth = username + ":" + password;
        string basicAuth;
        StrUtils::base64Encode(auth, basicAuth);
        addHeader("Authorization", "Basic " + basicAuth);
        return *this;
    }

    //批量添加字段，一般是 PostForm 用，在即上传文件又有字段时Post也用
    curlHandle &curlHandle::setFields(const map <string, string> &data){
        map< string, string >::const_iterator iter;
        for (iter = data.begin(); iter != data.end(); iter++){
            string k = iter->first;
            string v = iter->second;
            setField(k, v);
        }
        return *this;
    }

    //添加单个字段，POST用
    curlHandle &curlHandle::setField(const string &k, const string &v){
        vector <string> tvec;
        tvec.push_back(v);
        vals[k] = tvec;
        return *this;
    }

    //添加单个字段，可以添加数组，如 ：
    //client.addField("a","1") 、client.addField("a","2")
    //在接收的时候将收到 a=[1,2]
    curlHandle &curlHandle::addField(const string &k, const string &v){
        map < string, vector < string > > ::iterator
        iter;
        iter = vals.find(k);
        vector <string> tvec;
        if (iter != vals.end()){
            tvec = iter->second;
        }
        tvec.push_back(v);
        vals[k] = tvec;
        return *this;
    }

    //初始化请求信息
    int curlHandle::initCurlConf(HttpResponse &rsp){
        if (this->curlFP == NULL){
            this->curlFP = curl_easy_init();
        }
        if (this->curlFP == NULL){
            throw -1;
        }

        char dataBuf[HTTP_CLIENT_HTTP_BUF_SIZE];

        //长链接
        if (this->keepalive){
            curl_easy_setopt(curlFP, CURLOPT_TCP_KEEPALIVE, 1L);
            curl_easy_setopt(curlFP, CURLOPT_TCP_KEEPIDLE, 120L);
            curl_easy_setopt(curlFP, CURLOPT_TCP_KEEPINTVL, 60L);
        }
        else{
            curl_easy_setopt(curlFP, CURLOPT_TCP_KEEPALIVE, 0L);
        }

        //暂时不要校验https的证书
        curl_easy_setopt(curlFP, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curlFP, CURLOPT_SSL_VERIFYHOST, false);

        //在屏幕打印请求连接过程和返回http数据
        if (this->isDebug){
            curl_easy_setopt(curlFP, CURLOPT_VERBOSE, 1L);
        }
        //当根据Location:重定向时，自动设置header中的Referer:信息。
        curl_easy_setopt(curlFP, CURLOPT_AUTOREFERER, 1L);
        //返回的头部中有Location(一般直接请求的url没找到)，则继续请求Location对应的数据
        curl_easy_setopt(curlFP, CURLOPT_FOLLOWLOCATION, 1L);
        //查找次数，防止查找太深
        curl_easy_setopt(curlFP, CURLOPT_MAXREDIRS, 3L);
        //设置url
        curl_easy_setopt(curlFP, CURLOPT_URL, (char *) url.c_str());
        //超时：毫秒
        curl_easy_setopt(curlFP, CURLOPT_TIMEOUT_MS, timeout);
        //不要进度信息
        curl_easy_setopt(curlFP, CURLOPT_NOPROGRESS, 1L);
        //这货会影响超时
        curl_easy_setopt(curlFP, CURLOPT_NOSIGNAL, 1L);
        //设置header信息
        initHeaderList();
        if (headers.size() > 0){
            map< string, string >::const_iterator headerIter;
            for (headerIter = headers.begin(); headerIter != headers.end(); headerIter++){
                bzero(dataBuf, HTTP_CLIENT_HTTP_BUF_SIZE);
                sprintf(dataBuf, "%s: %s", headerIter->first.c_str(), headerIter->second.c_str());
                curl_slist_append(headerList, dataBuf);
            }
        }
        curl_easy_setopt(curlFP, CURLOPT_HTTPHEADER, headerList);
        //数据返回时的回调函数
        curl_easy_setopt(curlFP, CURLOPT_WRITEFUNCTION, receive_data_callback);
        curl_easy_setopt(curlFP, CURLOPT_WRITEDATA, (void *) &(rsp.rspStr));
        curl_easy_setopt(curlFP, CURLOPT_HEADERFUNCTION, header_callback);
        curl_easy_setopt(curlFP, CURLOPT_HEADERDATA, (void *) &(rsp.headerStr));
        return 0;
    }

    //初始化post的数据
    int curlHandle::initPostData(){
        if (formPost != NULL){
            curl_formfree(formPost);
            formLast = NULL;
            formPost = NULL;
        }
        char dataBuf[HTTP_CLIENT_HTTP_BUF_SIZE];
        //要POST的数据
        if (vals.size() > 0){
            map < string, vector < string > > ::const_iterator
            valsIter;
            vector< string >::const_iterator siter;
            for (valsIter = vals.begin(); valsIter != vals.end(); valsIter++){
                bzero(dataBuf, HTTP_CLIENT_HTTP_BUF_SIZE);
                //只有一个值时的情况
                if (valsIter->second.size() <= 1){
                    sprintf(dataBuf, "%s", valsIter->first.c_str());
                }
                else{
                    //多个值时以数组的形式提交
                    sprintf(dataBuf, "%s[]", valsIter->first.c_str());
                }
                for (siter = valsIter->second.begin(); siter != valsIter->second.end(); siter++){
                    curl_formadd(
                            &(this->formPost), &(this->formLast),
                            CURLFORM_COPYNAME, dataBuf,
                            CURLFORM_COPYCONTENTS, siter->c_str(),
                            CURLFORM_END
                    );
                }
            }
        }

        //如果有要上传的文件
        if (uploadFileList.size() > 0){
            vector< uploadFiles >::const_iterator fileIter;
            for (fileIter = uploadFileList.begin(); fileIter != uploadFileList.end(); fileIter++){
                curl_formadd(
                        &(this->formPost), &(this->formLast),
                        CURLFORM_PTRNAME, fileIter->fieldName.c_str(),
                        CURLFORM_FILE, fileIter->filePath.c_str(),
                        CURLFORM_FILENAME, fileIter->fileName.c_str(),
                        CURLFORM_END
                );
            }
        }
        if (this->formPost != NULL){
            curl_easy_setopt(curlFP, CURLOPT_HTTPPOST, this->formPost);
        }
        return 0;
    }

    //获取请求的句柄
    CURL *curlHandle::curlGet(HttpResponse &rsp){
        if (this->initCurlConf(rsp) != 0){
            return NULL;
        }
        curl_easy_setopt(this->curlFP, CURLOPT_HTTPGET, 1L);
        return this->curlFP;
    }

    //获取请求的句柄
    CURL *curlHandle::curlPostRaw(const string &rawData, HttpResponse &rsp){
        if (this->initCurlConf(rsp) != 0){
            return NULL;
        }
        curl_easy_setopt(this->curlFP, CURLOPT_POST, 1L);
        curl_easy_setopt(this->curlFP, CURLOPT_POSTFIELDS, rawData.c_str());
        return this->curlFP;
    }

    //获取请求的句柄
    CURL *curlHandle::curlPost(HttpResponse &rsp){
        if (headers.find("Content-Type") == headers.end()){
            this->addHeader("Content-Type", "application/octet-stream");
        }
        if (this->initCurlConf(rsp) != 0){
            return NULL;
        }
        return this->curlFP;
    }

    //获取请求的句柄
    CURL *curlHandle::curlHead(HttpResponse &rsp){
        if (this->initCurlConf(rsp) != 0){
            return NULL;
        }
        curl_easy_setopt(this->curlFP, CURLOPT_NOBODY, 1);
        return this->curlFP;
    }

    //清理资源
    curlHandle::~curlHandle(){
        cout << "[curlHandle::~curlHandle()]start clean resource......." << endl;
        curl_formfree(formPost);
        curl_easy_cleanup(curlFP);
        curl_slist_free_all(headerList);
    }
} //namespacecpputils