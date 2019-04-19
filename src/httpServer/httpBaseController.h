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
#include <ctemplate/template.h>
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

        //赋给模板的数据字典
        ctemplate::TemplateDictionary dict

    public:
        httpBaseController(httpRequest &r);

        virtual void beforeRun();

        virtual void run() = 0;

        virtual void afterRun();

        void execute(std::string &output) final;
    };
} //namespace cpputils


#endif //_CPPUTILS_HTTPBASECONTROLLER_H_201904162006_
