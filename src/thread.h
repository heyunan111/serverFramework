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

    /**
    * @brief 构造函数
    * @param[in] cb 线程执行函数
    * @param[in] name 线程名称
    */
    Thread(std::function<void()> cb, const std::string &name);

    ~Thread();

    [[nodiscard]] pid_t getId() const { return m_id; }

    [[nodiscard]]const std::string &getName() const { return m_name; }

    /**
     *@作用：等待线程执行完成
     *@参数：null
     *@返回值：null
     */
    void join();

    /**
     *@作用：获取当前线程智能指针
     *@参数：null
     *@返回值：Thread*
     */
    static Thread *GetThis();

    /**
     *@作用：获取当前线程名称
     *@参数：null
     *@返回值：const string
     */
    static const std::string &GetName();

    /**
     *@作用：设置当前线程名称
     *@参数：string
     *@返回值：null
     */
    static void SetName(const std::string &name);

private:
    /**
     *@作用：线程执行函数
     *@参数：函数
     *@返回值：void*
     */
    static void *run(void *arg);

private:
    pid_t m_id = -1;                //线程id
    pthread_t m_thread = 0;         //线程结构
    std::function<void()> m_cb;     //线程执行函数
    std::string m_name;             //线程名称
    hyn::mutex::Semaphore m_semaphore;   //信号量
};
}