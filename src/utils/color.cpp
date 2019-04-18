/*************************************************************************
 * File:	color.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-03-21 21:17
 ************************************************************************/
#include<iostream>
#include<string>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdint.h>
#include<unistd.h>
#include "color.h"

using namespace std;
namespace cpputils {

    void Color::Green(const string &inStr, int blink, int underline, string &outStr) {
        cliColorRender(inStr, COLOR_GREEN, 0, blink, underline, outStr);
    }

    void Color::LightGreen(const string &inStr, int blink, int underline, string &outStr) {
        cliColorRender(inStr, COLOR_LIGHTGREEN, 1, blink, underline, outStr);
    }

    void Color::Cyan(const string &inStr, int blink, int underline, string &outStr) {
        cliColorRender(inStr, COLOR_CYAN, 0, blink, underline, outStr);
    }

    void Color::LightCyan(const string &inStr, int blink, int underline, string &outStr) {
        cliColorRender(inStr, COLOR_LIGHTCYAN, 1, blink, underline, outStr);
    }

    void Color::Red(const string &inStr, int blink, int underline, string &outStr) {
        cliColorRender(inStr, COLOR_RED, 0, blink, underline, outStr);
    }

    void Color::LightRed(const string &inStr, int blink, int underline, string &outStr) {
        cliColorRender(inStr, COLOR_LIGHTRED, 1, blink, underline, outStr);
    }

    void Color::Yellow(const string &inStr, int blink, int underline, string &outStr) {
        cliColorRender(inStr, COLOR_YELLOW, 0, blink, underline, outStr);
    }

    void Color::Black(const string &inStr, int blink, int underline, string &outStr) {
        cliColorRender(inStr, COLOR_BLACK, 0, blink, underline, outStr);
    }

    void Color::DarkGray(const string &inStr, int blink, int underline, string &outStr) {
        cliColorRender(inStr, COLOR_DARKGRAY, 1, blink, underline, outStr);
    }

    void Color::LightGray(const string &inStr, int blink, int underline, string &outStr) {
        cliColorRender(inStr, COLOR_LIGHTGRAY, 0, blink, underline, outStr);
    }

    void Color::White(const string &inStr, int blink, int underline, string &outStr) {
        cliColorRender(inStr, COLOR_WHITE, 1, blink, underline, outStr);
    }

    void Color::Blue(const string &inStr, int blink, int underline, string &outStr) {
        cliColorRender(inStr, COLOR_BLUE, 0, blink, underline, outStr);
    }

    void Color::LightBlue(const string &inStr, int blink, int underline, string &outStr) {
        cliColorRender(inStr, COLOR_LIGHTBLUE, 1, blink, underline, outStr);
    }

    void Color::Purple(const string &inStr, int blink, int underline, string &outStr) {
        cliColorRender(inStr, COLOR_PURPLE, 0, blink, underline, outStr);
    }

    void Color::LightPurple(const string &inStr, int blink, int underline, string &outStr) {
        cliColorRender(inStr, COLOR_LIGHTPURPLE, 1, blink, underline, outStr);
    }

    void Color::Brown(const string &inStr, int blink, int underline, string &outStr) {
        cliColorRender(inStr, COLOR_BROWN, 0, blink, underline, outStr);
    }

    //拼装渲染颜色
    void Color::cliColorRender(const string &in, ColorType color, int weight, int blink, int underline, string &out) {
        string modifier;
        if (blink > 0) {
            modifier.append("05;");
        }
        if (underline > 0) {
            modifier.append("04;");
        }
        if (weight > 0) {
            char tmpWeight[8] = {0};
            sprintf(tmpWeight, "%d;", weight);
            modifier.append(tmpWeight);
        }
        if (modifier.size() <= 0) {
            modifier.append("0;");
        }
        char tmpColor[8] = {0};
        sprintf(tmpColor, "%d", color);
        size_t sz = sizeof(char) * (in.size() * 2 + 64);
        char *tmpOutput = (char *) malloc(sz);
        bzero(tmpOutput, sz);
        sprintf(tmpOutput, "\033[%s%sm%s\033[0m", modifier.c_str(), tmpColor, in.c_str());
        out.append(tmpOutput);
        free(tmpOutput);
    }

    //打印并换行
    void Color::println(const string &in, ColorType color, int weight, int blink, int underline) {
        string out;
        cliColorRender(in, color, weight, blink, underline, out);
        cout << out << endl;
    }
} //namespacecpputils
