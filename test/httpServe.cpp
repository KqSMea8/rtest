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

using namespace std;

void httpFunc1(runtofuServer::httpRequest &req, runtofuServer::httpResponse &rsp){
    cout << "httpFunc1" << endl;
}

void httpFunc2(runtofuServer::httpRequest &req, runtofuServer::httpResponse &rsp){
    cout << "httpFunc2" << endl;
}


int main(){
    string f = "./data/httpRequest.txt";
    string upf = "/Users/liuyongshuai/Documents/一种处理B+树重复键值的方法.pdf";
    size_t fsz = runtofuServer::SomeUtils::fileSize(f);
    size_t upfsz = runtofuServer::SomeUtils::fileSize(upf);
    cout << "filesize=" << upfsz << endl;
    char *fileContent = (char *) malloc(sizeof(char) * (fsz + 1));
    bzero(fileContent, sizeof(char) * (fsz + 1));
    runtofuServer::SomeUtils::getFileContent(f, fileContent);
    runtofuServer::httpRequest req;
    req.parseBody(fileContent, fsz);
    req.printReq();

    //生成响应信息
    runtofuServer::httpResponse rsp;
    rsp.setBody("{\"q\":\"http s\",\"p\":false,\"g\":[{\"type\":\"sug\",\"sa\":\"s_1\",\"q\":\"http status 500\"},{\"type\":\"sug\",\"sa\":\"s_2\",\"q\":\"http status 400\"},{\"type\":\"sug\",\"sa\":\"s_3\",\"q\":\"http socket\"},{\"type\":\"sug\",\"sa\":\"s_4\",\"q\":\"http session\"},{\"type\":\"sug\",\"sa\":\"s_5\",\"q\":\"httpstatus404解决步骤\"},{\"type\":\"sug\",\"sa\":\"s_6\",\"q\":\"http status\"},{\"type\":\"sug\",\"sa\":\"s_7\",\"q\":\"http stat jseea\"},{\"type\":\"sug\",\"sa\":\"s_8\",\"q\":\"http socket tcp区别\"},{\"type\":\"sug\",\"sa\":\"s_9\",\"q\":\"http status 404-not found\"},{\"type\":\"sug\",\"sa\":\"s_10\",\"q\":\"http status 500解决\"}]}");
    string htmlStr;
    rsp.renderHTML(htmlStr);
    cout << htmlStr << endl;
    string jsonStr;
    rsp.renderJson(jsonStr);
    cout << jsonStr << endl;

    //解析路由信息
    map <string, string> args;
    runtofuServer::httpRouter routers;
    routers.addRouter(
            runtofuServer::ROUTER_TYPE_PATH_INFO,
            "/abc/:aaaa:/:dddd",httpFunc1,"");
    routers.addRouter(
            runtofuServer::ROUTER_TYPE_REGEXP,
            "/abc/(\\d+)/(\\w+)",httpFunc2,"a=$1&b=$2");
    routers.matchRouter("/abc/wendao/444444",args);
    routers.matchRouter("/abc/5544554/wendao",args);
    routers.matchRouter("/abc/333333/8888",args);
    return 0;
}
