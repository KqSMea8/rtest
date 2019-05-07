/*************************************************************************
 * File:	color.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-03-21 21:17
 ************************************************************************/
#ifndef _UTILS_COLOR_H_
#define _UTILS_COLOR_H_

#include<string>
#include<map>

using namespace std;

namespace rtest {
    //颜色用的方法
    typedef void (*ColorFunc)(const string &, int, int, string &);

    typedef int ColorType;

    class Color {
    private:
        Color() {};

        Color(const Color &) {};

        Color &operator=(const Color &) {};
    public:
        const static ColorType COLOR_GREEN = 32;//绿色字体
        const static ColorType COLOR_LIGHTGREEN = 32;//淡绿
        const static ColorType COLOR_CYAN = 36;//青色/蓝绿色
        const static ColorType COLOR_LIGHTCYAN = 36;//淡青色
        const static ColorType COLOR_RED = 31;//红字体
        const static ColorType COLOR_LIGHTRED = 31;//淡红色
        const static ColorType COLOR_YELLOW = 33;//黄色字体
        const static ColorType COLOR_BLACK = 30;//黑色
        const static ColorType COLOR_DARKGRAY = 30;//深灰色
        const static ColorType COLOR_LIGHTGRAY = 37;//浅灰色
        const static ColorType COLOR_WHITE = 37;//白色
        const static ColorType COLOR_BLUE = 34;//蓝色
        const static ColorType COLOR_LIGHTBLUE = 34;//淡蓝
        const static ColorType COLOR_PURPLE = 35;//紫色
        const static ColorType COLOR_LIGHTPURPLE = 35;//淡紫色
        const static ColorType COLOR_BROWN = 33;//棕色

        static void Green(const string &inStr, int blink, int underline, string &outStr);

        static void LightGreen(const string &inStr, int blink, int underline, string &outStr);

        static void Cyan(const string &inStr, int blink, int underline, string &outStr);

        static void LightCyan(const string &inStr, int blink, int underline, string &outStr);

        static void Red(const string &inStr, int blink, int underline, string &outStr);

        static void LightRed(const string &inStr, int blink, int underline, string &outStr);

        static void Yellow(const string &inStr, int blink, int underline, string &outStr);

        static void Black(const string &inStr, int blink, int underline, string &outStr);

        static void DarkGray(const string &inStr, int blink, int underline, string &outStr);

        static void LightGray(const string &inStr, int blink, int underline, string &outStr);

        static void White(const string &inStr, int blink, int underline, string &outStr);

        static void Blue(const string &inStr, int blink, int underline, string &outStr);

        static void LightBlue(const string &inStr, int blink, int underline, string &outStr);

        static void Purple(const string &inStr, int blink, int underline, string &outStr);

        static void LightPurple(const string &inStr, int blink, int underline, string &outStr);

        static void Brown(const string &inStr, int blink, int underline, string &outStr);

        static void cliColorRender(const string &in, ColorType color, int weight, int blink, int underline, string &out);

        static void println(const string &in, ColorType color, int weight, int blink, int underline);
    };

} //namespacecpputils
#endif //_UTILS_COLOR_H_
