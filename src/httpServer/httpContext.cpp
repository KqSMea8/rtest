/*************************************************************************
 * File:	httpContext.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-20 17:43
 ************************************************************************/
#include<iostream>
#include "httpContext.h"

using namespace std;

namespace runtofuServer{
    httpContext::httpContext(int fd){
        this->fd = fd;
        this->rsp = new httpResponse();
        this->req = new httpRequest();
    }

    httpContext::~httpContext(){
        this->deleteRes();
    }

    //清理资源
    void httpContext::deleteRes(){
        if (this->req != NULL){
            delete this->req;
            this->req = NULL;
        }
        if (this->rsp != NULL){
            delete this->rsp;
            this->rsp = NULL;
        }
    }

    void httpContext::reset(){
        this->deleteRes();
        this->rsp = new httpResponse();
        this->req = new httpRequest();
    }

    //请求时间
    int httpContext::setStartTime(){
        gettimeofday(&(this->sTime), NULL);
        return 0;
    }

    //耗时
    int httpContext::getCostTime(){
        timeval end;
        gettimeofday(&end, NULL);
        int cost_time = (end.tv_sec - this->sTime.tv_sec) * 1000000 + (end.tv_usec - this->sTime.tv_usec);
        return cost_time;
    }
} //namespace runtofuServer
