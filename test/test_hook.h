/**
  ******************************************************************************
  * @file           : test_hook.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/10
  ******************************************************************************
  */



#ifndef SERVERFRAMEWORK_TEST_HOOK_H
#define SERVERFRAMEWORK_TEST_HOOK_H

#include "../src/Logger.h"
#include "../src/IOManager.h"

#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

void test_sleep() {
    hyn::iomanager::IOManager iom(2);
    iom.schedule([]() {
        sleep(2);
        info("sleep 2");
    });
    iom.schedule([]() {
        sleep(3);
        info("sleep 3");
    });
    info("sleep test");
}

void test_sock() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    //fcntl(sock, F_SETFL, O_NONBLOCK);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "14.119.104.189", &addr.sin_addr.s_addr);
    int rt = connect(sock, (const sockaddr *) &addr, sizeof(addr));
    info("connect rt = %d ,errno = %d", rt, errno);
    if (rt)
        return;
    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    rt = send(sock, data, sizeof(data), 0);
    info("send rt = %d,errno = %d", rt, errno);
    if (rt <= 0)
        return;
    string buff;
    buff.resize(4096);
    rt = recv(sock, &buff[0], buff.size(), 0);
    info("recv rt = %d,errno = %d", rt, errno);
    if (rt <= 0)
        return;
    buff.resize(rt);
    info("buff :\n%s", buff.c_str());
}

void test_main() {
    //test_sock();
    hyn::iomanager::IOManager iom;
    iom.schedule(test_sock);
}

#endif //SERVERFRAMEWORK_TEST_HOOK_H
