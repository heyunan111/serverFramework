/**
  ******************************************************************************
  * @file           : Servlet.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/23
  ******************************************************************************
  */
#pragma once

#include <utility>

#include "Http.h"
#include "HttpSession.h"
#include "mutex.h"
#include "util.h"

namespace hyn {

/**
 *@brief Servlet
 *@note HTTP Servlet 框架的基类，所有自定义 Servlet 都应该继承自该类并实现其纯虚函数。
 */
class Servlet {
public:
    using ptr = std::shared_ptr<Servlet>;

    explicit Servlet(std::string name) : m_name(std::move(name)) {}

    virtual ~Servlet() = default;

    /**
     * @brief 处理请求
     * @param[in] request HTTP请求
     * @param[in] response HTTP响应
     * @param[in] session HTTP连接
     * @return 是否处理成功
     */
    virtual int32_t handle(hyn::http::HttpRequest::ptr request, hyn::http::HttpResponse::ptr response,
                           hyn::HttpSession::ptr session) = 0;

protected:
    std::string m_name;
public:
    [[nodiscard]] const std::string &getName() const {
        return m_name;
    }

    void setMName(const std::string &mName) {
        m_name = mName;
    }
};

/**
 *@brief 函数式servlet
 *@note 是根据 URL 请求路径中的函数名来执行对应的函数，并返回执行结果。
 */
class FunctionServlet : public Servlet {
public:
    using ptr = std::shared_ptr<FunctionServlet>;
    ///回调函数类型
    using callback = std::function<int32_t(hyn::http::HttpRequest::ptr request, hyn::http::HttpResponse::ptr response,
                                           hyn::HttpSession::ptr session)>;

    /**
     *@brief 构造函数
     *@param cb 回调函数
     */
    explicit FunctionServlet(callback cb);

    int32_t handle(hyn::http::HttpRequest::ptr request, hyn::http::HttpResponse::ptr response,
                   hyn::HttpSession::ptr session) override;

private:
    callback m_cb;
};

/**
 *@brief 创建 Servlet 的接口
 *@note 用户可以通过实现该接口来自定义 Servlet 的创建过程。
 */
class IServletCreator {
public:
    using ptr = std::shared_ptr<IServletCreator>;

    virtual ~IServletCreator() = default;

    [[nodiscard]] virtual Servlet::ptr get() const = 0;

    [[nodiscard]] virtual std::string getName() const = 0;
};

/**
 *@brief servlet包装器
 *@note 将一个指向 Servlet 对象的智能指针包装成一个 IServletCreator 对象，从而可以将其注册到 HttpServer 中，使 HttpServer
 *在接收到客户端请求时能够动态创建该 Servlet 对象。
 */
class HoldServletCreator : public IServletCreator {
public:
    using ptr = std::shared_ptr<HoldServletCreator>;

    explicit HoldServletCreator(Servlet::ptr servlet) : m_servlet(std::move(servlet)) {}

    [[nodiscard]] Servlet::ptr get() const override {
        return m_servlet;
    }

    [[nodiscard]] std::string getName() const override {
        return m_servlet->getName();
    }

private:
    Servlet::ptr m_servlet;
};


template<class T>
class ServletCreator : public IServletCreator {
public:
    using ptr = std::shared_ptr<ServletCreator>;

    [[nodiscard]] Servlet::ptr get() const override {
        return Servlet::ptr(new T);
    }

    [[nodiscard]] std::string getName() const override {
        return util::TypeToName<T>();
    }
};

/**
 *@brief NotFoundServlet
 *@note 处理HTTP请求中未找到对应的资源时的情况
 */
class NotFoundServlet : public Servlet {
public:
    using ptr = std::shared_ptr<NotFoundServlet>;

    explicit NotFoundServlet(const std::string &name);

    int32_t handle(hyn::http::HttpRequest::ptr request, hyn::http::HttpResponse::ptr response,
                   hyn::HttpSession::ptr session) override;

private:
    ///默认返回的 HTTP 响应内容
    std::string m_content;

