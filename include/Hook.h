/**
  ******************************************************************************
  * @file           : Hook.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/10
  ******************************************************************************
  */
#pragma once

#include <unistd.h>

namespace hyn {

bool is_hook_enable();

void set_hook_enable(bool f);

} // hyn

extern "C" {

//sleep

typedef unsigned int (*sleep_fun)(unsigned int seconds);
extern sleep_fun sleep_f;

typedef int (*usleep_fun)(useconds_t usec);
extern usleep_fun usleep_f;

typedef int (*nanosleep_fun)(const struct timespec *req, struct timespec *rem);
extern nanosleep_fun nanosleep_f;
}//extern "C"


