/**
  ******************************************************************************
  * @file           : HttpSession.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/22
  ******************************************************************************
  */



#include "HttpSession.h"
#include "HttpParser.h"

#include <utility>

namespace hyn {
HttpSession::HttpSession(Socket::ptr sock, bool owner) : SocketStream(std::move(sock), owner) {
}

http::HttpRequest::ptr HttpSession::recvRequest() {
    return hyn::http::HttpRequest::ptr();
}

int HttpSession::sendResponse(http::HttpResponse::ptr rsp) {
    return 0;
}
} // hyn