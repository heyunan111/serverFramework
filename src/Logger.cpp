//
// Created by hyn on 2023/3/21.
//

#include "Logger.h"
#include <ctime>
#include <cstring>
#include <iostream>
#include <cstdarg>

const char *hyn::logger::Logger::s_level[LEVEL_COUNT] = {
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL"
};

void hyn::logger::Logger::open(const std::string &filename) {
    m_file_name = filename;
    m_fout.open(filename, std::ios::app);
    THROW_RUNTIME_ERROR_IF(m_fout.fail(), "open file failed" + filename);
    m_fout.seekp(0, std::ios::end);
    m_len = m_fout.tellp();
}

void hyn::logger::Logger::close() {
    m_fout.close();
}


/*
 * snprintf 格式化
 * va_list C语言中解决变参问题的一组宏
 * va_start/va_end  C 库宏 void va_start(va_list ap, last_arg) 初始化 ap 变量，它与 va_arg 和 va_end 宏是一起使用的。
 * last_arg 是最后一个传递给函数的已知的固定参数，即省略号之前的参数。这个宏必须在使用 va_arg 和 va_end 之前被调用。
 * vsnprintf 用于向一个字符串缓冲区打印格式化字符串，且可以限定打印的格式化字符串的最大长度。 c99/c++11
 */

void
hyn::logger::Logger::log(hyn::logger::Logger::Level level, const char *file, int line, const char *format, ...) {
    if (m_level > level) {
        return;
    }
    THROW_RUNTIME_ERROR_IF(m_fout.fail(), "open file failed" + m_file_name);
    time_t ticks = time(nullptr);
    struct tm *ptm = localtime(&ticks);
    char timestamp[32];
    memset(timestamp, 0, sizeof(timestamp));
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", ptm);
    //std::cout << timestamp << std::endl;
    int len;
    const char *fmt = "%s %s %s:%d ";
    len = snprintf(nullptr, 0, fmt, timestamp, s_level[level], file, line);
    if (len > 0) {
        char *buffer = new char[len + 1];
        snprintf(buffer, len + 1, fmt, timestamp, s_level[level], file, line);
        buffer[len] = 0;
        m_fout << buffer;
        delete[] buffer;
        m_len += len;
    }

    va_list arg_ptr;
    va_start(arg_ptr, format);
    len = vsnprintf(nullptr, 0, format, arg_ptr);
    va_end(arg_ptr);
    if (len > 0) {
        char *content = new char[len + 1];
        va_start(arg_ptr, format);
        vsnprintf(content, len + 1, format, arg_ptr);
        va_end(arg_ptr);
        content[len] = 0;
        m_fout << content;
        delete[] content;
        m_len += len;
    }

    m_fout << "\n";
    m_fout.flush();

    if (m_max > 0 && m_len >= m_max) {
        rotate();
    }
}

void hyn::logger::Logger::max(int bytes) {
    m_max = bytes;
}

void hyn::logger::Logger::level(hyn::logger::Logger::Level level) {
    m_level = level;
}

void hyn::logger::Logger::rotate() {
    close();
    time_t ticks = time(nullptr);
    struct tm *ptm = localtime(&ticks);
    char timestamp[32];
    memset(timestamp, 0, sizeof(timestamp));
    strftime(timestamp, sizeof(timestamp), ".%Y-%m-%d_%H-%M-%S", ptm);
    std::string filename = m_file_name + timestamp;
    if (rename(m_file_name.c_str(), filename.c_str()) != 0) {
        throw std::logic_error("rename log file failed: " + std::string(strerror(errno)));
    }
    open(m_file_name);
}
