/**
  ******************************************************************************
  * @file           : config.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/3/31.
  ******************************************************************************
  */
#pragma once

#include <memory>
#include <sstream>
#include <utility>
#include <map>
#include <boost/lexical_cast.hpp>
#include "Logger.h"

namespace hyn::config {

    //配置变量的基类
    class ConfigVarBase {
    public:
        typedef std::shared_ptr<ConfigVarBase> ptr;

        explicit ConfigVarBase(std::string name, std::string description = "") : m_name(std::move(name)),
                                                                                 m_description(std::move(description)) {
        }

        virtual ~ConfigVarBase() = default;;
    protected:
        std::string m_name;                     // 配置项的名称
        std::string m_description;              // 配置项的备注
    public:
        //[[nodiscard]]非弃值，c++17，如果一个被nodiscard标记了的表达式，如果我们在使用时弃值了，而且没有使用static_cast<void>将其
        // 转化为void时，编译器会抛出warning来提醒用户获取返回值
        [[nodiscard]] const std::string &getMName() const {
            return m_name;
        }

        [[nodiscard]] const std::string &getMDescription() const {
            return m_description;
        }

        virtual std::string to_string() = 0;

        virtual bool from_string(const std::string &val) = 0;
    };


    template<typename T>
    class ConfigVar : public ConfigVarBase {
    public:
        typedef std::shared_ptr<ConfigVarBase> ptr;

        ConfigVar(std::string name, const T &default_value, std::string description = "") :
                ConfigVarBase(name, description), m_val(default_value) {
        }

        std::string to_string() override {
            try {
                boost::lexical_cast<std::string>(m_val);
            } catch (...) {
                error("ConfigVar::to_string exception");
            }
            return "";
        };

        bool from_string(const std::string &val) override {
            try {
                m_val = boost::lexical_cast<T>(val);
            } catch (std::exception &e) {
                error("ConfigVar::from_string exception");
            }
            return false;
        }

    private:
        T m_val;
    };


    class Config {
    public:
        typedef std::map<std::string, ConfigVarBase::ptr> config_val_map;

        template<typename T>
        static typename ConfigVar<T>::ptr Look_up(const std::string &name,
                                                  const T &default_value,
                                                  const std::string &description = "") {
            auto temp = look_up<T>(name);
            if (temp) {
                info("look up name == nullptr");
            }
            if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos) {
                error("lookup name invalid");
                throw std::invalid_argument(name);
            }
            typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
            s_data[name] = v;
        }

        template<typename T>
        static typename ConfigVar<T>::ptr look_up(const std::string &name) {
            auto it = s_data.find(name);
            if (it == s_data.end())
                return nullptr;
            return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
        }

    private:
        static config_val_map s_data;
    };

}