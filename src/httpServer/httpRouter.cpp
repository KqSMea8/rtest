/*************************************************************************
 * File:	httpRouter.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-26 18:20
 ************************************************************************/
#include<iostream>
#include "httpRouter.h"
#include "../utils/strutils.h"
#include "../utils/some.h"
#include "../utils/regexp.h"

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
    const routerItem *httpRouter::matchPathInfoRouter(const string &uri, const routerItem *router, map <string, string> &args){
        //请求的URL的切片
        vector <string> pathInfo;
        StrUtils::strSplit(uri, '/', pathInfo);
        //事先配置好的路由的切片，要和URL逐个比对，若有:arg、:arg:这样的还要替换
        vector <string> confInfo;
        StrUtils::strSplit(router->config, '/', confInfo);
        if (pathInfo.size() > confInfo.size()){
            return NULL;
        }
        size_t i;
        for (i = 0; i < confInfo.size(); i++){
            string val = confInfo[i];
            //当前段以冒号“:”开头
            if (val[0] == ':'){
                if (pathInfo.size() > i){
                    string pathVal = pathInfo[i];
                    //非数字必须 :arg:，数字只能 :arg
                    if (!SomeUtils::isAllNumber(pathVal) && val[val.size() - 1] != ':'){
                        return NULL;
                    }
                    StrUtils::trimChar(val, ':');
                    args[val] = pathVal;
                }
            }
            else if (pathInfo.size() <= i || pathInfo[i] != val){
                return NULL;
            }
        }

        return NULL;
    }

    /**
     * 匹配正则路由，直接用正则表达式去匹配请求的URL，并把捕获的参数回传到Param配置里,如
     * config 为 `^ggtest/aid(\w+?)/cid(\d+)$`，Param 为 aid=$1&cid=$2
     * 则将请求中aid后面的字符串挑出来赋给aid，cid后面的字符串挑出来赋给cid
     */
    const routerItem *httpRouter::matchRegexpRouter(const string &uri, const routerItem *router, map <string, string> &args){
        RegExp reg(router->config);
        if (reg.reg_match(uri) != 0){
            cout << "httpRouter::matchRegexpRouter:regexp not match\t" << router->config << "\t" << uri << endl;
            return NULL;
        }
        vector <vector< string >> subList;
        reg.reg_match_all(uri, subList);
        string argStr = router->extParam;
        size_t i = 1;
        if (subList.size() > 0){
            vector < vector < string >> ::const_iterator
            iter;
            for (iter = subList.begin(); iter != subList.end(); iter++){
                vector< string >::const_iterator iter1;
                for (iter1 = iter->begin(); iter1 != iter->end(); iter1++){
                    if (*iter1 == uri){
                        continue;
                    }
                    string tmpArgStr = argStr;
                    char buf[256] = {0};
                    sprintf(buf, "$%lu", i++);
                    StrUtils::strReplace(argStr, buf, *iter1, tmpArgStr);
                    argStr = tmpArgStr;
                }
            }
        }

        vector <string> tmpArgVec;
        size_t pos;
        StrUtils::strSplit(argStr, '&', tmpArgVec);
        vector< string >::const_iterator vecStrIter;
        for (vecStrIter = tmpArgVec.begin(); vecStrIter != tmpArgVec.end(); vecStrIter++){
            pos = vecStrIter->find_first_of("=", 0);
            if (pos == string::npos){
                continue;
            }
            string k(*vecStrIter, 0, pos);
            string v(*vecStrIter, pos + 1);
            args[k] = v;
        }

        return NULL;
    }

    httpRouter::~httpRouter(){
        vector< routerItem * >::iterator iter;
        for (iter = routerList.begin(); iter != routerList.end(); iter++){
            delete *iter;
        }
    }
} //namespace runtofuServer
