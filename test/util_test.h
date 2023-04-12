/**
  ******************************************************************************
  * @file           : util_test.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/3/31
  ******************************************************************************
  */



#ifndef SERVERFRAMEWORK_UTIL_TEST_H
#define SERVERFRAMEWORK_UTIL_TEST_H

#include <iostream>
#include "../src/util.h"
#include "../src/Logger.h"

void test_ass() {
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->open("/home/hyn/test.log");
    error("test ", hyn::util::backtrace_to_string(10).c_str());
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->close();
}


#endif //SERVERFRAMEWORK_UTIL_TEST_H