    std::string m_name;
};

/**
 *@brief servlet分发器
 *@note 将所有的 Servlet 创建器或实例按照 URI 存储到映射表中，然后根据请求的 URI 获取对应的 Servlet 实例进行处理。
 *      这种实现方式比较灵活，可以根据需要动态添加或删除 Servlet，实现 Servlet 的热插拔功能。
 */
class ServletDispatch : public Servlet {
public:
    using ptr = std::shared_ptr<ServletDispatch>;
    using RWMutexType = mutex::RWMutex;

    ServletDispatch() : Servlet("ServletDispatch") {
        m_default.reset(new NotFoundServlet("hyn/1.0"));
    }

    int32_t handle(hyn::http::HttpRequest::ptr request, hyn::http::HttpResponse::ptr response,
                   hyn::HttpSession::ptr session) override;

    /**
     *@brief 添加servlet（使用uri和servlet）
     */
    void addServlet(const std::string &uri, const Servlet::ptr &servlet);

    /**
     *@brief 添加servlet（使用uri和回调函数）
     */
    void addServlet(const std::string &uri, const FunctionServlet::callback &cb);

    /**
     *@brief 添加全局servlet（uri servlet）
     *@note (支持通配符*)
     */
    void addGlobServlet(const std::string &uri, const Servlet::ptr &servlet);

    /**
    *@brief 添加全局servlet（使用uri和回调函数）
    *@note (支持通配符*)
    */
    void addGlobServlet(const std::string &uri, const FunctionServlet::callback &cb);

    /**
     *@brief 添加Servlet对象创建器
     */
    void addServletCreator(const std::string &uri, IServletCreator::ptr creator);

    template<typename T>
    void addServletCreator(const std::string &uri) {
        addServletCreator(uri, std::make_shared<ServletCreator<T>>());
    }

    /**
     *@brief 添加全局Servlet对象创建器
     *@note (支持通配符*)
     */
    void addGlobServletCreator(const std::string &uri, const IServletCreator::ptr &creator);

    template<typename T>
    void addGlobServletCreator(const std::string &uri) {
        addGlobServletCreator(uri, std::make_shared<ServletCreator<T>>());
    }

    /**
     * @brief 删除servlet
     * @param[in] uri uri
     */
    void delServlet(const std::string &uri);

    /**
     * @brief 删除模糊匹配servlet
     * @param[in] uri uri
     */
    void delGlobServlet(const std::string &uri);

    /**
     * @brief 返回默认servlet
     */
    Servlet::ptr getDefault() const { return m_default; }

    /**
     * @brief 设置默认servlet
     * @param[in] v servlet
     */
    void setDefault(Servlet::ptr v) { m_default = std::move(v); }

    /**
     * @brief 通过uri获取servlet
     * @param[in] uri uri
     * @return 返回对应的servlet
     */
    Servlet::ptr getServlet(const std::string &uri);

    /**
     * @brief 通过uri获取模糊匹配servlet
     * @param[in] uri uri
     * @return 返回对应的servlet
     */
    Servlet::ptr getGlobServlet(const std::string &uri);

    /**
     * @brief 通过uri获取servlet
     * @param[in] uri uri
     * @return 优先精准匹配,其次模糊匹配,最后返回默认
     */
    Servlet::ptr getMatchedServlet(const std::string &uri);

    /**
     *@brief 将所有URI与对应的ServletCreator信息输出（m_datas）
     *@param [out] infos
     */
    void listAllServletCreator(std::map<std::string, IServletCreator::ptr> &infos);

    /**
     *@brief 将所有全局的URI与对应的ServletCreator信息输出（m_globs）
     *@param [out] infos
     */
    void listAllGlobServletCreator(std::map<std::string, IServletCreator::ptr> &infos);

private:
    RWMutexType m_mutex;
    ///存储 URL 路径与 IServletCreator 对象之间的映射关系
    std::unordered_map<std::string, IServletCreator::ptr> m_datas;
    ///存储全局的 URL 路径与 IServletCreator 对象之间的映射关系(支持通配符*)
    std::vector<std::pair<std::string, IServletCreator::ptr>> m_globs;
    ///默认servlet
    Servlet::ptr m_default;
};


} // hyn


