/**
  ******************************************************************************
  * @file           : HttpParser.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/20
  ******************************************************************************
  */

#pragma once

#include "Http.h"
#include "http11_parser.h"
#include "httpclient_parser.h"


namespace hyn::http {

/**
 *@brief Http请求解析
 */
class HttpRequestParser {
public:
    using ptr = std::shared_ptr<HttpRequestParser>;

    /**
     *@brief 构造函数
     */
    HttpRequestParser();

    /**
     * @brief 解析HTTP响应协议
     * @param[in, out] data 协议数据内存
     * @param[in] len 协议数据内存大小
     * @param[in] chunck 是否在解析chunck
     * @return 返回实际解析的长度,并且移除已解析的数据
     */
    size_t execute(char *data, size_t len, bool chunck);

    /**
     * @brief 是否解析完成
     */
    int isFinished();

    /**
     * @brief 是否有错误
     */
    int hasError();

    /**
     * @brief 获取消息体长度
     */
    uint64_t getContentLength();

    /**
     * @brief 返回HTTP响应解析缓存大小
     */
    static uint64_t GetHttpRequestBufferSize();

    /**
     * @brief 返回HTTP响应最大消息体大小
     */
    static uint64_t GetHttpRequestMaxBodySize();

    /***************** getter and setter *****************/
    [[nodiscard]] const http_parser &getParser() const {
        return m_parser;
    }

    void setParser(const http_parser &mParser) {
        m_parser = mParser;
    }

    [[nodiscard]] const HttpRequest::ptr &getData() const {
        return m_data;
    }

    void setData(const HttpRequest::ptr &mData) {
        m_data = mData;
    }

    [[nodiscard]] int getError() const {
        return m_error;
    }

    void setError(int mError) {
        m_error = mError;
    }

private:
    /// http_parser
    http_parser m_parser{};
    /// HttpRequest结构
    HttpRequest::ptr m_data;
    /// 错误码
    /// 1000: invalid method
    /// 1001: invalid version
    /// 1002: invalid field
    int m_error;
};

/**
 *@brief Http响应解析
 */
class HttpResponseParser {
public:
    using ptr = std::shared_ptr<HttpResponseParser>;

    /**
     *@brief 构造函数
     */
    HttpResponseParser();

    /**
     * @brief 解析HTTP响应协议
     * @param[in, out] data 协议数据内存
     * @param[in] len 协议数据内存大小
     * @param[in] chunck 是否在解析chunck
     * @return 返回实际解析的长度,并且移除已解析的数据
     */
    size_t execute(char *data, size_t len, bool chunck);

    /**
     * @brief 是否解析完成
     */
    int isFinished();

    /**
     * @brief 是否有错误
     */
    int hasError();

    /**
     * @brief 获取消息体长度
     */
    uint64_t getContentLength();

    /**
     * @brief 返回HTTP响应解析缓存大小
     */
    static uint64_t GetHttpResponseBufferSize();

    /**
     * @brief 返回HTTP响应最大消息体大小
     */
    static uint64_t GetHttpResponseMaxBodySize();

    /***************** getter and setter *****************/
    [[nodiscard]] const httpclient_parser &getParser() const {
        return m_parser;
    }

    void setParser(const httpclient_parser &mParser) {
        m_parser = mParser;
    }

    [[nodiscard]] const HttpResponse::ptr &getData() const {
        return m_data;
    }

    void setData(const HttpResponse::ptr &mData) {
        m_data = mData;
    }

    [[nodiscard]] int getError() const {
        return m_error;
    }

    void setError(int mError) {
        m_error = mError;
    }
private:
    /// http_parser
    httpclient_parser m_parser{};
    /// HttpRequest结构
    HttpResponse::ptr m_data;
    /// 错误码
    /// 1000: invalid method
    /// 1001: invalid version
    /// 1002: invalid field
    int m_error;
};

/**
    *@brief 处理 HTTP 请求头部字段的回调函数
    *@param data 用户数据
    *@param field 字段名起始位置
    *@param flen 字段名长度
    *@param value 字段值起始位置
    *@param vlen 字段值长度
    */
void on_request_http_field(void *data, const char *field, size_t flen, const char *value, size_t vlen);

/**
 *@brief 处理 HTTP 请求方法的回调函数
 *@param data 用户数据
 *@param at 字符串起始位置
 *@param length 字符串长度
 */
void on_request_method(void *data, const char *at, size_t length);

/**
 *@brief 处理 HTTP 请求 URI 的回调函数
 *@param data 用户数据
 *@param at 字符串起始位置
 *@param length 字符串长度
 */
void on_request_uri(void *data, const char *at, size_t length);

/**
 *@brief 处理 HTTP URI 中的片段标识符的回调函数
 *@param data 用户数据
 *@param at 字符串起始位置
 *@param length 字符串长度
 */
void on_request_fragment(void *data, const char *at, size_t length);

/**
 *@brief 处理 HTTP 请求路径的回调函数
 *@param data 用户数据
 *@param at 字符串起始位置
 *@param length 字符串长度
 */
void on_request_path(void *data, const char *at, size_t length);

/**
 *@brief 处理 HTTP 查询字符串的回调函数
 *@param data 用户数据
 *@param at 字符串起始位置
 *@param length 字符串长度
 */
void on_request_query_string(void *data, const char *at, size_t length);

/**
 *@brief 处理 HTTP 协议版本的回调函数
 *@param data 用户数据
 *@param at 字符串起始位置
 *@param length 字符串长度
 */
void on_request_http_version(void *data, const char *at, size_t length);

/**
 *@brief 处理 HTTP 请求头部结束的回调函数
 *@param data 用户数据
 *@param at 字符串起始位置
 *@param length 字符串长度
 */
void on_request_header_done(void *data, const char *at, size_t length);


/**
 *@brief 处理 HTTP 请求头部字段的回调函数
 *@param data 用户数据
 *@param field 字段名起始位置
 *@param flen 字段名长度
 *@param value 字段值起始位置
 *@param vlen 字段值长度
 */
void on_response_http_field(void *data, const char *field, size_t flen, const char *value, size_t vlen);

/**
 *@brief 响应状态码原因短语回调函数
 *@param data 用户数据
 *@param at 字符串起始位置
 *@param length 字符串长度
 */
void on_response_reason_phrase(void *data, const char *at, size_t length);

/**
*@brief 响应状态码回调函数
*@param data 用户数据
*@param at 字符串起始位置
*@param length 字符串长度
*/
void on_response_status_code(void *data, const char *at, size_t length);

/**
 *@brief 编码块大小回调函数
 *@param data 用户数据
 *@param at 字符串起始位置
 *@param length 字符串长度
 */
void on_response_chunk_size(void *data, const char *at, size_t length);

/**
 *@brief 版本回调函数
 *@param data 用户数据
 *@param at 字符串起始位置
 *@param length 字符串长度
 */
void on_response_http_version(void *data, const char *at, size_t length);

/**
*@brief 头部解析完成回调函数
*@param data 用户数据
*@param at 字符串起始位置
*@param length 字符串长度
*/
void on_response_header_done(void *data, const char *at, size_t length);

/**
 *@brief 编码最后一个块回调函数
 *@param data 用户数据
 *@param at 字符串起始位置
 *@param length 字符串长度
 */
void on_response_last_chunk(void *data, const char *at, size_t length);


} // hyn::http


