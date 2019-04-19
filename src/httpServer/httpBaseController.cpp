/*************************************************************************
 * File:	httpBaseController.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-16 20:06
 ************************************************************************/
#include<iostream>
#include "httpBaseController.h"

using namespace std;

namespace cpputils{
    httpBaseController::httpBaseController(httpRequest &r) : req(r){

    }

    void httpBaseController::beforeRun(){

    }

    void httpBaseController::afterRun(){

    }


    void httpBaseController::execute(string &output){

    }
} //namespace cpputils
