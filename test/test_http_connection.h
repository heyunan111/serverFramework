/**
  ******************************************************************************
  * @file           : test_http_connction.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/24
  ******************************************************************************
  */



#ifndef SERVERFRAMEWORK_TEST_HTTP_CONNCTION_H
#define SERVERFRAMEWORK_TEST_HTTP_CONNCTION_H

#include <iostream>
#include "../src/HttpConnection.h"
#include "../src/Logger.h"
#include "../src/Address.h"
#include "../src/IOManager.h"

using namespace hyn;
using namespace http;
using namespace logger;
using namespace hyn::iomanager;

void run() {
    Address::ptr addr = Address::LookupAnyIPAddress("www.sylar.top");
    if (!addr) {
        error("get addr error");
        return;
    }
    info("%s", addr->toString().c_str());
    Socket::ptr sock = Socket::CreateTCP(addr);
    info("%s", sock->toString().c_str());
    bool rt = sock->connect(addr);
    if (!rt) {
        error("connect error");
        return;
    }
    HttpConnection::ptr conn(new HttpConnection(sock));
    HttpRequest::ptr req(new HttpRequest);
    req->setPath("/blog/");
    req->setHeader("host", "www.sylar.top");
    info("req:%s", req->toString().c_str());
    auto rsp = conn->recvResponse();
    if (!rsp) {
        error("recvResponse error");
        return;
    }
    info("rsp:%s", rsp->toString().c_str());
}

void test() {
    IOManager iom(2);
    iom.schedule(run);
}


#endif //SERVERFRAMEWORK_TEST_HTTP_CONNCTION_H
