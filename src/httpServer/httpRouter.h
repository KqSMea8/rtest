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

namespace rtest{

    //要执行的方法
    typedef void (*httpFunc)(httpRequest &req, httpResponse &rsp);

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

    class httpRouter{
    public:
        ~httpRouter();

        //路由列表信息
        std::vector< routerItem * > routerList;

        //添加一条路由信息
        void addRouter(const routerItem *r);

        void addRouter(const routerItem &r);

        void addRouter(ROUTER_TYPE t, const std::string &c, httpFunc f, const std::string &extParam);

        //匹配路由规则，如果有参数的话一起返回
        const routerItem *matchRouter(const std::string &uri, std::map <std::string, std::string> &args) const;

    private:
        /**
         * 匹配全路径路由
         *      :arg	只配置数字类型
         *      :arg:	可以匹配任意类型
         * 如：
         *      /ggtest/:id	匹配下面的URL，并把 id作为参数回传给controller层
         *          /ggtest/6666
         *          /ggtest/89999
         *      /ggtest/:name:	匹配下面的URL。并把name作为参数回传给controller层
         *          /ggtest/wendao
         *          /ggtest/abc
         *          /ggtest/44444
         **/
        bool matchPathInfoRouter(const std::string &uri, const routerItem *router, std::map <std::string, std::string> &args) const;

        /**
         * 匹配正则路由，直接用正则表达式去匹配请求的URL，并把捕获的参数回传到Param配置里,如
         * config 为 `^ggtest/aid(\w+?)/cid(\d+)$`，Param 为 aid=$1&cid=$2
         * 则将请求中aid后面的字符串挑出来赋给aid，cid后面的字符串挑出来赋给cid
         */
        bool matchRegexpRouter(const std::string &uri, const routerItem *router, std::map <std::string, std::string> &args) const;
    };
} //namespace rtest


#endif //_RUNTOFUSERVER_HTTPROUTER_H_201904261820_
