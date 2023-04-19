/**
  ******************************************************************************
  * @file           : Http.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/17
  ******************************************************************************
  */
#pragma once

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <boost/lexical_cast.hpp>

namespace hyn::http {

/* Request Methods */
#define HTTP_METHOD_MAP(XX)         \
  XX(0,  DELETE,      DELETE)       \
  XX(1,  GET,         GET)          \
  XX(2,  HEAD,        HEAD)         \
  XX(3,  POST,        POST)         \
  XX(4,  PUT,         PUT)          \
  /* pathological */                \
  XX(5,  CONNECT,     CONNECT)      \
  XX(6,  OPTIONS,     OPTIONS)      \
  XX(7,  TRACE,       TRACE)        \
  /* WebDAV */                      \
  XX(8,  COPY,        COPY)         \
  XX(9,  LOCK,        LOCK)         \
  XX(10, MKCOL,       MKCOL)        \
  XX(11, MOVE,        MOVE)         \
  XX(12, PROPFIND,    PROPFIND)     \
  XX(13, PROPPATCH,   PROPPATCH)    \
  XX(14, SEARCH,      SEARCH)       \
  XX(15, UNLOCK,      UNLOCK)       \
  XX(16, BIND,        BIND)         \
  XX(17, REBIND,      REBIND)       \
  XX(18, UNBIND,      UNBIND)       \
  XX(19, ACL,         ACL)          \
  /* subversion */                  \
  XX(20, REPORT,      REPORT)       \
  XX(21, MKACTIVITY,  MKACTIVITY)   \
  XX(22, CHECKOUT,    CHECKOUT)     \
  XX(23, MERGE,       MERGE)        \
  /* upnp */                        \
  XX(24, MSEARCH,     M-SEARCH)     \
  XX(25, NOTIFY,      NOTIFY)       \
  XX(26, SUBSCRIBE,   SUBSCRIBE)    \
  XX(27, UNSUBSCRIBE, UNSUBSCRIBE)  \
  /* RFC-5789 */                    \
  XX(28, PATCH,       PATCH)        \
  XX(29, PURGE,       PURGE)        \
  /* CalDAV */                      \
  XX(30, MKCALENDAR,  MKCALENDAR)   \
  /* RFC-2068, section 19.6.1.2 */  \
  XX(31, LINK,        LINK)         \
  XX(32, UNLINK,      UNLINK)       \
  /* icecast */                     \
  XX(33, SOURCE,      SOURCE)       \

/* Status Codes */
#define HTTP_STATUS_MAP(XX)                                                 \
  XX(100, CONTINUE,                        Continue)                        \
  XX(101, SWITCHING_PROTOCOLS,             Switching Protocols)             \
  XX(102, PROCESSING,                      Processing)                      \
  XX(200, OK,                              OK)                              \
  XX(201, CREATED,                         Created)                         \
  XX(202, ACCEPTED,                        Accepted)                        \
  XX(203, NON_AUTHORITATIVE_INFORMATION,   Non-Authoritative Information)   \
  XX(204, NO_CONTENT,                      No Content)                      \
  XX(205, RESET_CONTENT,                   Reset Content)                   \
  XX(206, PARTIAL_CONTENT,                 Partial Content)                 \
  XX(207, MULTI_STATUS,                    Multi-Status)                    \
  XX(208, ALREADY_REPORTED,                Already Reported)                \
  XX(226, IM_USED,                         IM Used)                         \
  XX(300, MULTIPLE_CHOICES,                Multiple Choices)                \
  XX(301, MOVED_PERMANENTLY,               Moved Permanently)               \
  XX(302, FOUND,                           Found)                           \
  XX(303, SEE_OTHER,                       See Other)                       \
  XX(304, NOT_MODIFIED,                    Not Modified)                    \
  XX(305, USE_PROXY,                       Use Proxy)                       \
  XX(307, TEMPORARY_REDIRECT,              Temporary Redirect)              \
  XX(308, PERMANENT_REDIRECT,              Permanent Redirect)              \
  XX(400, BAD_REQUEST,                     Bad Request)                     \
  XX(401, UNAUTHORIZED,                    Unauthorized)                    \
  XX(402, PAYMENT_REQUIRED,                Payment Required)                \
  XX(403, FORBIDDEN,                       Forbidden)                       \
  XX(404, NOT_FOUND,                       Not Found)                       \
  XX(405, METHOD_NOT_ALLOWED,              Method Not Allowed)              \
  XX(406, NOT_ACCEPTABLE,                  Not Acceptable)                  \
  XX(407, PROXY_AUTHENTICATION_REQUIRED,   Proxy Authentication Required)   \
  XX(408, REQUEST_TIMEOUT,                 Request Timeout)                 \
  XX(409, CONFLICT,                        Conflict)                        \
  XX(410, GONE,                            Gone)                            \
  XX(411, LENGTH_REQUIRED,                 Length Required)                 \
  XX(412, PRECONDITION_FAILED,             Precondition Failed)             \
  XX(413, PAYLOAD_TOO_LARGE,               Payload Too Large)               \
  XX(414, URI_TOO_LONG,                    URI Too Long)                    \
  XX(415, UNSUPPORTED_MEDIA_TYPE,          Unsupported Media Type)          \
  XX(416, RANGE_NOT_SATISFIABLE,           Range Not Satisfiable)           \
  XX(417, EXPECTATION_FAILED,              Expectation Failed)              \
  XX(421, MISDIRECTED_REQUEST,             Misdirected Request)             \
  XX(422, UNPROCESSABLE_ENTITY,            Unprocessable Entity)            \
  XX(423, LOCKED,                          Locked)                          \
  XX(424, FAILED_DEPENDENCY,               Failed Dependency)               \
  XX(426, UPGRADE_REQUIRED,                Upgrade Required)                \
  XX(428, PRECONDITION_REQUIRED,           Precondition Required)           \
  XX(429, TOO_MANY_REQUESTS,               Too Many Requests)               \
  XX(431, REQUEST_HEADER_FIELDS_TOO_LARGE, Request Header Fields Too Large) \
  XX(451, UNAVAILABLE_FOR_LEGAL_REASONS,   Unavailable For Legal Reasons)   \
  XX(500, INTERNAL_SERVER_ERROR,           Internal Server Error)           \
  XX(501, NOT_IMPLEMENTED,                 Not Implemented)                 \
  XX(502, BAD_GATEWAY,                     Bad Gateway)                     \
  XX(503, SERVICE_UNAVAILABLE,             Service Unavailable)             \
  XX(504, GATEWAY_TIMEOUT,                 Gateway Timeout)                 \
  XX(505, HTTP_VERSION_NOT_SUPPORTED,      HTTP Version Not Supported)      \
  XX(506, VARIANT_ALSO_NEGOTIATES,         Variant Also Negotiates)         \
  XX(507, INSUFFICIENT_STORAGE,            Insufficient Storage)            \
  XX(508, LOOP_DETECTED,                   Loop Detected)                   \
  XX(510, NOT_EXTENDED,                    Not Extended)                    \
  XX(511, NETWORK_AUTHENTICATION_REQUIRED, Network Authentication Required) \


/**
 * @brief HTTP方法枚举
 */
enum class HttpMethod {
#define XX(num, name, string) name = num,
    HTTP_METHOD_MAP(XX)
#undef XX
    INVALID_METHOD
};

/**
 * @brief HTTP状态枚举
 */
enum class HttpStatus {
#define XX(code, name, desc) name = code,
    HTTP_STATUS_MAP(XX)
#undef XX
};

/**
 *@brief HTTP方法名数组
 */
static const char *s_method_string[] = {
#define XX(num, name, string) #string,
        HTTP_METHOD_MAP(XX)
#undef XX
};

/**
 * @brief 将字符串方法名转成HTTP方法枚举
 * @param[in] m HTTP方法
 * @return HTTP方法枚举
 */
HttpMethod StringToHttpMethod(const std::string &str);

/**
 * @brief 将字符串指针转换成HTTP方法枚举
 * @param[in] m 字符串方法枚举
 * @return HTTP方法枚举
 */
HttpMethod CharToHttpMethod(const char *str);

/**
 * @brief 将HTTP方法枚举转换成字符串
 * @param[in] m HTTP方法枚举
 * @return 字符串
 */
const char *HttpMethodToString(const HttpMethod &httpMethod);

/**
 * @brief 将HTTP状态枚举转换成字符串
 * @param[in] m HTTP状态枚举
 * @return 字符串
 */
const char *HttpStatusToString(const HttpStatus &httpStatus);

/**
 *@brief 忽略大小写比较字符串的仿函数
 */
struct CaseInsensitiveLess {
    bool operator()(const std::string &lhs, const std::string &rhs) const;
};

/**
 * @brief 获取Map中的key值,并转成对应类型
 * @param[in] m Map数据结构
 * @param[in] key 关键字
 * @param[in] def 默认值
 * @return 如果存在且转换成功返回对应的值,否则返回默认值
 */
template<typename T, typename MapType>
bool getAs(const MapType &m, const std::string &key, const T &def = T()) {
    auto it = m.find(key);
    if (it == m.end())
        return def;
    try {
        return boost::lexical_cast<T>(it->second());
    } catch (...) {
    }
    return def;
}

/**
 * @brief 获取Map中的key值,并转成对应类型,返回是否成功
 * @param[in] m Map数据结构
 * @param[in] key 关键字
 * @param[out] value 保存转换后的值
 * @param[in] def 默认值
 * @return
 *      @retval true 转换成功, val 为对应的值
 *      @retval false 不存在或者转换失败 val = def
 */
template<typename T, typename MapType>
bool checkGetAs(const MapType &m, const std::string &key, T &value, const T &def = T()) {
    auto it = m.find(key);
    if (it == m.end()) {
        value = def;
        return false;
    }
    try {
        value = boost::lexical_cast<T>(it->second());
        return true;
    } catch (...) {
        value = def;
    }
    return false;
}

class HttpRequest {
public:
    using ptr = std::shared_ptr<HttpRequest>;
    using MapType = std::map<std::string, std::string, CaseInsensitiveLess>;
public:
    /**
     *@brief 构造函数
     *@param version 版本，默认：1.1
     *@param close 默认：true，若为false：keepalive
     */
    explicit HttpRequest(uint8_t version = 0x11, bool close = true);

