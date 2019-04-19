/*************************************************************************
 * 高仿PHP中的 pcre 函数库
 * 自己在pcre基础上把常用的功能封装下，比较不喜欢pcre++
 *
 * File:	regexp.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	Fri 22 Mar 2019 04:00:00 PM CST
 ************************************************************************/

#include<iostream>
#include <stdio.h>
#include <string.h>
#include "regexp.h"

using namespace std;

namespace runtofuServer{

    //初始化操作
    void RegExp::init(){
        if (re != NULL){
            return;
        }
        const char *error;
        int erroffset;
        const char *regp = pattern.c_str();
        re = pcre_compile(regp, 0, &error, &erroffset, NULL);
        if (re == NULL){
            printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
        }
    }

    //内部使用的匹配方法，这里的返回值是双层vector，考虑到会有多个捕获组的情况
    //外层的vector是匹配次数，内层的是一次匹配命中的所有的子串
    //matchLimit=-1表示不限制匹配次数
    int RegExp::reg_match_general(
            const std::string &str,
            size_t matchLimit,
            std::vector <std::vector< RegSubMatch > > &output
    ){
        init();
        if (matchLimit == 0){
            return -1;
        }
        if (re == NULL){
            return -1;
        }

        const char *cstr = str.c_str();
        size_t cstrlen = str.size();
        size_t start_offset = 0, matchedNum = 0;
        int rc, i;

        //至少要匹配一次
        while (start_offset < cstrlen){
            int ovector[PCRE_OVECCOUNT];
            rc = pcre_exec(re, NULL, cstr, cstrlen, start_offset, 0, ovector, PCRE_OVECCOUNT);
            if (rc < 0){
                return 0;
            }

            std::vector <RegSubMatch> subMatchList;

            for (i = 0; i < rc; i++){
                RegSubMatch subMatch;
                const char *substart = cstr + ovector[2 * i];
                int sublen = ovector[2 * i + 1] - ovector[2 * i];
                start_offset = ovector[2 * i + 1];
                char *match = (char *) malloc(sizeof(char) * (sublen + 1));
                bzero(match, sizeof(char) * (sublen + 1));
                memcpy(match, substart, sublen);
                subMatch.sub_str.append(match);
                subMatch.start_pos = ovector[2 * i];
                subMatchList.push_back(subMatch);
                free(match);
            }
            output.push_back(subMatchList);

            matchedNum++;
            if (matchLimit > 0 && matchedNum >= matchLimit){
                break;
            }
        }
        return 0;
    }

    //找出所有的子串：成功返回0，失败返回负数
    //内层的vector，第一个为整个匹配串，其他的表示捕获子串
    int RegExp::reg_match_all(const std::string &str, std::vector <std::vector< std::string > > &subList){
        std::vector <std::vector< RegSubMatch > > gMatch;
        int gret = reg_match_general(str, -1, gMatch);
        if (gret < 0){
            return gret;
        }
        if (gMatch.size() <= 0){
            return 0;
        }
        std::vector < std::vector < RegSubMatch > > ::const_iterator
        iter1;
        std::vector< RegSubMatch >::const_iterator iter2;
        for (iter1 = gMatch.begin(); iter1 != gMatch.end(); iter1++){
            std::vector <std::string> subMatch;
            for (iter2 = iter1->begin(); iter2 != iter1->end(); iter2++){
                subMatch.push_back(iter2->sub_str);
            }
            subList.push_back(subMatch);
        }
        return 0;
    }

    //用正则表达式切字符串：成功返回0，失败返回负数
    //忽略捕获组
    int RegExp::reg_split(const std::string &str, std::vector <std::string> &subList){
        std::vector <std::vector< RegSubMatch > > gMatch;
        int gret = reg_match_general(str, -1, gMatch);
        if (gret < 0){
			subList.push_back(str);
            return gret;
        }
        if (gMatch.size() <= 0){
			subList.push_back(str);
            return 0;
        }

        const char *cstr = str.c_str();
        size_t cstrlen = str.size();
        size_t prev_pos = 0, cur_pos = 0;
        std::vector < std::vector < RegSubMatch > > ::const_iterator
        iter1;
        std::vector< RegSubMatch >::const_iterator iter2;
        for (iter1 = gMatch.begin(); iter1 != gMatch.end(); iter1++){
            for (iter2 = iter1->begin(); iter2 != iter1->end(); iter2++){
                cur_pos = iter2->start_pos;
                if (cur_pos < prev_pos){
                    prev_pos = cur_pos + iter2->sub_str.size();
                    subList.push_back("");
                    break;
                }
                if (cur_pos == prev_pos){
                    subList.push_back("");
                    break;
                }
                char *match = (char *) malloc(sizeof(char) * (cur_pos - prev_pos + 1));
                bzero(match, sizeof(char) * sizeof(char) * (cur_pos - prev_pos + 1));
                memcpy(match, cstr + prev_pos, cur_pos - prev_pos);
                subList.push_back(match);
                free(match);
                prev_pos = cur_pos + iter2->sub_str.size();
                break;
            }
        }
        if (prev_pos < cstrlen){
            char *match = (char *) malloc(sizeof(char) * (cstrlen - prev_pos + 1));
            bzero(match, sizeof(char) * sizeof(char) * (cstrlen - prev_pos + 1));
            memcpy(match, cstr + prev_pos, cstrlen - prev_pos);
            subList.push_back(match);
            free(match);
        }
        return 0;
    }

