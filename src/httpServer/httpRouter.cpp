/*************************************************************************
 * File:	httpRouter.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-26 18:20
 ************************************************************************/
#include<iostream>
#include "httpRouter.h"
#include "../utils/strutils.h"

using namespace std;

namespace runtofuServer{
    //添加一条路由信息
    void httpRouter::addRouter(const routerItem *r){
        this->addRouter(r->type, r->config, r->func, r->extParam);
    }

    void httpRouter::addRouter(const routerItem &r){
        this->addRouter(&r);
    }

    void httpRouter::addRouter(ROUTER_TYPE t, const string &c, httpFunc f, const string &extParam){
        routerItem *ritem = new routerItem;
        ritem->type = t;
        ritem->config = c;
        ritem->func = f;
        ritem->extParam = extParam;
        routerList.push_back(ritem);
    }

    //匹配路由规则
    const routerItem *httpRouter::matchRouter(const string &uri, map <string, string> &args){
        const routerItem *ret = NULL;
        string tmpUri = uri;
        StrUtils::trimChar(tmpUri, '/');
        vector< routerItem * >::const_iterator iter;
        for (iter = routerList.begin(); iter != routerList.end(); iter++){
            if ((*iter)->type == ROUTER_TYPE_PATH_INFO){
                ret = matchPathInfoRouter(uri, *iter, args);
                if (ret != NULL){
                    return ret;
                }
            }
            else if ((*iter)->type == ROUTER_TYPE_REGEXP){
                ret = matchRegexpRouter(uri, *iter, args);
                if (ret != NULL){
                    return ret;
                }
            }
        }
        return ret;
    }

    const routerItem *httpRouter::matchPathInfoRouter(const string &uri, const routerItem *router, map <string, string> &args){
        return NULL;
    }

    const routerItem *httpRouter::matchRegexpRouter(const string &uri, const routerItem *router, map <string, string> &args){
        return NULL;
    }

    httpRouter::~httpRouter(){
        vector< routerItem * >::iterator iter;
        for (iter = routerList.begin(); iter != routerList.end(); iter++){
            delete *iter;
        }
    }
} //namespace runtofuServer
