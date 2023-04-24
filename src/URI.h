/**
  ******************************************************************************
  * @file           : uri.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/24
  ******************************************************************************
  */
#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include "Address.h"

/*
     foo://user@example.com:8042/over/there?name=ferret#nose
       \_/   \______________/\_________/ \_________/ \__/
        |           |            |            |        |
     scheme     authority       path        query   fragment

     authority   = [ userinfo "@" ] host [ ":" port ]
*/

namespace hyn {

class URI {
public:
    using ptr = std::shared_ptr<URI>;

    URI();

    /**
    * @brief 创建Uri对象
    * @param uri uri字符串
    * @return 解析成功返回Uri对象否则返回nullptr
    */
    static URI::ptr Create(const std::string &uri);

    /**
    * @brief 序列化到输出流
    * @param os 输出流
    * @return 输出流
    */
    std::ostream &dump(std::ostream &os) const;

    /**
     * @brief 转成字符串
     */
    std::string toString() const;

    /**
     * @brief 获取Address
     */
    Address::ptr createAddress() const;

    /**
     * @brief 是否默认端口
     */
    bool isDefaultPort() const;
    /************* getter and setter *************/
public:
    [[nodiscard]] const std::string &getScheme() const {
        return m_scheme;
    }

    void setScheme(const std::string &mScheme) {
        m_scheme = mScheme;
    }

    [[nodiscard]] const std::string &getUserinfo() const {
        return m_userinfo;
    }

    void setUserinfo(const std::string &mUserinfo) {
        m_userinfo = mUserinfo;
    }

    [[nodiscard]] const std::string &getHost() const {
        return m_host;
    }

    void setHost(const std::string &mHost) {
        m_host = mHost;
    }

    [[nodiscard]]int32_t getPort() const {
        return m_port;
    }

    void setPort(int32_t mPort) {
        m_port = mPort;
    }

    [[nodiscard]]const std::string &getPath() const {
        return m_path;
    }

    void setPath(const std::string &mPath) {
        m_path = mPath;
    }

    [[nodiscard]]const std::string &getQuery() const {
        return m_query;
    }

    void setQuery(const std::string &mQuery) {
        m_query = mQuery;
    }

    [[nodiscard]]const std::string &getFragment() const {
        return m_fragment;
    }

    void setFragment(const std::string &mFragment) {
        m_fragment = mFragment;
    }

private:
    std::string m_scheme;
    std::string m_userinfo;
    std::string m_host;
    int32_t m_port;
    std::string m_path;
    std::string m_query;
    std::string m_fragment;
};

}
