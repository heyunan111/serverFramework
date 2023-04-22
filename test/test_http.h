/**
  ******************************************************************************
  * @file           : test_http.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/19
  ******************************************************************************
  */



#ifndef SERVERFRAMEWORK_TEST_HTTP_H
#define SERVERFRAMEWORK_TEST_HTTP_H

#include <iostream>
#include "../src/Logger.h"
#include "../src/Http.h"

void test_req() {
    hyn::http::HttpRequest::ptr req(new hyn::http::HttpRequest);
    req->setHeader("host", "www.qq.com");
    req->setBody("hello world");
    req->dump(std::cout) << std::endl;
}

void test_resp() {
    hyn::http::HttpResponse::ptr resp(new hyn::http::HttpResponse);
    resp->setHeader("X-X", "qq");
    resp->setBody("hello world");
    resp->setStatus(hyn::http::HttpStatus::BAD_REQUEST);
    resp->setAutoClose(false);
    resp->dump(std::cout) << std::endl;
}

#endif //SERVERFRAMEWORK_TEST_HTTP_H
