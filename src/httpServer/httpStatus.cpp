/*************************************************************************
 * File:	httpStatus.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-04 17:34
 ************************************************************************/
#include<iostream>
#include "httpStatus.h"

using namespace std;

namespace cpputils{
    map <uint32_t, string> httpStatus::statusMap;

    //获取各状态的描述信息
    void httpStatus::statusDesc(HTTP_STATUS status, std::string &desc){
        if (statusMap.size() <= 0){
            statusMap[CONTINUE] = "Continue";
            statusMap[SWITCHING_PROTOCOLS] = "Switching Protocols";
            statusMap[OK] = "OK";
            statusMap[CREATED] = "Created";
            statusMap[ACCEPTED] = "Accepted";
            statusMap[NON_AUTHORITATIVE_INFORMATION] = "Non-Authoritative Information";
            statusMap[NO_CONTENT] = "No Content";
            statusMap[RESET_CONTENT] = "Reset Content";
            statusMap[PARTIAL_CONTENT] = "Partial Content";
            statusMap[MULTIPLE_CHOICES] = "Multiple Choices";
            statusMap[MOVED_PERMANENTLY] = "Moved Permanently";
            statusMap[FOUND] = "Found";
            statusMap[SEE_OTHER] = "See Other";
            statusMap[NOT_MODIFIED] = "Not Modified";
            statusMap[USE_PROXY] = "Use Proxy";
            statusMap[UNUSED] = "Unused";
            statusMap[TEMPORARY_REDIRECT] = "Temporary Redirect";
            statusMap[BAD_REQUEST] = "Bad Request";
            statusMap[UNAUTHORIZED] = "Unauthorized";
            statusMap[PAYMENT_REQUIRED] = "Payment Required";
            statusMap[FORBIDDEN] = "Forbidden";
            statusMap[NOT_FOUND] = "Not Found";
            statusMap[METHOD_NOT_ALLOWED] = "Method Not Allowed";
            statusMap[NOT_ACCEPTABLE] = "Not Acceptable";
            statusMap[PROXY_AUTHENTICATION_REQUIRED] = "Proxy Authentication Required";
            statusMap[REQUEST_TIME_OUT] = "Request Time out";
            statusMap[CONFLICT] = "Conflict";
            statusMap[GONE] = "Gone";
            statusMap[LENGTH_REQUIRED] = "Length Required";
            statusMap[PRECONDITION_FAILED] = "Precondition Failed";
            statusMap[REQUEST_ENTITY_TOO_LARGE] = "Request Entity Too Large";
            statusMap[REQUEST_URI_TOO_LARGE] = "Request-URI Too Large";
            statusMap[UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
            statusMap[REQUESTED_RANGE_NOT_SATISFIABLE] = "Requested range not satisfiable";
            statusMap[EXPECTATION_FAILED] = "Expectation Failed";
            statusMap[INTERNAL_SERVER_ERROR] = "Internal Server Error";
            statusMap[NOT_IMPLEMENTED] = "Not Implemented";
            statusMap[BAD_GATEWAY] = "Bad Gateway";
            statusMap[SERVICE_UNAVAILABLE] = "Service Unavailable";
            statusMap[GATEWAY_TIME_OUT] = "Gateway Time out";
            statusMap[VERSION_NOT_SUPPORTED] = "HTTP Version not supported";
        }
        map< uint32_t, string >::const_iterator iter;
        iter = statusMap.find(status);
        if (iter == statusMap.end()){
            desc = "unknown status";
        }
        else{
            desc = iter->second;
        }
    }
} //namespace cpputils
