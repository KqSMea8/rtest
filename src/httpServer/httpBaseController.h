/*************************************************************************
 * File:	httpBaseController.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-16 20:06
 ************************************************************************/
#ifndef _CPPUTILS_HTTPBASECONTROLLER_H_201904162006_
#define _CPPUTILS_HTTPBASECONTROLLER_H_201904162006_

#include<string>
#include<vector>
#include<map>
#include "httpRequest.h"
#include "httpResponse.h"
#include "httpStatus.h"

namespace cpputils{
    class httpBaseController{
    private:
        enum{
            HTML, JSON
        } OUTPUT_TYPE;

        //要执行的模板名称
        string tplName;

        //响应类型
        OUTPUT_TYPE rspType;

        //请求信息
        httpRequest &req;

        //响应信息
        httpResponse rsp;

    public:
        httpBaseController(httpRequest &r);

        virtual void beforeRun();

        virtual void run() = 0;

        virtual void afterRun();
    };
} //namespace cpputils


#endif //_CPPUTILS_HTTPBASECONTROLLER_H_201904162006_
