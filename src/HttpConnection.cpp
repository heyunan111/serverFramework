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
using namespace http;
HttpConnection::HttpConnection(Socket::ptr sock, bool owner) : SocketStream(std::move(sock), owner) {
}

HttpConnection::~HttpConnection() {
    debug("~HttpConnection");
}

http::HttpResponse::ptr HttpConnection::recvResponse() {
    HttpResponseParser::ptr Parser(new HttpResponseParser);
    uint64_t buff_size = HttpResponseParser::GetHttpResponseBufferSize();
    std::shared_ptr<char> buff(new char[buff_size + 1], [](const char *ptr) {
        delete[] ptr;
    });
    char *data = buff.get();
    int offset = 0;
    for (;;) {
        int len = read(data + offset, buff_size - offset);
        if (len <= 0) {
            close();
            return nullptr;
        }
        len += offset;
        data[len] = '\0';
        size_t nparser = Parser->execute(data, len, false);
        if (Parser->hasError()) {
            close();
            return nullptr;
        }
        offset = len - nparser;
        if (offset == (int) buff_size) {
            close();
            return nullptr;
        }
        if (Parser->isFinished())
            break;
    }
    auto &client_parser = Parser->getParser();
    std::string body;
    if (client_parser.chunked) {
        int len = offset;
        do {
            bool begin = true;
            do {
                if (!begin || len == 0) {
                    int rt = read(data + len, buff_size - len);
                    if (rt <= 0) {
                        close();
                        return nullptr;
                    }
                    len += rt;
                }
                data[len] = '\0';
                size_t nparse = Parser->execute(data, len, true);
                if (Parser->hasError()) {
                    close();
                    return nullptr;
                }
                len -= nparse;
                if (len == (int) buff_size) {
                    close();
                    return nullptr;
                }
                begin = false;

                debug("content_len:%d", client_parser.content_len);
                if (client_parser.content_len + 2 <= len) {
                    body.append(data, client_parser.content_len);
                    memmove(data, data + client_parser.content_len + 2, len - client_parser.content_len - 2);
                    len -= client_parser.content_len + 2;
                } else {
                    body.append(data, len);
                    int left = client_parser.content_len - len + 2;
                    while (left > 0) {
                        int rt = read(data, left > (int) buff_size ? (int) buff_size : left);
                        if (rt <= 0) {
                            close();
                            return nullptr;
                        }
                        body.append(data, rt);
                        left -= rt;
                    }
                    body.resize(body.size() - 2);
                    len = 0;
                }
            } while (!Parser->isFinished());
        } while (!client_parser.chunks_done);
    } else {
        auto length = (int64_t) Parser->getContentLength();
        if (length > 0) {
            body.resize(length);

            int len = 0;
            if (length >= offset) {
                memcpy(&body[0], data, offset);
                len = offset;
            } else {
                memcpy(&body[0], data, length);
                len = (int) length;
            }
            length -= offset;
            if (length > 0) {
                if (readFixSize(&body[len], length) <= 0) {
                    close();
                    return nullptr;
                }
            }
        }
    }
    /// FIXME:if(!body.empty())
    return Parser->getData();
}

int HttpConnection::sendRequest(const http::HttpRequest::ptr &req) {
    std::stringstream ss;
    ss << req->toString();
    std::string data = ss.str();
    return writeFixSize(data.c_str(), data.size());
}
} // hyn