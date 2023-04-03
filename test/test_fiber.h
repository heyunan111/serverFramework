/**
  ******************************************************************************
  * @file           : test_fiber.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/2
  ******************************************************************************
  */



#pragma once

#include <iostream>
#include "../hyn_include.h"

void run_in_f() {
    std::cout << "run in f begin \n";
    hyn::fiber::Fiber::YieldToHold();
    std::cout << "run in f end \n";
    hyn::fiber::Fiber::YieldToHold();
}

void test_fiber() {
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->open("/home/hyn/test.log");
    hyn::fiber::Fiber::GetThis();
    std::cout << "main begin\n";
    hyn::fiber::Fiber::ptr fiber(new hyn::fiber::Fiber(run_in_f));
    fiber->swap_in();
    std::cout << "main after swap in\n";
    fiber->swap_in();
    std::cout << "main after end\n";
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->close();
}