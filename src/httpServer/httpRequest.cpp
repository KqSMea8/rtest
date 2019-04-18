/*************************************************************************
 * File:	httpRequest.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-04 14:38
 ************************************************************************/
#include<iostream>
#include<sstream>
#include<stdio.h>
#include<string.h>
#include "httpRequest.h"
#include "../utils/strutils.h"
#include "../utils/regexp.h"
#include "../utils/color.h"
#include "../utils/terminal_table.h"

using namespace std;

namespace cpputils{
    httpRequest::httpRequest(const void *body, size_t len){
        this->init(body, len);
    }

    httpRequest::~httpRequest(){

    }

    //初始化解析请求信息
    int httpRequest::init(const void *body, size_t len){
        cout << "initLen=" << len << endl;
        if (len == 0){
            return -1;
        }
        const char *sepChar1 = "\r\n";
        const char *sepChar2 = "\r\n\r\n";
        char tmpBuf[HTTP_HEADER_BUF_SIZE];
        string tmpStr;
        vector <string> tmpVec;
        size_t sz, pos;
        vector< string >::const_iterator vecStrIter;
        const char *tmpReqBody = (char *) body;
        const char *tmpHeaderCur, *tmpHeaderPre = tmpReqBody;
        //以类似于"GET /abc HTTP/1.1"开头
        tmpHeaderCur = strstr(tmpReqBody, sepChar1);
        if (tmpHeaderCur == NULL){
            return -1;
        }
        sz = tmpHeaderCur - tmpHeaderPre;
        bzero(tmpBuf, HTTP_HEADER_BUF_SIZE);
        memcpy(tmpBuf, tmpHeaderPre, sz);
        tmpStr = tmpBuf;
        StrUtils::strSplit(tmpStr, ' ', tmpVec);
        if (tmpVec.size() < 3){
            return -1;
        }
        //method
        StrUtils::trimSpace(tmpVec[0]);
        StrUtils::toUpper(tmpVec[0]);
        if (tmpVec[0] == "GET"){
            method = GET;
        }
        else if (tmpVec[0] == "GET"){
            method = GET;
        }
        else if (tmpVec[0] == "POST"){
            method = POST;
        }
        else if (tmpVec[0] == "PUT"){
            method = PUT;
        }
        else if (tmpVec[0] == "HEAD"){
            method = HEAD;
        }
        else if (tmpVec[0] == "DELETE"){
            method = DELETE;
        }
        else{
            return -1;
        }
        //uri
        StrUtils::trimSpace(tmpVec[1]);
        uri = tmpVec[1];
        if ((pos = uri.find_first_of("?", 0)) != string::npos){
            string argStr(uri, pos + 1);
            this->parseArgs(argStr);
        }
        //protocol
        StrUtils::trimSpace(tmpVec[2]);
        protocol = tmpVec[2];
        tmpHeaderCur += 2;
        tmpHeaderPre = tmpReqBody + len;
        //有消息体的请求类型
        if ((tmpHeaderPre = strstr(tmpHeaderCur, sepChar2)) == NULL){
            tmpHeaderPre = tmpReqBody + len;
        }
        tmpStr.clear();
        tmpVec.clear();
        tmpStr.append(tmpHeaderCur, tmpHeaderPre - tmpHeaderCur);
        RegExp reg(sepChar1);
        reg.reg_split(tmpStr, tmpVec);
        if (tmpVec.size() > 0){
            for (vecStrIter = tmpVec.begin(); vecStrIter != tmpVec.end(); vecStrIter++){
                pos = vecStrIter->find_first_of(":", 0);
                if (pos == string::npos || pos == 0){
                    continue;
                }
                string k(*vecStrIter, 0, pos);
                string v(*vecStrIter, pos + 1);
                StrUtils::toLower(k);
                headers[k] = v;
            }
        }

        //如果是get/head/delete可以返回了
        if (method == GET || method == HEAD || method == DELETE){
            return 0;
        }

        tmpHeaderPre += 4;
        if (tmpHeaderPre >= tmpReqBody + len){
            return 0;
        }
        //body的长度
        sz = tmpReqBody + len - tmpHeaderPre;
        rawBody = (char *) malloc(sizeof(char) * (sz + 1));
        bzero(rawBody, sizeof(char) * (sz + 1));
        memcpy(rawBody, tmpHeaderPre, sz);
        rawBodyLen = sz;

        //如果是普通的POST
        string cntType;
        string upBoundary;
        this->getHeader("Content-Type", cntType);
        if (cntType.find_first_of(";", 0) != string::npos){
            tmpVec.clear();
            StrUtils::strSplit(cntType, ';', tmpVec);
            for (vecStrIter = tmpVec.begin(); vecStrIter != tmpVec.end(); vecStrIter++){
                string tmpCntType = *vecStrIter;
                StrUtils::trimSpace(tmpCntType);
                if (tmpCntType.find_first_of("/", 0) != string::npos){
                    cntType = tmpCntType;
                }
                else if (tmpCntType.find_first_of("boundary=", 0) == 0){
                    upBoundary = tmpCntType.substr(9);
                }
            }
        }
        StrUtils::trimSpace(cntType);
        StrUtils::toLower(cntType);
        if (cntType == NORMAL_POST_CONTENT_TYPE){
            this->parseArgs(rawBody);
            return 0;
        }
        //如果不是上传文件
        if (cntType != UPLOAD_POST_CONTENT_TYPE){
            return 0;
        }

        //解析以multipart/form-data上传的字段或文件
        this->parseMultiPartFormDataArgs(upBoundary.c_str());
        return 0;
    }

