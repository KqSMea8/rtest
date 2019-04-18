/*************************************************************************
 * File:	byte_buffer.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	Mon 01 Apr 2019 03:03:45 PM CST
 ************************************************************************/

#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include "byte_buffer.h"

using namespace std;

namespace cpputils{
    ByteBuffer::ByteBuffer(){
        curPos = 0;
        totalCapacity = 0;
        dataSize = 0;
        data = NULL;
    }

    //增加空间
    void ByteBuffer::checkDataCapacity(size_t incr_size){
        if (totalCapacity > 0 && dataSize > 0 && totalCapacity - dataSize > incr_size){
            return;
        }
        if (incr_size < BYTE_BUFFER_INCREMENT_SIZE){
            incr_size = BYTE_BUFFER_INCREMENT_SIZE;
        }
        else{
            incr_size = (incr_size / BYTE_BUFFER_INCREMENT_SIZE + 1) * BYTE_BUFFER_INCREMENT_SIZE;
        }
        size_t newSize = sizeof(uint8_t) * (incr_size + totalCapacity);
        if (data == NULL){
            data = (uint8_t *) malloc(newSize);
            bzero(data, newSize);
        }
        else{
            data = (uint8_t *) realloc(data, newSize);
        }

        totalCapacity += incr_size;
    }

    ByteBuffer::~ByteBuffer(){
        free(data);
    }

    //前置++
    void ByteBuffer::operator++(){
        ++curPos;
        if (curPos >= dataSize){
            curPos = dataSize - 1;
        }
        if (curPos < 0){
            curPos = 0;
        }
        return;
    }

    //后置++
    void ByteBuffer::operator++(int n){
        curPos++;
        if (curPos >= dataSize){
            curPos = dataSize - 1;
        }
        if (curPos < 0){
            curPos = 0;
        }
        return;
    }

    void ByteBuffer::operator+=(size_t n){
        curPos += n;
        if (curPos >= dataSize){
            curPos = dataSize - 1;
        }
        if (curPos < 0){
            curPos = 0;
        }
        return;
    }

    uint8_t *ByteBuffer::operator[](size_t p){
        return data + p;
    }

    void ByteBuffer::operator=(size_t p){
        if (p < 0 || p >= dataSize){
            return;
        }
        curPos = p;
    }

    //添加字节数据
    void ByteBuffer::append(void *d, size_t len){
        checkDataCapacity(len);
        memcpy(data + dataSize, d, len);
        dataSize += len;
    }

    void ByteBuffer::appendInt(int d){
        return append(&d, sizeof(int));
    }

    void ByteBuffer::appendInt8(int8_t d){
        return append(&d, sizeof(int8_t));
    }

    void ByteBuffer::appendInt16(int16_t d){
        return append(&d, sizeof(int16_t));
    }

    void ByteBuffer::appendInt32(int32_t d){
        return append(&d, sizeof(int32_t));
    }

    void ByteBuffer::appendInt64(int64_t d){
        return append(&d, sizeof(int64_t));
    }

    void ByteBuffer::appendUint(unsigned int d){
        return append(&d, sizeof(unsigned int));
    }

    void ByteBuffer::appendUint8(uint8_t d){
        return append(&d, sizeof(uint8_t));
    }

    void ByteBuffer::appendUint16(uint16_t d){
        return append(&d, sizeof(uint16_t));
    }

    void ByteBuffer::appendUint32(uint32_t d){
        return append(&d, sizeof(uint32_t));
    }

    void ByteBuffer::appendUint64(uint64_t d){
        return append(&d, sizeof(uint64_t));
    }

    void ByteBuffer::appendSize(size_t d){
        return append(&d, sizeof(size_t));
    }

    void ByteBuffer::appendBool(bool d){
        return append(&d, sizeof(bool));
    }

    void ByteBuffer::appendFloat(float d){
        return append(&d, sizeof(float));
    }

    void ByteBuffer::appendDouble(double d){
        return append(&d, sizeof(double));
    }

    void ByteBuffer::appendString(const string &d){
        checkDataCapacity(d.size());
        memcpy(data + dataSize, d.c_str(), d.size());
        dataSize += d.size();
    }

    //操作指定位置上的字节
    uint8_t *ByteBuffer::at(size_t pos){
        return data + pos;
    }

    //转换方法
    int ByteBuffer::toInt(){
        int d = 0;
        getData(&d, sizeof(int));
        return d;
    }

    int8_t ByteBuffer::toInt8(){
        int8_t d = 0;
        getData(&d, sizeof(int8_t));
        return d;
    }

    int16_t ByteBuffer::toInt16(){
        int16_t d = 0;
        getData(&d, sizeof(int16_t));
        return d;
    }

    int32_t ByteBuffer::toInt32(){
        int32_t d = 0;
        getData(&d, sizeof(int32_t));
        return d;
    }

    int64_t ByteBuffer::toInt64(){
        int64_t d = 0;
        getData(&d, sizeof(int64_t));
        return d;
    }

    unsigned int ByteBuffer::toUint(){
        unsigned int d = 0;
        getData(&d, sizeof(unsigned int));
        return d;
    }

    uint8_t ByteBuffer::toUint8(){
        uint8_t d = 0;
        getData(&d, sizeof(uint8_t));
        return d;
    }

    uint16_t ByteBuffer::toUint16(){
        uint16_t d = 0;
        getData(&d, sizeof(uint16_t));
        return d;
    }

    uint32_t ByteBuffer::toUint32(){
        uint32_t d = 0;
        getData(&d, sizeof(uint32_t));
        return d;
    }

    uint64_t ByteBuffer::toUint64(){
        uint64_t d = 0;
        getData(&d, sizeof(uint64_t));
        return d;
    }

    size_t ByteBuffer::toSize(){
        size_t d = 0;
        getData(&d, sizeof(size_t));
        return d;
    }

    float ByteBuffer::toFloat(){
        float d = 0;
        getData(&d, sizeof(float));
        return d;
    }

    double ByteBuffer::toDouble(){
        double d = 0;
        getData(&d, sizeof(double));
        return d;
    }

    bool ByteBuffer::toBool(){
        bool d = false;
        getData(&d, sizeof(bool));
        return d;
    }

    void ByteBuffer::toString(string &s, size_t len){
        size_t cpos = curPos;
        if (len < 0){
            len = dataSize - cpos;
        }
        int i;
        for (i = cpos; i < dataSize && i < cpos + len; i++){
            char c = *(data + i);
            s.append((char *) &c);
        }
    }

    uint8_t *ByteBuffer::rawData(){
        return data;
    }

    void ByteBuffer::getData(void *d, size_t len){
        memcpy(d, data + curPos, len);
    }

    size_t ByteBuffer::size(){
        return dataSize;
    }

    size_t ByteBuffer::cPos(){
        return curPos;
    }
}