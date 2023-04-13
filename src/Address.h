/**
  ******************************************************************************
  * @file           : Address.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/12
  ******************************************************************************
  */
#pragma once

#include <memory>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
namespace hyn {

class IPAddress;

/**
 *@brief Address
 */
class Address {
public:
    typedef std::shared_ptr<Address> ptr;

    /**
     *@brief虚析构函数
     */
    virtual ~Address();

    /**
     *@brief 返回协议簇
     */
    [[nodiscard]] int getFamily() const;

    /**
     *@brief 返回sockaddr指针，只读
     */
    [[nodiscard]] virtual const sockaddr *getAddr() const = 0;

    /**
     *@brief 返回sockaddr指针，读写
     */
    virtual sockaddr *getAddr() = 0;

    /**
     *@brief 返回sockaddr长度
     */
    [[nodiscard]] virtual socklen_t getAddrLen() const = 0;

    /**
     *@brief 可读性输出地址
     */
    virtual std::ostream &insert(std::ostream &os) const = 0;

    /**
     *@brief 返回可读性字符串
     */
    [[nodiscard]] std::string toString() const;

    /**
     *@brief 小于
     */
    bool operator<(const Address &rhs) const;

    /**
     * @brief 等于函数
     */
    bool operator==(const Address &rhs) const;

    /**
     * @brief 不等于函数
     */
    bool operator!=(const Address &rhs) const;

    /**
     *@brief 通过sockaddr创建Address
     *@param in addr sockaddr指针
     *@param in addrlen sockaddr的长度
     *@return 返回和sockaddr匹配的Address,失败返回nullptr
     */
    static ptr Create(const sockaddr *addr, socklen_t addrlen);

    /**
     *@brief 通过host地址返回对应条件的所有address
     *@param out result 保存满足条件的address
     *@param in host 域名，服务器名等
     *@param in family 协议簇(AF_INET,AD_INET6)
     *@param in type scoket类型 如：SOCK_STREAM
     *@param in protocol 协议(TCP,UDP)
     *@return 返回是否转换成功
     */
    static bool Lookup(std::vector<Address::ptr> &result, const std::string &host, int family = AF_INET, int type = 0,
                       int protocol = 0);

    /**
     *@brief 通过host地址返回对应地址的任意Address
     *@param[in] host 域名,服务器名等.举例: www.sylar.top[:80] (方括号为可选内容)
     * @param[in] family 协议族(AF_INT, AF_INT6, AF_UNIX)
     * @param[in] type socketl类型SOCK_STREAM、SOCK_DGRAM 等
     * @param[in] protocol 协议,IPPROTO_TCP、IPPROTO_UDP 等
     * @return 返回满足条件的任意Address,失败返回nullptr
     */
    static ptr LockupAny(const std::string &host, int type = 0, int protocol = 0);


};

class IPAddress : public Address {
public:
    typedef std::shared_ptr<IPAddress> ptr;

    virtual IPAddress::ptr broadcastAddress(uint32_t prefix_len) = 0;

    virtual IPAddress::ptr networkAddress(uint32_t prefix_len) = 0;

    virtual IPAddress::ptr subnetMask(uint32_t prefix_len) = 0;

    [[nodiscard]] virtual uint32_t getPort() const = 0;

    virtual void setPort(uint16_t port) = 0;
};

class IPv4Address : public IPAddress {
public:
    std::shared_ptr<IPv4Address> ptr;

    explicit IPv4Address(const sockaddr_in &addr);

    explicit IPv4Address(const std::string &addr = "", uint32_t port = 0);

    sockaddr *getAddr() override;

    [[nodiscard]] const sockaddr *getAddr() const override;

    [[nodiscard]] socklen_t getAddrLen() const override;

    std::ostream &insert(std::ostream &os) const override;

    IPAddress::ptr broadcastAddress(uint32_t prefix_len) override;

    IPAddress::ptr networkAddress(uint32_t prefix_len) override;

    IPAddress::ptr subnetMask(uint32_t prefix_len) override;

    [[nodiscard]] uint32_t getPort() const override;

    void setPort(uint16_t port) override;

private:
    sockaddr_in m_addr{};
};

class IPv6Address : public IPAddress {
public:
    std::shared_ptr<IPv6Address> ptr;

    IPv6Address(const sockaddr_in6 &addr);

    IPv6Address(const std::string &addr, uint32_t port = 0);

    sockaddr *getAddr() override;

    const sockaddr *getAddr() const override;

    socklen_t getAddrLen() const override;

    std::ostream &insert(std::ostream &os) const override;

    IPAddress::ptr broadcastAddress(uint32_t prefix_len) override;

    IPAddress::ptr networkAddress(uint32_t prefix_len) override;

    IPAddress::ptr subnetMask(uint32_t prefix_len) override;

    uint32_t getPort() const override;

    void setPort(uint16_t port) override;

private:
    sockaddr_in6 m_addr;
};

class UnixAddress : public Address {
public:
    std::shared_ptr<UnixAddress> ptr;

    UnixAddress(const std::string &path);

    sockaddr *getAddr() override;

    const sockaddr *getAddr() const override;

    socklen_t getAddrLen() const override;

    std::ostream &insert(std::ostream &os) const override;

private:
    sockaddr_un m_addr;
    socklen_t m_length;
};

class UnknowAddress : public Address {
public:
    std::shared_ptr<UnknowAddress> ptr;

    UnknowAddress(int family);

    sockaddr *getAddr() override;

    const sockaddr *getAddr() const override;

    socklen_t getAddrLen() const override;

    std::ostream &insert(std::ostream &os) const override;

private:
    sockaddr m_addr;
};

}//namespace hyn


