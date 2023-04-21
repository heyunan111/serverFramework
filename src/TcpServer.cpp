/**
  ******************************************************************************
  * @file           : TCPserver.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/20
  ******************************************************************************
  */



#include "TcpServer.h"
#include "Logger.h"

namespace hyn {
///FIXME:config
TcpServer::TcpServer(IOManager *worker, IOManager *ioWorker, IOManager *acceptWorker) : m_worker(worker),
                                                                                        m_ioWorker(ioWorker),
                                                                                        m_acceptWorker(acceptWorker),
                                                                                        m_recvTimeout(1200000),
                                                                                        m_name("/1.0.0"),
                                                                                        m_isStop(true) {}

TcpServer::~TcpServer() {
    for (auto &i: m_socks)
        i->close();
    m_socks.clear();
}

bool TcpServer::bind(const Address::ptr &addr) {
    Socket::ptr sock = Socket::CreateTCP(addr);
    if (!sock->bind(addr)) {
        error("bind fail errno:%d,errstr:%s,addr[%s]", errno, strerror(errno), addr->toString().c_str());
        return false;
    }
    if (!sock->listen()) {
        error("listen fail errno:%d,errstr:%s,addr[%s]", errno, strerror(errno), addr->toString().c_str());
        return false;
    }
    m_socks.emplace_back(sock);
    info("type:%s,name:%s is bind success", m_type.c_str(), m_name.c_str());
    return true;
}

bool TcpServer::bind(const std::vector<Address::ptr> &addrs, std::vector<Address::ptr> &fails) {
    for (auto &i: addrs) {
        if (!bind(i)) {
            fails.emplace_back(i);
        }
    }
    if (!fails.empty()) {
        m_socks.clear();
        return false;
    }
    return true;
}

bool TcpServer::start() {
    if (!m_isStop)
        return false;
    m_isStop = false;
    for (auto &i: m_socks) {
        m_acceptWorker->schedule([capture0 = shared_from_this(), i] { capture0->startAccept(i); });
    }
    return true;
}

void TcpServer::stop() {
    m_isStop = true;
    auto self = shared_from_this();
    m_acceptWorker->schedule([this, self]() {
        for (auto &sock: m_socks) {
            sock->cancelAll();
            sock->close();
        }
        m_socks.clear();
    });
}

void TcpServer::handleClient(const Socket::ptr &client) {
    info("handleClient:%s", client->toString().c_str());
}

void TcpServer::startAccept(const Socket::ptr &sock) {
    while (!m_isStop) {
        Socket::ptr client = sock->accept();
        if (client) {
            client->setRecvTimeout(static_cast<int64_t>(m_recvTimeout));
            m_ioWorker->schedule([capture0 = shared_from_this(), client] { capture0->handleClient(client); });
        } else {
            error("accept errno=%d,errstr=%s", errno, strerror(errno));
        }
    }
}

std::string TcpServer::toString(const std::string &prefix) {
    std::stringstream ss;
    ss << prefix << "[type=" << m_type
       << " name=" << m_name
       << " worker=" << (m_worker ? m_worker->get_name() : "")
       << " accept=" << (m_acceptWorker ? m_acceptWorker->get_name() : "")
       << " recv_timeout=" << m_recvTimeout << "]" << std::endl;
    std::string pfx = prefix.empty() ? "    " : prefix;
    for (auto &i: m_socks) {
        ss << pfx << pfx << i->toString() << std::endl;
    }
    return ss.str();
}

} // hyn