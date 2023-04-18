/**
  ******************************************************************************
  * @file           : Http.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/17
  ******************************************************************************
  */
#include "Http.h"

namespace hyn::http {

HttpMethod StringToHttpMethod(const std::string &str) {
#define XX(num, name, string)                           \
    if (strcmp(#string, str.c_str()) == 0){             \
    return HttpMethod::name;                            \
    }
    HTTP_METHOD_MAP(XX)
#undef XX
    return HttpMethod::INVALID_METHOD;
}

HttpMethod CharToHttpMethod(const char *str) {
#define XX(num, name, string)                           \
    if (strncmp(#string, str,strlen(#string)) == 0){    \
    return HttpMethod::name;                            \
    }
    HTTP_METHOD_MAP(XX)
#undef XX
    return HttpMethod::INVALID_METHOD;
}

const char *HttpMethodToString(const HttpMethod &httpMethod) {
    auto idx = static_cast<uint32_t>(httpMethod);
    if (idx >= (sizeof(s_method_string) / sizeof(s_method_string[0]))) {
        return "<unknown>";
    }
    return s_method_string[idx];
}

const char *HttpStatusToString(const HttpStatus &httpStatus) {
    switch (httpStatus) {
#define XX(code, name, msg) \
    case HttpStatus::name:\
    return #msg;
        HTTP_STATUS_MAP(XX)
#undef XX
        default:
            return "<unknown>";
    }
}
bool CaseInsensitiveLess::operator()(const std::string &lhs, const std::string &rhs) const {
    return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}

HttpRequest::HttpRequest(uint8_t version, bool close) : m_autoClose(close), m_isWebSocket(false), m_parserParamFlag(0),
                                                        m_method(HttpMethod::GET), m_version(version), m_path("/") {
}

std::shared_ptr<HttpRequest> HttpRequest::creatResponse() {
    ///FIXME
}

std::string HttpRequest::getHeader(const std::string &key, const std::string &def) const {
    auto it = m_headers.find(key);
    return it == m_headers.end() ? def : it->second;
}

std::string HttpRequest::getParam(const std::string &key, const std::string &def) {
    ///FIXME
    auto it = m_params.find(key);
    return it == m_params.end() ? def : it->second;
}

std::string HttpRequest::getCookie(const std::string &key, const std::string &def) {
    ///FIXME
    auto it = m_cookies.find(key);
    return it == m_cookies.end() ? def : it->second;
}

void HttpRequest::setHeader(const std::string &key, const std::string &value) {
    m_headers[key] = value;
}

void HttpRequest::setParam(const std::string &key, const std::string &value) {
    m_params[key] = value;
}

void HttpRequest::setCookie(const std::string &key, const std::string &value) {
    m_cookies[key] = value;
}

void HttpRequest::delHeader(const std::string &key) {
    m_headers.erase(key);
}

void HttpRequest::delParam(const std::string &key) {
    m_params.erase(key);
}

void HttpRequest::delCookie(const std::string &key) {
    m_cookies.erase(key);
}

bool HttpRequest::hasHeader(const std::string &key, std::string *value) {
    auto it = m_headers.find(key);
    if (it == m_headers.end()) {
        return false;
    }
    if (value) {
        *value = it->second;
    }
    return true;
}

bool HttpRequest::hasParam(const std::string &key, std::string *value) {
    auto it = m_params.find(key);
    if (it == m_params.end()) {
        return false;
    }
    if (value) {
        *value = it->second;
    }
    return true;
}

bool HttpRequest::hasCookie(const std::string &key, std::string *value) {
    auto it = m_cookies.find(key);
    if (it == m_cookies.end()) {
        return false;
    }
    if (value) {
        *value = it->second;
    }
    return true;
}

std::ostream &HttpRequest::dump(std::ostream &os) const {

    //<method> <request-URL> <version>
    //<headers>
    //<entity-body>

    //GET /index.html?search=keyword#section1 HTTP/1.1
    //Host: www.example.com
    //User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.82 Safari/537.36
    //Connection: keep-alive

    os << HttpMethodToString(m_method) << " " << m_path << (m_query.empty() ? "" : "?") << m_query
       << (m_fragment.empty() ? "" : "#") << m_fragment << "HTTP/" << (static_cast<uint32_t>(m_version >> 4)) <<
       "." << static_cast<uint32_t>(m_version & 0x0f) << "\r\n";
    if (!m_isWebSocket)
        os << "connection: " << (m_autoClose ? "close" : "keep-alive") << "\r\n";
    for (auto &i: m_headers) {
        if (!m_isWebSocket && strcasecmp(i.first.c_str(), "connection") == 0)
            continue;
        os << i.first << ": " << i.second << "\r\n";
    }
    if (!m_body.empty()) {
        os << "connection-length: " << m_body.size() << "\r\n\r\n" << m_body;
    } else {
        os << "\r\n";
    }
    return os;
}

std::string HttpRequest::toString() const {
    std::stringstream ss;
    dump(ss);
    return ss.str();
}

void HttpRequest::initClose() {
    std::string conn = getHeader("connection");
    if (!conn.empty()) {
        if (strcasecmp(conn.c_str(), "keep-alive") == 0) {
            m_autoClose = false;
        } else {
            m_autoClose = true;
        }
    }
}

void HttpRequest::initPram() {

}

void HttpRequest::initQueryParam() {

}

void HttpRequest::initBodyParam() {

}

void HttpRequest::initCookies() {

}


} // hyn::http