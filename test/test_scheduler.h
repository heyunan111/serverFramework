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

#include "../src/Logger.h"
#include "../src/Scheduler.h"
#include <iostream>

void t_fiber() {
    static int s_count = 5;
    info("test in fiber s_count= %d", s_count);
    sleep(1);
    if (--s_count >= 0) {
        hyn::scheduler::Scheduler::GetThis()->schedule(&t_fiber);
    }
}


void test_sch() {

    debug("main");
    hyn::scheduler::Scheduler sc(3, true, "test");
    sc.start();
    sleep(2);
    info("schedule");
    sc.schedule(&t_fiber);
    sc.stop();
    info("end");
}

#endif //SERVERFRAMEWORK_TEST_SCHEDULER_H
