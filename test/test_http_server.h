/**
 ******************************************************************************
 * @file           : test_http_server.h
 * @author         : hyn
 * @brief          : None
 * @attention      : None
 * @date           : 2023/4/23
 ******************************************************************************
 */

#ifndef SERVERFRAMEWORK_TEST_HTTP_SERVER_H
#define SERVERFRAMEWORK_TEST_HTTP_SERVER_H

#include "../src/HttpServer.h"
#include "../src/Logger.h"

using namespace hyn;
using namespace http;

std::string htmlToString(const std::string &filePath) {
  std::ifstream file(filePath);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

void run() {
  HttpServer::ptr server(new HttpServer);
  Address::ptr addr = Address::LookupAnyIPAddress("0.0.0.0:5005");
  while (!server->bind(addr)) {
    sleep(2);
  }
  std::string htmlFilePath = "/home/hyn/serverFramework/index.html";
  std::string htmlString = htmlToString(htmlFilePath);
  auto sd = server->getDispatch();
  sd->addServlet("/hyn/xx",
                 [](const HttpRequest::ptr &req, const HttpResponse::ptr &resp,
                    const HttpSession::ptr &session) {
                   resp->setBody(req->toString());
                   return 0;
                 });
  sd->addGlobServlet("/hyn/*", [](const HttpRequest::ptr &req,
                                  const HttpResponse::ptr &resp,
                                  const HttpSession::ptr &session) {
    resp->setBody("Glob:\r\n" + req->toString());
    return 0;
  });
  sd->addServlet("/hyn/index", [htmlString](const HttpRequest::ptr &req,
                                            const HttpResponse::ptr &resp,
                                            const HttpSession::ptr &session) {
    resp->setBody(htmlString);
    return 0;
  });
  server->start();
}

void test() {
  IOManager iom(2);
  iom.schedule(run);
}

#endif // SERVERFRAMEWORK_TEST_HTTP_SERVER_H
