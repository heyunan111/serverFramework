/**
  ******************************************************************************
  * @file           : util.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/3/31
  ******************************************************************************
  */
#include <execinfo.h>
#include <sstream>
#include <unistd.h>
#include <sys/syscall.h>

#include "util.h"
#include "Logger.h"
#include "fiber.h"

pid_t hyn::util::GetThreadId() {
    return syscall(SYS_gettid);
}

void hyn::util::backtrace(std::vector<std::string> &bt, int size, int skip) {
    void **array = (void **) malloc((sizeof(void *) * size));
    size_t s = ::backtrace(array, size);
    char **strings = backtrace_symbols(array, s);
    if (strings == nullptr) {
        error("backtrace_symbols error");
        return;
    }
    for (size_t i = skip; i < s; ++i) {
        bt.emplace_back(strings[i]);
    }
    free(strings);
    free(array);
}

std::string hyn::util::backtrace_to_string(int size, int skip, const std::string &prefix) {
    std::vector<std::string> bt;
    backtrace(bt, size, skip);
    std::stringstream ss;
    for (const auto &i: bt) {
        ss << prefix << i << '\n';
    }
    return ss.str();
}

uint64_t hyn::util::get_fiber_id() {
    return hyn::fiber::Fiber::GetFiberId();
}

uint64_t hyn::util::GetCurrentMS() {
    timeval tv{};
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000ul + tv.tv_usec / 1000;
}

uint64_t hyn::util::GetCurrentUS() {
    timeval tv{};
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 * 1000ul + tv.tv_usec;
}



