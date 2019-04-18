/*************************************************************************
 * File:	stream.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-04-08 10:36
 ************************************************************************/
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdint.h>
#include<iostream>
#include<fstream>
#include<string>

using namespace std;

int main(){
    filebuf in;
    if (!in.open("./httpRequest.txt", std::ios::in)){
        std::cout << ">>>>>>>>-------fail to open file" << std::endl;
        return -1;
    }

    std::istream iss(&in);
    std::string str;

    while (!iss.eof()){
        if (iss.bad()){
            std::cout << ">>>>>>>>-------input stream corrupted" << std::endl;
            break;
        }
        if (iss.fail()){
            std::cout << ">>>>>>>>-------bad data" << std::endl;
            break;
        }

        std::getline(iss, str);
        cout << str << endl;
        break;
    }
    in.close();
    return 0;
}