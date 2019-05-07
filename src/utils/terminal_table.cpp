/*************************************************************************
 * File:	terminal_table.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	Fri 22 Mar 2019 05:07:48 PM CST
 ************************************************************************/

#include<iostream>
#include "terminal_table.h"
#include "some.h"
#include "strutils.h"
#include "rune_utils.h"
#include "regexp.h"

using namespace std;

namespace rtest {
    static const char *replaceStr[] = {
            "“", "\"",
            "”", "\"",
            "\t", "    ",
            "…", "...",
            "—", "-"
    };

    //替换掉会影响整行宽度的字符
    static void replaceInvalidStr(string &originStr) {
        string tmpStr = "";
        int i, j = 0;
        size_t rlen = sizeof(replaceStr) / sizeof(char *);
        for (i = 0; i < rlen; i += 2) {
            string s1 = replaceStr[i];
            string s2 = replaceStr[i + 1];
            StrUtils::strReplace(originStr, s1, s2, tmpStr);
            originStr = tmpStr;
            tmpStr = "";
        }
    }


    //对表格单元快速排序
    void TerminalTable::quickSortTableCellWidth(vector<tableRowWidth *> &rowWidths, int left, int right) {
        if (left > right) {
            return;
        }
        int i, j;
        tableRowWidth *t, *temp;

        //最左边为基准数
        temp = rowWidths[left];
        i = left;
        j = right;
        while (i != j) {
            //先从最右边开始查找
            while (rowWidths[j]->allowWidth <= temp->allowWidth && i < j) {
                j--;
            }
            //再找左边的
            while (rowWidths[i]->allowWidth >= temp->allowWidth && i < j) {
                i++;
            }
            //交换位置
            if (i < j) {
                t = rowWidths[i];
                rowWidths[i] = rowWidths[j];
                rowWidths[j] = t;
            }
        }
        //最终将基准数归位
        rowWidths[left] = rowWidths[i];
        rowWidths[i] = temp;
        quickSortTableCellWidth(rowWidths, left, i - 1);
        quickSortTableCellWidth(rowWidths, i + 1, right);

    }

    TerminalTable::TerminalTable(){
        maxColumnNum = 0;
        allTableAllowWidth = 0;
        headerFontColorType = Color::COLOR_YELLOW;
        rowFontColorType = -1;
        borderColorType = -1;
    }

    //设置相关的颜色：表头的字体颜色，默认Yellow
    void TerminalTable::setHeaderFontColorType(ColorType t) {
        headerFontColorType = t;
    }
    void TerminalTable::genHeaderStrWidthColor(string &str) {
        if (headerFontColorType > 0) {
            string output;
            Color::cliColorRender(str, headerFontColorType, 1, 0, 0, output);
            str = output;
        }
    }

    //设置相关的颜色：表格内容的字体颜色
    void TerminalTable::setRowFontColorType(ColorType t) {
        rowFontColorType = t;
    }
    void TerminalTable::genRowStrWidthColor(string &str) {
        if (rowFontColorType > 0) {
            string output;
            Color::cliColorRender(str, rowFontColorType, 0, 0, 0, output);
            str = output;
        }
    }

    //设置相关的颜色：边框的颜色
    void TerminalTable::setBorderColorType(ColorType t) {
        borderColorType = t;
    }
    void TerminalTable::genBorderStrWidthColor(string &str) {
        if (borderColorType > 0) {
            string output;
            Color::cliColorRender(str, borderColorType, 0, 0, 0, output);
            str = output;
        }
    }

    //添加表头数据
    void TerminalTable::addHeadData(const vector <string> &headData) {
        if (headData.size() <= 0) {
            return;
        }
        vector<string>::const_iterator iter;
        for (iter = headData.begin(); iter != headData.end(); iter++) {
            string tmpStr = *iter;
            replaceInvalidStr(tmpStr);
            rawheadData.push_back(tmpStr);
        }
        if (rawheadData.size() > maxColumnNum) {
            maxColumnNum = rawheadData.size();
        }
    }

    //添加表头数据
    void TerminalTable::addHeadData(int count, ...) {
        if (count <= 0) {
            return;
        }
        va_list ap;
        int j;
        va_start(ap, count);
        for (j = 0; j < count; j++) {
            const char *str = va_arg(ap, char * );
            string tmpStr = str;
            replaceInvalidStr(tmpStr);
            rawheadData.push_back(tmpStr);
        }
        if (rawheadData.size() > maxColumnNum) {
            maxColumnNum = rawheadData.size();
        }

        va_end(ap);
        return;
    }