    //是否跟当前字符串匹配：成功返回0，失败返回负数
    //忽略捕获组，第一个匹配项等于整个字符串即可
    int RegExp::reg_match(const std::string &str){
        std::vector <std::vector< RegSubMatch > > gMatch;
        int gret = reg_match_general(str, -1, gMatch);
        if (gret < 0){
            return gret;
        }
        if (gMatch.size() <= 0){
            return -1;
        }
        std::vector <RegSubMatch> subM = gMatch[0];
        if (subM.size() <= 0){
            return -1;
        }
        if (subM[0].sub_str == str){
            return 0;
        }
        return -1;
    }

    //正则替换：成功返回0，失败返回负数
    int RegExp::reg_replace(const std::string &str, const std::string &replace, std::string &newStr){
        std::vector <std::string> subList;
        int splitRet = reg_split(str, subList);
        if (splitRet < 0){
            newStr.append(str);
            return splitRet;
        }
        vector< string >::const_iterator iter;
        size_t subLen = subList.size();
        if (subLen <= 0){
            newStr.append(str);
            return 0;
        }
        int i;
        for (i = 0; i < subLen; i++){
            newStr.append(subList[i]);
            if (i < subLen - 1){
                newStr.append(replace);
            }
        }
        return 0;
    }

    //高仿PHP的 preg_replace_callback() 函数，利用回调函数替换字符串，生成新的字符串
    //成功返回0，失败返回负数
    //回调函数：0表示整个匹配的字符串，1表示正则里的第一个捕获项、2表示第二个、依次类推。。。。
    int RegExp::reg_replace_callback(
            const std::string &originStr,
            std::string &newStr,
            void (*callbackFunc)(const std::vector <std::string> &subMatchStrs, std::string &replaceStr)
    ){
        std::vector <std::vector< RegSubMatch > > gMatch;
        int gret = reg_match_general(originStr, -1, gMatch);
        if (gret < 0){
            newStr.append(originStr);
            return gret;
        }
        if (gMatch.size() <= 0){
            newStr.append(originStr);
            return 0;
        }
        const char *cstr = originStr.c_str();
        size_t cstrlen = originStr.size();
        size_t prev_pos = 0, cur_pos = 0;
        std::vector < std::vector < RegSubMatch > > ::const_iterator
        iter1;
        std::vector< RegSubMatch >::const_iterator iter2;
        for (iter1 = gMatch.begin(); iter1 != gMatch.end(); iter1++){
            int cnt = 0;
            size_t subLen = iter1->size();
            if (subLen <= 0){
                continue;
            }
            RegSubMatch firstSubMatch = (*iter1)[0];
            cur_pos = firstSubMatch.start_pos;
            if (cur_pos < prev_pos){
                prev_pos = cur_pos + firstSubMatch.sub_str.size();
                continue;
            }
            char *match = (char *) malloc(sizeof(char) * (cur_pos - prev_pos + 1));
            bzero(match, sizeof(char) * sizeof(char) * (cur_pos - prev_pos + 1));
            memcpy(match, cstr + prev_pos, cur_pos - prev_pos);
            newStr.append(match);
            free(match);
            prev_pos = cur_pos + firstSubMatch.sub_str.size();
            //要被替换的
            std::vector <std::string> subMatchStr;
            int i;
            for (i = 0; i < subLen; i++){
                subMatchStr.push_back((*iter1)[i].sub_str);
            }
            string replaceStr;
            callbackFunc(subMatchStr, replaceStr);
            newStr.append(replaceStr);
        }
        if (prev_pos < cstrlen){
            char *match = (char *) malloc(sizeof(char) * (cstrlen - prev_pos + 1));
            bzero(match, sizeof(char) * sizeof(char) * (cstrlen - prev_pos + 1));
            memcpy(match, cstr + prev_pos, cstrlen - prev_pos);
            newStr.append(match);
            free(match);
        }
        return 0;
    }

    RegExp::~RegExp(){
        if (re != NULL){
            pcre_free(re);
        }
    }
}
