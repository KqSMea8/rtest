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

#define MAX_EVENTS 1024
#define LISTEN_BACKLOG_SIZE 8

namespace runtofuServer{
    class httpServer{
    public:
        httpServer(uint16_t port);

        ~httpServer();

        //路由信息
        httpRouter routers;

        //启动函数
        void start();

    private:
        //端口号
        const uint16_t port;

        //监听句柄
        int listenFD;
        int epollFD;

        //声明epoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件
        struct epoll_event ev;
        //监听事件数组
        struct epoll_event events[MAX_EVENTS];

        //开始监听
        void listen();

        //接收请求
        void accept();

        //设置句柄属性
        void setNonBlocking(int sock);

        //重用
        void setReUseAddr(int sock);
    };
} //namespace runtofuServer


#endif //_CPPUTILS_HTTPSERVER_H_201904041437_
