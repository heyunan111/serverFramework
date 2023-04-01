/**
  ******************************************************************************
  * @file           : mutex.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/1
  ******************************************************************************
  */



#pragma once

#include "thread.h"

#include <boost/noncopyable.hpp>
#include <functional>
#include <pthread.h>
#include <semaphore.h>
#include <semaphore>

/**
 *@类名：Semaphore : noncopyable
 *@参数：m_semaphore
 */

class Semaphore : boost::noncopyable_::noncopyable {
public:
    Semaphore(uint32_t const = 0);

    ~Semaphore();

    void wait();

    void notify();

private:
    sem_t m_semaphore;
};