    /**
     *@brief 创建一个与该请求相关联的响应对象
     */
    std::shared_ptr<HttpRequest> creatResponse();

    /**
    * @brief 获取HTTP请求的头部参数
    * @param[in] key 关键字
    * @param[in] def 默认值
    * @return 如果存在则返回对应值,否则返回默认值
    */
    [[nodiscard]] std::string getHeader(const std::string &key, const std::string &def = "") const;

    /**
    * @brief 获取HTTP请求的参数
    * @param[in] key 关键字
    * @param[in] def 默认值
    * @return 如果存在则返回对应值,否则返回默认值
    */
    std::string getParam(const std::string &key, const std::string &def = "");

    /**
    * @brief 获取HTTP请求的Cookie参数
    * @param[in] key 关键字
    * @param[in] def 默认值
    * @return 如果存在则返回对应值,否则返回默认值
    */
    std::string getCookie(const std::string &key, const std::string &def = "");


    /**
     * @brief 设置HTTP请求的头部参数
     * @param[in] key 关键字
     * @param[in] val 值
     */
    void setHeader(const std::string &key, const std::string &value);


    /**
     * @brief 设置HTTP请求的参数
     * @param[in] key 关键字
     * @param[in] val 值
     */
    void setParam(const std::string &key, const std::string &value);

