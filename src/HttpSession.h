/**
  ******************************************************************************
  * @file           : HttpSession.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/22
  ******************************************************************************
  */

#pragma once

#include "SocketStream.h"
#include "Http.h"

namespace hyn {

/**
 *@brief HttpSession
 */
class HttpSession : public SocketStream {
public:
    using ptr = std::shared_ptr<HttpSession>;

    /**
      *@brief 构造函数
      *@param sock Socket类
      *@param owner 是否完全控制（是的话结束之后会关闭socket）
      */
    explicit HttpSession(Socket::ptr sock, bool owner = true);

    /**
     * @brief 接收HTTP请求
     */
    http::HttpRequest::ptr recvRequest();

    /**
     * @brief 发送HTTP响应
     * @param[in] rsp HTTP响应
     * @return >0 发送成功
     *         =0 对方关闭
     *         <0 Socket异常
     */
    int sendResponse(http::HttpResponse::ptr rsp);
};

} // hyn


