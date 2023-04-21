/**
  ******************************************************************************
  * @file           : TCPserver.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/20
  ******************************************************************************
  */
#pragma once

#include <memory>
#include <boost/noncopyable.hpp>
#include <functional>
#include "Address.h"
#include "IOManager.h"
#include "Socket.h"
#include "iniFile.h"

namespace hyn {
using namespace iomanager;

class TcpServer : public std::enable_shared_from_this<TcpServer> {
public:
    using ptr = std::shared_ptr<TcpServer>;

    /**
     *@brief 构造函数
     *@param  worker socket客户端工作的协程调度器
     *@param acceptWorker 服务器socket执行接收socket连接的协程调度器
     *@param ioWorker
     */
    explicit TcpServer(IOManager *worker = IOManager::GetThis(), IOManager *ioWorker = IOManager::GetThis(),
                       IOManager *acceptWorker = IOManager::GetThis());

    virtual ~TcpServer();

    /**
     *@brief 绑定地址
     *@param addr 地址
     *@return 是否成功
     */
    virtual bool bind(const Address::ptr &addr);

    /**
     *@brief 绑定地址
     *@param addrs 需要绑定的地址数组
     *@param [out] fails 绑定失败的地址
     *@return 是否成功绑定
     */
    virtual bool bind(const std::vector<Address::ptr> &addrs, std::vector<Address::ptr> &fails);

    /**
    * @brief 启动服务
    * @pre 需要bind成功后执行
    */
    virtual bool start();

    /**
     * @brief 停止服务
     */
    virtual void stop();

protected:
    /**
     *@brief 处理新连接的Sock类
     */
    virtual void handleClient(const Socket::ptr &client);

    /**
     *@brief 开始接收连接
     */
    virtual void startAccept(const Socket::ptr &sock);

    virtual std::string toString(const std::string &prefix = "");

private:
    ///新连接的socket工作的调度器
    iomanager::IOManager *m_worker;
    iomanager::IOManager *m_ioWorker;
    ///监听socket数组
    std::vector<Socket::ptr> m_socks;
    ///服务器socket接受连接的调度器
    iomanager::IOManager *m_acceptWorker;
    ///接收超时时间ms
    uint64_t m_recvTimeout;
    ///服务器名称
    std::string m_name;
    ///服务器类型
    std::string m_type{"tcp"};
    ///服务是否停止
    bool m_isStop;
public:
    [[nodiscard]] uint64_t getRecvTimeout() const {
        return m_recvTimeout;
    }

    void setRecvTimeout(uint64_t mRecvTimeout) {
        m_recvTimeout = mRecvTimeout;
    }

    [[nodiscard]] const std::string &getName() const {
        return m_name;
    }

    void setName(const std::string &mName) {
        m_name = mName;
    }

    [[nodiscard]] const std::string &getType() const {
        return m_type;
    }

    void setType(const std::string &mType) {
        m_type = mType;
    }

    [[nodiscard]] bool isIsStop() const {
        return m_isStop;
    }

    void setIsStop(bool mIsStop) {
        m_isStop = mIsStop;
    }

    [[nodiscard]] const std::vector<Socket::ptr> &getMSocks() const {
        return m_socks;
    }
};

} // hyn

