/**
  ******************************************************************************
  * @file           : Address.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/12
  ******************************************************************************
  */



#include "Address.h"
#include <sstream>

namespace hyn {

int Address::getFamily() const {
    return getAddr()->sa_family;
}

std::string Address::toString() const {
    std::stringstream ss;
    insert(ss);
    return ss.str();
}

bool Address::operator<(const Address &rhs) const {
    socklen_t minlen = std::min(getAddrLen(), rhs.getAddrLen());
    int result = memcmp(getAddr(), rhs.getAddr(), minlen);
    if (result < 0) {
        return true;
    } else if (result > 0) {
        return false;
    }
    if (getAddr() < rhs.getAddr()) {
        return true;
    }
    return false;
}

bool Address::operator==(const Address &rhs) const {
    return getAddr() == rhs.getAddr() && memcmp(getAddr(), rhs.getAddr(), getAddrLen()) == 0;
}

bool Address::operator!=(const Address &rhs) const {
    return !(*this == rhs);
}

IPv4Address::IPv4Address(uint32_t address, uint32_t port) {

}

sockaddr *IPv4Address::getAddr() {
    return nullptr;
}

const sockaddr *IPv4Address::getAddr() const {
    return nullptr;
}

socklen_t IPv4Address::getAddrLen() const {
    return 0;
}

std::ostream &IPv4Address::insert(std::ostream &os) const {
    return <#initializer#>;
}

IPAddress::ptr IPv4Address::broadcastAddress(uint32_t prefix_len) {
    return hyn::IPAddress::ptr();
}

IPAddress::ptr IPv4Address::networkAddress(uint32_t prefix_len) {
    return hyn::IPAddress::ptr();
}

IPAddress::ptr IPv4Address::subnetMask(uint32_t prefix_len) {
    return hyn::IPAddress::ptr();
}

uint32_t IPv4Address::getPort() const {
    return 0;
}

void IPv4Address::setPort(uint16_t port) {

}
}