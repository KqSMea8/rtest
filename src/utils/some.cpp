/*************************************************************************
 * File:	some.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019年03月21日 星期四 23时28分50秒
 ************************************************************************/

#include<iostream>
#include<string.h>
#include<uuid/uuid.h>
#include<stdio.h>
#include<sys/stat.h>
#include<dirent.h>
#include<sys/time.h>
#include<time.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<sys/ioctl.h>
#include<math.h>
#include<stdint.h>
#include<unistd.h>
#include<string>
#include "some.h"

using namespace std;

namespace runtofuServer{
    const char *SomeUtils::urandomDevice = "/dev/urandom";

    //获取屏幕终端的宽高
    void SomeUtils::getTerminalSize(size_t *width, size_t *height){
        if (width != NULL){
            *width = 0;
        }
        if (height != NULL){
            *height = 0;
        }
        struct winsize sz;
        int r = ioctl(0, TIOCGWINSZ, &sz);
        if (r != 0){
            perror("ioctl failed");
            return;
        }
        if (width != NULL){
            *width = sz.ws_col;
        }
        if (height != NULL){
            *height = sz.ws_row;
        }
    }

    //对整数向上圆整为2^n倍数
    size_t SomeUtils::up2n(size_t num){
        if (num <= 2){
            return 2;
        }
        size_t n = ceil(log((double) num) / log(2.0));
        size_t ret = pow(2.0, n);
        return ret;
    }

    //判断文件是否存在
    bool SomeUtils::isFileExist(const std::string &filePath){
        struct stat buffer;
        return (stat(filePath.c_str(), &buffer) == 0);
    }

    //获取文件大小
    size_t SomeUtils::fileSize(const std::string &filePath){
        struct stat statbuf;
        stat(filePath.c_str(), &statbuf);
        size_t size = statbuf.st_size;
        return size;
    }

    //读取文件全部内容
    int SomeUtils::getFileContent(const std::string &filePath, void *buf){
        FILE *fp = fopen(filePath.c_str(), "rb");
        if (fp == NULL){
            perror("open file failed\n");
            return -1;
        }
        char *tmpBuf = (char *) buf;
        size_t bsize = 4096;
        while (!feof(fp)){
            size_t n = fread(tmpBuf, bsize, 1, fp);
            tmpBuf += n * bsize;
        }
        fclose(fp);
        if (ferror(fp)){
            return -1;
        }
        return 0;
    }

    //封装的提取文件的base name
    void SomeUtils::fileBaseName(const std::string &filePath, std::string &baseName){
        char buf[512] = {0};
        sprintf(buf, "%s", filePath.c_str());
        char *bname = strrchr(buf, '/');
        if (bname == NULL){
            bname = buf;
        }
        else if (bname < buf + strlen(buf)){
            bname++;
        }
        baseName = bname;
    }

    //从java扒过来的hash_code计算
    uint64_t SomeUtils::java_hash_code(const void *data, int len){
        uint64_t h = 0;
        const char *d = (char *) data;
        int off = 0;
        int i;
        for (i = 0; i < len; i++){
            h = 31 * h + (int) d[off++];
        }
        return h;
    }

    //获取从设备读取的随机字节
    void SomeUtils::genDeviceRandomBytes(char *buf, size_t len){
        FILE *fp = fopen(urandomDevice, "r");
        if (fp == NULL){
            perror("open file failed");
            return;
        }
        fgets(buf, len, fp);
        fclose(fp);
    }

    //从设备读取随机字符串
    void SomeUtils::getDeviceRandom(string &random, size_t limit){
        size_t cnum = sizeof(char) * limit * 4;
        char *buf = (char *) malloc(cnum);
        bzero(buf, cnum);
        genDeviceRandomBytes(buf, cnum);
        int i, tmpI;
        char randBuf[32] = {0};
        char *tmpIBuf = (char *) &tmpI;
        string tmpOutPut;
        for (i = 0; i < cnum; i++){
            memset(randBuf, 0, 32);
            tmpIBuf[0] = *(buf + i * 4);
            tmpIBuf[1] = *(buf + i * 4 + 1);
            tmpIBuf[2] = *(buf + i * 4 + 2);
            tmpIBuf[3] = *(buf + i * 4 + 3);
            sprintf(randBuf, "%x", tmpI);
            tmpOutPut.append(randBuf);
            if (tmpOutPut.size() >= limit){
                break;
            }
        }
        free(buf);
        string tmpRandom(tmpOutPut, 0, limit);
        random = tmpRandom;
    }

    //获取本机的IP地址
    void SomeUtils::getLocalIP(vector <uint32_t> &ipList){
        char hostname[128] = {0};
        int ret = gethostname(hostname, sizeof(hostname));
        if (ret == -1){
            return;
        }
        cout << hostname << endl;
        struct hostent *hent;
        hent = gethostbyname(hostname);
        if (NULL == hent){
            return;
        }
        int i;
        for (i = 0; hent->h_addr_list[i]; i++){
            uint32_t u = ntohl(((struct in_addr *) hent->h_addr_list[i])->s_addr);
            ipList.push_back(u);
        }
        return;
    }

    //IP地址转字符串
    void SomeUtils::long2IP(uint32_t ip, string &ipStr){
        char buf[32] = {0};
        sprintf(buf, "%d.%d.%d.%d", ip >> 24, ip << 8 >> 24, ip << 16 >> 24, ip << 24 >> 24);
        ipStr = buf;
    }

    //当前的微秒数
    void SomeUtils::getMicroSeconds(uint64_t &ms){
        struct timeval tv;
        int r = gettimeofday(&tv, NULL);
        if (r != 0){
            perror("gettimeofday failed");
            return;
        }
        ms = 1000000 * (uint64_t) tv.tv_sec + (uint64_t) tv.tv_usec;
    }

    //当前的纳秒数
    void SomeUtils::getNanoSeconds(uint64_t &ns){
#ifdef linux || _UNIX
        pid_t pid = getpid();
        clockid_t clk_id;
        int r = clock_getcpuclockid(pid, &clk_id);
        if (r != 0){
            perror("clock_getcpuclockid failed");
            return;
        }
        struct timespec ts;
        r = clock_gettime(clk_id, &ts);
        if (r != 0){
            perror("clock_gettime failed");
            return;
        }
        time_t ct = time(NULL);
        ns = 1000000000 * (uint64_t) ct + (uint64_t) ts.tv_nsec;
#endif
#ifdef __APPLE__ && __MACH__
        uint64_t ms;
        getMicroSeconds(ms);
        ns = ms *1000;
#endif
    }

    //产生一个 0至RAND_MAX 之间的随机数
    int SomeUtils::getRandom(){
        char buf[5] = {0};
        genDeviceRandomBytes(buf, 5);
        uint32_t seed;
        char *tmp = (char *) &seed;
        *tmp = buf[0];
        *(tmp + 1) = buf[1];
        *(tmp + 2) = buf[2];
        *(tmp + 3) = buf[3];
        srand(seed);
        int r = rand();
        return r;
    }

    //debug
    void SomeUtils::printStringVector(const std::vector <string> &output, const std::string &delim){
        cout << delim << endl;
        vector< string >::const_iterator iter;
        for (iter = output.begin(); iter != output.end(); iter++){
            cout << *iter << endl;
        }
    }
} //namespacecpputils
