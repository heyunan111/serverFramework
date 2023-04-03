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

#include "../hyn_include.h"
#include <iostream>

void test_sch() {


    hyn::scheduler::Scheduler sc;
    sc.start();
    sc.stop();


}

#endif //SERVERFRAMEWORK_TEST_SCHEDULER_H