    //添加行数据
    void TerminalTable::addRowData(const vector <string> &rowData) {
        if (rowData.size() <= 0) {
            return;
        }
        vector<string>::const_iterator iter;
        vector <string> tmpRows;
        for (iter = rowData.begin(); iter != rowData.end(); iter++) {
            string tmpStr = *iter;
            replaceInvalidStr(tmpStr);
            tmpRows.push_back(tmpStr);
        }
        rawRowData.push_back(tmpRows);

        if (tmpRows.size() > maxColumnNum) {
            maxColumnNum = tmpRows.size();
        }
    }

    //添加行数据
    void TerminalTable::addRowData(int count, ...) {
        if (count <= 0) {
            return;
        }
        va_list ap;
        int j;
        va_start(ap, count);
        vector <string> tmpRows;
        for (j = 0; j < count; j++) {
            const char *str = va_arg(ap, char * );
            string tmpStr = str;
            replaceInvalidStr(tmpStr);
            tmpRows.push_back(tmpStr);
        }
        rawRowData.push_back(tmpRows);
        if (tmpRows.size() > maxColumnNum) {
            maxColumnNum = tmpRows.size();
        }

        va_end(ap);
        return;
    }

    //整理各列的宽度信息等
    void TerminalTable::reProcessTableWidth() {
        //根据屏幕宽度计算表格的总宽度，留一些冗余宽度
        size_t screenWidth = 0;
        SomeUtils::getTerminalSize(&screenWidth, NULL);
        allTableAllowWidth = screenWidth - maxColumnNum * 3 - 3;

        //初始化各列宽度
        int i;
        for (i = 0; i < maxColumnNum; i++) {
            maxColumnWidth.push_back(0);;
        }

        //遍历每行数据，找出来每列的最大宽度，表格对齐要用
        vector<string>::iterator iterCol;
        vector < vector < string > > ::iterator
        iterRow;
        size_t w = 0;
        for (iterCol = rawheadData.begin(); iterCol != rawheadData.end(); iterCol++) {
            maxColumnWidth[w++] = RuneUtils::runeStringWidth(*iterCol);
        }
        for (iterRow = rawRowData.begin(); iterRow != rawRowData.end(); iterRow++) {
            w = 0;
            for (iterCol = iterRow->begin(); iterCol != iterRow->end(); iterCol++) {
                size_t rwidth = RuneUtils::runeStringWidth(*iterCol);
                if (maxColumnWidth[w] < rwidth) {
                    maxColumnWidth[w] = rwidth;
                }
                w++;
            }
        }

        //计算一下各列允许总宽度，如果超宽要截断处理
        size_t allWidth = 0;
        vector < tableRowWidth * > rowWidths;
        for (i = 0; i < maxColumnNum; i++) {
            size_t w = maxColumnWidth[i];
            allWidth += w;
            tableRowWidth *tmpRowW = new tableRowWidth();
            tmpRowW->columnNo = i;
            tmpRowW->allowWidth = w;
            rowWidths.push_back(tmpRowW);
        }

        //如果各小格子的宽度和大于屏幕宽度，每轮循环都将最宽的列折行，一直折到适合屏幕大小为止
        while (allWidth > allTableAllowWidth) {
            size_t diff = allWidth - allTableAllowWidth;
            quickSortTableCellWidth(rowWidths, 0, maxColumnNum - 1);
            if (rowWidths[0]->allowWidth <= 1) {
                break;
            }
            //本次总宽度消除量
            size_t reduce = rowWidths[0]->allowWidth / 4;
            if (reduce > diff) {
                reduce = diff;
            }
            rowWidths[0]->allowWidth -= reduce;
            allWidth -= reduce;
        }

        //重新设置各列宽度最大值
        for (i = 0; i < maxColumnNum; i++) {
            tableRowWidth *rowW = rowWidths[i];
            maxColumnWidth[rowW->columnNo] = rowW->allowWidth + 2;
            delete rowW;
        }
    }

