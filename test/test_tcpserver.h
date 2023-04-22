/**
  ******************************************************************************
  * @file           : test_tcpserver.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/21
  ******************************************************************************
  */



#ifndef SERVERFRAMEWORK_TEST_TCPSERVER_H
#define SERVERFRAMEWORK_TEST_TCPSERVER_H

#include <memory>

#include "../src/Logger.h"
#include "../src/Address.h"
#include "../src/TcpServer.h"

void run() {
    auto addr1 = hyn::IPAddress::LookupAny("0.0.0.0:8033");
    auto addr2 = std::make_shared<hyn::UnixAddress>("/unix_addr");
    std::vector<hyn::Address::ptr> addrs;
    addrs.emplace_back(addr1);
    addrs.emplace_back(addr2);
    hyn::TcpServer::ptr tcp_server(new hyn::TcpServer);
    std::vector<hyn::Address::ptr> fail;
    tcp_server->bind(addrs, fail);
    tcp_server->start();
}


#endif //SERVERFRAMEWORK_TEST_TCPSERVER_H