    /**
     * @brief 设置HTTP请求的Cookie参数
     * @param[in] key 关键字
     * @param[in] val 值
     */
    void setCookie(const std::string &key, const std::string &value);

    /**
    * @brief 删除HTTP请求的头部参数
    * @param[in] key 关键字
    */
    void delHeader(const std::string &key);

    /**
    * @brief 删除HTTP请求的参数
    * @param[in] key 关键字
    */
    void delParam(const std::string &key);

    /**
    * @brief 删除HTTP请求的Cookie参数
    * @param[in] key 关键字
    */
    void delCookie(const std::string &key);

    /**
     * @brief 判断HTTP请求的头部参数是否存在
     * @param[in] key 关键字
     * @param[out] val 如果存在,val非空则赋值
     * @return 是否存在
     */
    bool hasHeader(const std::string &key, std::string *value = nullptr);

    /**
     * @brief 判断HTTP请求的参数是否存在
     * @param[in] key 关键字
     * @param[out] val 如果存在,val非空则赋值
     * @return 是否存在
     */
    bool hasParam(const std::string &key, std::string *value = nullptr);

    /**
     * @brief 判断HTTP请求的Cookie参数是否存在
     * @param[in] key 关键字
     * @param[out] val 如果存在,val非空则赋值
     * @return 是否存在
     */
    bool hasCookie(const std::string &key, std::string *value = nullptr);

