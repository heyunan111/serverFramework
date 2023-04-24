/**
  ******************************************************************************
  * @file           : HttpConnection.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/24
  ******************************************************************************
  */

#pragma once

#include "SocketStream.h"
#include "HttpParser.h"

namespace hyn {

/**
 *@brief HttpConnection
 */
class HttpConnection : public SocketStream {
public:
    using ptr = std::shared_ptr<HttpConnection>;

    /**
     * @brief 构造函数
     * @param[in] sock Socket类
     * @param[in] owner 是否掌握所有权
     */
    HttpConnection(Socket::ptr sock, bool owner = true);

    ~HttpConnection();

    /**
    * @brief 接收HTTP响应
    */
    http::HttpResponse::ptr recvResponse();

    /**
     * @brief 发送HTTP请求
     * @param[in] req HTTP请求结构
     */
    int sendRequest(const http::HttpRequest::ptr &req);

private:
    ///创建时间
    uint64_t m_createTime = 0;
    ///请求次数
    uint64_t m_request = 0;
};

} // hyn

