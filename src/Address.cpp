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
#include "Logger.h"
#include "endian.h"


namespace hyn {

/**
 *@brief 计算子网掩码
 *@param 输入子网掩码前面有多少位1
 *@return 子网掩码的二进制
 */
template<class T>
static T CreateMask(uint32_t bits) {
    return (1 << (sizeof(T) * 8 - bits)) - 1;
}

/**
 *@brief 计算二进制表示中有多少个1
 */
template<class T>
static uint32_t CountBytes(T value) {
    uint32_t result = 0;
    for (; value; ++result) {
        value &= value - 1;
    }
    return result;
}

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

IPv4Address::IPv4Address(const sockaddr_in &addr) {
    m_addr = addr;
}

IPv4Address::IPv4Address(const std::string &addr, uint32_t port) {
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);
    if (addr.empty()) {
        m_addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        if (inet_pton(AF_INET, addr.c_str(), &m_addr.sin_addr.s_addr) != 1) {
            error("IPv4Address inet_pton error");
            return;
        }
    }
}

sockaddr *IPv4Address::getAddr() {
    return (sockaddr *) &m_addr;
}

const sockaddr *IPv4Address::getAddr() const {
    return (sockaddr *) &m_addr;
}

socklen_t IPv4Address::getAddrLen() const {
    return sizeof(m_addr);
}

std::ostream &IPv4Address::insert(std::ostream &os) const {
    char buff[INET_ADDRSTRLEN];
    if (!inet_ntop(AF_INET, &m_addr.sin_addr.s_addr, buff, INET_ADDRSTRLEN)) {
        error("IPv4Address::insert inet_ntop error");
        return os;
    }
    os << buff;
    return os;
}

IPAddress::ptr IPv4Address::broadcastAddress(uint32_t prefix_len) {
    if (prefix_len > 32)
        return nullptr;
    sockaddr_in baddr(m_addr);
    baddr.sin_addr.s_addr |= byteswapOnLittleEndian(CreateMask<uint32_t>(prefix_len));
    return std::make_shared<IPv4Address>(baddr);
}

IPAddress::ptr IPv4Address::networkAddress(uint32_t prefix_len) {
    if (prefix_len > 32) {
        return nullptr;
    }

    sockaddr_in baddr(m_addr);
    baddr.sin_addr.s_addr &= byteswapOnLittleEndian(
            CreateMask<uint32_t>(prefix_len));
    return std::make_shared<IPv4Address>(baddr);
}

IPAddress::ptr IPv4Address::subnetMask(uint32_t prefix_len) {
    sockaddr_in subnet{};
    memset(&subnet, 0, sizeof(subnet));
    subnet.sin_family = AF_INET;
    subnet.sin_addr.s_addr = ~byteswapOnLittleEndian(CreateMask<uint32_t>(prefix_len));
    return std::make_shared<IPv4Address>(subnet);
}

uint32_t IPv4Address::getPort() const {
    return ntohs(m_addr.sin_port);
}

void IPv4Address::setPort(uint16_t port) {
    m_addr.sin_port = htons(port);
}


IPv6Address::IPv6Address(const sockaddr_in6 &addr) {
    m_addr = addr;
}

IPv6Address::IPv6Address(const std::string &addr, uint32_t port) {
    m_addr.sin6_family = AF_INET;
    m_addr.sin6_port = htons(port);
    //ipv6地址不需要大小端转换，ipv6地址是固定的
}

sockaddr *IPv6Address::getAddr() {
    return nullptr;
}

const sockaddr *IPv6Address::getAddr() const {
    return nullptr;
}

socklen_t IPv6Address::getAddrLen() const {
    return 0;
}

std::ostream &IPv6Address::insert(std::ostream &os) const {

}

IPAddress::ptr IPv6Address::broadcastAddress(uint32_t prefix_len) {
    return hyn::IPAddress::ptr();
}

IPAddress::ptr IPv6Address::networkAddress(uint32_t prefix_len) {
    return hyn::IPAddress::ptr();
}

IPAddress::ptr IPv6Address::subnetMask(uint32_t prefix_len) {
    return hyn::IPAddress::ptr();
}

uint32_t IPv6Address::getPort() const {
    return 0;
}

void IPv6Address::setPort(uint16_t port) {

}
}