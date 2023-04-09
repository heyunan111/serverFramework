/**
  ******************************************************************************
  * @file           : test_iomanager.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/9
  ******************************************************************************
  */



#ifndef SERVERFRAMEWORK_TEST_IOMANAGER_H
#define SERVERFRAMEWORK_TEST_IOMANAGER_H

#include "../include/IOManager.h"
#include "../hyn_include.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

void test_fiber() {
    info("test_fiber");
};

void test45() {
    std::cout << "hell\n";
}

void test1() {
    hyn::iomanager::IOManager iom;
    iom.schedule(&test_fiber);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    inet_pton(AF_INET, "115.239.210.27", &addr.sin_addr.s_addr);
    iom.addEvent(sock, hyn::iomanager::IOManager::READ, [] {
        info("connect");
    });
    connect(sock, (sockaddr *) &addr, sizeof(addr));
}

void test() {
    test1();
}

#endif //SERVERFRAMEWORK_TEST_IOMANAGER_H
