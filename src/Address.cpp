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

Address::ptr Address::Create(const sockaddr *addr) {
    if (addr == nullptr)
        return nullptr;
    Address::ptr res;
    switch (addr->sa_family) {
        case AF_INET:
            res.reset(new IPv4Address(*(const sockaddr_in *) addr));
            break;
        case AF_INET6:
            res.reset(new IPv6Address(*(const sockaddr_in6 *) addr));
            break;
        default:
            res.reset(new UnknowAddress(*addr));
            break;
    }
    return res;
}

bool Address::Lookup(std::vector<Address::ptr> &result, const std::string &host, int family, int type, int protocol) {
    addrinfo hints{}, *res, *next;
    hints.ai_addrlen = 0;
    hints.ai_addr = nullptr;
    hints.ai_family = family;
    hints.ai_flags = 0;
    hints.ai_canonname = nullptr;
    hints.ai_next = nullptr;
    hints.ai_protocol = protocol;
    hints.ai_socktype = type;

    ///主机名
    std::string node;
    ///服务名
    const char *service = nullptr;

    //对主机名进行解析，代码中首先判断主机名是否是 IPv6 地址，如果是，则通过字符串查找找到 IPv6 地址中的节点和服务信息；
    if (!host.empty() && host[0] == '[') {
        //该函数返回一个指向第一个匹配项的指针。如果没有找到匹配项，则返回NULL
        const char *end_of_ipv6 = strchr(host.c_str(), ']');
        if (end_of_ipv6) {
            if ((*end_of_ipv6 + 1) == ':') {
                service = end_of_ipv6 + 2;
            }
            node = host.substr(1, end_of_ipv6 - host.c_str() - 1);
        }
    }

    //如果不是，则从主机名字符串中查找节点和服务信息。
    if (node.empty()) {
        service = (const char *) memchr(host.c_str(), ':', host.size());
        if (service) {
            if (!strchr(service + 1, ':')) {
                node = host.substr(0, service - host.c_str());
                ++service;
            }
        }
    }

    if (node.empty()) {
        node = host;
    }

    //接下来将节点和服务信息作为参数调用 getaddrinfo() 函数进行地址解析。如果解析失败，则返回 false，否则将解析结果中的地址信息存储在
    int error_ = getaddrinfo(node.c_str(), service, &hints, &res);
    if (error_) {
        debug("Address::Lookup getaddrinfo error host :%s,family :%d, type :%d,err :%d,errstr :%s", host.c_str(),
              family,
              type, errno, strerror(errno));
        return false;
    }

    //result 中，其中每个地址通过 Create() 函数创建一个 Address 类型的智能指针，并将其加入到 result 中。
    next = res;
    while (next) {
        result.emplace_back(Create(next->ai_addr));
        next = next->ai_next;
    }

    //最后释放 getaddrinfo() 函数分配的资源，返回 result 是否为空的结果。
    freeaddrinfo(res);
    return !result.empty();
}

Address::ptr Address::LockupAny(const std::string &host, int family, int type, int protocol) {
    std::vector<Address::ptr> res;
    if (Address::Lookup(res, host, family, type, protocol))
        return res[0];
    return nullptr;
}

std::shared_ptr<IPAddress> Address::LookupAnyIPAddress(const std::string &host, int family, int type, int protocol) {
    std::vector<Address::ptr> res;
    if (Lookup(res, host, family, type, protocol)) {
        for (auto &i: res) {
            auto v = std::dynamic_pointer_cast<IPAddress>(i);
            if (v)
                return v;
        }
    }
    return nullptr;
}

