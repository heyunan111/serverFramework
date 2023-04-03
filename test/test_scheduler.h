/**
  ******************************************************************************
  * @file           : test_scheduler.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/3
  ******************************************************************************
  */



#ifndef SERVERFRAMEWORK_TEST_SCHEDULER_H
#define SERVERFRAMEWORK_TEST_SCHEDULER_H

#include "../include/hyn_include.h"
#include <iostream>

void test_sch() {
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->open("/home/hyn/test.log");

    hyn::scheduler::Scheduler sc;
    sc.stop();

    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->close();
}

#endif //SERVERFRAMEWORK_TEST_SCHEDULER_H
