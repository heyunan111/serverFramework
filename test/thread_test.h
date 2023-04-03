/**
  ******************************************************************************
  * @file           : thread_test.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/1
  ******************************************************************************
  */



#ifndef SERVERFRAMEWORK_THREAD_TEST_H
#define SERVERFRAMEWORK_THREAD_TEST_H

#include "../hyn_include.h"

#include <vector>
#include <iostream>
void fun1(){
    std::cout << hyn::thread::Thread::GetName() << "  ";
    std::cout << hyn::thread::Thread::GetThis()->get_name() << "四代巨大的 ";
    std::cout << hyn::thread::Thread::GetThis()->get_id();
    error("name: %s",hyn::thread::Thread::GetThis()->get_name().c_str());
    std::cout<<'\n';
    sleep(5);
}

void fun2(){

}

void test_thread(){
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->open("/home/hyn/test.log");
    std::vector<hyn::thread::Thread::ptr> thrs;
    for (int i = 0; i < 5; ++i) {
        hyn::thread::Thread::ptr thr(new hyn::thread::Thread(&fun1,"name"+std::to_string(i)));
        thrs.emplace_back(thr);
    }
    for (int i = 0; i < 5; ++i) {
        thrs[i]->join();
    }
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->close();
}

#endif //SERVERFRAMEWORK_THREAD_TEST_H
