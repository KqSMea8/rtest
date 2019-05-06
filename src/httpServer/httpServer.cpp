/*************************************************************************
 * File:	httpServer.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-04 14:37
 ************************************************************************/
#include<iostream>
#include "httpServer.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using apache::thrift::concurrency::Runnable;
using apache::thrift::concurrency::ThreadManager;
using apache::thrift::concurrency::PosixThreadFactory;

namespace runtofuServer{
    class httpTask : public Runnable{
    public:
        httpTask(int fd) : sockFD(fd){}

        virtual ~httpTask(){}

        void run();

        void readD();

    private:
        int sockFD;
    };

    void httpTask::run(){
        cout << "start task:" << sockFD << endl;
        char buf[MAX_SOCK_BUF_SIZE];
        //读到的字节数
        int nread;
        char output[1024] = {0};
        sprintf(output, "sockFD=%d", sockFD);
        do{
            bzero(buf, MAX_SOCK_BUF_SIZE);
            nread = read(this->sockFD, buf, MAX_SOCK_BUF_SIZE);
            cout << nread << endl;
            //读到数据
            if (nread > 0){
                cout << "read:" << nread << "\t" << buf << ",errno:" << errno << endl;
            }
                //读取失败
            else if (nread < 0){
                //没有数据了
                if (errno == EAGAIN){
                    cout << "[EAGAIN]read:" << nread << ",errno:" << errno << ",no data" << endl;
                    continue;
                }
                    //可能被内部中断信号打断,经过验证对非阻塞socket并未收到此错误,应该可以省掉该步判断
                else if (errno == EINTR){
                    cout << "read:" << nread << ",errno:" << errno << ",interrupt" << endl;
                    break;
                }
                    //客户端主动关闭
                else{
                    cout << "read:" << nread << ",errno:" << errno << ",peer error" << endl;
                    break;
                }
            }
                //客户端主动关闭
            else if (nread == 0){
                cout << "ReadThread, read:" << nread << ",errno:" << errno << ",peer close" << endl;
                break;
            }
        }while (true);
        write(this->sockFD, output, strlen(output));
        close(this->sockFD);
        cout << "end task:" << sockFD << endl;
    }

    httpServer::httpServer(uint16_t p) : port(p), listenFD(0), epollFD(0){
        this->threadManager = apache::thrift::concurrency::ThreadManager::newSimpleThreadManager(30);
        boost::shared_ptr <PosixThreadFactory> threadFactory = boost::shared_ptr< PosixThreadFactory >(new PosixThreadFactory());
        threadFactory->setStackSize(8);
        this->threadManager->threadFactory(threadFactory);
        this->threadManager->start();
    }

    //启动函数
    void httpServer::start(){
        this->listen();
        while (true){
            this->accept();
        }
    }

    //开始监听
    void httpServer::listen(){
        if (this->listenFD > 0){
            close(this->listenFD);
        }
        if (this->epollFD > 0){
            close(this->epollFD);
        }

        //创建监听socket
        this->listenFD = socket(AF_INET, SOCK_STREAM, 0);
        if (this->listenFD < 0){
            perror("create socket failed");
            exit(1);
        }

        //设置属性
        setNonBlocking(this->listenFD);
        setReUseAddr(this->listenFD);

        //创建epoll,对2.6.8以后的版本,其参数无效,只要大于0的数值就行,内核自己动态分配
        this->epollFD = epoll_create(MAX_EVENTS);
        if (this->epollFD < 0){
            perror("epoll_create failed");
            exit(1);
        }

        //设置与要处理的事件相关的文件描述符
        this->ev.data.fd = this->listenFD;
        //设置要处理的事件类型
        this->ev.events = EPOLLIN | EPOLLET;
        //注册epoll事件
        int ret = epoll_ctl(this->epollFD, EPOLL_CTL_ADD, this->listenFD, &(this->ev));
        if (ret != 0){
            perror("epoll_ctl");
            close(this->listenFD);
            close(this->epollFD);
            exit(1);
        }

        //监听
        struct sockaddr_in svraddr;
        bzero(&svraddr, sizeof(svraddr));
        svraddr.sin_family = AF_INET;
        svraddr.sin_addr.s_addr = htonl(INADDR_ANY);
        svraddr.sin_port = htons(this->port);
        bind(this->listenFD, (sockaddr * ) & svraddr, sizeof(svraddr));
        //监听,准备接收连接
        ret = ::listen(this->listenFD, LISTEN_BACKLOG_SIZE);
        if (ret != 0){
            perror("listen");
            close(this->listenFD);
            close(this->epollFD);
            exit(1);
        }
    }

    //接收请求
    void httpServer::accept(){
        struct sockaddr_in cliaddr;
        socklen_t clilen;
        //接收到的连接socket临时变量
        int connfd;
        //临时变量,有多少个socket有事件
        int nfds, i;
        //等待epoll事件的发生,如果当前有信号的句柄数大于输出事件数组的最大大小,
        //超过部分会在下次epoll_wait时输出,事件不会丢
        nfds = epoll_wait(this->epollFD, this->events, MAX_EVENTS, 500);
        //处理所发生的所有事件
        for (i = 0; i < nfds; ++i){
            //是本监听socket上的事件
            if (this->events[i].data.fd == this->listenFD){
                cout << "events:" << this->events[i].events << ",errno:" << errno << endl;
                //有连接到来
                if (this->events[i].events & EPOLLIN){
                    do{
                        clilen = sizeof(struct sockaddr);
                        connfd = ::accept(this->listenFD, (sockaddr *) &cliaddr, &clilen);
                        if (connfd > 0){
                            this->setNonBlocking(connfd);
                            cout << "accept:" << connfd << ",errno:" << errno << ",connect:" << inet_ntoa(cliaddr.sin_addr) << ":" << ntohs(cliaddr.sin_port) << endl;
                            if (this->threadManager.get() != NULL){
                                boost::shared_ptr <Runnable> task = boost::shared_ptr< Runnable >(new httpTask(connfd));
                                this->threadManager->add(task);
                            }
                        }
                        else{
                            cout << "accept:" << connfd << ",errno:" << errno << endl;
                            //没有连接需要接收了
                            if (errno == EAGAIN){
                                break;
                            }
                            else{
                                //此时说明该描述字已经出错了,需要重新创建和监听
                                this->listen();
                            }
                        }
                    }while (true);
                }
                else if (this->events[i].events & EPOLLERR || this->events[i].events & EPOLLHUP){//有异常发生
                    //此时说明该描述字已经出错了,需要重新创建和监听
                    this->listen();
                }
            }
        }
    }

    //设置句柄属性
    void httpServer::setNonBlocking(int sock){
        int opts;
        opts = fcntl(sock, F_GETFL);
        if (opts < 0){
            perror("fcntl(sock,GETFL)");
            exit(1);
        }
        opts = opts | O_NONBLOCK;
        if (fcntl(sock, F_SETFL, opts) < 0){
            perror("fcntl(sock,SETFL,opts)");
            exit(1);
        }
    }

    //重用
    void httpServer::setReUseAddr(int sock){
        int opt;
        opt = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(&opt)) < 0){
            perror("setsockopt");
            exit(1);
        }
    }

    httpServer::~httpServer(){

    }
} //namespace runtofuServer
