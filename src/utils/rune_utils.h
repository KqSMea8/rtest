/*************************************************************************
 * 翻译自：https://github.com/mattn/go-runewidth
 *
 * File:	rune_utils.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019年03月21日 星期四 21时21分20秒
 ************************************************************************/

#ifndef _UTILS_RUNE_WIDTH_H_
#define _UTILS_RUNE_WIDTH_H_

#include<stdint.h>
#include<stdarg.h>

namespace cpputils {
    class RuneUtils {
    private:
        RuneUtils() {};

        RuneUtils(const RuneUtils &) {};

        RuneUtils &operator=(const RuneUtils &) {};
        typedef struct _runeInterval {
            int32_t first;
            int32_t last;
        } runeInterval;

        static runeInterval privateTable[];
        static size_t privateTableLen;

        static runeInterval nonPrintTable[];
        static size_t nonPrintTableLen;

        static runeInterval combiningTable[];
        static size_t combiningTableLen;

        static runeInterval doubleWidthTable[];
        static size_t doubleWidthTableLen;

        static runeInterval ambiguousTable[];
        static size_t ambiguousTableLen;

        static runeInterval emojiTable[];
        static size_t emojiTableLen;

        static runeInterval notAssignedTable[];
        static size_t notAssignedTableLen;

        static runeInterval neutralTable[];
        static size_t neutralTableLen;

        static int isRuneInTable(int32_t rune, runeInterval *table, size_t tableLen);

        static int isRuneInTables(int32_t rune, int count, ...);

        //是否为东亚语系
        static void getIsEastAsianLocale();

        static int isEastAsianLocale;
    public:
        //单个字符宽度
        static size_t runeWidth(int32_t rune);

        //一串字符串宽度
        static size_t runeStringWidth(const std::string &str);

        //在右边补齐到指定宽度
        static void runeFillRight(const std::string &str, size_t width, std::string &output);

        //在左边补齐到指定宽度
        static void runeFillLeft(const std::string &str, size_t width, std::string &output);

        //将一段字符串折成多行，每行的宽度最多为width
        static void runeWrap(const std::string &str, size_t width, std::string &output, size_t &lineNum);

    };

} //namespacecpputils

#endif // _UTILS_RUNE_WIDTH_H_
