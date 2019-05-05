/*************************************************************************
 * File:	test.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019年03月21日 星期四 17时08分17秒
 ************************************************************************/

#include<iostream>
#include<string>
#include<sstream>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdint.h>
#include<unistd.h>
#include "../src/utils/strutils.h"
#include "../src/utils/md5.h"
#include "../src/utils/color.h"
#include "../src/utils/byte_buffer.h"
#include "../src/utils/some.h"
#include "../src/utils/regexp.h"
#include "../src/utils/rune_utils.h"
#include "../src/client/http/http_client_utils.h"
#include "../src/httpServer/httpRequest.h"

using namespace std;

const char *headersStrTmp = "GET /add1.php HTTP/1.1\r\n"
                            "Connection: keep-alive\r\n"
                            "Host: wendao.abc.com\r\n"
                            "Origin: http://wendao.abc.com\r\n"
                            "User-Agent: Mozilla/5.0 (iPhone; CPU iPhone OS 6_1_4 like Mac OS X) AppleWebKit/536.26 (KHTML, like Gecko) Version/6.0 Mobile/10B350 Safari/8536.25\r\n"
                            "X-Requested-With: XMLHttpRequest\r\n"
                            "Referer: http://wendao.abc.com/abc/def/ssssss/?id=8554443825&gin=987532\r\n"
                            "Content-Type: multipart/form-data; boundary=----WebKitFormBoundary5ca6082fa3e74\r\n"
                            "Content-Length: 91099\r\n"
                            "Cookie: ssuid=65535\r\n"
                            ""
                            "sffasfdassfdasdfasdfsasdfsasfdsasfd";

void printVector1(const std::vector <string> &output){
    vector< string >::const_iterator iter;
    cout << endl;
    cout << endl;
    for (iter = output.begin(); iter != output.end(); iter++){
        cout << *iter << endl;
    }
    cout << endl;
    cout << endl;
}

void printMap1(const std::map <string, string> &output){
    map< string, string >::const_iterator iter;
    cout << endl;
    cout << endl;
    for (iter = output.begin(); iter != output.end(); iter++){
        cout << "k=" << iter->first << "\tv=" << iter->second << endl;
    }
    cout << endl;
    cout << endl;
}

