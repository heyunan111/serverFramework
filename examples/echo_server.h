/**
  ******************************************************************************
  * @file           : echo_server.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/21
  ******************************************************************************
  */



#ifndef SERVERFRAMEWORK_ECHO_SERVER_H
#define SERVERFRAMEWORK_ECHO_SERVER_H

#include "../hyn.h"

using namespace hyn;

class EchoServer : public TcpServer {
public:
    explicit EchoServer(int type);

    void handleClient(const Socket::ptr &client) override;

private:
    int m_type = 0;
};

EchoServer::EchoServer(int type)
        : m_type(type) {
}

void EchoServer::handleClient(const Socket::ptr &client) {
    info("handleClient:%s", client->toString().c_str());
    ByteArray::ptr ba(new ByteArray);
    while (true) {
        ba->clear();
        std::vector<iovec> iovs;
        ba->getWriteBuffers(iovs, 1024);

        int rt = client->recv(&iovs[0], iovs.size());
        if (rt == 0) {
            info("handleClient:%s close", client->toString().c_str());
            break;
        } else if (rt < 0) {
            error("client error rt=%d,errno=%d,errstr=%s", rt, errno, strerror(errno));
            break;
        }
        ba->setPos(ba->getPos() + rt);
        ba->setPos(0);
        //SYLAR_LOG_INFO(g_logger) << "recv rt=" << rt << " data=" << std::string((char*)iovs[0].iov_base, rt);
        if (m_type == 1) {//text
            std::cout << ba->toString();// << std::endl;
        } else {
            std::cout << ba->toHexString();// << std::endl;
        }
        std::cout.flush();
    }
}

int type = 1;

void run() {
    info("server type=%d", type);
    EchoServer::ptr es(new EchoServer(type));
    auto addr = Address::LookupAny("0.0.0.0:5005");
    while (!es->bind(addr)) {
        sleep(2);
    }
    es->start();
}

void test(const std::string &arg) {
    if (arg == "-t") {
        type = 1;
    } else if (arg == "-b") {
        type = 2;
    } else {
        return;
    }

    IOManager iom(2);
    iom.schedule(run);
}

#endif //SERVERFRAMEWORK_ECHO_SERVER_H
