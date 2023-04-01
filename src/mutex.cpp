/**
  ******************************************************************************
  * @file           : my_mutex.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/1
  ******************************************************************************
  */



#include "../include/mutex.h"
#include "../include/Logger.h"

hyn::my_mutex::Semaphore::Semaphore(const uint32_t count) {
    if (sem_init(&m_semaphore, 0, count)) {
        error("sem init error");
        THROW_RUNTIME_ERROR_IF(1, "sem init error");
    }
}

hyn::my_mutex::Semaphore::~Semaphore() {
    sem_destroy(&m_semaphore);
}

void hyn::my_mutex::Semaphore::wait() {
    if (!sem_wait(&m_semaphore))
        return;
}

void hyn::my_mutex::Semaphore::notify() {
    if (!sem_post(&m_semaphore)) {
        error("sem post error");
        THROW_RUNTIME_ERROR_IF(1, "sem post error");
    }
}
