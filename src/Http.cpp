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


}

std::string HttpRequest::getHeader(const std::string &key, const std::string &def) const {
    auto it = m_headers.find(key);
    return it == m_headers.end() ? def : it->second;
}

std::string HttpRequest::getParam(const std::string &key, const std::string &def) {
    return std::string();
}

std::string HttpRequest::getCookie(const std::string &key, const std::string &def) {
    return std::string();
}

void HttpRequest::setHeader(const std::string &key, const std::string &value) {

}

void HttpRequest::setParam(const std::string &key, const std::string &value) {

}

void HttpRequest::setCookie(const std::string &key, const std::string &value) {

}

void HttpRequest::delHeader(const std::string &key) {

}

void HttpRequest::delParam(const std::string &key) {

}

void HttpRequest::delCookie(const std::string &key) {

}

bool HttpRequest::hasHeader(const std::string &key, std::string *value) {
    return false;
}

bool HttpRequest::hasParam(const std::string &key, std::string *value) {
    return false;
}

bool HttpRequest::hasCookie(const std::string &key, std::string *value) {
    return false;
}
} // hyn::http