/**
  ******************************************************************************
  * @file           : HttpServer.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/23
  ******************************************************************************
  */



#include "HttpServer.h"
#include "Logger.h"

namespace hyn {
HttpServer::HttpServer(bool keepalive, IOManager *worker, IOManager *ioWorker, IOManager *acceptWorker) : TcpServer(
        worker, ioWorker, acceptWorker), m_isKeepalive(keepalive) {
    m_dispatch.reset(new ServletDispatch);

    m_type = "http";
    ///FIXME
    //m_dispatch->addServlet("/_/status", Servlet::ptr(new StatusServlet));
    //m_dispatch->addServlet("/_/config", Servlet::ptr(new ConfigServlet));
}

void HttpServer::setName(const std::string &mName) {
    TcpServer::setName(mName);
}

void HttpServer::handleClient(const Socket::ptr &client) {
    info("handleClient:%s", client->toString().c_str());
    HttpSession::ptr session(new HttpSession(client));
    for (;;) {
        auto req = session->recvRequest();
        if (!req) {
            debug("recv http request fail, errno=%d,errstr=%s,client=%s,keepalive=%d", errno, strerror(errno),
                  client->toString().c_str(), m_isKeepalive);
            break;
        }
        http::HttpResponse::ptr rsp(new http::HttpResponse(req->getVersion(), req->isAutoClose() || !m_isKeepalive));
        rsp->setHeader("Server", getName());
        m_dispatch->handle(req, rsp, session);
        session->sendResponse(rsp);
        if (!m_isKeepalive || req->isAutoClose()) {
            break;
        }
    }
    session->close();
}
} // hyn