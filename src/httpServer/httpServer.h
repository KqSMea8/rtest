/*************************************************************************
 * File:	httpServer.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-04 14:37
 ************************************************************************/
#ifndef _CPPUTILS_HTTPSERVER_H_201904041437_
#define _CPPUTILS_HTTPSERVER_H_201904041437_

#include<string>
#include<vector>
#include<map>
#include "httpRouter.h"
#include "httpRequest.h"
#include "httpResponse.h"
#include "httpStatus.h"

namespace runtofuServer{
    class httpServer{
    public:
        httpServer();

        ~httpServer();

        //路由信息
        httpRouter routers;
    };
} //namespace runtofuServer


#endif //_CPPUTILS_HTTPSERVER_H_201904041437_
