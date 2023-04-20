/**
  ******************************************************************************
  * @file           : test_parser.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/20
  ******************************************************************************
  */



#ifndef SERVERFRAMEWORK_TEST_PARSER_H
#define SERVERFRAMEWORK_TEST_PARSER_H

#include "../hyn.h"

char test_request_data[] = "GET / HTTP/1.1\r\n"
                           "Host: www.qq.com\r\n"
                           "Content-Length: 10\r\n"
                           "123456789";

void test_request_parser() {
    hyn::http::HttpRequestParser parser;
    std::string tmp = test_request_data;
    size_t s = parser.execute(&test_request_data[0], tmp.size());
    info("execute rt = %d,has_error = %d,is_finished = %d,ContentLength = %d", s, parser.hasError(),
         parser.isFinished(), parser.getContentLength());
    info("%s", parser.getData()->toString().c_str());
    info("%s", tmp.c_str());
    info("---------------------------------------------------------------");
}

char test_response_data[] = "HTTP/1.1 200 OK\r\n"
                            "Date: Tue, 04 Jun 2019 15:43:56 GMT\r\n"
                            "Server: Apache\r\n"
                            "Last-Modified: Tue, 12 Jan 2010 13:48:00 GMT\r\n"
                            "ETag: \"51-47cf7e6ee8400\"\r\n"
                            "Accept-Ranges: bytes\r\n"
                            "Content-Length: 81\r\n"
                            "Cache-Control: max-age=86400\r\n"
                            "Expires: Wed, 05 Jun 2019 15:43:56 GMT\r\n"
                            "Connection: Close\r\n"
                            "Content-Type: text/html\r\n\r\n"
                            "<html>\r\n"
                            "<meta http-equiv=\"refresh\" content=\"0;url=http://www.baidu.com/\">\r\n"
                            "</html>\r\n";

void test_response_parser() {
    hyn::http::HttpResponseParser::ptr parser;
    std::string tmp = test_response_data;
    size_t s = parser->execute(&tmp[0], tmp.size(), true);
    info("execute rt = %d,has_error = %d,is_finished = %d,ContentLength = %d", s, parser->hasError(),
         parser->isFinished(), parser->getContentLength());
    tmp.resize(tmp.size() - s);
    info("%s", parser->getData()->toString().c_str());
    info("%s", tmp.c_str());
}

#endif //SERVERFRAMEWORK_TEST_PARSER_H
