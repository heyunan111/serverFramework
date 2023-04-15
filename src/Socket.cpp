/**
  ******************************************************************************
  * @file           : Socket.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/12
  ******************************************************************************
  */

#include "Socket.h"
#include "FDManger.h"
#include "Logger.h"

namespace hyn {
Socket::ptr Socket::CreateTCP(const Address::ptr &address) {
    Socket::ptr socket1(new Socket(address->getFamily(), static_cast<int>(Type::TCP), 0));
    return socket1;
}

/*对于UDP类型的Socket，在创建完Socket对象后还需要调用newSock()函数，因为UDP是无连接的协议，需要在发送数据之前创建套接字。
 *而TCP是有连接的协议，创建套接字的过程已经包含在了连接的过程中，所以不需要调用newSock()函数。*/

Socket::ptr Socket::CreateUDP(const Address::ptr &address) {
    Socket::ptr socket1(new Socket(address->getFamily(), static_cast<int>(Type::UDP), 0));
    socket1->newSock();
    socket1->m_isConnected = true;
    return socket1;
}

Socket::ptr Socket::CreateTCPSocket4() {
    Socket::ptr socket1(new Socket(static_cast<int>(Family::IPv4), static_cast<int>(Type::TCP), 0));
    return socket1;
}

Socket::ptr Socket::CreateUDPSocket4() {
    Socket::ptr socket1(new Socket(static_cast<int>(Family::IPv4), static_cast<int>(Type::UDP), 0));
    socket1->newSock();
    socket1->m_isConnected = true;
    return socket1;
}

Socket::ptr Socket::CreateTCPSocket6() {
    Socket::ptr socket1(new Socket(static_cast<int>(Family::IPv6), static_cast<int>(Type::TCP), 0));
    return socket1;
}

Socket::ptr Socket::CreateUDPSocket6() {
    Socket::ptr socket1(new Socket(static_cast<int>(Family::IPv6), static_cast<int>(Type::UDP), 0));
    socket1->newSock();
    socket1->m_isConnected = true;
    return socket1;
}

Socket::ptr Socket::CreateUnixTCPSocket() {
    Socket::ptr socket1(new Socket(static_cast<int>(Family::UNIX), static_cast<int>(Type::TCP), 0));
    return socket1;
}

Socket::ptr Socket::CreateUnixUDPSocket() {
    Socket::ptr socket1(new Socket(static_cast<int>(Family::UNIX), static_cast<int>(Type::UDP), 0));
    return socket1;
}

int64_t Socket::getSendTimeout() const {
    FdCtx::ptr ctx = FdMgr::GetInstance()->get(m_sock);
    if (ctx)
        return static_cast<int64_t>(ctx->get_time(SO_SNDTIMEO));
    return -1;
}

void Socket::setSendTimeout(int64_t v) {
    timeval tv{int(v / 1000), int(v % 1000 * 1000)};
    setOption(SOL_SOCKET, SO_SNDTIMEO, tv);
}

int64_t Socket::getRecvTimeout() {
    FdCtx::ptr ctx = FdMgr::GetInstance()->get(m_sock);
    if (ctx)
        return static_cast<int64_t>(ctx->get_time(SO_RCVTIMEO));
    return -1;
}

void Socket::setRecvTimeout(int64_t v) {
    timeval tv{int(v / 1000), int(v % 1000 * 1000)};
    setOption(SOL_SOCKET, SO_RCVTIMEO, tv);
}

bool Socket::getOption(int level, int option, void *result, socklen_t *len) {
    if (getsockopt(m_sock, level, option, result, len)) {
        debug("getsockopt error fd:%d,lever:%d,option:%d,errno:%d,errstr:%s", m_sock, level, option, errno,
              strerror(errno));
        return false;
    }
    return true;
}

bool Socket::setOption(int level, int option, const void *result, socklen_t len) {
    if (setsockopt(m_sock, level, option, result, len)) {
        debug("setOption error fd:%d,lever:%d,option:%d,errno:%d,errstr:%s", m_sock, level, option, errno,
              strerror(errno));
        return false;
    }
    return true;
}

Socket::ptr Socket::accept() {
    return hyn::Socket::ptr();
}

bool Socket::bind(Address::ptr addr) {
    return false;
}

bool Socket::connect(const Address::ptr addr, uint64_t timeout_ms) {
    return false;
}

bool Socket::reconnect(uint64_t timeout_ms) {
    return false;
}

bool Socket::listen(int backlog) {
    return false;
}

bool Socket::close() {
    return false;
}

int Socket::send(const void *buffer, size_t length, int flags) {
    return 0;
}

int Socket::send(const iovec *buffers, size_t length, int flags) {
    return 0;
}

int Socket::sendTo(const void *buffer, size_t length, const Address::ptr to, int flags) {
    return 0;
}

int Socket::sendTo(const iovec *buffers, size_t length, const Address::ptr to, int flags) {
    return 0;
}

int Socket::recv(void *buffer, size_t length, int flags) {
    return 0;
}

int Socket::recv(iovec *buffers, size_t length, int flags) {
    return 0;
}

int Socket::recvFrom(void *buffer, size_t length, Address::ptr from, int flags) {
    return 0;
}

int Socket::recvFrom(iovec *buffers, size_t length, Address::ptr from, int flags) {
    return 0;
}

Address::ptr Socket::getRemoteAddress() {
    return hyn::Address::ptr();
}

Address::ptr Socket::getLocalAddress() {
    return hyn::Address::ptr();
}

bool Socket::isValid() const {
    return false;
}

int Socket::getError() {
    return 0;
}

std::ostream &Socket::dump(std::ostream &os) const {

}

std::string Socket::toString() const {
    return std::string();
}

bool Socket::cancelRead() {
    return false;
}

bool Socket::cancelWrite() {
    return false;
}

bool Socket::cancelAccept() {
    return false;
}

bool Socket::cancelAll() {
    return false;
}

void Socket::initSock() {

}

void Socket::newSock() {

}

bool Socket::init(int sock) {
    return false;
}
} // hyn