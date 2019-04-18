/*************************************************************************
 * File:	byte_buffer.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	Mon 01 Apr 2019 03:03:39 PM CST
 ************************************************************************/

#ifndef _UTILS_BYTE_BUFFER_H_
#define _UTILS_BYTE_BUFFER_H_

#include <iostream>
#include <vector>
#include <map>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <unistd.h>
#include <memory.h>

using namespace std;

//每次增长的总大小
#define BYTE_BUFFER_INCREMENT_SIZE 256

namespace cpputils{
    class ByteBuffer{
    private:
        uint8_t *data; //数据位置
        size_t curPos; //当前操作的位置
        size_t dataSize; //当前数据的总大小
        size_t totalCapacity;//总容量大小

        //增加空间，
        void checkDataCapacity(size_t incr_size);

    public:
        ByteBuffer();

        ~ByteBuffer();

        //前置++
        void operator++();

        //后置++
        void operator++(int n);

        void operator+=(size_t n);

        uint8_t *operator[](size_t p);

        void operator=(size_t p);

        //添加字节数据
        void append(void *d, size_t len);

        void appendInt(int d);

        void appendInt8(int8_t d);

        void appendInt16(int16_t d);

        void appendInt32(int32_t d);

        void appendInt64(int64_t d);

        void appendUint(unsigned int d);

        void appendUint8(uint8_t d);

        void appendUint16(uint16_t d);

        void appendUint32(uint32_t d);

        void appendUint64(uint64_t d);

        void appendSize(size_t d);

        void appendBool(bool d);

        void appendFloat(float d);

        void appendDouble(double d);

        void appendString(const string &d);

        //操作指定位置上的字节
        uint8_t *at(size_t pos);

        //转换方法
        int toInt();

        int8_t toInt8();

        int16_t toInt16();

        int32_t toInt32();

        int64_t toInt64();

        unsigned int toUint();

        uint8_t toUint8();

        uint16_t toUint16();

        uint32_t toUint32();

        uint64_t toUint64();

        size_t toSize();

        float toFloat();

        double toDouble();

        bool toBool();

        void toString(string &s, size_t len = -1);

        uint8_t *rawData();

        void getData(void*d,size_t len);

        size_t size();

        size_t cPos();
    };
}
#endif
