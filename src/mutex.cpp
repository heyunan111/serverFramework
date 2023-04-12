/**
  ******************************************************************************
  * @file           : mutex.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/1
  ******************************************************************************
  */



#include "mutex.h"
#include "Logger.h"

hyn::mutex::Semaphore::Semaphore(const uint32_t count) {
    if (sem_init(&m_semaphore, 0, count)) {
        error("sem init error");
        THROW_RUNTIME_ERROR_IF(1, "sem init error");
    }
}

hyn::mutex::Semaphore::~Semaphore() {
    sem_destroy(&m_semaphore);
}

void hyn::mutex::Semaphore::wait() {
    if (sem_wait(&m_semaphore)) {
        fatal("sem wait error");
        THROW_RUNTIME_ERROR_IF(1, "sem wait error");
    }
}

void hyn::mutex::Semaphore::notify() {
    if (sem_post(&m_semaphore)) {
        fatal("sem post error");
        THROW_RUNTIME_ERROR_IF(1, "sem post error");
    }
}
