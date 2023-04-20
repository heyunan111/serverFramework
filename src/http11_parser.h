#pragma once

#include "http11_common.h"

typedef struct http_parser {
    ///当前解析状态的状态机状态
    int cs;
    ///HTTP 响应正文部分的起始位置
    size_t body_start;
    ///HTTP 响应正文部分的长度
    int content_len;
    ///已经读取的字节数
    size_t nread;
    ///当前正在处理的 token 的起始位置
    size_t mark;
    ///HTTP 请求头部字段的起始位置
    size_t field_start;
    ///HTTP 请求头部字段的长度
    size_t field_len;
    ///HTTP 请求的查询字符串的起始位置
    size_t query_start;
    ///XML 格式的响应是否已发送
    int xml_sent;
    ///JSON 格式的响应是否已发送
    int json_sent;
    ///一个指针，指向在解析过程中需要使用的额外数据
    void *data;
    ///是否使用宽松的 URI 解析方式
    int uri_relaxed;
    ///指向处理 HTTP 请求头部字段的回调函数的指针
    field_cb http_field;
    ///指向处理 HTTP 请求方法的回调函数的指针
    element_cb request_method;
    ///指向处理 HTTP 请求 URI 的回调函数的指针
    element_cb request_uri;
    ///指向处理 HTTP URI 中的片段标识符的回调函数的指针
    element_cb fragment;
    ///指向处理 HTTP 请求路径的回调函数的指针
    element_cb request_path;
    ///指向处理 HTTP 查询字符串的回调函数的指针
    element_cb query_string;
    ///指向处理 HTTP 协议版本的回调函数的指针
    element_cb http_version;
    ///指向处理 HTTP 请求头部结束的回调函数的指针
    element_cb header_done;

} http_parser;

/**
 *@brief 创建
 *@param
 *@return
 */
int http_parser_init(http_parser *parser);

/**
 *@brief 判断是否结束
 *@param
 *@return 结束：1，错误：-1，其他：0
 */
int http_parser_finish(http_parser *parser);

size_t http_parser_execute(http_parser *parser, const char *data, size_t len, size_t off);

int http_parser_has_error(http_parser *parser);

int http_parser_is_finished(http_parser *parser);

#define http_parser_nread(parser) (parser)->nread 