    //是否为ajax请求
    bool httpRequest::isAjax(){
        string v;
        this->getHeader("X-Requested-With", v);
        return v == "XMLHttpRequest";
    }

    //提取header信息
    void httpRequest::getHeader(const std::string &key, std::string &val){
        string k = key;
        StrUtils::toLower(k);
        map< string, string >::const_iterator iter;
        if ((iter = headers.find(k)) != headers.end()){
            val = iter->second;
        }
    }

    //解析请求参数
    void httpRequest::parseArgs(const std::string &argStr){
        vector <string> tmpArgVec;
        size_t pos;
        StrUtils::strSplit(argStr, '&', tmpArgVec);
        vector< string >::const_iterator vecStrIter;
        for (vecStrIter = tmpArgVec.begin(); vecStrIter != tmpArgVec.end(); vecStrIter++){
            pos = vecStrIter->find_first_of("=", 0);
            if (pos == string::npos){
                continue;
            }
            string k(*vecStrIter, 0, pos);
            string v(*vecStrIter, pos + 1);
            this->args[k].push_back(v);
        }
    }


    /**
     * 简单的解析参数，请求的Content-Type必须是multipart/form-data，如：
     *      此类请求中，一个参数的开始是 "--" + boundary
     *      然后加上参数的Header信息，格式为字段名和字段值，二者之间使用“：”连接，如：Content-Type：text/plain
     *      加上一个空行（\r\n）
     *      发送的参数值
     *      整个请求的报文数据以 "--" + boundary + "--" 结束
     * 如下，boundary = "----WebKitFormBoundary5cab1a0c5b5f5"
     * ------WebKitFormBoundary5cab1a0c5b5f5
     * Content-Disposition: form-data; name="key"
     * Content-Type: text/plain; charset=UTF-8  //可省略
     *
     * value
     * ------WebKitFormBoundary5cab1a0c5b5f5
     * ..........其他请求参数........
     * ------WebKitFormBoundary5cab1a0c5b5f5--
     * @param boundary
     */
    void httpRequest::parseMultiPartFormDataArgs(const char *boundary){
        const char *tmpBuf = rawBody;
        const char *cur, *next, *next1, *tmpPtr = NULL;
        string tmpBoundary = boundary;
        tmpBoundary.append("\r\n");
        size_t i, pos;
        //寻找分界符
        while ((cur = strstr(tmpBuf, tmpBoundary.c_str())) != NULL){
            string fieldName, fieldValue, fileName, fileContentType;
            cur += tmpBoundary.size();
            string tmpPart;
            tmpPtr = cur;
            //先读一行，判断是字段还是文件
            while (true){
                if (*tmpPtr == '\r' && tmpPtr < rawBody + rawBodyLen && *(tmpPtr + 1) == '\n'){
                    break;
                }
                tmpPart.append(tmpPtr++, 1);
            }
            StrUtils::trimSpace(tmpPart);
            //非法数据
            if (tmpPart.size() == 0 || tmpPart.find("Content-Disposition") == string::npos){
                return;
            }
            vector <string> tmpVec;
            StrUtils::strSplit(tmpPart, ';', tmpVec);
            if (tmpVec.size() == 0){
                return;
            }
            for (i = 0; i < tmpVec.size(); i++){
                StrUtils::trimSpace(tmpVec[i]);
                if (tmpVec[i].find("filename=") == 0){
                    fileName = tmpVec[i].substr(9);
                    StrUtils::trimChar(fileName, '"');
                    continue;
                }
                if (tmpVec[i].find("name=") == 0){
                    fieldName = tmpVec[i].substr(5);
                    StrUtils::trimChar(fieldName, '"');
                    continue;
                }
            }
            //字段
            if (fileName.size() == 0){
                //跳过\r\n\r\n
                tmpPtr += 4;
                while (true){
                    if (*tmpPtr == '\r' && tmpPtr < rawBody + rawBodyLen && *(tmpPtr + 1) == '\n'){
                        break;
                    }
                    fieldValue.append(tmpPtr++, 1);
                }
                tmpPtr += 2;
                tmpBuf = tmpPtr;
                this->args[fieldName].push_back(fieldValue);
                continue;
            }
            //如果是文件的话，下一行是Content-Type
            tmpPtr += 2;
            string tmpFileContentType;
            while (true){
                if (*tmpPtr == '\r' && tmpPtr < rawBody + rawBodyLen && *(tmpPtr + 1) == '\n'){
                    break;
                }
                tmpFileContentType.append(tmpPtr++, 1);
            }
            if ((pos = tmpFileContentType.find(":")) != string::npos){
                fileContentType = tmpFileContentType.substr(pos + 1);
                StrUtils::trimSpace(fileContentType);
            }
            //跳过\r\n\r\n
            tmpPtr += 4;
            next = tmpPtr;
            cout << "fileName=" << fileName << "\tfieldName=" << fieldName << "\tfileContentType=" << fileContentType << endl;
            cout << "remain size=" << rawBody + rawBodyLen - tmpPtr << "\trawBodyLen=" << rawBodyLen << endl;
            //搜索\r，下一个字符必须是\n，前面的字符串必须是分界符
            cout << "tmpBoundary.size()=" << tmpBoundary.size() << "\ttmpBoundary=" << tmpBoundary << endl;
            while (next < rawBody + rawBodyLen){
                next++;
                if (next - tmpPtr < tmpBoundary.size()){
                    continue;
                }
                //分找分界符了，回退到上一个\r\n
                if (*next == '\r' && *(next + 1) == '\n'){
                    string abc(next - tmpBoundary.size(), tmpBoundary.size());
                    if (abc.find(boundary) != string::npos){
                        cout << "find\t" << tmpBoundary << endl;
                        while (true){
                            next--;
                            if (*next == '\r' && *(next + 1) == '\n'){
                                break;
                            }
                        }
                        break;
                    }

                }
            }
            if (next == NULL){
                cout << "next is NULL" << endl;
                return;
            }
            UPLOAD_FILE upF;
            upF.contentType = fileContentType;
            upF.fileLen = next - tmpPtr;
            cout << "upF.fileLen=" << upF.fileLen << endl;
            upF.fileName = fileName;
            upF.fileBuf = (char *) malloc(sizeof(char) * upF.fileLen);
            memcpy(upF.fileBuf, tmpPtr, upF.fileLen);
            uploadFiles[fileName] = upF;
            tmpBuf = next;
        }
    }

