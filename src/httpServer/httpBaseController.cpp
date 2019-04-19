/*************************************************************************
 * File:	httpBaseController.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-16 20:06
 ************************************************************************/
#include<iostream>
#include "httpBaseController.h"

using namespace std;

namespace cpputils{
    httpBaseController::httpBaseController(const httpRequest &r):req(r),rsp(NULL),dict(NULL){
		rsp = new httpResponse(r);
		string dictName = this->tplName;
		if(dictName.size() <= 0){
			dictName = "default";
		}
		dict = new ctemplate::TemplateDictionary(dictName);
    }

    void httpBaseController::beforeRun(){

    }

    void httpBaseController::afterRun(){

    }


    void httpBaseController::execute(string &output){
		string body;
    	ctemplate::Template *tpl;
    	tpl = ctemplate::Template::GetTemplate(this->tplName, ctemplate::DO_NOT_STRIP);
    	tpl->Expand(&body, this->dict);
		this->rsp->setBody(body);
		if(rspType == JSON){
			this->rsp->renderJson(output);
		}
		else if(rspType == HTML){
			this->rsp->renderHTML(output);
		}
    }

	httpBaseController::~httpBaseController(){
		free(rsp);
		free(dict);
	}
} //namespace cpputils
