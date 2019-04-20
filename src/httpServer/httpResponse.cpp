/*************************************************************************
 * File:	httpResponse.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-04 14:39
 ************************************************************************/
#include<iostream>
#include<time.h>
#include "httpResponse.h"

using namespace std;

void genCurData(uint32_t maxAgeTime, string &d){
    char expire_time[128] = {0};
    time_t expire;
    time(&expire);
    expire += maxAgeTime;
    strftime(expire_time, sizeof(expire_time), "%a, %d %b %Y %H:%M:%S GMT", localtime(&expire));
    d = expire_time;
}

namespace runtofuServer{
    //初始化一些头信息
    httpResponse::httpResponse(){
        this->setHeader("Referrer-Policy", "origin-when-cross-origin");
        this->setHeader("X-Powered-By", "runtofu");
        this->setHeader("Server", "runtofu");
        this->setHeader("Cache-Control", "max-age=86400");
        string exp;
        genCurData(86400, exp);
        this->setHeader("Expires", exp);
        string d;
        genCurData(0, d);
        this->setHeader("Date", d);
        this->status = httpStatus::OK;
        this->setHeader("Content-Type", "text/html; charset=utf-8");
    }

    //设置响应值
    void httpResponse::setStatus(httpStatus::HTTP_STATUS status){
        this->status = status;
    }

    //设置头信息
    void httpResponse::addHeader(const std::string &k, const std::string &v){
        string headerKey = k;
        HttpClientUtils::canonicalHeaderKey(headerKey);
        this->headers[headerKey].push_back(v);
    }

    void httpResponse::setHeader(const std::string &k, const std::string &v){
        string headerKey = k;
        HttpClientUtils::canonicalHeaderKey(headerKey);
        vector <string> tmp;
        tmp.push_back(v);
        this->headers[headerKey] = tmp;
    }

    void httpResponse::getHeader(const std::string &k, vector <string> &vs){
        string headerKey = k;
        HttpClientUtils::canonicalHeaderKey(headerKey);
        vector <string> tmp;
        std::map < std::string, std::vector < std::string > > ::const_iterator
        iter;
        if ((iter = headers.find(headerKey)) == headers.end()){
            return;
        }
        vs = iter->second;
    }

    //设置cookie信息
    void httpResponse::setCookie(const std::string &k, const std::string &v, uint32_t maxAgeTime, const std::string &path, const std::string &domain, bool secure, bool httponly){
        if (k.size() <= 0){
            return;
        }
        string ckBuf;
        ckBuf.append(k + "=" + v);

        //过期时间
        if (maxAgeTime > 0){
            string date;
            genCurData(maxAgeTime, date);
            char exp[256] = {0};
            sprintf(exp, "; Expires=%s; Max-Age=%u", date.c_str(), maxAgeTime);
            ckBuf.append(exp);
        }
        else{
            ckBuf.append("; Max-Age=0");
        }

        //path信息
        if (path.size() > 0){
            char p[256] = {0};
            sprintf(p, "; Path=%s", path.c_str());
            ckBuf.append(p);
        }
        else{
            ckBuf.append("; Path=/");
        }

        //domain信息
        if (domain.size() > 0){
            char d[256] = {0};
            sprintf(d, "; Domain=%s", domain.c_str());
            ckBuf.append(d);
        }

        //Secure
        if (secure){
            ckBuf.append("; Secure");
        }

        //httponly
        if (httponly){
            ckBuf.append("; HttpOnly");
        }
        this->addHeader("Set-Cookie", ckBuf);
    }

    //设置响应的body信息
    void httpResponse::setBody(const std::string &body){
        this->body = body;
    }

    //生成header信息
    void httpResponse::genHeader(string &h){
        string statusStr;
        httpStatus::statusDesc(this->status, statusStr);
        char r[64] = {0};
        sprintf(r, "HTTP/1.1 %d %s\r\n", this->status, statusStr.c_str());
        h = r;
        map < string, vector < string >> ::const_iterator
        mapIter;
        vector< string >::const_iterator vecIter;
        for (mapIter = this->headers.begin(); mapIter != this->headers.end(); mapIter++){
            string hkey = mapIter->first;
            for (vecIter = mapIter->second.begin(); vecIter != mapIter->second.end(); vecIter++){
                h.append(hkey + ": " + *vecIter + "\r\n");
            }
        }
    }

    //返回html响应信息
    void httpResponse::renderHTML(std::string &rsp){
        this->genHeader(rsp);
        rsp += "\r\n";
        rsp += body;
    }

    //返回json信息
    void httpResponse::renderJson(std::string &rsp){
        this->setHeader("Content-Type", "application/json; charset=utf-8");
        this->genHeader(rsp);
        rsp += "\r\n";
        rsp += body;
    }
} //namespace runtofuServer
