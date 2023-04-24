/**
  ******************************************************************************
  * @file           : HttpConnection.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/24
  ******************************************************************************
  */



#include "HttpConnection.h"
#include "Logger.h"

#include <utility>

namespace hyn {
HttpConnection::HttpConnection(Socket::ptr sock, bool owner) : SocketStream(std::move(sock), owner) {
}

HttpConnection::~HttpConnection() {
    debug("~HttpConnection");
}

http::HttpResponse::ptr HttpConnection::recvResponse() {
    return hyn::http::HttpResponse::ptr();
}

int HttpConnection::sendRequest(http::HttpRequest::ptr req) {
    std::stringstream ss;
    ss << req->toString();
    std::string data = ss.str();
    return writeFixSize(data.c_str(), data.size());
}
} // hyn