/*************************************************************************
 * File:	httpTask.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-05-06 13:07
 ************************************************************************/
#ifndef _RUNTOFUSERVER_HTTPTASK_H_201905061307_
#define _RUNTOFUSERVER_HTTPTASK_H_201905061307_

#include<string>
#include<vector>
#include<map>
#include "httpRequest.h"
#include "httpResponse.h"
#include "httpRouter.h"
#include "../utils/byte_buffer.h"
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>

#define MAX_SOCK_BUF_SIZE 8192

namespace rtest{
    class httpTask : public apache::thrift::concurrency::Runnable{
    public:
        httpTask(int fd, std::string cip, const httpRouter &r) : sockFD(fd), clientIP(cip), routers(r){}

        virtual ~httpTask(){}

        void run();

    private:
        int sockFD;
        std::string clientIP;
        const httpRouter &routers;
    };
} //namespace rtest


#endif //_RUNTOFUSERVER_HTTPTASK_H_201905061307_