    //打印解析出来的请求信息
    void httpRequest::printReq(){
        cout << "method=" << method << endl;
        cout << "uri=" << uri << endl;
        cout << "protocol=" << protocol << endl;
        map< string, string >::const_iterator iter;
        string sepline;

        TerminalTable tHeader;
        tHeader.addHeadData(2, "key", "value");
        for (iter = headers.begin(); iter != headers.end(); iter++){
            tHeader.addRowData(2, iter->first.c_str(), iter->second.c_str());
        }
        string tstr;
        tHeader.renderToString(tstr);
        cout << tstr;
        cout << endl;
        if (args.size() > 0){
            cout << endl;
            map < string, vector < string > > ::const_iterator
            mapVecIter;
            TerminalTable tArgs;
            tArgs.addHeadData(2, "key", "value");
            for (mapVecIter = args.begin(); mapVecIter != args.end(); mapVecIter++){
                string v;
                StrUtils::strJoin(mapVecIter->second, "\n", v);
                tArgs.addRowData(2, mapVecIter->first.c_str(), v.c_str());
            }
            string tstr;
            tArgs.renderToString(tstr);
            cout << tstr;
            cout << endl;
        }

        map< string, UPLOAD_FILE >::const_iterator fileIter;
        for (fileIter = uploadFiles.begin(); fileIter != uploadFiles.end(); fileIter++){
            FILE *fp = fopen("/Users/liuyongshuai/Documents/Zoom/abc.pdf", "w+");
            if (fp){
                fwrite(fileIter->second.fileBuf, fileIter->second.fileLen, 1, fp);
                fclose(fp);
            }
            cout << "fileName=" << fileIter->second.fileName << "\tfileLen=" << fileIter->second.fileLen << endl;
        }
    }
} //namespace cpputils
