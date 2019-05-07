/*************************************************************************
 * File:	singletonBase.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-19 19:23
 ************************************************************************/
#ifndef _RUNTOFUSERVER_SINGLETONBASE_H_201904191923_
#define _RUNTOFUSERVER_SINGLETONBASE_H_201904191923_

namespace rtest{
    class singletonBase{
    protected:
        singletonBase(){}

        ~singletonBase(){}

    private:
        singletonBase(const singletonBase &);

        const singletonBase &operator=(const singletonBase &);
    };
} //namespace rtest


#endif //_RUNTOFUSERVER_SINGLETONBASE_H_201904191923_
