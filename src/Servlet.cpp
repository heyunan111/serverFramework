/**
  ******************************************************************************
  * @file           : Servlet.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/23
  ******************************************************************************
  */



#include "Servlet.h"

#include <utility>
#include <fnmatch.h>

namespace hyn {
FunctionServlet::FunctionServlet(FunctionServlet::callback cb) : Servlet("FunctionServlet"), m_cb(std::move(cb)) {
}

int32_t FunctionServlet::handle(hyn::http::HttpRequest::ptr request, hyn::http::HttpResponse::ptr response,
                                hyn::HttpSession::ptr session) {
    return m_cb(request, response, session);
}

int32_t ServletDispatch::handle(hyn::http::HttpRequest::ptr request, hyn::http::HttpResponse::ptr response,
                                hyn::HttpSession::ptr session) {
    auto servlet = getMatchedServlet(request->getPath());
    if (servlet)
        servlet->handle(request, response, session);
    return 0;
}

void ServletDispatch::addServlet(const std::string &uri, const Servlet::ptr &servlet) {
    RWMutexType::WriteLock lock(m_mutex);
    m_datas[uri] = std::make_shared<HoldServletCreator>(servlet);
}

void ServletDispatch::addServlet(const std::string &uri, const FunctionServlet::callback &cb) {
    RWMutexType::WriteLock lock(m_mutex);
    m_datas[uri] = std::make_shared<HoldServletCreator>(std::make_shared<FunctionServlet>(cb));
}

//存在多个相同 URI 的 Servlet 实例是因为同一个 Web 应用程序中可能会有多个处理相同 URI 的 Servlet，这些 Servlet 可能在
//不同的情况下被调用例如，一个应用程序中可能有一个 Servlet 用于处理用户登录请求，而另一个 Servlet 用于处理用户注册请求，
//它们可能都使用相同的 URI，例如 "/user"。
//在这种情况下，每个 Servlet 都是独立的，它们可能会有不同的实现方式，处理不同的业务逻辑。因此，可以在一个应用程序中存在多个相同
//URI 的 Servlet 实例。在 Servlet 容器中，通过 Servlet 映射来区分不同 URI 的 Servlet，确保每个请求都能被正确的处理。
void ServletDispatch::addGlobServlet(const std::string &uri, const Servlet::ptr &servlet) {
    RWMutexType::WriteLock lock(m_mutex);
    for (auto it = m_globs.begin(); it != m_globs.end(); ++it) {
        if (it->first == uri) {
            m_globs.erase(it);
            break;
        }
    }
    m_globs.emplace_back(uri, std::make_shared<HoldServletCreator>(servlet));
}

void ServletDispatch::addGlobServlet(const std::string &uri, const FunctionServlet::callback &cb) {
    RWMutexType::WriteLock lock(m_mutex);
    m_globs.emplace_back(uri, std::make_shared<HoldServletCreator>(std::make_shared<FunctionServlet>(cb)));
}

void ServletDispatch::addServletCreator(const std::string &uri, IServletCreator::ptr creator) {
    RWMutexType::WriteLock lock(m_mutex);
    m_datas[uri] = std::move(creator);
}

void ServletDispatch::addGlobServletCreator(const std::string &uri, const IServletCreator::ptr &creator) {
    RWMutexType::WriteLock lock(m_mutex);
    for (auto it = m_globs.begin();
         it != m_globs.end(); ++it) {
        if (it->first == uri) {
            m_globs.erase(it);
            break;
        }
    }
    m_globs.emplace_back(uri, creator);
}

void ServletDispatch::delServlet(const std::string &uri) {
    RWMutexType::WriteLock lock(m_mutex);
    m_datas.erase(uri);
}

void ServletDispatch::delGlobServlet(const std::string &uri) {
    RWMutexType::WriteLock lock(m_mutex);
    for (auto it = m_globs.begin(); it != m_globs.end(); ++it) {
        if (it->first == uri) {
            m_globs.erase(it);
            break;
        }
    }
}

void ServletDispatch::listAllServletCreator(std::map<std::string, IServletCreator::ptr> &infos) {
    RWMutexType::ReadLock lock(m_mutex);
    for (auto &i: m_datas)
        infos[i.first] = i.second;
}

void ServletDispatch::listAllGlobServletCreator(std::map<std::string, IServletCreator::ptr> &infos) {
    RWMutexType::ReadLock lock(m_mutex);
    for (auto &i: m_globs)
        infos[i.first] = i.second;
}

Servlet::ptr ServletDispatch::getServlet(const std::string &uri) {
    RWMutexType::ReadLock lock(m_mutex);
    auto it = m_datas.find(uri);
    return it == m_datas.end() ? nullptr : it->second->get();
}

Servlet::ptr ServletDispatch::getGlobServlet(const std::string &uri) {
    RWMutexType::ReadLock lock(m_mutex);
    for (auto &m_data: m_datas) {
        if (m_data.first == uri)
            return m_data.second->get();
    }
    return nullptr;
}

Servlet::ptr ServletDispatch::getMatchedServlet(const std::string &uri) {
    RWMutexType::ReadLock lock(m_mutex);
    auto mit = m_datas.find(uri);
    if (mit != m_datas.end()) {
        return mit->second->get();
    }
    for (auto &m_glob: m_globs) {
        if (!fnmatch(m_glob.first.c_str(), uri.c_str(), 0)) {
            return m_glob.second->get();
        }
    }
    return m_default;
}

NotFoundServlet::NotFoundServlet(const std::string &name) : Servlet("NotFoundServlet"), m_name(name) {
    m_content = "<html><head><title>404 Not Found"
                "</title></head><body><center><h1>404 Not Found</h1></center>"
                "<hr><center>" + name + "</center></body></html>";
}

int32_t NotFoundServlet::handle(hyn::http::HttpRequest::ptr request, hyn::http::HttpResponse::ptr response,
                                hyn::HttpSession::ptr session) {
    response->setStatus(hyn::http::HttpStatus::NOT_FOUND);
    response->setHeader("Server", "hyn/1.0.0");
    response->setHeader("Content-Type", "text/html");
    response->setBody(m_content);
    return 0;
}
} // hyn