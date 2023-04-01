/**
  ******************************************************************************
  * @file           : Logger.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/3/21
  ******************************************************************************
  */
#pragma once

#include <string>
#include <fstream>
#include "singleton.h"
#include "exceptdef.h"


namespace hyn::logger {

#define debug(format, ...) \
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->log(hyn::logger::Logger::Level::DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define info(format, ...) \
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->log(hyn::logger::Logger::Level::INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define warn(format, ...) \
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->log(hyn::logger::Logger::Level::WARN, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define error(format, ...) \
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->log(hyn::logger::Logger::Level::ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define fatal(format, ...) \
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->log(hyn::logger::Logger::Level::FATAL, __FILE__, __LINE__, format, ##__VA_ARGS__)

    class Logger {
        friend singleton::Singleton<hyn::logger::Logger>;
    public:
        Logger(const Logger &logger1) = delete;

        Logger &operator=(const Logger &logger1) = delete;

    public:

        enum Level {
            DEBUG = 0,
            INFO,
            WARN,
            ERROR,
            FATAL,
            LEVEL_COUNT
        };

        void log(Level level, const char *file, int line, const char *format, ...);

        void open(const std::string &filename);

        void close();

        void max(int bytes);

        void level(Level level);

    private:
        Logger() = default;

        ~Logger() = default;

        void rotate();

        int m_max = 1048576;//默认10MB
        int m_len = 0;
        Level m_level = Level::DEBUG;
        std::string m_file_name;
        std::ofstream m_fout;
        static const char *s_level[LEVEL_COUNT];
    };


} // logger


