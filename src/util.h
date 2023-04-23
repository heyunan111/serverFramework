/**
  ******************************************************************************
  * @file           : util.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/3/31
  ******************************************************************************
  */
#pragma once


#include <vector>
#include <string>
#include <sys/time.h>
#include <cxxabi.h>

namespace hyn::util {
pid_t GetThreadId();

uint64_t get_fiber_id();

void backtrace(std::vector<std::string> &bt, int size, int skip = 1);

std::string backtrace_to_string(int size = 64, int skip = 2, const std::string &prefix = "");

///获取当前时间
uint64_t GetCurrentMS();

uint64_t GetCurrentUS();

///返回类型名称
template<class T>
const char *TypeToName() {
    static const char *s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
    return s_name;
}
}