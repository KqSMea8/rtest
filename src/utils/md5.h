/*************************************************************************
 * File:	md5.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019年03月21日 星期四 23时25分31秒
 ************************************************************************/

#ifndef _UTILS_MD5_H_
#define _UTILS_MD5_H_

#include <string>
#include <stdint.h>
#include <stddef.h>

namespace runtofuServer {
    class MD5 {
    private:

        uint32_t lo, hi;
        uint32_t a, b, c, d;
        unsigned char buffer[64];
        uint32_t block[16];

        //转换
        void make_digest(char *md5str, const unsigned char *digest);

        const void *body(const void *data, size_t size);

        void MD5Update(const void *data, size_t size);

        void MD5Final(unsigned char *result);

        void init();

    public:

        //MD5值的字符串表示
        int MD5Str(const void *input, size_t inputLen, char *output, size_t outputLen);

        //MD5值的原始值
        int MD5Raw(const void *input, size_t inputLen, unsigned char *output, size_t outputLen);

        //文件的MD5值的字符串表示
        int MD5FileStr(const char *file, char *output, size_t outputLen);

        //文件的MD5值的原始值
        int MD5FileRaw(const char *file, unsigned char *output, size_t outputLen);
    };

} //namespacecpputils
#endif // _UTILS_MD5_H_
