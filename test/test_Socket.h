/**
  ******************************************************************************
  * @file           : test_Socket.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/15
  ******************************************************************************
  */



#ifndef SERVERFRAMEWORK_TEST_SOCKET_H
#define SERVERFRAMEWORK_TEST_SOCKET_H

#include "../src/Logger.h"
#include "../src/Address.h"
#include "../src/IOManager.h"
#include "../src/Socket.h"

using namespace hyn;

void test_socket() {
    IPAddress::ptr addr = Address::LookupAnyIPAddress("www.baidu.com");
    if (addr) {
        info("get address : %s", addr->toString().c_str());
    } else {
        info("get address fail");
        return;
    }

    Socket::ptr sock = Socket::CreateTCP(addr);
    addr->setPort(80);
    info("%s", addr->toString().c_str());
    if (!sock->connect(addr)) {
        info("connect %s fail", addr->toString().c_str());
    } else {
        info("connect %s connect", addr->toString().c_str());
    }
    const char buff[] = "GET / HTTP/1.0\r\n\r\n";
    int rt = sock->send(buff, sizeof(buff));
    if (rt <= 0) {
        info("send fail");
        return;
    }
    std::string buffs;
    buffs.resize(4096);
    rt = sock->recv(&buffs[0], buffs.size());
    if (rt <= 0) {
        info("recv error");
        return;
    }
    buffs.resize(rt);
    info("%s", buffs.c_str());
}

void test_sososs() {
    iomanager::IOManager iom;
    iom.schedule(&test_socket);
}

#endif //SERVERFRAMEWORK_TEST_SOCKET_H
