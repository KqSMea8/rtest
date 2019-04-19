/*************************************************************************
 * File:	strutils.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019年03月21日 星期四 16时54分00秒
 ************************************************************************/

#ifndef _UTILS_STRUTILS_H_
#define _UTILS_STRUTILS_H_

#include <string>
#include <vector>
#include <iconv.h>
#include <stdint.h>
#include <stddef.h>

using namespace std;
namespace runtofuServer{
    class StrUtils{
    private:
        StrUtils(){};

        StrUtils(const StrUtils &){};

        StrUtils &operator=(const StrUtils &){};
        static char base64digits[];
        static char base64val[];
        static unsigned char hexchars[];
    public:
        //utf8转为unicode
        static int utf8ToUnicode(const string &str, vector <int32_t> &unicodes);

        //unicode转为utf8
        static int unicodesToUTF8(const vector <int32_t> &unicodes, string &str);

        //单个unicode转
        static int unicodeToUTF8(int32_t unicode, string &str);

        //base64 encode
        static int base64Encode(const string &in, string &out);

        //base64 decode
        static int base64Decode(const string &in, string &out);

        //url encode
        static int urlEncode(const string &in, string &out);

        //url encode
        static int urlDecode(const string &in, string &out);

        //简简单单的替换
        static void strReplace(const string &in, const string &replace, const string &newStr, string &out);

        //简简单单的切割
        static void strSplit(const string &in, char c, vector <string> &out);

        //简简单单的连接
        static void strJoin(const vector <string> &in, const string &j, string &out);

        //转为大写
        static void toUpper(string &str);

        //转为小写
        static void toLower(string &str);

        //去掉两端的空格
        static void trimSpace(string &str);

        //去掉端的空格
        static void trimLeftSpace(string &str);

        //去掉端的空格
        static void trimRightSpace(string &str);

        //去掉两端的Char
        static void trimChar(string &str, char c);

        //去掉端的Char
        static void trimLeftChar(string &str, char c);

        //去掉端的Char
        static void trimRightChar(string &str, char c);
    };
} //namespacecpputils
#endif //_UTILS_STRUTILS_H_
