/*************************************************************************
 * File:	httpServe.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-08 14:33
 ************************************************************************/
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdint.h>
#include "../httpServer/httpRequest.h"
#include "../httpServer/httpResponse.h"
#include "../utils/some.h"
#include "../utils/strutils.h"
#include "../httpServer/httpRouter.h"
#include "../httpServer/httpServer.h"

using namespace std;

void httpIndex(runtofuServer::httpRequest &req, runtofuServer::httpResponse &rsp){
    cout << "httpIndex" << endl;
}

void httpFunc1(runtofuServer::httpRequest &req, runtofuServer::httpResponse &rsp){
    cout << "httpFunc1" << endl;
}

void httpFunc2(runtofuServer::httpRequest &req, runtofuServer::httpResponse &rsp){
    cout << "httpFunc2" << endl;
}

void httpFunc3(runtofuServer::httpRequest &req, runtofuServer::httpResponse &rsp){
    cout << "httpFunc3" << endl;
}

void printArgs(map <string, string> &args){
    cout << endl;
    map< string, string >::const_iterator iter;
    for (iter = args.begin(); iter != args.end(); iter++){
        cout << iter->first << "\t" << iter->second << endl;
    }
    cout << endl;
}

int main(){
    runtofuServer::httpServer svr(5000);
    svr.routers.addRouter(runtofuServer::ROUTER_TYPE_PATH_INFO, "", httpIndex, "");
    svr.routers.addRouter(runtofuServer::ROUTER_TYPE_PATH_INFO, "article/:aid", httpFunc1, "");
    svr.routers.addRouter(runtofuServer::ROUTER_TYPE_PATH_INFO, "article/:aid/comment/:cid", httpFunc2, "");
    svr.routers.addRouter(runtofuServer::ROUTER_TYPE_REGEXP, "article/(comment|mark)", httpFunc3, "t=$1");
    svr.start();
    return 0;
}
