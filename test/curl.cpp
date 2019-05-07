/*************************************************************************
 * File:	curl.cpp
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019-03-26 12:24
 ************************************************************************/
#include<stdlib.h>
#include<unistd.h>
#include<iostream>
#include<string.h>
#include<stdio.h>
#include<stdint.h>
#include <curl/curl.h>
#include "../src/client/http/http_client_utils.h"
#include "../src/utils/regexp.h"
#include "../src/utils/color.h"

#define CURL_STATICLIB
using namespace std;

//string strUrl = "https://new.qq.com/cmsn/20190327/20190327006559.html";
string strUrl = "http://10.96.114.84/add1.php";

size_t receive_data(void *contents, size_t size, size_t nmemb, void *stream) {
    string *str = (string *) stream;
    (*str).append((char *) contents, size * nmemb);
    cout << *str << endl;
    return size * nmemb;
}

int httpPost() {

    std::string strResponse;
    CURLcode res;
    char szJsonData[1024] = "a=6&b=5";
    CURL *pCURL = curl_easy_init();
    struct curl_slist *headers = NULL;
    if (pCURL == NULL) {
        return CURLE_FAILED_INIT;
    }

    CURLcode ret;
    ret = curl_easy_setopt(pCURL, CURLOPT_URL, strUrl.c_str());
    std::cout << "ret=" << ret << std::endl;

    ret = curl_easy_setopt(pCURL, CURLOPT_POST, 1L);
    //headers = curl_slist_append(headers, "content-type:application/json");

    ret = curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, headers);

    ret = curl_easy_setopt(pCURL, CURLOPT_POSTFIELDS, szJsonData);

    ret = curl_easy_setopt(pCURL, CURLOPT_TIMEOUT, 33333);

    ret = curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, receive_data);

    ret = curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, (void *) &strResponse);

    res = curl_easy_perform(pCURL);
    cout << "res=" << res << endl;
    cout << "strResponse=" << strResponse << endl;
    curl_easy_cleanup(pCURL);
    return res;
}

//回调函数  得到响应内容
int write_data(void *buffer, int size, int nmemb, void *userp) {
    std::string *str = dynamic_cast<std::string *>((std::string *) userp);
    str->append((char *) buffer, size * nmemb);
    cout << "write_data=" << *str << endl;
    return nmemb;
}

int uploadFiles() {
    string response;
    CURL *curl;
    CURLcode ret;
    curl = curl_easy_init();
    struct curl_httppost *post = NULL;
    struct curl_httppost *last = NULL;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, (char *) strUrl.c_str());           //指定url
        curl_formadd(&post, &last, CURLFORM_PTRNAME, "path", CURLFORM_PTRCONTENTS, "device_cover",
                     CURLFORM_END);//form-data key(path) 和 value(device_cover)
        curl_formadd(&post, &last, CURLFORM_PTRNAME, "file", CURLFORM_FILE, "./Makefile", CURLFORM_FILENAME,
                     "hello.jpg", CURLFORM_END);// form-data key(file) "./test.jpg"为文件路径  "hello.jpg" 为文件上传时文件名
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);                     //构造post参数
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);          //绑定相应
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &response);        //绑定响应内容的地址

        ret = curl_easy_perform(curl);                          //执行请求
        cout << "response=" << response << endl;
        if (ret == 0) {
            curl_easy_cleanup(curl);
            return 0;
        }
        else {
            return ret;
        }
    }
    else {
        return -1;
    }
}

int httpGet() {
    std::string strResponse;
    CURLcode res;
    CURL *pCURL = curl_easy_init();

    if (pCURL == NULL) {
        return CURLE_FAILED_INIT;
    }

    curl_easy_setopt(pCURL, CURLOPT_URL, strUrl.c_str());
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(pCURL, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(pCURL, CURLOPT_TIMEOUT, 333333);
    curl_easy_setopt(pCURL, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, receive_data);
    curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, (void *) &strResponse);
    res = curl_easy_perform(pCURL);
    cout << "--------------get=" << strResponse << endl;
    curl_easy_cleanup(pCURL);
    return res;
}

void printMapStringString(map <string, string> &m) {
    map<string, string>::const_iterator iter;
    for (iter = m.begin(); iter != m.end(); iter++) {
        cout << "k=" << iter->first << "\tv=" << iter->second << endl;
    }
    cout << endl;
}

