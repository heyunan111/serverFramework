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

#include <netdb.h>
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
    virtual ~Address() = default;

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

    /**
     * @brief 通过域名,IP,服务器名创建IPAddress
     * @param[in] address 域名,IP,服务器名等.举例: www.sylar.top
     * @param[in] port 端口号
     * @return 调用成功返回IPAddress,失败返回nullptr
     */
    static ptr Create(const std::string &address, uint16_t port);

    /**
     *@brief 获取该地址的广播地址
     *@param prefix_len 子网掩码位数
     *@return 成功返回IPAddress，失败nullptr
     */
    virtual IPAddress::ptr broadcastAddress(uint32_t prefix_len) = 0;

    /**
     *@brief 获取该地址的网段
     *@param prefix_len 子网掩码位数
     *@return 成功返回IPAddress，失败nullptr
     */
    virtual IPAddress::ptr networkAddress(uint32_t prefix_len) = 0;

    /**
     *@brief 获取子网掩码地址
     *@param prefix_len 子网掩码位数
     *@return 成功返回IPAddress，失败nullptr
     */
    virtual IPAddress::ptr subnetMask(uint32_t prefix_len) = 0;

    /**
     *@brief 获取端口号
     */
    [[nodiscard]] virtual uint32_t getPort() const = 0;

    /**
     *@brief 设置端口号
     */
    virtual void setPort(uint16_t port) = 0;
};

class IPv4Address : public IPAddress {
public:
    std::shared_ptr<IPv4Address> ptr;

    /**
    * @brief 通过sockaddr_in构造IPv4Address
    * @param[in] address sockaddr_in结构体
    */
    explicit IPv4Address(const sockaddr_in &addr);

    /**
    * @brief 通过地址和端口构造IPv4Address
    * @param[in] address IPv4地址
    * @param[in] port 端口号
    */
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

    IPv6Address();

    /**
     *@brief 通过sockaddr_in6构造
     *@param addr sockaddr_in6
     */
    explicit IPv6Address(const sockaddr_in6 &addr);

    /**
     *@brief 通过地址和端口构造
     *@param addr IPv6地址
     *@param port 端口
     *@return
     */
    explicit IPv6Address(const std::string &addr, uint32_t port = 0);

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
    sockaddr_in6 m_addr{};
};

class UnixAddress : public Address {
public:
    std::shared_ptr<UnixAddress> ptr;

    UnixAddress();

    /**
     * @brief 通过路径构造UnixAddress
     * @param in path UnixSocket路径(长度小于UNIX_PATH_MAX)
     */
    explicit UnixAddress(const std::string &path);

    sockaddr *getAddr() override;

    [[nodiscard]] const sockaddr *getAddr() const override;

    [[nodiscard]] socklen_t getAddrLen() const override;

    std::ostream &insert(std::ostream &os) const override;

    void setAddrLen(uint32_t v);

    [[nodiscard]] std::string getPath() const;

private:
    sockaddr_un m_addr{};
    socklen_t m_length{};
};

class UnknowAddress : public Address {
public:
    std::shared_ptr<UnknowAddress> ptr;

    explicit UnknowAddress(int family);

    explicit UnknowAddress(const sockaddr &addr);

    sockaddr *getAddr() override;

    [[nodiscard]] const sockaddr *getAddr() const override;

    [[nodiscard]] socklen_t getAddrLen() const override;

    std::ostream &insert(std::ostream &os) const override;

private:
    sockaddr m_addr{};
};

/**
 *@brief 重载<<，流式输出address
 */
std::ostream &operator<<(std::ostream &os, const Address &addr);

}//namespace hyn


