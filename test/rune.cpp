#include<iostream>
#include<cstring>
#include "../src/utils/strutils.h"
#include "../src/utils/md5.h"
#include "../src/utils/color.h"
#include "../src/utils/some.h"
#include "../src/utils/rune_utils.h"

using namespace std;

int main() {
    string str = "擘画强军蓝图，指引奋进征程。2013年到2018年全国两会期间，中共中央总书记、国家主席、中央军委主席习近平连续出席解放军和武警部队代表团全体会议并发表重要讲话，提出一系列新思想、新论断、新要求。6年来，全军部队认真贯彻习主席重要讲话精神，牢固确立习近平强军思想的指导地位，重振政治纲纪，重塑组织形态，重整斗争格局，重构建设布局，重树作风形象，在中国特色强军之路上迈出坚定步伐。";
    size_t screenWidth;
    rtest::SomeUtils::getTerminalSize(&screenWidth, NULL);
    string output;
    size_t lineNum;
    rtest::RuneUtils::runeWrap(str, 10, output, lineNum);
    cout << "runeWrap..... lineNum = " << lineNum << endl;
    cout << output << endl;
}
