/*************************************************************************
 * File:	svr_read.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-26 15:37
 ************************************************************************/
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdint.h>
#include<sys/epoll.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<sys/time.h>
#include<sys/shm.h>
#include <arpa/inet.h>
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#define TEST_PORT  5000
#define BUFFER_SIZE 1024

int main(){
    int sock_cli = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(TEST_PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ///连接服务器，成功返回0，错误返回-1
    if (connect(sock_cli, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        perror("connect");
        exit(1);
    }

    char sendbuf[BUFFER_SIZE] = {0};
    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL){
        send(sock_cli, sendbuf, strlen(sendbuf), 0); ///发送
        if (strcmp(sendbuf, "exit\n") == 0)
            break;
        memset(sendbuf, 0, sizeof(sendbuf));
    }

    close(sock_cli);
    return 0;
}