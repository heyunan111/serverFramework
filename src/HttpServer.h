/**
  ******************************************************************************
  * @file           : HttpServer.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/23
  ******************************************************************************
  */
#pragma once

#include "TcpServer.h"
#include "HttpSession.h"
#include "Servlet.h"

namespace hyn {

/**
 *@brief HttpServer
 */
class HttpServer : public TcpServer {
public:
    using ptr = std::shared_ptr<HttpServer>;

    /**
     *@brief 构造函数
     *@param keepalive 是否保持长连接
     *@param  worker socket客户端工作的协程调度器
     *@param ioWorker 工作调度器
     *@param acceptWorker 服务器socket执行接收socket连接的协程调度器
     */
    explicit HttpServer(bool keepalive = false, IOManager *worker = IOManager::GetThis(),
                        IOManager *ioWorker = IOManager::GetThis(), IOManager *acceptWorker = IOManager::GetThis());

    void setName(const std::string &mName) override;

    const ServletDispatch::ptr &getDispatch() const {
        return m_dispatch;
    }

    void setDispatch(const ServletDispatch::ptr &mDispatch) {
        m_dispatch = mDispatch;
    }

protected:
    void handleClient(const Socket::ptr &client) override;

private:
    ///是否支持长连接
    bool m_isKeepalive;
    ///servlet分发器
    ServletDispatch::ptr m_dispatch;
};

} // hyn


