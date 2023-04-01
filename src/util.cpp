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

#include "../include/util.h"
#include "../include/Logger.h"


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

