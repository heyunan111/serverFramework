/**
  ******************************************************************************
  * @file           : SocketStream.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/22
  ******************************************************************************
  */

#include "SocketStream.h"

#include <utility>

namespace hyn {

SocketStream::SocketStream(Socket::ptr sock, bool owner) : m_socket(std::move(sock)), m_owner(owner) {
}

SocketStream::~SocketStream() {
    if (m_owner && m_socket)
        m_socket->close();
}

int SocketStream::read(void *buffer, size_t length) {
    if (!isConnected())
        return -1;
    return m_socket->recv(buffer, length);
}

int SocketStream::read(ByteArray::ptr ba, size_t length) {
    if (!isConnected())
        return -1;
    std::vector<iovec> iovecs;
    ba->getReadBuffers(iovecs, length);
    int rt = m_socket->recv(&iovecs[0], iovecs.size());
    if (rt > 0)
        ba->setPos(ba->getPos() + rt);
    return rt;
}

int SocketStream::write(const void *buffer, size_t length) {
    if (!isConnected())
        return -1;
    return m_socket->send(buffer, length);
}

int SocketStream::write(ByteArray::ptr ba, size_t length) {
    if (!isConnected())
        return -1;
    std::vector<iovec> iovecs;
    ba->getWriteBuffers(iovecs, length);
    int rt = m_socket->send(&iovecs[0], iovecs.size());
    if (rt > 0)
        ba->setPos(ba->getPos() + rt);
    return rt;
}

void SocketStream::close() {
    if (m_socket)
        m_socket->close();
}

bool SocketStream::isConnected() const {
    return m_socket && m_socket->isConnected();
}
}