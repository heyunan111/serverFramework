/**
  ******************************************************************************
  * @file           : test_hook.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/10
  ******************************************************************************
  */



#ifndef SERVERFRAMEWORK_TEST_HOOK_H
#define SERVERFRAMEWORK_TEST_HOOK_H

#include "../hyn_include.h"

void test_sleep() {
    hyn::iomanager::IOManager iom(2);
    iom.schedule([]() {
        sleep(2);
        info("sleep 2");
    });
    iom.schedule([]() {
        sleep(3);
        info("sleep 3");
    });
    info("sleep test");
}


#endif //SERVERFRAMEWORK_TEST_HOOK_H
