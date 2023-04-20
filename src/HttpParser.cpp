/**
  ******************************************************************************
  * @file           : HttpParser.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/20
  ******************************************************************************
  */


#include "Logger.h"
#include "HttpParser.h"
#include "iniFile.h"
namespace hyn::http {

///限制HTTP请求的缓冲区大小
static uint64_t s_http_request_buffer_size = 0;
///限制HTTP请求最大正文大小
static uint64_t s_http_request_max_body_size = 0;
///限制HTTP响应的缓冲区大小
static uint64_t s_http_response_buffer_size = 0;
///限制HTTP响应的最大正文大小
static uint64_t s_http_response_max_body_size = 0;


struct RequestSizeIniter {
    RequestSizeIniter() {
        int a = hyn::singleton::Singleton<hyn::ini::IniFile>::get_instance()->get("HTTP", "request_buffer_size");
        s_http_request_buffer_size = static_cast<uint64_t>(a);
        int b = hyn::singleton::Singleton<hyn::ini::IniFile>::get_instance()->get("HTTP", "request_max_body_size");
        s_http_request_max_body_size = static_cast<uint64_t>(b);
        int c = hyn::singleton::Singleton<hyn::ini::IniFile>::get_instance()->get("HTTP", "response_buffer_size");
        s_http_response_buffer_size = static_cast<uint64_t>(c);
        int d = hyn::singleton::Singleton<hyn::ini::IniFile>::get_instance()->get("HTTP", "response_max_body_size");
        s_http_response_max_body_size = static_cast<uint64_t>(d);
    }
};

static RequestSizeIniter init;

HttpRequestParser::HttpRequestParser() : m_error(0) {
    m_data.reset(new hyn::http::HttpRequest);
    http_parser_init(&m_parser);
    m_parser.request_method = on_request_method;
    m_parser.request_uri = on_request_uri;
    m_parser.fragment = on_request_fragment;
    m_parser.request_path = on_request_path;
    m_parser.query_string = on_request_query_string;
    m_parser.http_version = on_request_http_version;
    m_parser.header_done = on_request_header_done;
    m_parser.http_field = on_request_http_field;
    m_parser.data = this;
}


size_t HttpRequestParser::execute(char *data, size_t len) {
    //http_parser_execute函数返回一个size_t类型的值，表示解析器实际处理的数据长度。在函数内部，该值被保存在offset变量中。
    size_t offset = http_parser_execute(&m_parser, data, len, 0);

    //接下来，函数调用memmove函数，将缓冲区中未被解析的数据向前移动（覆盖已经解析过的数据）。这样，下一次解析时就可以继续从缓冲区剩余的数据开始解析。
    memmove(data, data + offset, (len - offset));
    return offset;
}

int HttpRequestParser::isFinished() {
    return http_parser_finish(&m_parser);
}

int HttpRequestParser::hasError() {
    return m_error || http_parser_has_error(&m_parser);
}

uint64_t HttpRequestParser::getContentLength() {
    return m_data->getHeaderAs<uint64_t>("content-length", 0);
}

uint64_t HttpRequestParser::GetHttpRequestBufferSize() {
    return s_http_request_buffer_size;
}

uint64_t HttpRequestParser::GetHttpRequestMaxBodySize() {
    return s_http_request_max_body_size;
}

void on_request_http_field(void *data, const char *field, size_t flen, const char *value, size_t vlen) {
    auto *parser = static_cast<HttpRequestParser *>(data);
    if (flen == 0) {
        warn("invalid http request field length == 0");
        return;
    }
    parser->getData()->setHeader(std::string(field, flen), std::string(value, vlen));
}

void on_request_method(void *data, const char *at, size_t length) {
    auto *parser = static_cast<HttpRequestParser *>(data);
    HttpMethod m = CharToHttpMethod(at);

    if (m == HttpMethod::INVALID_METHOD) {
        warn("invalid http request method: &s", std::string(at, length).c_str());
        parser->setError(1000);
        return;
    }
    parser->getData()->setMethod(m);
}

void on_request_uri(void *data, const char *at, size_t length) {
}

void on_request_fragment(void *data, const char *at, size_t length) {
    auto *parser = static_cast<HttpRequestParser *>(data);
    parser->getData()->setFragment(std::string(at, length));
}

void on_request_path(void *data, const char *at, size_t length) {
    auto *parser = static_cast<HttpRequestParser *>(data);
    parser->getData()->setPath(std::string(at, length));
}

void on_request_query_string(void *data, const char *at, size_t length) {
    auto *parser = static_cast<HttpRequestParser *>(data);
    parser->getData()->setQuery(std::string(at, length));
}

void on_request_http_version(void *data, const char *at, size_t length) {
    auto *parser = static_cast<HttpRequestParser *>(data);
    uint8_t v;
    if (strncmp(at, "HTTP/1.1", length) == 0) {
        v = 0x11;
    } else if (strncmp(at, "HTTP/1.0", length) == 0) {
        v = 0x10;
    } else {
        warn("invalid http request version: %s", std::string(at, length).c_str());
        parser->setError(1001);
        return;
    }
    parser->getData()->setVersion(v);
}

void on_request_header_done(void *data, const char *at, size_t length) {
}

HttpResponseParser::HttpResponseParser() : m_error(0) {
    m_data.reset(new hyn::http::HttpResponse);
    httpclient_parser_init(&m_parser);
    m_parser.reason_phrase = on_response_reason_phrase;
    m_parser.status_code = on_response_status_code;
    m_parser.chunk_size = on_response_chunk_size;
    m_parser.http_version = on_response_http_version;
    m_parser.header_done = on_response_header_done;
    m_parser.last_chunk = on_response_last_chunk;
    m_parser.http_field = on_response_http_field;
    m_parser.data = this;
}

size_t HttpResponseParser::execute(char *data, size_t len, bool chunck) {
    //如果chunk参数为true，它将初始化HTTP解析器。这是因为HTTP响应可以分块传输，并且如果传输是分块的，则需要初始化解析器以处理多个块。
    if (chunck)
        httpclient_parser_init(&m_parser);

    //然后，该函数将通过调用httpclient_parser_execute函数来解析响应。最后，它将从缓冲区中移动已解析的字节数，并返回已解析的字节数。
    size_t offset = httpclient_parser_execute(&m_parser, data, len, 0);
    memmove(data, data + offset, (len - offset));
    return offset;
}

int HttpResponseParser::isFinished() {
    return httpclient_parser_finish(&m_parser);
}

int HttpResponseParser::hasError() {
    return m_error || httpclient_parser_has_error(&m_parser);
}

uint64_t HttpResponseParser::getContentLength() {
    return m_data->getHeaderAs<uint64_t>("content-length", 0);
}

uint64_t HttpResponseParser::GetHttpResponseBufferSize() {
    return s_http_response_buffer_size;
}

uint64_t HttpResponseParser::GetHttpResponseMaxBodySize() {
    return s_http_response_max_body_size;
}

void
on_response_http_field(void *data, const char *field, size_t flen, const char *value, size_t vlen) {
    auto *parser = static_cast<HttpResponseParser *>(data);
    if (flen == 0) {
        warn("invalid http response field length == 0");
        return;
    }
    parser->getData()->setHeader(std::string(field, flen), std::string(value, vlen));
}

void on_response_reason_phrase(void *data, const char *at, size_t length) {
    auto *parser = static_cast<HttpResponseParser *>(data);
    parser->getData()->setReason(std::string(at, length));

}

void on_response_status_code(void *data, const char *at, size_t length) {
    auto *parser = static_cast<HttpResponseParser *>(data);
    auto s = static_cast<HttpStatus>(atoi(at));
    parser->getData()->setStatus(s);
}

void on_response_chunk_size(void *data, const char *at, size_t length) {
}

void on_response_http_version(void *data, const char *at, size_t length) {
    auto *parser = static_cast<HttpResponseParser *>(data);
    uint8_t v;
    if (strncmp(at, "HTTP/1.1", length) == 0) {
        v = 0x11;
    } else if (strncmp(at, "HTTP/1.0", length) == 0) {
        v = 0x10;
    } else {
        warn("invalid http response version: %s", std::string(at, length).c_str());
        parser->setError(1001);
        return;
    }

    parser->getData()->setVersion(v);
}

void on_response_header_done(void *data, const char *at, size_t length) {
}

void on_response_last_chunk(void *data, const char *at, size_t length) {
}
} // hyn