#include<iostream>
#include<string>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdint.h>
#include "../src/utils/regexp.h"

using namespace std;

void printVector(const std::vector <string> &output) {
    vector<string>::const_iterator iter;
    cout << endl;
    cout << endl;
    for (iter = output.begin(); iter != output.end(); iter++) {
        cout << *iter << endl;
    }
    cout << endl;
    cout << endl;
}

void fn(const std::vector <std::string> &subMatchStrs, std::string &replaceStr) {
    printVector(subMatchStrs);
    replaceStr = "wendao";
}

int main() {
    string htmlStr = "<div class=\"dropdown global-dropdown\">"
                     "   <button class=\"global-dropdown-toggle\" data-toggle=\"dropdown\" type=\"button\">"
                     "       <span class=\"sr-only\">Toggle navigation</span>"
                     "       <i aria-hidden='true' data-hidden = \"true\" class=\"fa fa-bars\"></i>"
                     "   </button>"
                     "   <div class=\"dropdown-menu-nav global-dropdown-menu\">"
                     "       <ul>"
                     "           <li class=\"home active\">"
                     "               <a title=\"Projects\" class=\"dashboard-shortcuts-projects\" href=\"/dashboard/projects\">"
                     "                   <div class=\"shortcut-mappings\">"
                     "                       <div class=\"key\">"
                     "                           <i aria-label=\"hidden\" class=\"fa fa-arrow-up\"></i>"
                     "                       </div>"
                     "                   </div>"
                     "               </a>"
                     "           </li>"
                     "       </ul>"
                     "   </div>"
                     "</div>";

    string pattern = "<div(.*?)>";
    runtofuServer::RegExp regExp(pattern);
    std::string splitNewStr55;
    regExp.reg_replace_callback(htmlStr,  splitNewStr55,fn);
	cout << splitNewStr55 << endl;
}
