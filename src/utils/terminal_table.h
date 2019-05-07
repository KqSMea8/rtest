/*************************************************************************
 * File:	terminal_table.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	Fri 22 Mar 2019 05:07:41 PM CST
 ************************************************************************/

#ifndef _UTILS_TERMINAL_TABLE_H_
#define _UTILS_TERMINAL_TABLE_H_

#include <vector>
#include <string>
#include <stdarg.h>
#include "color.h"

using namespace std;

namespace rtest{
    //整个表格
    class TerminalTable{

    protected:
        //一个小单元格的数据
        typedef struct _table_row_cell{
            size_t columnNo;                    //第几列
            size_t maxWidth;                    //允许的宽度
            vector <string> cellStrs;           //本小格子的数据，可能要分多行
        } tableRowCell;

        //一行的数据及处理逻辑
        typedef struct _table_row{
            int rowType;                        //本行数据的类型，是表头还是数据内容，0为表头，1为行数据
            size_t lineNum;                     //本行数据的行数
            vector <tableRowCell> cellList;     //本行所有小格子的数据
        } tableRow;

        //小格子宽度排序用的
        typedef struct _table_row_width{
            size_t columnNo;                    //第几列
            size_t allowWidth;                   //允许的宽度
        } tableRowWidth;
    private:
        vector <string> rawheadData;            //原始的表头数据
        vector <vector< string > > rawRowData;    //原始的行数据信
        vector <size_t> rawRowColNums;          //各行的原始列数
        size_t maxColumnNum;                    //列的数量，以最多的一行的列为准
        vector <size_t> maxColumnWidth;         //每列的最大宽度，对齐用的
        size_t allTableAllowWidth;              //表格允许的最大宽度，根据屏幕及总列数计算出来的
        vector <tableRow> rowData;              //所有经过处理后的行，包括表头数据

        ColorType headerFontColorType; //表头的字体颜色，默认Yellow
        ColorType rowFontColorType;     //表格内容的字体颜色
        ColorType borderColorType;      //边框的颜色

        //整理各列的宽度信息等
        void reProcessTableWidth();

        //给各行的数据折行处理
        void wrapTableRows(tableRow &row, vector <string> &rowStr);

        //重新处理各行的数据
        void reProcessTableRowData();

        //渲染一行的数据
        void renderSingleRow(tableRow &row, string &output);

        void quickSortTableCellWidth(vector< tableRowWidth * > &rowWidths, int left, int right);


        void genHeaderStrWidthColor(string &str);

        void genBorderStrWidthColor(string &str);

        void genRowStrWidthColor(string &str);

    public:

        TerminalTable();

        //添加表头数据
        void addHeadData(const vector <string> &headData);

        void addHeadData(int num, ...);

        //添加行数据
        void addRowData(const vector <string> &rowData);

        void addRowData(int num, ...);

        //输出最终的结果
        void renderToString(string &table);

        //设置相关的颜色：表头的字体颜色，默认Yellow
        void setHeaderFontColorType(ColorType t);

        //设置相关的颜色：表格内容的字体颜色
        void setRowFontColorType(ColorType t);

        //设置相关的颜色：边框的颜色
        void setBorderColorType(ColorType t);
    };
}
#endif