/*ifa_next 是一个指向下一个接口地址结构体的指针。通过遍历这个链表，可以获得所有网络接口的信息。
ifa_name 是一个字符串，表示这个接口的名称。
ifa_flags 是一个无符号整数，表示这个接口的属性。
ifa_addr 是一个指向 sockaddr 结构体的指针，表示这个接口的 IP 地址。
ifa_netmask 是一个指向 sockaddr 结构体的指针，表示这个接口的子网掩码。
ifa_ifu 是一个共用体，包含了接口的广播地址或点对点目标地址。
ifa_data 是一个指向地址特定数据的指针，用于存储有关接口的其他信息。*/
bool Address::GetInterfaceAddresses(std::multimap<std::string, std::pair<Address::ptr, uint32_t>> &result, int family) {
    ifaddrs *next, *res;
    if (getifaddrs(&res) != 0) {
        debug("Address::GetInterfaceAddresses error err = %d, errstr = %s", errno, strerror(errno));
        return false;
    }

    try {
        for (next = res; next; next = next->ifa_next) {
            Address::ptr addr;
            uint32_t prefix_len = ~0u;
            if (family != AF_UNSPEC && family != next->ifa_addr->sa_family)
                continue;

            switch (next->ifa_addr->sa_family) {
                case AF_INET: {
                    addr = Address::Create(next->ifa_addr);
                    uint32_t netmask = ((sockaddr_in *) next->ifa_netmask)->sin_addr.s_addr;
                    prefix_len = CountBytes(netmask);
                    break;
                }
                case AF_INET6: {
                    addr = Address::Create(next->ifa_addr);
                    in6_addr &netmask = ((sockaddr_in6 *) next->ifa_netmask)->sin6_addr;
                    prefix_len = 0;
                    for (int i = 0; i < 16; ++i) {
                        prefix_len += CountBytes(netmask.s6_addr[i]);
                    }
                    break;
                }
                default:
                    break;
            }
            if (addr)
                result.insert(std::make_pair(next->ifa_name, std::make_pair(addr, prefix_len)));
        }
    } catch (...) {
        debug("Address::GetInterfaceAddresses exception");
        freeifaddrs(res);
        return false;
    }
    freeifaddrs(res);
    return !result.empty();
}

bool Address::GetInterfaceAddresses(std::vector<std::pair<Address::ptr, uint32_t>> &result, const std::string &iface,
                                    int family) {
    //如果传入的 iface 参数为空或者为 "*"，则表示获取本机
    if (iface.empty() || iface == "*") {
        if (family == AF_INET || family == AF_UNSPEC) {
            result.emplace_back(Address::ptr(new IPv4Address()), 0u);
        }
        if (family == AF_INET6 || family == AF_UNSPEC) {
            result.emplace_back(Address::ptr(new IPv6Address()), 0u);
        }
        return true;
    }
    std::multimap<std::string, std::pair<Address::ptr, uint32_t> > results;
    if (!GetInterfaceAddresses(results, family))
        return false;

    auto its = results.equal_range(iface);
    for (; its.first != its.second; ++its.first) {
        result.push_back(its.first->second);
    }
    return !result.empty();
}

IPAddress::ptr IPAddress::Create(const std::string &address, uint16_t port) {
    //AI_NUMERICHOST表示address参数必须是一个数值格式的地址，而不是主机名。
    // AF_UNSPEC表示结果协议族既可以是IPv4，也可以是IPv6。

    addrinfo hints{}, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;

    //getaddrinfo 函数的返回值为 0 表示成功，否则返回一个非零的错误码。如果成功，res 参数指向一个链表，
    // 我们需要遍历这个链表来获取网络地址信息，然后将这些信息存储到套接字地址结构体中，最终用于套接字的创
    // 建和连接等操作。遍历完链表后，我们需要使用 freeaddrinfo 函数来释放 getaddrinfo 分配的内存空间。

    int error_ = getaddrinfo(address.c_str(), nullptr, &hints, &res);
    if (error_ != 0) {
        error("IPAddress::Create getaddrinfo error,errno = %d,errtr = %s", errno, strerror(errno));
        return nullptr;
    }

    try {
        IPAddress::ptr result = std::dynamic_pointer_cast<IPAddress>(Address::Create(res->ai_addr));
        if (result)
            result->setPort(port);
        freeaddrinfo(res);
        return result;
    } catch (...) {
        freeaddrinfo(res);
        return nullptr;
    }
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
    os << buff << ":" << ntohs(m_addr.sin_port);
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

IPv6Address::IPv6Address() {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin6_family = AF_INET6;
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
    os << "[" << buff << "]:" << ntohs(m_addr.sin6_port);
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


UnknowAddress::UnknowAddress(int family) {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sa_family = family;
}

UnknowAddress::UnknowAddress(const sockaddr &addr) {
    m_addr = addr;
}

sockaddr *UnknowAddress::getAddr() {
    return &m_addr;
}

const sockaddr *UnknowAddress::getAddr() const {
    return &m_addr;
}

socklen_t UnknowAddress::getAddrLen() const {
    return sizeof(m_addr);
}

std::ostream &UnknowAddress::insert(std::ostream &os) const {
    os << "[UnknownAddress family=" << m_addr.sa_family << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Address &addr) {
    return addr.insert(os);
}

}