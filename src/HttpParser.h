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


    /***************** getter and setter *****************/
    [[nodiscard]] const http_parser &getMParser() const {
        return m_parser;
    }

    void setMParser(const http_parser &mParser) {
        m_parser = mParser;
    }

    [[nodiscard]] const HttpRequest::ptr &getMData() const {
        return m_data;
    }

    void setMData(const HttpRequest::ptr &mData) {
        m_data = mData;
    }

    [[nodiscard]] int getMError() const {
        return m_error;
    }

    void setMError(int mError) {
        m_error = mError;
    }

private:
    /// http_parser
    http_parser m_parser;
    /// HttpRequest结构
    HttpRequest::ptr m_data;
    /// 错误码
    /// 1000: invalid method
    /// 1001: invalid version
    /// 1002: invalid field
    int m_error;
};

} // hyn::http


