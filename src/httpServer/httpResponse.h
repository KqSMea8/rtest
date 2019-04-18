/*************************************************************************
 * File:	httpResponse.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-04 14:39
 ************************************************************************/
#ifndef _CPPUTILS_HTTPRESPONSE_H_201904041439_
#define _CPPUTILS_HTTPRESPONSE_H_201904041439_

#include<string>
#include<vector>
#include<map>
#include <stdint.h>
#include "httpStatus.h"
#include "../utils/byte_buffer.h"
#include "../client/http/http_client_utils.h"
#include "httpRequest.h"

namespace cpputils{
    class httpResponse{
    private:
        httpStatus::HTTP_STATUS status;
        std::map <std::string, std::vector< std::string>> headers;
        std::string body;
        std::string rsp;
        const httpRequest &req;

        //生成header信息
        void genHeader(string &h);

    public:
        httpResponse(const httpRequest &req);

        //设置响应值
        void setStatus(httpStatus::HTTP_STATUS status);

        //设置头信息
        void setHeader(const std::string &k, const std::string &v);

        void addHeader(const std::string &k, const std::string &v);

        //设置cookie信息
        void setCookie(
                const std::string &k,//KV是必需的
                const std::string &v,
                uint32_t maxAgeTime,//相对过期时间，单位秒，删除cookie时传负值
                const std::string &path,//指定的路径信息
                const std::string &domain,//指定的域名，默认为创建cookie的网页所属域名
                bool secure,//只对HTTPS请求可见，对HTTP请求不可见
                bool httponly//对浏览器端的javascript中的document对象不可见
        );

        //设置响应的body信息
        void setBody(const std::string &body);

        //返回html响应信息
        void renderHTML(std::string &rsp);

        //返回json信息
        void renderJson(std::string &rsp);
    };
} //namespace cpputils


#endif //_CPPUTILS_HTTPRESPONSE_H_201904041439_
