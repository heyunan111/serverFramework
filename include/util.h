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

namespace hyn::util {
    pid_t GetThreadId();

    uint64_t get_fiber_id();

    void backtrace(std::vector<std::string> &bt, int size, int skip = 1);

    std::string backtrace_to_string(int size, int skip = 2, const std::string &prefix = "");
}