/**
  ******************************************************************************
  * @file           : fiber.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/1
  ******************************************************************************
  */
#pragma once

#include <ucontext.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <functional>

namespace hyn::fiber {

    class Fiber : public std::enable_shared_from_this<Fiber> {
    public:
        std::shared_ptr<Fiber> ptr;
        enum State{
            INIT,
            HOLD,
            EXEC,
            TERM,
            READY
        };
    public:
        Fiber() = delete;
        Fiber(std::function<void()> cb,size_t stacksize = 0);
        ~Fiber();

        void reset(std::function<void()> cb);
        void call();

    private:
    };
}