    /**
     *@brief 检查并获取HTTP请求的头部参数
     *@param key 关键字
     *@param [out]value 返回值
     *@param def 默认值
     *@return 如果存在且转换成功返回true，否则false
     */
    template<typename T>
    bool checkGetHeaderAs(const std::string &key, T &value, const T &def = T()) {
        return checkGetAs(m_headers, key, value, def);
    }

    /**
     *@brief 序列化输出流
     */
    std::ostream &dump(std::ostream &os) const;

    /**
     *@brief 转成字符串
     */
    [[nodiscard]] std::string toString() const;

private:
    /**
     *@brief 初始化是否keep-live
     */
    void initClose();

    void initPram();

    void initQueryParam();

    void initBodyParam();

    void initCookies();

public:
    /****************       Getter and Setter       ****************/

    [[nodiscard]] bool isAutoClose() const {
        return m_autoClose;
    }

    void setAutoClose(bool mAutoClose) {
        m_autoClose = mAutoClose;
    }

    [[nodiscard]] bool isIsWebSocket() const {
        return m_isWebSocket;
    }

    void setIsWebSocket(bool mIsWebSocket) {
        m_isWebSocket = mIsWebSocket;
    }

    [[nodiscard]] uint8_t getParserParamFlag() const {
        return m_parserParamFlag;
    }

    void setParserParamFlag(uint8_t mParserParamFlag) {
        m_parserParamFlag = mParserParamFlag;
    }

    [[nodiscard]] HttpMethod getMethod() const {
        return m_method;
    }

    void setMethod(HttpMethod mMethod) {
        m_method = mMethod;
    }

    [[nodiscard]] uint8_t getVersion() const {
        return m_version;
    }

    void setVersion(uint8_t mVersion) {
        m_version = mVersion;
    }

    [[nodiscard]] const std::string &getPath() const {
        return m_path;
    }

    void setPath(const std::string &mPath) {
        m_path = mPath;
    }

    [[nodiscard]] const std::string &getQuery() const {
        return m_query;
    }

    void setQuery(const std::string &mQuery) {
        m_query = mQuery;
    }

    [[nodiscard]] const std::string &getFragment() const {
        return m_fragment;
    }

    void setFragment(const std::string &mFragment) {
        m_fragment = mFragment;
    }

    [[nodiscard]] const std::string &getBody() const {
        return m_body;
    }

    void setBody(const std::string &mBody) {
        m_body = mBody;
    }

    [[nodiscard]] const MapType &getHeaders() const {
        return m_headers;
    }

    void setHeaders(const MapType &mHeaders) {
        m_headers = mHeaders;
    }

    [[nodiscard]] const MapType &getParams() const {
        return m_params;
    }

    void setParams(const MapType &mParams) {
        m_params = mParams;
    }

    [[nodiscard]] const MapType &getCookies() const {
        return m_cookies;
    }

    void setCookies(const MapType &mCookies) {
        m_cookies = mCookies;
    }

private:
    //https://www.example.com:8080/index.html?search=keyword#section1
    //协议为HTTPS，主机名为www.example.com，端口号为8080，路径为/index.html，查询参数为search=keyword，片段标识符为section1。

