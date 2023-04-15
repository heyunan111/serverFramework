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
#include "IOManager.h"

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

int64_t Socket::getRecvTimeout() const {
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
    Socket::ptr sock(new Socket(m_family, m_type, m_protocol));
    int newsock = ::accept(m_sock, nullptr, nullptr);
    if (newsock == -1) {
        error("accept(socket:%d) error,errno = %d,errstr = %s", m_sock, errno, strerror(errno));
        return nullptr;
    }
    if (sock->init(newsock))
        return sock;
    return nullptr;

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
    if (m_remoteAddress)
        return m_remoteAddress;
    Address::ptr res;
    switch (m_family) {
        case AF_INET: {
            res.reset(new IPv4Address());
            break;
        }
        case AF_INET6: {
            res.reset(new IPv6Address());
            break;
        }
        case AF_UNIX: {
            res.reset(new UnixAddress());
            break;
        }
        default:
            res.reset(new UnknowAddress(m_family));
            break;
    }
    socklen_t addrlen = res->getAddrLen();
    if (getpeername(m_sock, res->getAddr(), &addrlen)) {
        error("getpeername error sock:%d,errno:%d,strerror:%s", m_sock, errno, strerror(errno));
        return Address::ptr(new UnknowAddress(m_family));
    }
    if (m_family == AF_UNIX) {
        UnixAddress::ptr addr = std::dynamic_pointer_cast<UnixAddress>(res);
        addr->setAddrLen(addrlen);
    }
    m_remoteAddress = res;
    return m_remoteAddress;
}

Address::ptr Socket::getLocalAddress() {
    if (m_localAddress)
        return m_localAddress;
    Address::ptr res;
    switch (m_family) {
        case AF_INET: {
            res.reset(new IPv4Address());
            break;
        }
        case AF_INET6: {
            res.reset(new IPv6Address());
            break;
        }
        case AF_UNIX: {
            res.reset(new UnixAddress());
            break;
        }
        default:
            res.reset(new UnknowAddress(m_family));
            break;
    }
    socklen_t addrlen = res->getAddrLen();
    if (getsockname(m_sock, res->getAddr(), &addrlen)) {
        error("getsockname error sock:%d,errno:%d,strerror:%s", m_sock, errno, strerror(errno));
        return Address::ptr(new UnknowAddress(m_family));
    }
    if (m_family == AF_UNIX) {
        UnixAddress::ptr addr = std::dynamic_pointer_cast<UnixAddress>(res);
        addr->setAddrLen(addrlen);
    }
    m_localAddress = res;
    return m_localAddress;
}

bool Socket::isValid() const {
    return m_sock != -1;
}

int Socket::getError() {
    int error = 0;
    socklen_t len = sizeof(error);
    if (!getOption(SOL_SOCKET, SO_ERROR, &error, &len)) {
        error = errno;
    }
    return error;
}

std::ostream &Socket::dump(std::ostream &os) const {
    os << "[Socket sock = " << m_sock << " m_isConnected = " << m_isConnected << " m_family = " << m_family
       << " type = " << m_type << " m_protocol = " << m_protocol;
    if (m_localAddress)
        os << " LocalAddress = " << m_localAddress->toString();
    if (m_remoteAddress)
        os << " RemoteAddress = " << m_remoteAddress->toString();
    os << "]";
    return os;
}

std::string Socket::toString() const {
    std::stringstream ss;
    dump(ss);
    return ss.str();
}

bool Socket::cancelRead() {
    return iomanager::IOManager::GetThis()->cancelEvent(m_sock, iomanager::IOManager::READ);
}

bool Socket::cancelWrite() {
    return iomanager::IOManager::GetThis()->cancelEvent(m_sock, iomanager::IOManager::WRITE);
}

bool Socket::cancelAccept() {
    return iomanager::IOManager::GetThis()->cancelEvent(m_sock, iomanager::IOManager::READ);
}

bool Socket::cancelAll() {
    return iomanager::IOManager::GetThis()->cancelAll(m_sock);
}

void Socket::initSock() {
    int val = 1;
    setOption(SOL_SOCKET, SO_REUSEADDR, val);
    if (m_type == SOCK_STREAM) {
        setOption(IPPROTO_TCP, TCP_NODELAY, val);
    }
}

void Socket::newSock() {
    m_sock = socket(m_family, m_type, m_protocol);
    if (m_sock != -1) {
        initSock();
    } else {
        error("socket(family:%d,type:%d,protocol%d)errno = %d,errstr = %s", m_family, m_type, m_protocol, errno,
              strerror(errno));
    }
}

bool Socket::init(int sock) {
    FdCtx::ptr ctx = FdMgr::GetInstance()->get(sock);
    if (ctx && ctx->is_socket() && !ctx->is_close()) {
        m_sock = sock;
        m_isConnected = true;
        initSock();
        getRemoteAddress();
        getLocalAddress();
        return true;
    }
    return false;
}
} // hyn