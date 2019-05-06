/*************************************************************************
 * File:	httpTask.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-05-06 13:07
 ************************************************************************/
#include<iostream>
#include "httpTask.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using apache::thrift::concurrency::Runnable;
using apache::thrift::concurrency::ThreadManager;
using apache::thrift::concurrency::PosixThreadFactory;

namespace runtofuServer{

    void httpTask::run(){
        ByteBuffer bbfu;
        cout << "start task:" << sockFD << endl;
        char buf[MAX_SOCK_BUF_SIZE];
        //读到的字节数
        int nread;
        do{
            bzero(buf, MAX_SOCK_BUF_SIZE);
            nread = read(this->sockFD, buf, MAX_SOCK_BUF_SIZE);
            cout << nread << endl;
            //读到数据
            if (nread > 0){
                cout << "read:" << nread << "\t" << buf << ",errno:" << errno << endl;
                bbfu.append(buf, nread);
                //此处的数据读取方式较为暴力，假设所有的数据一次性到达
                if (nread < MAX_SOCK_BUF_SIZE){
                    break;
                }
            }
                //读取失败
            else if (nread < 0){
                //没有数据了
                if (errno == EAGAIN){
                    cout << "[EAGAIN]read:" << nread << ",errno:" << errno << ",no data" << endl;
                    break;
                }
                    //可能被内部中断信号打断,经过验证对非阻塞socket并未收到此错误,应该可以省掉该步判断
                else if (errno == EINTR){
                    cout << "[EINTR]read:" << nread << ",errno:" << errno << ",interrupt" << endl;
                    break;
                }
                    //客户端主动关闭
                else{
                    cout << "[other]read:" << nread << ",errno:" << errno << ",peer error" << endl;
                    break;
                }
            }
                //客户端主动关闭
            else if (nread == 0){
                cout << "ReadThread, read:" << nread << ",errno:" << errno << ",peer close" << endl;
                break;
            }
        }while (true);

        //解析请求
        httpRequest httpReq;
        httpReq.parseBody(bbfu.rawData(), bbfu.size());
        httpReq.printReq();

        string rspStr;
        httpResponse httpRsp;
        httpRsp.setBody("{\"q\":\"http s\",\"p\":false,\"g\":[{\"type\":\"sug\",\"sa\":\"s_1\",\"q\":\"http status 500\"},{\"type\":\"sug\",\"sa\":\"s_2\",\"q\":\"http status 400\"},{\"type\":\"sug\",\"sa\":\"s_3\",\"q\":\"http socket\"},{\"type\":\"sug\",\"sa\":\"s_4\",\"q\":\"http session\"},{\"type\":\"sug\",\"sa\":\"s_5\",\"q\":\"httpstatus404解决步骤\"},{\"type\":\"sug\",\"sa\":\"s_6\",\"q\":\"http status\"},{\"type\":\"sug\",\"sa\":\"s_7\",\"q\":\"http stat jseea\"},{\"type\":\"sug\",\"sa\":\"s_8\",\"q\":\"http socket tcp区别\"},{\"type\":\"sug\",\"sa\":\"s_9\",\"q\":\"http status 404-not found\"},{\"type\":\"sug\",\"sa\":\"s_10\",\"q\":\"http status 500解决\"}]}");
        httpRsp.renderJson(rspStr);

        write(this->sockFD, rspStr.c_data(), rspStr.size());
        close(this->sockFD);
        cout << "end task:" << sockFD << endl;
    }

} //namespace runtofuServer