    ///谁否自动关闭(长连接)
    bool m_autoClose;
    ///是否为websocket
    bool m_isWebSocket;
    ///请求解析时是否已经解析过参数
    uint8_t m_parserParamFlag;
    ///HTTP方法
    HttpMethod m_method;
    ///HTTP版本：1.0:0x10 1.1:0x11 ......
    uint8_t m_version;
    ///请求路径
    std::string m_path;
    ///请求参数
    std::string m_query{};
    ///片段标识符（请求fragment）
    std::string m_fragment{};
    ///请求消息体
    std::string m_body{};
    ///请求头部 MAP
    MapType m_headers;
    ///请求参数 MAP
    MapType m_params;
    ///请求Cookie MAP
    MapType m_cookies;
};


class HttpResponse {
public:
    /// HTTP响应结构智能指针
    typedef std::shared_ptr<HttpResponse> ptr;
    /// MapType
    typedef std::map<std::string, std::string, CaseInsensitiveLess> MapType;

    explicit HttpResponse(uint8_t version = 0x11, bool close = true);

    /**
     * @brief 获取HTTP请求的头部参数
     * @param[in] key 关键字
     * @param[in] def 默认值
     * @return 如果存在则返回对应值,否则返回默认值
     */
    [[nodiscard]] std::string getHeader(const std::string &key, const std::string &def = "") const;

    /**
     * @brief 设置HTTP请求的头部参数
     * @param[in] key 关键字
     * @param[in] val 值
     */
    void setHeader(const std::string &key, const std::string &value);

    /**
    * @brief 删除HTTP请求的头部参数
    * @param[in] key 关键字
    */
    void delHeader(const std::string &key);

    /**
     *@brief 序列化输出流
     */
    std::ostream &dump(std::ostream &os) const;

    /**
     *@brief 转成字符串
     */
    [[nodiscard]] std::string toString() const;

    /**
   * @brief 检查并获取响应头部参数
   * @tparam T 值类型
   * @param[in] key 关键字
   * @param[out] val 值
   * @param[in] def 默认值
   * @return 如果存在且转换成功返回true,否则失败val=def
   */
    template<class T>
    bool checkGetHeaderAs(const std::string &key, T &val, const T &def = T()) {
        return checkGetAs(m_headers, key, val, def);
    }

    /**
     * @brief 获取响应的头部参数
     * @tparam T 转换类型
     * @param[in] key 关键字
     * @param[in] def 默认值
     * @return 如果存在且转换成功返回对应的值,否则返回def
     */
    template<class T>
    T getHeaderAs(const std::string &key, const T &def = T()) {
        return getAs(m_headers, key, def);
    }
    /************* getter and setter *************/
public:
    [[nodiscard]] HttpStatus getStatus() const {
        return m_status;
    }

    void setStatus(HttpStatus mStatus) {
        m_status = mStatus;
    }

    [[nodiscard]] uint8_t getVersion() const {
        return m_version;
    }

    void setVersion(uint8_t mVersion) {
        m_version = mVersion;
    }

    [[nodiscard]] bool isAutoClose() const {
        return m_autoClose;
    }

    void setAutoClose(bool mAutoClose) {
        m_autoClose = mAutoClose;
    }

    [[nodiscard]] bool isWebsocket() const {
        return m_websocket;
    }

    void setWebsocket(bool mWebsocket) {
        m_websocket = mWebsocket;
    }

    [[nodiscard]] const std::string &getBody() const {
        return m_body;
    }

    void setBody(const std::string &mBody) {
        m_body = mBody;
    }

    [[nodiscard]] const std::string &getReason() const {
        return m_reason;
    }

    void setReason(const std::string &mReason) {
        m_reason = mReason;
    }

    [[nodiscard]] const MapType &getHeaders() const {
        return m_headers;
    }

    void setHeaders(const MapType &mHeaders) {
        m_headers = mHeaders;
    }

private:
    //HTTP/1.1 200 OK\r\n
    //Content-Type: text/html\r\n
    //Content-Length: 1024\r\n
    //Server: Apache/2.2.14 (Win32)\r\n
    //...
    //\r\n

    ///响应状态
    HttpStatus m_status;
    ///版本
    uint8_t m_version;
    ///是否自动关闭
    bool m_autoClose;
    ///是否websocket
    bool m_websocket{false};
    ///消息体
    std::string m_body{};
    ///原因
    std::string m_reason{};
    ///头部MAP
    MapType m_headers;
    ///Cookies
    std::vector<std::string> m_cookies;
};

} // hyn::http

