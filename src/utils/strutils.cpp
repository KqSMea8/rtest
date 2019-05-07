/*************************************************************************
 * File:	strutils.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019年03月21日 星期四 16时54分19秒
 ************************************************************************/

#include<iostream>
#include<pcrecpp.h>
#include<ctype.h>
#include "strutils.h"

using namespace std;
namespace rtest{
#define ENCODE64(c)     (((c) & 0xC0) ? -1 : base64digits[(int)c])
#define DECODE64(c)     (((c) & 0x80) ? -1 : base64val[(int)c])
    unsigned char StrUtils::hexchars[] = "0123456789ABCDEF";
    char StrUtils::base64digits[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char StrUtils::base64val[] = {
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
            -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
            -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
    };

    //提取字符串的unicode值
    int StrUtils::utf8ToUnicode(const std::string &str, std::vector <int32_t> &unicodes){
        if (str.size() <= 0){
            return -1;
        }
        int32_t unicode;
        const char *tmp = str.c_str();
        const char *endPtr = str.c_str() + str.size();
        for (;;){
            uint8_t c = *tmp;
            if (c <= (uint8_t) 0x7F){
                //单字节编码形式：0xxxxxxx
                unicode = (int32_t) c;
                tmp++;
            }
            else if (c <= (uint8_t) 0xDF){
                //双字节编码形式：110xxxxx 10xxxxxx
                int32_t t1 = (int32_t)(*tmp & 0x1F) << 6;
                int32_t t2 = (int32_t)(*(tmp + 1) & 0x3F);
                unicode = t1 | t2;
                tmp += 2;
            }
            else if (c <= (uint8_t) 0xEF){
                //三字节编码形式：1110xxxx 10xxxxxx 10xxxxxx
                int32_t t1 = (int32_t)(*tmp & 0x0F) << 12;
                int32_t t2 = (int32_t)(*(tmp + 1) & 0x3F) << 6;
                int32_t t3 = (int32_t)(*(tmp + 2) & 0x3F);
                unicode = t1 | t2 | t3;
                tmp += 3;
            }
            else if (c <= (uint8_t) 0xF7){
                //四字节编码形式：11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                int32_t t1 = (int32_t)(*tmp & 0x07) << 18;
                int32_t t2 = (int32_t)(*(tmp + 1) & 0x3F) << 12;
                int32_t t3 = (int32_t)(*(tmp + 2) & 0x3F) << 6;
                int32_t t4 = (int32_t)(*(tmp + 3) & 0x3F);
                unicode = t1 | t2 | t3 | t4;
                tmp += 4;
            }
            else if (c <= (uint8_t) 0xFB){
                //五字节编码形式：111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                int32_t t1 = (int32_t)(*tmp & 0x03) << 24;
                int32_t t2 = (int32_t)(*(tmp + 1) & 0x3F) << 18;
                int32_t t3 = (int32_t)(*(tmp + 2) & 0x3F) << 12;
                int32_t t4 = (int32_t)(*(tmp + 3) & 0x3F) << 6;
                int32_t t5 = (int32_t)(*(tmp + 4) & 0x3F);
                unicode = t1 | t2 | t3 | t4 | t5;
                tmp += 5;
            }
            else if (c <= (uint8_t) 0xFD){
                //六字节编码形式：1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                int32_t t1 = (int32_t)(*tmp & 0x01) << 30;
                int32_t t2 = (int32_t)(*(tmp + 1) & 0x3F) << 24;
                int32_t t3 = (int32_t)(*(tmp + 2) & 0x3F) << 18;
                int32_t t4 = (int32_t)(*(tmp + 3) & 0x3F) << 12;
                int32_t t5 = (int32_t)(*(tmp + 4) & 0x3F) << 6;
                int32_t t6 = (int32_t)(*(tmp + 5) & 0x3F);
                unicode = t1 | t2 | t3 | t4 | t5 | t6;
                tmp += 6;
            }
            unicodes.push_back(unicode);
            if (tmp >= endPtr){
                break;
            }
        }
        return 0;
    }

    //将unicode再转为utf8字符串
    int StrUtils::unicodesToUTF8(const std::vector <int32_t> &unicodes, std::string &str){
        if (unicodes.size() <= 0){
            return -1;
        }
        vector< int32_t >::const_iterator iter;

        for (iter = unicodes.begin(); iter != unicodes.end(); iter++){
            int32_t unicode = *iter;
            char tmpStrBuf[6] = {0};
            if (unicode <= 0x0000007F){
                //单字节:0xxxxxxx
                *tmpStrBuf = (unicode & 0x7F);
            }
            else if (unicode >= 0x00000080 && unicode <= 0x000007FF){
                //双字节:110xxxxx 10xxxxxx
                *(tmpStrBuf + 1) = (unicode & 0x3F) | 0x80;
                *tmpStrBuf = ((unicode >> 6) & 0x1F) | 0xC0;
            }
            else if (unicode >= 0x00000800 && unicode <= 0x0000FFFF){
                //三字节:1110xxxx 10xxxxxx 10xxxxxx
                *(tmpStrBuf + 2) = (unicode & 0x3F) | 0x80;
                *(tmpStrBuf + 1) = ((unicode >> 6) & 0x3F) | 0x80;
                *tmpStrBuf = ((unicode >> 12) & 0x0F) | 0xE0;
            }
            else if (unicode >= 0x00010000 && unicode <= 0x001FFFFF){
                //四字节:11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                *(tmpStrBuf + 3) = (unicode & 0x3F) | 0x80;
                *(tmpStrBuf + 2) = ((unicode >> 6) & 0x3F) | 0x80;
                *(tmpStrBuf + 1) = ((unicode >> 12) & 0x3F) | 0x80;
                *tmpStrBuf = ((unicode >> 18) & 0x07) | 0xF0;
            }
            else if (unicode >= 0x00200000 && unicode <= 0x03FFFFFF){
                //五字节:111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                *(tmpStrBuf + 4) = (unicode & 0x3F) | 0x80;
                *(tmpStrBuf + 3) = ((unicode >> 6) & 0x3F) | 0x80;
                *(tmpStrBuf + 2) = ((unicode >> 12) & 0x3F) | 0x80;
                *(tmpStrBuf + 1) = ((unicode >> 18) & 0x3F) | 0x80;
                *tmpStrBuf = ((unicode >> 24) & 0x03) | 0xF8;
            }
            else if (unicode >= 0x04000000 && unicode <= 0x7FFFFFFF){
                //六字节:1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                *(tmpStrBuf + 5) = (unicode & 0x3F) | 0x80;
                *(tmpStrBuf + 4) = ((unicode >> 6) & 0x3F) | 0x80;
                *(tmpStrBuf + 3) = ((unicode >> 12) & 0x3F) | 0x80;
                *(tmpStrBuf + 2) = ((unicode >> 18) & 0x3F) | 0x80;
                *(tmpStrBuf + 1) = ((unicode >> 24) & 0x3F) | 0x80;
                *tmpStrBuf = ((unicode >> 30) & 0x01) | 0xFC;
            }
            else{
                continue;
            }
            str.append(tmpStrBuf);
        }
        return 0;
    }

    //单个unicode码转utf8
    int StrUtils::unicodeToUTF8(int32_t unicode, std::string &str){
        std::vector <int32_t> tmpRune;
        tmpRune.push_back(unicode);
        return unicodesToUTF8(tmpRune, str);
    }

    //base64 encode
    int StrUtils::base64Encode(const std::string &in, std::string &out){
        size_t inlen = in.size();
        if (inlen <= 0){
            return -1;
        }

        const char *inStr = in.c_str();
        char c;
        for (; inlen >= 3; inlen -= 3, inStr += 3){
            c = ENCODE64((inStr[0] >> 2) & 0x3F);
            if (c == -1){
                out.clear();
                return -1;
            }
            out.push_back(c);
            c = ENCODE64(((inStr[0] << 4) & 0x30) | ((inStr[1] >> 4) & 0x0F));
            if (c == -1){
                out.clear();
                return -1;
            }
            out.push_back(c);
            c = ENCODE64(((inStr[1] << 2) & 0x3C) | ((inStr[2] >> 6) & 0x03));
            if (c == -1){
                out.clear();
                return -1;
            }
            out.push_back(c);
            c = ENCODE64(inStr[2] & 0x3F);
            if (c == -1){
                out.clear();
                return -1;
            }
            out.push_back(c);
        }

        if (inlen > 0){
            char fragment;
            c = ENCODE64((inStr[0] >> 2) & 0x3F);
            if (c == -1){
                out.clear();
                return -1;
            }
            out.push_back(c);
            fragment = (inStr[0] << 4) & 0x30;
            if (inlen > 1){
                fragment |= (inStr[1] >> 4) & 0x0F;
            }
            c = ENCODE64(fragment);
            if (c == -1){
                out.clear();
                return -1;
            }
            out.push_back(c);
            if (inlen < 2){
                out.push_back('=');
            }
            else{
                c = ENCODE64((inStr[1] << 2) & 0x3C);
                if (c == -1){
                    out.clear();
                    return -1;
                }
            }
            out.push_back('=');
        }
        return 0;
    }

    //base64 decode
    int StrUtils::base64Decode(const std::string &in, std::string &out){
        if (in.size() <= 0){
            return -1;
        }
        size_t inlen = in.size();
        const char *inStr = in.c_str();
        int over = 0;
        register char digit0, digit1, digit2, digit3;

        // decode each four base64 characters
        for (; inlen >= 4; inlen -= 4, inStr += 4){
            if ((digit0 = DECODE64(inStr[0])) == -1){
                return -1;
            }
            if ((digit1 = DECODE64(inStr[1])) == -1){
                return -1;
            }
            out.push_back(((digit0 << 2) & 0xFC) | ((digit1 >> 4) & 0x03));

            if (inStr[2] != '='){
                if ((digit2 = DECODE64(inStr[2])) == -1){
                    return -1;
                }
                out.push_back(((digit1 << 4) & 0xF0) | ((digit2 >> 2) & 0x0F));
                if (inStr[3] != '='){
                    if ((digit3 = DECODE64(inStr[3])) == -1){
                        return -1;
                    }
                    out.push_back(((digit2 << 6) & 0xC0) | (digit3 & 0x3F));
                }
                else{
                    over = 1;
                    break;
                }
            }
            else{
                over = 1;
                break;
            }
        }

        if (!over && inlen > 0){
            return -1;
        }

        return 0;
    }

    //url encode
    int StrUtils::urlEncode(const std::string &in, std::string &out){
        size_t inlen = in.size();
        if (inlen <= 0){
            return -1;
        }
        unsigned char c;
        unsigned const char *inStart = (unsigned const char *) in.c_str();
        unsigned const char *inEnd = inStart + inlen;
        while (inStart < inEnd){
            c = *inStart++;
            if (c == ' '){
                out.push_back('+');
            }
            else if ((c < '0' && c != '-' && c != '.') ||
                     (c < 'A' && c > '9') ||
                     (c > 'Z' && c < 'a' && c != '_') ||
                     (c > 'z')){
                out.push_back('%');
                out.push_back(hexchars[c >> 4]);
                out.push_back(hexchars[c & 15]);
            }
            else{
                out.push_back(c);
            }
        }
        return 0;
    }

    //转为大写
    void StrUtils::toUpper(string &str){
        size_t i, len = str.size();
        for (i = 0; i < len; i++){
            str[i] = toupper(str[i]);
        }
    }

    //转为小写
    void StrUtils::toLower(string &str){
        size_t i, len = str.size();
        for (i = 0; i < len; i++){
            str[i] = tolower(str[i]);
        }
    }

    //url encode
    int StrUtils::urlDecode(const std::string &in, std::string &out){
        size_t inlen = in.size();
        if (inlen <= 0){
            return -1;
        }

        int value;
        int c;
        const char *inStr = in.c_str();

        while (inlen--){
            if (*inStr == '+'){
                out.push_back(' ');
            }
            else if (*inStr == '%' && inlen >= 2 && isxdigit((int) *(inStr + 1)) && isxdigit((int) *(inStr + 2))){
                c = ((unsigned char *) (inStr + 1))[0];
                if (isupper(c)){
                    c = tolower(c);
                }
                value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;
                c = ((unsigned char *) (inStr + 1))[1];
                if (isupper(c)){
                    c = tolower(c);
                }
                value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;
                out.push_back((char) value);
                inStr += 2;
                inlen -= 2;
            }
            else{
                out.push_back(*inStr);
            }
            inStr++;
        }
        return 0;
    }

    //简简单单的替换
    void StrUtils::strReplace(const string &in, const string &replace, const string &r, string &out){
        string tmpIN = in;
        size_t start_pos = 0, pos;
        while ((pos = tmpIN.find(replace, start_pos)) != string::npos){
            tmpIN.replace(pos, replace.size(), r);
            start_pos += replace.size();
        }
        out = tmpIN;
    }

    //简简单单的切割
    void StrUtils::strSplit(const string &in, char c, vector <string> &out){
        string tmpIN = in;
        size_t prev_pos = 0, pos = 0;
        while ((pos = tmpIN.find(c, prev_pos)) != string::npos){
            string tmp(in, prev_pos, pos - prev_pos);
            out.push_back(tmp);
            prev_pos = pos + 1;
        }
        if (prev_pos < in.size()){
            string tmp(in, prev_pos, in.size() - prev_pos);
            out.push_back(tmp);
        }
    }

    //简简单单的连接
    void StrUtils::strJoin(const vector <string> &in, const string &j, string &out){
        size_t i, len = in.size();
        for (i = 0; i < len; i++){
            out.append(in[i]);
            if (i != len - 1){
                out.append(j);
            }
        }
    }

    //去掉两端的空格
    void StrUtils::trimSpace(string &str){
        size_t slen = str.size();
        if (slen <= 0){
            return;
        }
        const char *pstart = str.c_str();
        const char *pend = pstart + slen - 1;
        char c;
        int i;
        for (i = 0; i < slen; i++){
            c = *pstart;
            if (isspace(c) == 0){
                break;
            }
            pstart++;
        }
        for (i = slen - 1; (i >= 0 && pend > pstart); i--){
            c = *pend;
            if (isspace(c) == 0){
                break;
            }
            pend--;
        }
        string tmpStr(pstart, pend - pstart + 1);
        str = tmpStr;
    }

    //去掉端的空格
    void StrUtils::trimLeftSpace(string &str){
        size_t slen = str.size();
        if (slen <= 0){
            return;
        }
        const char *pstart = str.c_str();
        const char *pend = pstart + slen - 1;
        char c;
        int i;
        for (i = 0; i < slen; i++){
            c = *pstart;
            if (isspace(c) == 0){
                break;
            }
            pstart++;
        }
        string tmpStr(pstart, pend - pstart + 1);
        str = tmpStr;
    }

    //去掉端的空格
    void StrUtils::trimRightSpace(string &str){
        size_t slen = str.size();
        if (slen <= 0){
            return;
        }
        const char *pstart = str.c_str();
        const char *pend = pstart + slen - 1;
        char c;
        int i;
        for (i = slen - 1; (i >= 0 && pend > pstart); i--){
            c = *pend;
            if (isspace(c) == 0){
                break;
            }
            pend--;
        }
        string tmpStr(pstart, pend - pstart + 1);
        str = tmpStr;
    }

    //去掉两端的Char
    void StrUtils::trimChar(string &str, char ch){
        size_t slen = str.size();
        if (slen <= 0){
            return;
        }
        const char *pstart = str.c_str();
        const char *pend = pstart + slen - 1;
        char c;
        int i;
        for (i = 0; i < slen; i++){
            c = *pstart;
            if (c != ch){
                break;
            }
            pstart++;
        }
        for (i = slen - 1; (i >= 0 && pend > pstart); i--){
            c = *pend;
            if (c != ch){
                break;
            }
            pend--;
        }
        string tmpStr(pstart, pend - pstart + 1);
        str = tmpStr;
    }

    //去掉端的Char
    void StrUtils::trimLeftChar(string &str, char ch){
        size_t slen = str.size();
        if (slen <= 0){
            return;
        }
        const char *pstart = str.c_str();
        const char *pend = pstart + slen - 1;
        char c;
        int i;
        for (i = 0; i < slen; i++){
            c = *pstart;
            if (c != ch){
                break;
            }
            pstart++;
        }
        string tmpStr(pstart, pend - pstart + 1);
        str = tmpStr;
    }

    //去掉端的Char
    void StrUtils::trimRightChar(string &str, char ch){
        size_t slen = str.size();
        if (slen <= 0){
            return;
        }
        const char *pstart = str.c_str();
        const char *pend = pstart + slen - 1;
        char c;
        int i;
        for (i = slen - 1; (i >= 0 && pend > pstart); i--){
            c = *pend;
            if (c != ch){
                break;
            }
            pend--;
        }
        string tmpStr(pstart, pend - pstart + 1);
        str = tmpStr;
    }
} //namespacecpputils