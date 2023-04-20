#pragma once

#include "http11_common.h"

typedef struct httpclient_parser {
    ///解析器当前的状态
    int cs;
    ///响应报文的正文开始位置
    size_t body_start;
    ///响应报文中的 Content-Length 头部字段值
    int content_len;
    ///响应状态码
    int status;
    ///响应报文是否采用 chunked 编码
    int chunked;
    ///chunked 编码是否解析完成
    int chunks_done;
    ///是否为关闭连接的响应报文
    int close;
    ///已读取的字节数
    size_t nread;
    ///标记位置
    size_t mark;
    ///HTTP 头部字段开始位置
    size_t field_start;
    ///HTTP 头部字段长度
    size_t field_len;
    ///用户数据
    void *data;

    ///HTTP 头部字段回调函数
    field_cb http_field;
    ///响应状态码原因短语回调函数
    element_cb reason_phrase;
    ///响应状态码回调函数
    element_cb status_code;
    ///chunked 编码块大小回调函数
    element_cb chunk_size;
    ///HTTP 版本回调函数
    element_cb http_version;
    ///HTTP 头部解析完成回调函数
    element_cb header_done;
    ///chunked 编码最后一个块回调函数
    element_cb last_chunk;
} httpclient_parser;

///创建
int httpclient_parser_init(httpclient_parser *parser);


int httpclient_parser_finish(httpclient_parser *parser);

int httpclient_parser_execute(httpclient_parser *parser, const char *data, size_t len, size_t off);

int httpclient_parser_has_error(httpclient_parser *parser);

int httpclient_parser_is_finished(httpclient_parser *parser);

#define httpclient_parser_nread(parser) (parser)->nread 
