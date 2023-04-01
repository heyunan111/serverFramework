/**
  ******************************************************************************
  * @file           : thread.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/1
  ******************************************************************************
  */



#pragma once

#include <unistd.h>
#include <boost/noncopyable.hpp>
#include <memory>
#include <functional>
#include "mutex.h"

namespace hyn::thread {

class Thread : public boost::noncopyable {
public:
    typedef std::shared_ptr<Thread> ptr;

    Thread(std::function<void()> cb, const std::string &name);

    ~Thread();

    [[nodiscard]] pid_t get_id() const { return m_id; }

    [[nodiscard]]const std::string &get_name() const { return m_name; }

    void join();

    static Thread *GetThis();

    static const std::string &GetName();

    static void SetName(const std::string &name);

private:
    static void *run(void *arg);

private:
    pid_t m_id = -1;
    pthread_t m_thread = 0;
    std::function<void()> m_cb;
    std::string m_name;
};
}