int main(){
    string s = "C/C++语言提供了几个标准库函数，可以将字符串转换为任意类型(整型、长整型、浮点型等)。";
    vector <int32_t> unicodes;
    runtofuServer::StrUtils::utf8ToUnicode(s, unicodes);
    vector< int32_t >::iterator iter;

    for (iter = unicodes.begin(); iter != unicodes.end(); iter++){
        cout << *iter << "\t";
    }
    cout << endl;

    //再转回来
    string str;
    runtofuServer::StrUtils::unicodesToUTF8(unicodes, str);
    cout << str << endl;

    //计算md5
    char buf[33] = {0};
    runtofuServer::MD5 md5;
    md5.MD5Str(s.c_str(), s.size(), buf, 33);
    cout << buf << endl;

    string rawStr = "各地区、各部门要抓紧做好本地区、本领域的劳动节假期调整落实工作，采取有效措施，保障交通运力，加强旅游服务，丰富产品供给，强化综合调控，确保平稳运行。";
    string b64en;
    string b64de;
    runtofuServer::StrUtils::base64Encode(rawStr, b64en);
    runtofuServer::StrUtils::base64Decode(b64en, b64de);
    cout << rawStr << endl;
    cout << b64en << endl;
    cout << b64de << endl;

    string rawUrl = "http://wiki.intra.xiaojukeji.com/pages/viewpage.action?pageId=114204118#id-4.计价&收银服务-术语说明";
    string urlEn;
    string urlDe;
    runtofuServer::StrUtils::urlEncode(rawUrl, urlEn);
    runtofuServer::StrUtils::urlDecode(urlEn, urlDe);
    cout << rawUrl << endl;
    cout << urlEn << endl;
    cout << urlDe << endl;

    string out1;
    string out2;
    runtofuServer::Color::Green(rawStr, 1, 1, out1);
    runtofuServer::Color::Yellow(rawStr, 0, 0, out2);
    cout << out1 << endl;
    cout << out2 << endl;

    size_t screenWidth, screenHeight;
    runtofuServer::SomeUtils::getTerminalSize(&screenWidth, &screenHeight);
    cout << screenWidth << "\t" << screenHeight << endl;

    string trimSp1 = "  c\n\t   ";
    runtofuServer::StrUtils::trimSpace(trimSp1);
    cout << "trimSp=" << trimSp1 << "\tsize=" << trimSp1.size() << endl;
    string trimSp2 = "  c\n\t   ";
    runtofuServer::StrUtils::trimLeftSpace(trimSp2);
    cout << "trimSp2=" << trimSp2 << "\tsize=" << trimSp2.size() << endl;
    string trimSp3 = "  c\n\t   ";
    runtofuServer::StrUtils::trimRightSpace(trimSp3);
    cout << "trimSp3=" << trimSp3 << "\tsize=" << trimSp3.size() << endl;

    string trimCH1 = "abcaaaaaaa";
    runtofuServer::StrUtils::trimChar(trimCH1, 'a');
    cout << "trimCH1=" << trimCH1 << "\tsize=" << trimCH1.size() << endl;

    runtofuServer::RegExp reg1(";");
    string ck = "a=c;d=f;a=adsfadfadfadfadfadf;d=453453453534;;;;;;;;-";
    vector <string> reg1SubList;
    reg1.reg_split(ck, reg1SubList);
    printVector1(reg1SubList);

    cout << "-------runtofuServer::StrUtils::strSplit-------" << endl;
    vector <string> split2SubList;
    runtofuServer::StrUtils::strSplit(ck, ';', split2SubList);
    printVector1(split2SubList);
    cout << endl;

    cout << "-------runtofuServer::HttpClientUtils::splitRawCookie-------" << endl;
    map <string, string> cookie2SubList;
    runtofuServer::HttpClientUtils::splitRawCookie(ck, cookie2SubList);
    printMap1(cookie2SubList);
    string ck1;
    runtofuServer::HttpClientUtils::joinRawCookie(cookie2SubList, ck1);
    cout << ck1 << endl;
    cout << endl;

    cout << "---------/dev/urandom---------" << endl;
    string deviceRandom1;
    runtofuServer::SomeUtils::getDeviceRandom(deviceRandom1, 30);
    cout << deviceRandom1 << endl;
    cout << endl;

    cout << "---------local ip---------" << endl;
    vector <uint32_t> ipList;
    runtofuServer::SomeUtils::getLocalIP(ipList);
    cout << endl;

    cout << "---------clock---------" << endl;
    uint64_t ms, ns;
    runtofuServer::SomeUtils::getMicroSeconds(ms);
    runtofuServer::SomeUtils::getNanoSeconds(ns);
    cout << "ms=" << ms << "\tns=" << ns << endl;
    cout << endl;

    cout << "---------random---------" << endl;
    int i;
    for (i = 0; i < 10; i++){
        int r = runtofuServer::SomeUtils::getRandom();
        cout << "random int=" << r << endl;
    }
    cout << endl;

    cout << "---------userAgent---------" << endl;
    string ua;
    for (i = 0; i < 10; i++){
        runtofuServer::HttpClientUtils::genRandomPCUserAgent(ua);
        cout << "PC-User-Agent=" << ua << endl;
        ua.clear();
    }
    for (i = 0; i < 10; i++){
        runtofuServer::HttpClientUtils::genRandomMobileUserAgent(ua);
        cout << "Mobile-User-Agent=" << ua << endl;
        ua.clear();
    }
    cout << endl;

    cout << "---------isFileExists---------" << endl;
    const char *fs[] = {
            "./str.cpp",
            "./abc",
            "/dev",
            "wendao",
    };
    size_t fsLen = sizeof(fs) / sizeof(char *);
    for (i = 0; i < fsLen; i++){
        const char *f = fs[i];
        bool r = runtofuServer::SomeUtils::isFileExist(f);
        cout << "file[" << f << "] isExist:" << r << endl;
        string bname;
        runtofuServer::SomeUtils::fileBaseName(f, bname);
        cout << "file[" << f << "] basename:" << bname << endl;
    }
    cout << endl;

    cout << "---------fileSize & getContent---------" << endl;
    string tmpFilePath = "/Users/liuyongshuai/Documents/service.conf";
    if (runtofuServer::SomeUtils::isFileExist(tmpFilePath)){
        size_t fileSize = runtofuServer::SomeUtils::fileSize(tmpFilePath);
        cout << "fileSize=" << fileSize << endl;
        char *tmpFileBuf = (char *) malloc(sizeof(char) * fileSize + 1);
        bzero(tmpFileBuf, sizeof(char) * fileSize + 1);
        runtofuServer::SomeUtils::getFileContent(tmpFilePath, tmpFileBuf);
        cout << "getFileContent=" << endl;
    }

    cout << endl;
    //cout << tmpFileBuf << endl;

    cout << "---------byte_buffer---------" << endl;
    runtofuServer::ByteBuffer bb;
    bb.appendInt(12345);
    //bb++;
    cout << "int64_t=" << bb.toInt64() << endl;
    cout << "uint64_t=" << bb.toUint64() << endl;
    string dddd;
    bb = 0;
    bb.toString(dddd);
    cout << "toString=" << dddd << endl;
    cout << endl;


    cout << "---------httpRequest---------" << endl;
    cout << "httpRequest::GET=" << runtofuServer::httpRequest::GET << endl;
    cout << "httpRequest::POST=" << runtofuServer::httpRequest::POST << endl;
    cout << "httpRequest::PUT=" << runtofuServer::httpRequest::PUT << endl;
    cout << "httpRequest::HEAD=" << runtofuServer::httpRequest::HEAD << endl;
    cout << "httpRequest::DELETE=" << runtofuServer::httpRequest::DELETE << endl;
    cout << endl;

    cout << "---------isAllNumber---------" << endl;
    cout << runtofuServer::SomeUtils::isAllNumber("444444444") << endl;
    cout << runtofuServer::SomeUtils::isAllNumber("444444f444") << endl;
    cout << runtofuServer::SomeUtils::isAllNumber("0") << endl;
    cout << runtofuServer::SomeUtils::isAllNumber("") << endl;
};
