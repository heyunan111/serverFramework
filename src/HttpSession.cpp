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
using namespace http;
HttpSession::HttpSession(Socket::ptr sock, bool owner) : SocketStream(std::move(sock), owner) {
}

http::HttpRequest::ptr HttpSession::recvRequest() {
    HttpRequestParser::ptr parser(new HttpRequestParser);
    auto buff_size = HttpRequestParser::GetHttpRequestBufferSize();
    std::shared_ptr<char> buffer(new char[buff_size], [](char *p) {
        delete[] p;
    });
    char *data = buffer.get();
    int offset = 0;
    for (;;) {
        int len = read(data + offset, buff_size - offset);
        if (len <= 0) {
            close();
            return nullptr;
        }
        len += offset;
        auto npar = parser->execute(data, len);
        if (parser->hasError()) {
            close();
            return nullptr;
        }
        offset = len - npar;
        if (offset == buff_size) { //请求过大
            close();
            return nullptr;
        }
        if (parser->isFinished())
            break;
    }
    int64_t length = parser->getContentLength();
    if (length > 0) {
        std::string body;
        body.resize(length);

        int len = 0;
        if (length >= offset) {
            memcpy(&body[0], data, offset);
            len = offset;
        } else {
            memcpy(&body[0], data, length);
            len = length;
        }
        length -= offset;
        if (length > 0) {
            if (readFixSize(&body[len], length) <= 0) {
                close();
                return nullptr;
            }
        }
        parser->getData()->setBody(body);
    }

    parser->getData()->initClose();
    return parser->getData();

}

int HttpSession::sendResponse(http::HttpResponse::ptr rsp) {
    std::stringstream ss;
    ss << rsp->toString();
    std::string data = ss.str();
    return writeFixSize(data.c_str(), data.size());
}
} // hyn