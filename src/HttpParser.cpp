/**
  ******************************************************************************
  * @file           : HttpParser.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/20
  ******************************************************************************
  */



#include "HttpParser.h"

namespace hyn::http {
HttpRequestParser::HttpRequestParser() : m_error(0) {
    m_data.reset(new hyn::http::HttpRequest);
}

size_t HttpRequestParser::execute(char *data, size_t len, bool chunck) {
    return 0;
}

int HttpRequestParser::isFinished() {
    return 0;
}

int HttpRequestParser::hasError() {
    return 0;
}

uint64_t HttpRequestParser::getContentLength() {
    return 0;
}

uint64_t HttpRequestParser::GetHttpResponseBufferSize() {
    return 0;
}

uint64_t HttpRequestParser::GetHttpResponseMaxBodySize() {
    return 0;
}

void
HttpRequestParser::on_request_http_field(void *data, const char *field, size_t flen, const char *value, size_t vlen) {

}

void HttpRequestParser::on_request_method(void *data, const char *at, size_t length) {

}

void HttpRequestParser::on_request_uri(void *data, const char *at, size_t length) {

}

void HttpRequestParser::on_request_fragment(void *data, const char *at, size_t length) {

}

void HttpRequestParser::on_request_path(void *data, const char *at, size_t length) {

}

void HttpRequestParser::on_request_query_string(void *data, const char *at, size_t length) {

}

void HttpRequestParser::on_request_http_version(void *data, const char *at, size_t length) {

}

void HttpRequestParser::on_request_header_done(void *data, const char *at, size_t length) {

}

HttpResponseParser::HttpResponseParser() {

}

size_t HttpResponseParser::execute(char *data, size_t len, bool chunck) {
    return 0;
}

int HttpResponseParser::isFinished() {
    return 0;
}

int HttpResponseParser::hasError() {
    return 0;
}

uint64_t HttpResponseParser::getContentLength() {
    return 0;
}

uint64_t HttpResponseParser::GetHttpResponseBufferSize() {
    return 0;
}

uint64_t HttpResponseParser::GetHttpResponseMaxBodySize() {
    return 0;
}

void
HttpResponseParser::on_response_http_field(void *data, const char *field, size_t flen, const char *value, size_t vlen) {

}

void HttpResponseParser::on_response_reason_phrase(void *data, const char *at, size_t length) {

}

void HttpResponseParser::on_response_status_code(void *data, const char *at, size_t length) {

}

void HttpResponseParser::on_response_chunk_size(void *data, const char *at, size_t length) {

}

void HttpResponseParser::on_response_http_version(void *data, const char *at, size_t length) {

}

void HttpResponseParser::on_response_header_done(void *data, const char *at, size_t length) {

}

void HttpResponseParser::on_response_last_chunk(void *data, const char *at, size_t length) {

}
} // hyn