/*************************************************************************
 * File:	httpRouter.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-26 18:20
 ************************************************************************/
#ifndef _RUNTOFUSERVER_HTTPROUTER_H_201904261820_
#define _RUNTOFUSERVER_HTTPROUTER_H_201904261820_

#include<string>
#include<vector>
#include<map>
#include "httpRequest.h"
#include "httpResponse.h"

namespace runtofuServer{

    //要执行的方法
    typedef void (*httpFunc)(httpRequest &req, httpResponse &rsp);

    class httpRouter{
    public:
        //路由匹配类型
        typedef enum _router_type{
            ROUTER_TYPE_PATH_INFO = 1,//全路径匹配
            ROUTER_TYPE_REGEXP//正则匹配
        } ROUTER_TYPE;

        //单个路由匹配项
        typedef struct _router_item{
            ROUTER_TYPE type;//路由匹配类型
            std::string config;//规则信息
            httpFunc func;//控制层
            std::string extParam;//额外的参数
        } routerItem;

        //路由列表信息
        std::vector< routerItem * > routerList;

        //添加一条路由信息
        void addRouter();
    };
} //namespace runtofuServer


#endif //_RUNTOFUSERVER_HTTPROUTER_H_201904261820_
