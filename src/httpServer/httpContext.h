/*************************************************************************
 * File:	httpContext.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-20 17:43
 ************************************************************************/
#ifndef _RUNTOFUSERVER_HTTPCONTEXT_H_201904201743_
#define _RUNTOFUSERVER_HTTPCONTEXT_H_201904201743_

#include<string>
#include<vector>
#include<map>
#include "httpResponse.h"
#include "httpRequest.h"

namespace runtofuServer{
    class httpContext{
    public:
        int fd;//请求socket句柄s
        timeval sTime;
        httpResponse *rsp;
        httpRequest *req;

        httpContext(int fd);

        ~httpContext();

        //清理资源
        void deleteRes();

        void reset();

        //请求时间
        int setStartTime();

        //耗时
        int getCostTime();
    };
} //namespace runtofuServer


#endif //_RUNTOFUSERVER_HTTPCONTEXT_H_201904201743_
