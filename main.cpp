#include "src/Logger.h"

#include <sys/types.h>     // 类型定义
#include <sys/socket.h>    // socket相关定义
#include <netinet/in.h>    // INADDR_ANY、INADDR_NONE等宏定义
#include <arpa/inet.h>     // 网络地址转换函数inet_ntoa、inet_aton等
#include <netdb.h>         // 提供gethostbyname等DNS操作函数
#include <fcntl.h>         // 提供open等文件操作函数
#include <unistd.h>        // 提供read、write、close等文件操作函数
#include <errno.h>         // 提供errno全局变量，保存出错状态
#include <cstring>         // 提供memset、memcpy、memmove、strerror等函数
#include <cstdio>          // 提供printf、sprintf等函数
#include <cstdlib>         // 提供exit、malloc、free等函数
#include <iostream>        // 提供C++风格的输入输出支持

#include "src/Hook.h"

#include <thread>

#include "test/test_http_connection.h"
//#include "test/test_Socket.h"


using namespace std;

int main() {
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->open("/home/hyn/test_log.log");
    //test();
    //test_sososs();
    for (long i = 0; i < 1000000; ++i) {
        for (int j = 0; j < 100; ++j) {
            std::cout << " ";
        }
        debug("debug");
    }
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->close();
}