int main() {
    rtest::curlHandle curlFP;
    curlFP.setUrl(strUrl);
    curlFP.setDebug(true);
    curlFP.addFile("firstFile", "/Users/liuyongshuai/Documents/service.conf", "filename");
    curlFP.addField("shop_id", "6756790892347398579");
    curlFP.addField("shop_id", "333333333333333");
    curlFP.addField("item_id", "1234567890");
    curlFP.setTimeout(1000);
    curlFP.addHeader("Host", "test.wendao.com");

    cout << "---------POST---------" << endl;
    rtest::HttpResponse post;
    rtest::HttpClientUtils::Post(curlFP, post);
    cout << post.rspStr << endl;
    post.printHeaders();

    cout << "---------PostRaw---------" << endl;
    string rawStr = "asfdadfasdfadfdas";
    rtest::HttpResponse postRaw;
    rtest::HttpClientUtils::PostRaw(curlFP, rawStr, postRaw);
    cout << postRaw.rspStr << endl;
    postRaw.printHeaders();

    cout << "---------Get---------" << endl;
    rtest::HttpResponse get;
    rtest::HttpClientUtils::Get(curlFP, get);
    cout << get.rspStr << endl;
    get.printHeaders();
}
/**
官方文档：https://curl.haxx.se/libcurl/c/curl_easy_setopt.html

 
#include <curl/curl.h>
CURLcodecurl_easy_setopt(CURL *handle, CURLoption option, parameter);


说明：
此函数用来告诉 libcurl 执行什么样的动作。该函数有 3 个参数(该函数的可设置选项非常之多)：
第 1 个参数 handle 是由 curl_easy_init() 返回的句柄；
第 2 个参数是可以设置的选项(CURLoption)；
第 3 个参数是与第 2 个选项相关联的参数，
        这个参数可以是 long 型，
        也可以是一个函数指针(function pointer)，
        还可以是一个对象的指针 (object pointer)，
        或者是一个 curl_off_t 类型，
    这些参数类型必须由选项值(CURLoption)来确定。


网络选项(NETWORK OPTIONS)：

1. CURLOPT_URL
这个选项后面接实际要处理的 URL ，这个 URL 是一个以 '\0' 结尾的字符串或参数指针(关于这个参数的简单示例见：http://www.groad.net/bbs/read.php?tid-1641.html)。如果 URL 参数不写上协议头(如 "http://" 或者 "ftp:// 等等 )，那么函数会自己进行猜解所给的主机上用的是哪一种服务协议。假如你给的这个地址是一个不被 支持的协议，那么在其后执行 curl_easy_perform() 函数或 curl_multi_perform() 函数时， libcurl 将返回错误(CURLE_UNSUPPORTED_PROTOCOL)。这个选项是唯一一个必须在 curl_easy_perform() 调用之前就要设置的选项。

2. CURLOPT_POSTFIELDSIZE
该选项要求第 3 个参数 parameter 是一个 void * 指针，它指向一个向 HTTP 服务器 POST 出去的一段数据，
    这段数据要根据服务器的具体要求填写。
    该选项用法参考示例：http://www.groad.net/bbs/read.php?tid-3909.html

3. CURLOPT_WRITEFUNCTION
使用该选项时，要求第 3 个参数中的回调函数必须是下面的函数原型：

size_t function( char *ptr, size_t size, size_t nmemb, void *userdata);
在启动会话时，一旦检测到有需要接收的数据时，回调函数被调用。
    ptr 所指向的数据大小由 size 和 nmemb 的乘积获得。
    函数最后需要返回接收数据的大小。如果不使用该函数，那么接收到的数据会直接打印到终端；
    使用该函数，那么接收到的数据保存在 ptr 所执向的区域，可以利用此来保存接收下来的数据。

4. CURLOPT_USERAGENT
该选项要求传递一个以 '\0' 结尾的字符串指针，这个字符串用来在向服务器请求时发送 HTTP 头部中的 User-Agent 信息，
    有些服务器是需要检测这个信息的，如果没有设置 User-Agent，
    那么服务器拒绝请求。设置后，可以骗过服务器对此的检查。

5.  CURLOPT_WRITEDATA
使用该选项时，第 3 个参数作为用户数据的指针而传递到使用  CURLOPT_WRITEFUNCTION 选项时指定的回调函数中(第 4 个参数)。
    如果不想用回调函数而保存数据，那么可以使用 CURLOPT_WRITEDATA 选项，
    使用该选项时，函数的第 3 个参数必须是个 FILE 指针，
    函数会将接收到的数据自动的写到这个 FILE 指针所指向的文件流中。

6. CURLOPT_VERBOSE
在使用该选项且第 3 个参数为 1 时，curl 库会显示详细的操作信息。这对程序的调试具有极大的帮助。

7. CURLOPT_NOBODY
使用该选项时，若第 3 个参数设为 1，这样在输出中就不会包含主体内容部分。
    这仅是对在传输的所有内容中，含有“头部”和“主题内容”两部分的协议而言。
    如 HTTP(S) 服务器，在这种情况下将会使 libcurl 库仅发出一个头部请求。

8. CURLOPT_HEADER
使用该选项时，第 3 个参数设置为 1，那么会通知 curl 库在输出时要同时包含 "头部“ 和 "主题内容" 两个部分。
    该选项仅是对那些同时包含了”头部“和”主题内容“这两部分的协议而言(如 HTTP)。

9. CURLOPT_HEADERFUNCTION
该选项与上面第 3 个选项 CURLOPT_WRITEFUNCTION 类似，只要它一接收到头部信息时，它就会执行回调函数。
    需要注意的是，回调函数里处理的头部包含了所有收到的响应的头部信息，而不只是最后一次的响应。
    如果需要处理其中的一个头部，那么自己需要在所收集的头部信息中进行区分。

10. CURLOPT_WRITEHEADER 和  CURLOPT_HEADERDATA
这两个选项是同一种意思。它们和第 5 条中的 CURLOPT_WRITEDATA 选项功能一样，
    表示在接收到头部信息并调用回调函数时，给回调函数传递第 4 个参数。

11. CURLOPT_INFILESIZE
当向服务器上传文件时，该选项用来告诉 curl 库期望上传的文件的大小。
    使用该选项时，应该给函数第 3 个参数的应该是个 long 型变量。
    如果用的是 SCP 传输，那么该选项强制使用 CURLOPT_INFILESIZE_LARGE 。

12. CURLOPT_INFILESIZE_LARGE
该选项和 CURLOPT_INFILESIZE 功能一样，但是它要求函数的第 3 个参数必须是个 curl_off_t 类型。
    curl_off_t 为 int64_t 类型，而 int64_t 定义在 stdint.h 中：

if __WORDSIZE == 64 
typedef long int                int64_t;
# else 
__extension__ 
typedef long long int           int64_t; 



13. CURLOPT_QUOTE 和 CURLOPT_POSTQUOTE
这两个选项的功能类似，它们的共同点都是给 FTP 或 SFTP 传递命令。
    这些命令应该放在 struct slist 链表中存储，
    使用时需要用 curl_slist_append() 函数将这些命令打包起来，然后一起发送出去。
    它们的不同点是：CURLOPT_QUOTE 选项要求命令要在 FTP 传输请求之前就要发送到库，
    而 CURLOPT_POSTQUOTE 则可以在 FTP 传输请求发送完后发送。比如像下面的执行顺序是无法达到目的的：
. ... ...
curl_easy_perform(curl);
curl_easy_setopt(curl, CURLOPT_QUOTE, headerlist);
curl_easy_perform(curl);
... ...

    上面假设第 1 条 curl_easy_perform(curl); 
    语句已经发送了 FTP 传输请求，然后再用 CURLOPT_QUOTE 发送命令是错误的，
    这里应该使用 CURLOPT_POSTQUOTE 选项。
    使用 CURLOPT_QUOTE 选项需要先用 curl_easy_perform() 函数将其发送，
    然后再用一次 curl_easy_perform() 发送 FTP 传输请求。
    也就是说需要执行两次  curl_easy_perform() 函数。
    而用  CURLOPT_POSTQUOTE 选项则不需要这样，它只要将选项设置好后，
    然后只执行一次 curl_easy_perform() 函数即可。

14. CURLOPT_READFUNCTION 和 CURLOPT_READDATA
这两个选项和上面的  CURLOPT_WRITEFUNCTION 和 CURLOPT_WRITEDATA 类似。
    在 CURLOPT_READFUNCTION 的回调函数里，
    第 1 个参数 ptr 指针用来接收从第 4 个参数传递过来的数据(这个参数往往是个文件流指针)，
    而这个参数是使用 CURLOPT_READDATA 选项时传递过来的。

15. CURLOPT_UPLOAD
在使用该选项时，第 3 个参数设置为 1，表示要准备上传文件。
    这个参数往往会配合 CURLOPT_READDATA，CURLOPT_INFILESIZE_LARGE，
    以及 CURLOPT_INFILESIZE 这几个选项一起用。
    如果是使用 HTTP 协议，那么使用 PUT 的方法进行上传，除非另有指定。

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CURLOPT_HEADER：设为1，则在返回的内容里包含http header；

CURLOPT_FOLLOWLOCATION：设为0，则不会自动301，302跳转；

*CURLOPT_INFILESIZE: 当你上传一个文件到远程站点，这个选项告诉PHP你上传文件的大小。
*CURLOPT_VERBOSE: 如果你想CURL报告每一件意外的事情，设置这个选项为一个非零值。
*CURLOPT_HEADER: 如果你想把一个头包含在输出中，设置这个选项为一个非零值。
*CURLOPT_NOPROGRESS: 如果你不会PHP为CURL传输显示一个进程条，设置这个选项为一个非零值。

 

注意：PHP自动设置这个选项为非零值，你应该仅仅为了调试的目的来改变这个选项。

*CURLOPT_NOBODY: 如果你不想在输出中包含body部分，设置这个选项为一个非零值。
*CURLOPT_FAILONERROR: 如果你想让PHP在发生错误(HTTP代码返回大于等于300)时，不显示，设置这个选项为一人非零值。默认行为是返回一个正常页，忽略代码。
*CURLOPT_UPLOAD: 如果你想让PHP为上传做准备，设置这个选项为一个非零值。
*CURLOPT_POST: 如果你想PHP去做一个正规的HTTP POST，设置这个选项为一个非零值。这个POST是普通的 application/x-www-from-urlencoded 类型，多数被HTML表单使用。
*CURLOPT_FTPLISTONLY: 设置这个选项为非零值，PHP将列出FTP的目录名列表。
*CURLOPT_FTPAPPEND: 设置这个选项为一个非零值，PHP将应用远程文件代替覆盖它。
*CURLOPT_NETRC: 设置这个选项为一个非零值，PHP将在你的 ~./netrc 文件中查找你要建立连接的远程站点的用户名及密码。
*CURLOPT_FOLLOWLOCATION: 设置这个选项为一个非零值(象 “Location: “)的头，服务器会把它当做HTTP头的一部分发送(注意这是递归的，PHP将发送形如 “Location: “的头)。
*CURLOPT_PUT: 设置这个选项为一个非零值去用HTTP上传一个文件。要上传这个文件必须设置CURLOPT_INFILE和CURLOPT_INFILESIZE选项.
*CURLOPT_MUTE: 设置这个选项为一个非零值，PHP对于CURL函数将完全沉默。
*CURLOPT_TIMEOUT: 设置一个长整形数，作为最大延续多少秒。
*CURLOPT_LOW_SPEED_LIMIT: 设置一个长整形数，控制传送多少字节。
*CURLOPT_LOW_SPEED_TIME: 设置一个长整形数，控制多少秒传送CURLOPT_LOW_SPEED_LIMIT规定的字节数。
*CURLOPT_RESUME_FROM: 传递一个包含字节偏移地址的长整形参数，(你想转移到的开始表单)。
*CURLOPT_SSLVERSION: 传递一个包含SSL版本的长参数。默认PHP将被它自己努力的确定，在更多的安全中你必须手工设置。
*CURLOPT_TIMECONDITION: 传递一个长参数，指定怎么处理CURLOPT_TIMEVALUE参数。你可以设置这个参数为TIMECOND_IFMODSINCE 或 TIMECOND_ISUNMODSINCE。这仅用于HTTP。
*CURLOPT_TIMEVALUE: 传递一个从1970-1-1开始到现在的秒数。这个时间将被CURLOPT_TIMEVALUE选项作为指定值使用，或被默认TIMECOND_IFMODSINCE使用。

下列选项的值将被作为字符串：

*CURLOPT_URL: 这是你想用PHP取回的URL地址。你也可以在用curl_init()函数初始化时设置这个选项。
*CURLOPT_USERPWD: 传递一个形如[username]:[password]风格的字符串,作用PHP去连接。
*CURLOPT_PROXYUSERPWD: 传递一个形如[username]:[password] 格式的字符串去连接HTTP代理。
*CURLOPT_RANGE: 传递一个你想指定的范围。它应该是”X-Y”格式，X或Y是被除外的。HTTP传送同样支持几个间隔，用逗句来分隔(X-Y,N-M)。
*CURLOPT_POSTFIELDS: 传递一个作为HTTP “POST”操作的所有数据的字符串。
*CURLOPT_REFERER: 在HTTP请求中包含一个”referer”头的字符串。
*CURLOPT_USERAGENT: 在HTTP请求中包含一个”user-agent”头的字符串。
*CURLOPT_FTPPORT: 传递一个包含被ftp “POST”指令使用的IP地址。这个POST指令告诉远程服务器去连接我们指定的IP地址。这个字符串可以是一个IP地址，一个主机名，一个网络界面名(在UNIX下)，或是‘-’(使用系统默认IP地址)。
*CURLOPT_COOKIE: 传递一个包含HTTP cookie的头连接。
*CURLOPT_SSLCERT: 传递一个包含PEM格式证书的字符串。
*CURLOPT_SSLCERTPASSWD: 传递一个包含使用CURLOPT_SSLCERT证书必需的密码。
*CURLOPT_COOKIEFILE: 传递一个包含cookie数据的文件的名字的字符串。这个cookie文件可以是Netscape格式，或是堆存在文件中的HTTP风格的头。
*CURLOPT_CUSTOMREQUEST: 当进行HTTP请求时，传递一个字符被GET或HEAD使用。为进行DELETE或其它操作是有益的，更Pass a string to be used instead of GET or HEAD when doing an HTTP request. This is useful for doing or another, more obscure, HTTP request.

注意: 在确认你的服务器支持命令先不要去这样做。

下列的选项要求一个文件描述(通过使用fopen()函数获得)：

*CURLOPT_FILE: 这个文件将是你放置传送的输出文件，默认是STDOUT.
*CURLOPT_INFILE: 这个文件是你传送过来的输入文件。
*CURLOPT_WRITEHEADER: 这个文件写有你输出的头部分。
*CURLOPT_STDERR: 这个文件写有错误而不是stderr。


https://www.cnblogs.com/fnlingnzb-learner/p/5835411.html

一、LibCurl基本编程框架
libcurl是一个跨平台的网络协议库，支持http, https, ftp, gopher, telnet, dict, file, 和ldap 协议。libcurl同样支持HTTPS证书授权，HTTP POST, HTTP PUT, FTP 上传, HTTP基本表单上传，代理，cookies,和用户认证。想要知道更多关于libcurl的介绍，可以到官网 http://curl.haxx.se/上去了解，在这里不再详述。
win32版的libcurl下载地址:http://curl.freeby.pctools.cl/download/libcurl-7.18.0-win32-msvc.zip
在基于LibCurl的程序里，主要采用callback function （回调函数）的形式完成传输任务，用户在启动传输前设置好各类参数和回调函数，当满足条件时libcurl将调用用户的回调函数实现特定功能。下面是利用libcurl完成传输任务的流程：
1.       调用curl_global_init()初始化libcurl
2.       调用curl_easy_init()函数得到 easy interface型指针
3.       调用curl_easy_setopt()设置传输选项
4.       根据curl_easy_setopt()设置的传输选项，实现回调函数以完成用户特定任务
5.       调用curl_easy_perform()函数完成传输任务
6.       调用curl_easy_cleanup()释放内存
在整过过程中设置curl_easy_setopt()参数是最关键的，几乎所有的libcurl程序都要使用它。

二、一些基本的函数
1.CURLcode curl_global_init(long flags);
描述：
这个函数只能用一次。(其实在调用curl_global_cleanup 函数后仍然可再用)
如果这个函数在curl_easy_init函数调用时还没调用，它讲由libcurl库自动调用，所以多线程下最好主动调用该函数以防止在线程中curl_easy_init时多次调用。
注意：虽然libcurl是线程安全的，但curl_global_init是不能保证线程安全的，所以不要在每个线程中都调用curl_global_init，应该将该函数的调用放在主线程中。
参数：flags
CURL_GLOBAL_ALL                      //初始化所有的可能的调用。
CURL_GLOBAL_SSL                      //初始化支持 安全套接字层。
CURL_GLOBAL_WIN32            //初始化win32套接字库。
CURL_GLOBAL_NOTHING         //没有额外的初始化。

2 void curl_global_cleanup(void);
描述：在结束libcurl使用的时候，用来对curl_global_init做的工作清理。类似于close的函数。
注意：虽然libcurl是线程安全的，但curl_global_cleanup是不能保证线程安全的，所以不要在每个线程中都调用curl_global_init，应该将该函数的调用放在主线程中。

3 char *curl_version( );
描述: 打印当前libcurl库的版本。

4 CURL *curl_easy_init( );
curl_easy_init用来初始化一个CURL的指针(有些像返回FILE类型的指针一样). 相应的在调用结束时要用curl_easy_cleanup函数清理.
一般curl_easy_init意味着一个会话的开始. 它会返回一个easy_handle(CURL*对象), 一般都用在easy系列的函数中.

5 void curl_easy_cleanup(CURL *handle);
这个调用用来结束一个会话.与curl_easy_init配合着用.
参数:
CURL类型的指针.

6 CURLcode curl_easy_setopt(CURL *handle, CURLoption option, parameter);
这个函数最重要了.几乎所有的curl 程序都要频繁的使用它.它告诉curl库.程序将有如何的行为. 比如要查看一个网页的html代码等.(这个函数有些像ioctl函数)参数:
1 CURL类型的指针
2 各种CURLoption类型的选项.(都在curl.h库里有定义,man 也可以查看到)
3 parameter 这个参数 既可以是个函数的指针,也可以是某个对象的指针,也可以是个long型的变量.它用什么这取决于第二个参数.
CURLoption 这个参数的取值很多.具体的可以查看man手册.

7 CURLcode curl_easy_perform(CURL *handle);
描述:这个函数在初始化CURL类型的指针 以及curl_easy_setopt完成后调用. 就像字面的意思所说perform就像是个舞台.让我们设置的
option 运作起来.参数:
CURL类型的指针.

三、 curl_easy_setopt函数部分选项介绍
本节主要介绍curl_easy_setopt中跟http相关的参数。该函数是curl中非常重要的函数，curl所有设置都是在该函数中完成的，该函数的设置选项众多，注意本节的阐述的只是部分常见选项。
CURLOPT_URL
设置访问URL

CURLOPT_WRITEFUNCTION，CURLOPT_WRITEDATA
回调函数原型为：size_t function( void *ptr, size_t size, size_t nmemb, void *stream); 函数将在libcurl接收到数据后被调用，因此函数多做数据保存的功能，如处理下载文件。CURLOPT_WRITEDATA 用于表明CURLOPT_WRITEFUNCTION函数中的stream指针的来源。
如果你没有通过CURLOPT_WRITEFUNCTION属性给easy handle设置回调函数，libcurl会提供一个默认的回调函数，它只是简单的将接收到的数据打印到标准输出。你也可以通过 CURLOPT_WRITEDATA属性给默认回调函数传递一个已经打开的文件指针，用于将数据输出到文件里。

CURLOPT_HEADERFUNCTION，CURLOPT_HEADERDATA
回调函数原型为 size_t function( void *ptr, size_t size,size_t nmemb, void *stream); libcurl一旦接收到http 头部数据后将调用该函数。CURLOPT_WRITEDATA 传递指针给libcurl，该指针表明CURLOPT_HEADERFUNCTION 函数的stream指针的来源。

CURLOPT_READFUNCTION CURLOPT_READDATA
libCurl需要读取数据传递给远程主机时将调用CURLOPT_READFUNCTION指定的函数，函数原型是：size_t function(void *ptr, size_t size, size_t nmemb,void *stream). CURLOPT_READDATA 表明CURLOPT_READFUNCTION函数原型中的stream指针来源。

CURLOPT_NOPROGRESS，CURLOPT_PROGRESSFUNCTION，CURLOPT_PROGRESSDATA
跟数据传输进度相关的参数。CURLOPT_PROGRESSFUNCTION 指定的函数正常情况下每秒被libcurl调用一次，为了使CURLOPT_PROGRESSFUNCTION被调用，CURLOPT_NOPROGRESS必须被设置为false，CURLOPT_PROGRESSDATA指定的参数将作为CURLOPT_PROGRESSFUNCTION指定函数的第一个参数

CURLOPT_TIMEOUT，CURLOPT_CONNECTIONTIMEOUT:
CURLOPT_TIMEOUT 由于设置传输时间，CURLOPT_CONNECTIONTIMEOUT 设置连接等待时间

CURLOPT_FOLLOWLOCATION
设置重定位URL

CURLOPT_RANGE: CURLOPT_RESUME_FROM:
断点续传相关设置。CURLOPT_RANGE 指定char *参数传递给libcurl，用于指明http域的RANGE头域，例如：
表示头500个字节：bytes=0-499
表示第二个500字节：bytes=500-999
表示最后500个字节：bytes=-500
表示500字节以后的范围：bytes=500-
第一个和最后一个字节：bytes=0-0,-1
同时指定几个范围：bytes=500-600,601-999
CURLOPT_RESUME_FROM 传递一个long参数给libcurl，指定你希望开始传递的 偏移量。

四、 curl_easy_perform 函数说明（error 状态码）
该函数是完成curl_easy_setopt指定的所有选项，本节重点介绍curl_easy_perform的返回值。返回0意味一切ok，非0代表错误发生。主要错误码说明：
CURLE_OK
    任务完成一切都好
CURLE_UNSUPPORTED_PROTOCOL
    不支持的协议，由URL的头部指定
CURLE_COULDNT_CONNECT
    不能连接到remote 主机或者代理
CURLE_REMOTE_ACCESS_DENIED
    访问被拒绝
CURLE_HTTP_RETURNED_ERROR
    Http返回错误
CURLE_READ_ERROR
读本地文件错误
要获取详细的错误描述字符串，可以通过const char *curl_easy_strerror(CURLcode errornum ) 这个函数取得.

五、libcurl使用的HTTP消息头
    当使用libcurl发送http请求时，它会自动添加一些http头。我们可以通过CURLOPT_HTTPHEADER属性手动替换、添加或删除相应 的HTTP消息头。
    Host
    http1.1（大部分http1.0)版本都要求客户端请求提供这个信息头。
    Pragma
    "no-cache"。表示不要缓冲数据。
    Accept
    "* / *"。表示允许接收任何类型的数据。
Expect
        以POST的方式向HTTP服务器提交请求时，libcurl会设置该消息头为"100-continue"，它要求服务器在正式处理该请求之前，返回一 个"OK"消息。如果POST的数据很小，libcurl可能不会设置该消息头。
自定义选项
        当前越来越多的协议都构建在HTTP协议之上（如：soap），这主要归功于HTTP的可靠性，以及被广泛使用的代理支持（可以穿透大部分防火墙）。 这些协议的使用方式与传统HTTP可能有很大的不同。对此，libcurl作了很好的支持。
自定义请求方式(CustomRequest)
HTTP支持GET, HEAD或者POST提交请求。可以设置CURLOPT_CUSTOMREQUEST来设置自定义的请求方式，libcurl默认以GET方式提交请求：
curl_easy_setopt(easy_handle, CURLOPT_CUSTOMREQUEST, "MYOWNREQUEST");

修改消息头
        HTTP协议提供了消息头，请求消息头用于告诉服务器如何处理请求；响应消息头则告诉浏览器如何处理接收到的数据。在libcurl中，你可以自由的添加 这些消息头：

struct curl_slist *headers=NULL; // init to NULL is important
headers = curl_slist_append(headers, "Hey-server-hey: how are you?");
headers = curl_slist_append(headers, "X-silly-content: yes");
// pass our list of custom made headers
curl_easy_setopt(easyhandle, CURLOPT_HTTPHEADER, headers);
curl_easy_perform(easyhandle); // transfer http
curl_slist_free_all(headers); // free the header list

对于已经存在的消息头，可以重新设置它的值：

headers = curl_slist_append(headers, "Accept: Agent-007");
headers = curl_slist_append(headers, "Host: munged.host.line");
删除消息头
        对于一个已经存在的消息头，设置它的内容为空，libcurl在发送请求时就不会同时提交该消息头：

headers = curl_slist_append(headers, "Accept:");


六、获取http应答头信息
发出http请求后，服务器会返回应答头信息和应答数据，如果仅仅是打印应答头的所有内容，则直接可以通过curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, 打印函数)的方式来完成，这里需要获取的是应答头中特定的信息，比如应答码、cookies列表等，则需要通过下面这个函数：
CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ... );
info参数就是我们需要获取的内容，下面是一些参数值:
1.CURLINFO_RESPONSE_CODE
获取应答码
2.CURLINFO_HEADER_SIZE
头大小
3.CURLINFO_COOKIELIST
cookies列表

除了获取应答信息外，这个函数还能获取curl的一些内部信息，如请求时间、连接时间等等。

更多的参数可以参考API文档。



七、多线程问题
首先一个基本原则就是：绝对不应该在线程之间共享同一个libcurl handle(CURL *对象)，不管是easy handle还是multi handle（本文只介绍easy_handle）。一个线程每次只能使用一个handle。
libcurl是线程安全的，但有两点例外：信号(signals)和SSL/TLS handler。
 信号用于超时失效名字解析(timing out name resolves)。
 libcurl依赖其他的库来支持SSL/STL，所以用多线程的方式访问HTTPS或FTPS的URL时，应该满足这些库对多线程 操作的一些要求。详细可以参考：
OpenSSL: http://www.openssl.org/docs/crypto/threads.html#DESCRIPTION

GnuTLS: http://www.gnu.org/software/gnutls/manual/html_node/Multi_002dthreaded-applications.html

NSS: 宣称是多线程安全的。

八、什么时候libcurl无法正常工作
传输失败总是有原因的。你可能错误的设置了一些libcurl的属性或者没有正确的理解某些属性的含义，或者是远程主机返回一些无法被正确解析的内容。
这里有一个黄金法则来处理这些问题：将CURLOPT_VERBOSE属性设置为1，libcurl会输出通信过程中的一些细节。
 如果使用的是http协 议，请求头/响应头也会被输出。将CURLOPT_HEADER设为1，这些头信息将出现在消息的内容中。
当然不可否认的是，libcurl还存在bug。
如果你对相关的协议了解越多，在使用libcurl时，就越不容易犯错。

九、关于密码
客户端向服务器发送请求时，许多协议都要求提供用户名与密码。libcurl提供了多种方式来设置它们。
一些协议支持在URL中直接指定用户名和密码，类似于： protocol://user:password@example.com/path/。
 libcurl能正确的识别这种URL中的用户名与密码并执行 相应的操作。如果你提供的用户名和密码中有特殊字符，首先应该对其进行URL编码。
也可以通过CURLOPT_USERPWD属性来设置用户名与密码。参数是格式如 “user:password ”的字符串：
curl_easy_setopt(easy_handle, CURLOPT_USERPWD, "user_name:password");
有时候在访问代理服务器的时候，可能时时要求提供用户名和密码进行用户身份验证。这种情况下，libcurl提供了另 一个属性CURLOPT_PROXYUSERPWD：
curl_easy_setopt(easy_handle, CURLOPT_PROXYUSERPWD, "user_name:password");
在UNIX平台下，访问FTP的用户名和密码可能会被保存在$HOME/.netrc文件中。libcurl支持直接从这个文件中获取用户名与密码：
curl_easy_setopt(easy_handle, CURLOPT_NETRC, 1L);
在使用SSL时，可能需要提供一个私钥用于数据安全传输，通过CURLOPT_KEYPASSWD来设置私钥：
curl_easy_setopt(easy_handle, CURLOPT_KEYPASSWD, "keypassword");

十、HTTP验证
在使用HTTP协议时，客户端有很多种方式向服务器提供验证信息。默认的 HTTP验证方法是"Basic”，
    它将用户名与密码以明文的方式、经Base64编码后保存在HTTP请求头中，发往服务器。当然这不太安全。
当前版本的libcurl支持的验证方法有：basic, Digest, NTLM, Negotiate, GSS-Negotiate and SPNEGO。（
    译者感叹：搞Web这么多年，尽然不知道这些Http的验证方式，实在惭愧。）可以通过CURLOPT_HTTPAUTH属性来设置具体 的验证方式：
curl_easy_setopt(easy_handle, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
向代理服务器发送验证信息时，可以通过CURLOPT_PROXYAUTH设置验证方式：
curl_easy_setopt(easy_handle, CURLOPT_PROXYAUTH, CURLAUTH_NTLM);
也可以同时设置多种验证方式（通过按位与）， 使用‘CURLAUTH_ANY‘将允许libcurl可以选择任何它所支持的验证方式。通过CURLOPT_HTTPAUTH或 CURLOPT_PROXYAUTH属性设置的多种验证方式，libcurl会在运行时选择一种它认为是最好的方式与服务器通信：
curl_easy_setopt(easy_handle, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST|CURLAUTH_BASIC);
// curl_easy_setopt(easy_handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);



十一、代码示例
        下载的libcurl中自带了很多示例代码，在docs\examples目录下，建议下载该库后好好阅读一下这些代码。
libcurl的API文档在docs\libcurl\index.html中。

*/
