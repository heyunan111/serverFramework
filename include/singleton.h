/**
  ******************************************************************************
  * @file           : sigleton.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2022/8/20
  ******************************************************************************
  */

#pragma once

namespace hyn ::singleton {

    template<typename T>
    class Singleton {

    private:
        Singleton() = default;

        ~Singleton() = default;

    public:

        static T *get_instance() {
            if (m_instance == nullptr) {
                m_instance = new T();
                return m_instance;
            }
            return m_instance;
        }

        Singleton(T &&) = delete;

        Singleton(const T &) = delete;

        void operator=(const T &) = delete;

    private:
        static T *m_instance;
    };

    template<typename T>
    T *Singleton<T>::m_instance;
}