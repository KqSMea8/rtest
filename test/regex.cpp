/*************************************************************************
 * File:	regex.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-03-22 15:08
 ************************************************************************/
#include<iostream>
#include<string>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdint.h>
#include "../src/utils/regexp.h"

using namespace std;

#define PCRE_OVECCOUNT 128

void printVector(const std::vector <string> &output){
    vector< string >::const_iterator iter;
    cout << endl;
    cout << endl;
    for (iter = output.begin(); iter != output.end(); iter++){
        cout << *iter << endl;
    }
    cout << endl;
    cout << endl;
}

int find_string_sub_match(const std::string &pattern, const std::string &str, std::vector <string> &output);

void fn(const std::vector <std::string> &subMatchStrs, std::string &replaceStr){
    printVector(subMatchStrs);
    replaceStr = "wendao";
}

int main(){
    string rawStr = "<strong>执行结果</strong>"
                    "<img title=\"\" alt=\"这里写图片描述\" src=\"https://img-blog.csdn.net/20180801145535161?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3N1amlhbndlaTExMjM=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70\">"
                    "<link href=\"https://csdnimg.cn/release/phoenix/mdeditor/markdown_views-258a4616f7.css\" rel=\"stylesheet\">"
                    "<img title=\"\" alt=\"这里写图片描述\" src=\"https://img-blog.csdn.net/20180801145535161?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3N1amlhbndlaTExMjM=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70\">";
    string pattern = "<img\\s+(?:.*?)src=\"(.*?)\">";

    std::vector <string> matchs;
    find_string_sub_match(pattern, rawStr, matchs);
    printVector(matchs);

    string rawStr1 = "<img src=\"http://abc-com\">";
    string regexp1 = "<img\\s+src=\"http://abc.com\">";
    std::vector <string> matchs1;
    find_string_sub_match(regexp1, rawStr1, matchs1);
    printVector(matchs1);
    cout << (matchs[0] == rawStr1) << endl;

    runtofuServer::RegExp regExp(pattern);
    std::vector <std::vector< runtofuServer::RegExp::RegSubMatch > > output1;
    regExp.reg_match_general(rawStr, -1, output1);
    cout << output1.size() << endl;
    cout << "reg_match_general" << endl;
    std::vector < std::vector < runtofuServer::RegExp::RegSubMatch > > ::const_iterator
    iter21;
    for (iter21 = output1.begin(); iter21 != output1.end(); iter21++){
        cout << iter21->size() << endl;
        std::vector< runtofuServer::RegExp::RegSubMatch >::const_iterator iter22;
        for (iter22 = iter21->begin(); iter22 != iter21->end(); iter22++){
            cerr << "substr=\n\t" << iter22->sub_str << endl;
            cout << "start_pos=\n\t" << iter22->start_pos << endl;
        }
    }


    cout << "reg_match_all" << endl;
    std::vector <std::vector< std::string > > matchAll;
    regExp.reg_match_all(rawStr, matchAll);
    cout << matchAll.size() << endl;
    std::vector < std::vector < std::string > > ::const_iterator
    iter31;
    for (iter31 = matchAll.begin(); iter31 != matchAll.end(); iter31++){
        printVector(*iter31);
    }

    cout << "reg_split" << endl;
    string splitStr = "Showing 11 changed files  with 246 additions and 148 deletions";
    runtofuServer::RegExp regExp1("\\s+");
    std::vector <std::string> splitList;
    regExp1.reg_split(splitStr, splitList);
    printVector(splitList);

    cout << "reg_split_1" << endl;
    string splitStr2 = "${2}abc\\${2}";
    runtofuServer::RegExp regExp22("(?<!\\)$\\{[\\d]\\}");
    std::vector <std::string> splitList2;
    regExp22.reg_split(splitStr2, splitList2);
    printVector(splitList2);

    cout << "reg_match" << endl;
    string matchStr = "2019-03-29 12:34:22";
    runtofuServer::RegExp regExp2("^[\\d]{4}-[\\d]+-[\\d]{2}.*$");
    int matchRet = regExp2.reg_match(matchStr);
    cout << "matchRet=" << matchRet << endl;

    string testTestTest = "abc\0d";
    cout << testTestTest << endl << endl;

    cout << "reg_match" << endl;
    string matchStr2 = "/abcd/333333/8888";
    runtofuServer::RegExp regExp22("/abcd/(\\d+)/(\\w+)");
    int matchRet2 = regExp22.reg_match(matchStr2);
    cout << "matchRet=" << matchRet2 << endl;

    string testTestTest = "abc\0d";
    cout << testTestTest << endl << endl;



    cout << "reg_replace" << endl;
    string splitStr44 = "Showing 11 changed files  with 246 additions and 148 deletions";
    runtofuServer::RegExp regExp44("(\\s+an[a-z]*?)\\s");
    std::string splitNewStr44;
    regExp44.reg_replace(splitStr44, "${1}aa${1}", splitNewStr44);
    cout << splitNewStr44 << endl << endl;

    cout << "reg_replace_callback" << endl;
    std::string splitNewStr55;
    regExp.reg_replace_callback(rawStr,  splitNewStr55,fn);
    cout << splitNewStr55 << endl << endl;


    //测试正则是否匹配
    cout << "simple regexp check........" << endl;
    string locale = "zh_CN.utf8";
    string localePattern = "^[a-z][a-z][a-z]?(?:_[A-Z][A-Z])?\\.(.+)";
    runtofuServer::RegExp regExp99(localePattern);
    std::vector <std::vector< std::string > > matchAll99;
    regExp99.reg_match_all(locale,matchAll99);
    for (iter31 = matchAll99.begin(); iter31 != matchAll99.end(); iter31++){
        printVector(*iter31);
    }
}

//正则匹配查找子串
int find_string_sub_match(const std::string &pattern, const std::string &str, std::vector <string> &output){
    pcre *re;
    const char *error;
    int erroffset;
    int rc, i;

    const char *regp = pattern.c_str();
    const char *cstr = str.c_str();
    size_t cstrlen = str.size();
    size_t start_offset = 0;

    //编译正则
    re = pcre_compile(regp, 0, &error, &erroffset, NULL);
    if (re == NULL){
        printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
        return -1;
    }

    while (start_offset < cstrlen){
        int ovector[PCRE_OVECCOUNT];
        rc = pcre_exec(re, NULL, cstr, strlen(cstr), start_offset, 0, ovector, PCRE_OVECCOUNT);
        if (rc < 0){
            if (rc == PCRE_ERROR_NOMATCH){
                printf("no match ...\n");
            }
            else{
                printf("Matching error %d\n", rc);
            }
            pcre_free(re);
            return -1;
        }

        for (i = 0; i < rc; i++){
            const char *substart = cstr + ovector[2 * i];
            int sublen = ovector[2 * i + 1] - ovector[2 * i];
            start_offset = ovector[2 * i + 1];
            char *match = (char *) malloc(sizeof(char) * (sublen + 1));
            bzero(match, sizeof(char) * (sublen + 1));
            memcpy(match, substart, sublen);
            output.push_back(match);
            free(match);
        }
    }

    return 0;
}
