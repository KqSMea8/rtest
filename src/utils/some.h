/*************************************************************************
 * File:	some.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019年03月21日 星期四 23时28分41秒
 ************************************************************************/

#ifndef _UTILS_SOME_H_
#define _UTILS_SOME_H_

#include <stdint.h>
#include <vector>
#include <string>

using namespace std;

namespace runtofuServer{
    class SomeUtils{
    private:
        static const char *urandomDevice;

        SomeUtils(){};

        SomeUtils(const SomeUtils &){};

        SomeUtils &operator=(const SomeUtils &){};
    public:
        //从java扒过来的
        static uint64_t java_hash_code(const void *data, int len);

        //获取屏幕终端的宽高
        static void getTerminalSize(size_t *width, size_t *height);

        //对整数向上圆整为2^n倍数
        static size_t up2n(size_t num);

        //判断字符串全部由数字组成
        static bool isAllNumber(const std::string &str);

        //判断文件是否存在
        static bool isFileExist(const std::string &filePath);

        //获取文件大小
        static size_t fileSize(const std::string &filePath);

        //读取文件全部内容
        static int getFileContent(const std::string &filePath, void *buf);

        //封装的提取文件的base name
        static void fileBaseName(const std::string &filePath, std::string &baseName);

        //debug用的
        static void printStringVector(const std::vector <std::string> &output, const std::string &delim);

        //获取从设备读取的随机字节
        static void genDeviceRandomBytes(char *buf, size_t len);

        //从设备读取随机字符串
        static void getDeviceRandom(string &random, size_t limit);

        //获取本机IP地址
        static void getLocalIP(vector <uint32_t> &ipList);

        //IP地址转字符串
        static void long2IP(uint32_t ip, string &ipStr);

        //当前的微秒数
        static void getMicroSeconds(uint64_t &ms);

        //当前的纳秒数
        static void getNanoSeconds(uint64_t &ns);

        //产生一个 0至RAND_MAX 之间的随机数
        static int getRandom();
    };

} //namespacecpputils

#endif // _UTILS_SOME_H_