    //给各行的数据折行处理
    void TerminalTable::wrapTableRows(tableRow &row, vector <string> &rowStr) {
        int i, j;
        //各小格子折行后的最大行数
        size_t maxLineNum = 0;

        vector<string>::iterator iterCol;

        //统一每列最大宽度
        for (i = 0; i < maxColumnNum; i++) {
            string tmpStr = rowStr[i];
            size_t w = maxColumnWidth[i] - 2;
            size_t wrapWidth = RuneUtils::runeStringWidth(tmpStr);
            size_t lineNum = 1;
            //如果本列的数据超过了本列最大允许的宽度
            if (wrapWidth > w) {
                string wrapStr;
                RuneUtils::runeWrap(tmpStr, w, wrapStr, lineNum);
            }
            if (lineNum > maxLineNum) {
                maxLineNum = lineNum;
            }
        }
        row.lineNum = maxLineNum;

        RegExp splitRow("\n");

        //计算并补齐各小格子的行数
        for (i = 0; i < maxColumnNum; i++) {
            string tmpStr = rowStr[i];
            size_t w = maxColumnWidth[i] - 2;
            size_t wrapWidth = RuneUtils::runeStringWidth(tmpStr);
            size_t lineNum = 1;

            //如果本列的数据超过了本列最大允许的宽度
            if (wrapWidth > w) {
                string wrapStr;
                RuneUtils::runeWrap(tmpStr, w, wrapStr, lineNum);
                rowStr[i] = wrapStr;
            }

            //对本小格子的数据折行处理
            tmpStr = rowStr[i];
            vector <string> subCells;
            splitRow.reg_split(tmpStr, subCells);
            for (j = 0; j < subCells.size(); j++) {
                string skipValidStr;
                StrUtils::strReplace(subCells[j], "\n", "", skipValidStr);
                subCells[j] = skipValidStr;
            }
            //所有的小格子折的行数要保持一致
            if (subCells.size() < maxLineNum) {
                for (j = subCells.size(); j < maxLineNum; j++) {
                    subCells.push_back(" ");
                }
            }

            tableRowCell cell;
            for (iterCol = subCells.begin(); iterCol != subCells.end(); iterCol++) {
                string tmpCell = " " + *iterCol + " ";
                cell.cellStrs.push_back(tmpCell);
            }
            cell.columnNo = i;
            cell.maxWidth = w;
            row.cellList.push_back(cell);
        }
        return;
    }

    //重新处理各行的数据
    void TerminalTable::reProcessTableRowData() {
        reProcessTableWidth();

        int i, j;

        //把所有的行的列数补齐到一致，方便输出
        vector<string>::iterator iterCol;
        vector < vector < string > > ::iterator
        iterRow;

        //先补齐表头的数据
        if (rawheadData.size() > 0 && rawheadData.size() < maxColumnNum) {
            for (i = rawheadData.size(); i < maxColumnNum; i++) {
                rawheadData.push_back(" ");
            }
        }

        //再补齐各行的列数
        for (iterRow = rawRowData.begin(); iterRow != rawRowData.end(); iterRow++) {
            if (iterRow->size() < maxColumnNum) {
                for (i = iterRow->size(); i < maxColumnNum; i++) {
                    iterRow->push_back(" ");
                }
            }
        }

        //如果有某小格子的内容特别特别的长的话，统一给各行折一下行
        int cnt = 0;
        if (rawheadData.size() > 0) {
            tableRow headObj;
            headObj.rowType = 0;
            wrapTableRows(headObj, rawheadData);
            rowData.push_back(headObj);
        }

        for (iterRow = rawRowData.begin(); iterRow != rawRowData.end(); iterRow++) {
            tableRow rowObj;
            rowObj.rowType = 1;
            wrapTableRows(rowObj, *iterRow);
            rowData.push_back(rowObj);
        }

        //将每列的数据补整齐
        vector<tableRow>::iterator iterTable;
        vector<tableRowCell>::iterator iterCell;
        for (iterTable = rowData.begin(); iterTable != rowData.end(); iterTable++) {
            i = 0;
            for (iterCell = iterTable->cellList.begin(); iterCell != iterTable->cellList.end(); iterCell++) {
                size_t w = maxColumnWidth[i++];
                for (iterCol = iterCell->cellStrs.begin(); iterCol != iterCell->cellStrs.end(); iterCol++) {
                    string tmpStr;
                    RuneUtils::runeFillRight(*iterCol, w, tmpStr);
                    *iterCol = tmpStr;
                }
            }
        }
    }

    //渲染一行的数据
    void TerminalTable::renderSingleRow(tableRow &row, string &output) {
        vector<string>::iterator iter;
        int i, j;
        string sep = "|";
        genBorderStrWidthColor(sep);
        for (i = 0; i < row.lineNum; i++) {
            for (j = 0; j < maxColumnNum; j++) {
                output.append(sep);
                string rowContent = row.cellList[j].cellStrs[i];
                //搞一下颜色
                if (row.rowType == 0) {
                    genHeaderStrWidthColor(rowContent);
                }
                else if (row.rowType == 1) {
                    genRowStrWidthColor(rowContent);
                }
                //再添加到输出里
                output.append(rowContent);
            }
            output.append(sep);
            output.append("\n");
        }
    }

    //输出最终的结果
    void TerminalTable::renderToString(string &table) {
        reProcessTableRowData();

        string lineSep = "+";
        int i, j;
        for (i = 0; i < maxColumnNum; i++) {
            size_t w = maxColumnWidth[i];
            for (j = 0; j < w; j++) {
                lineSep.append("-");
            }
            lineSep.append("+");
        }
        genBorderStrWidthColor(lineSep);

        table.append(lineSep).append("\n");
        vector<tableRow>::iterator iter;
        for (iter = rowData.begin(); iter != rowData.end(); iter++) {
            string rowStr;
            renderSingleRow(*iter, rowStr);
            table.append(rowStr);
            table.append(lineSep).append("\n");
        }
    }
}
