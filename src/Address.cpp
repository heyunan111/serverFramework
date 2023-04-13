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
    if (inet_pton(AF_INET6, addr.c_str(), &m_addr.sin6_addr.s6_addr) == 1) {
        error("IPv6Address::IPv6Address inet_pton error");
    }
}

sockaddr *IPv6Address::getAddr() {
    return (sockaddr *) &m_addr;
}

const sockaddr *IPv6Address::getAddr() const {
    return (sockaddr *) &m_addr;
}

socklen_t IPv6Address::getAddrLen() const {
    return sizeof(m_addr);
}

std::ostream &IPv6Address::insert(std::ostream &os) const {
    char buff[INET6_ADDRSTRLEN];
    if (!inet_ntop(AF_INET6, &m_addr.sin6_addr.s6_addr, buff, INET6_ADDRSTRLEN)) {
        error("IPv6Address::insert inet_ntop error");
        return os;
    }
    os << buff;
    return os;
}

IPAddress::ptr IPv6Address::broadcastAddress(uint32_t prefix_len) {
    sockaddr_in6 baddr(m_addr);
    baddr.sin6_addr.s6_addr[prefix_len / 8] |=
            CreateMask<uint8_t>(prefix_len % 8);
    for (uint32_t i = prefix_len / 8 + 1; i < 16; ++i) {
        baddr.sin6_addr.s6_addr[i] = 0xff;
    }
    return std::make_shared<IPv6Address>(baddr);
}

IPAddress::ptr IPv6Address::networkAddress(uint32_t prefix_len) {
    sockaddr_in6 baddr(m_addr);
    baddr.sin6_addr.s6_addr[prefix_len / 8] &=
            CreateMask<uint8_t>(prefix_len % 8);
    for (uint32_t i = prefix_len / 8 + 1; i < 16; ++i) {
        baddr.sin6_addr.s6_addr[i] = 0x00;
    }
    return std::make_shared<IPv6Address>(baddr);
}

IPAddress::ptr IPv6Address::subnetMask(uint32_t prefix_len) {
    sockaddr_in6 subnet{};
    memset(&subnet, 0, sizeof(subnet));
    subnet.sin6_family = AF_INET6;
    subnet.sin6_addr.s6_addr[prefix_len / 8] =
            ~CreateMask<uint8_t>(prefix_len % 8);

    for (uint32_t i = 0; i < prefix_len / 8; ++i) {
        subnet.sin6_addr.s6_addr[i] = 0xff;
    }
    return std::make_shared<IPv6Address>(subnet);
}

uint32_t IPv6Address::getPort() const {
    return ntohs(m_addr.sin6_port);
}

void IPv6Address::setPort(uint16_t port) {
    m_addr.sin6_port = htons(port);
}

/*(sockaddr_un*)0是将0强制类型转换为指向sockaddr_un类型的指针，这个指针实际上是一个空指针。然后通过这个指针访问sun_path成员，
可以得到该成员变量的大小。
因为sun_path的长度为108，而sizeof(sockaddr_un)为110，所以MAX_PATH_LEN的值为108。需要注意的是，这里减去的1是因为Unix域套接
字的路径名的最后一个字节必须为null字节，因此不能使用。*/
static const size_t max_path_len = sizeof(((sockaddr_un *) nullptr)->sun_path) - 1;

UnixAddress::UnixAddress() {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sun_family = AF_UNIX;
    m_length = offsetof(sockaddr_un, sun_path) + max_path_len;
}

UnixAddress::UnixAddress(const std::string &path) {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sun_family = AF_UNIX;
    m_length = path.size() + 1;
    if (!path.empty() && path[0] == '\0') {
        --m_length;
    }

    if (m_length > sizeof(m_addr.sun_path)) {
        throw std::logic_error("path too long");
    }
    memcpy(m_addr.sun_path, path.c_str(), m_length);
    m_length += offsetof(sockaddr_un, sun_path);
}

sockaddr *UnixAddress::getAddr() {
    return (sockaddr *) &m_addr;
}

const sockaddr *UnixAddress::getAddr() const {
    return (sockaddr *) &m_addr;
}

socklen_t UnixAddress::getAddrLen() const {
    return m_length;
}

/*
 * "abstract socket"地址是一种特殊的Unix域套接字地址，它不是以文件路径的形式存在，而是以类似于TCP/IP套接字地址的形式存在。
 * 在这种情况下，sun_path的第一个字节会被设置为0，后面跟着一个以null字符结尾的字符串作为地址标识符。因此，如果m_length大于
 * offsetof(sockaddr_un, sun_path)（即地址中有地址标识符），且地址中的第一个字节为'\0'，则该地址被认为是一个"abstract
 * socket"地址，需要在输出地址字符串前添加"\0"。
 * */
std::ostream &UnixAddress::insert(std::ostream &os) const {
    if (m_length > offsetof(sockaddr_un, sun_path) && m_addr.sun_path[0] == '\0') {
        return os << "\\0" << std::string(m_addr.sun_path + 1, m_length - offsetof(sockaddr_un, sun_path) - 1);
    }
    return os << m_addr.sun_path;
}

void UnixAddress::setAddrLen(uint32_t v) {
    m_length = v;
}

std::string UnixAddress::getPath() const {
    std::stringstream ss;
    if (m_length > offsetof(sockaddr_un, sun_path) && m_addr.sun_path[0] == '\0') {
        ss << "\\0" << std::string(m_addr.sun_path + 1, m_length - offsetof(sockaddr_un, sun_path) - 1);
    } else {
        ss << m_addr.sun_path;
    }
    return ss.str();
}


}