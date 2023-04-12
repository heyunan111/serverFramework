/**
  ******************************************************************************
  * @file           : FDManger.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/10
  ******************************************************************************
  */



#include "../include/FDManger.h"
#include "../include/Hook.h"

namespace hyn {
FdCtx::FdCtx(int fd) : m_isInit(false), m_isSocket(false), m_isSysNonblock(false), m_isUsrNonblock(false), m_isClose(
        false), m_fd(fd), m_recvTimeOut(-1), m_sendTimeOut(-1) {
    init();
}

bool FdCtx::init() {
    if (m_isInit)
        return true;
    //调用fstat函数来获取文件描述符的状态，并将状态存储在fd_stat结构体中。
    struct stat fd_stat{};
    int ret = fstat(m_fd, &fd_stat);
    //如果fstat函数返回-1，则表示文件描述符无效，设置m_isInit和m_isSocket为false。
    //否则，设置m_isInit为true，并通过S_ISSOCK宏判断文件描述符是否是套接字（socket）。
    if (ret == -1) {
        m_isInit = false;
        m_isSocket = false;
    } else {
        m_isInit = true;
        m_isSocket = S_ISSOCK(fd_stat.st_mode);
    }
    //如果文件描述符是套接字，则通过fcntl_f函数获取文件描述符的标志位，如果标志位中没有设置O_NONBLOCK，则设置该标志位，以便以后进行非阻塞I/O操作。同时，将m_sysNonblock设置为true。
    //如果文件描述符不是套接字，则将m_sysNonblock设置为false。
    if (m_isSocket) {
        int flag = fcntl_f(m_fd, F_GETFL, 0);
        if (!(flag & O_NONBLOCK)) {
            fcntl_f(m_fd, F_SETFL, flag | O_NONBLOCK);
            m_isSysNonblock = true;
        }
    } else {
        m_isSysNonblock = false;
    }
    return m_isInit;
}

bool FdCtx::is_sys_nonblock() const {
    return m_isSysNonblock;
}

void FdCtx::set_is_sys_nonblock(bool mIsSysNonblock) {
    m_isSysNonblock = mIsSysNonblock;
}

bool FdCtx::is_usr_nonblock() const {
    return m_isUsrNonblock;
}

void FdCtx::set_is_usr_nonblock(bool mIsUsrNonblock) {
    m_isUsrNonblock = mIsUsrNonblock;
}

void FdCtx::set_time(int type, uint64_t time) {
    if (type == SO_RCVTIMEO) {
        m_recvTimeOut = time;
    } else {
        m_sendTimeOut = time;
    }
}

uint64_t FdCtx::get_time(int type) const {
    if (type == SO_RCVTIMEO) {
        return m_recvTimeOut;
    } else {
        return m_sendTimeOut;
    }
}

FDManger::FDManger() {
    m_datas.resize(64);
}

FdCtx::ptr FDManger::get(int fd, bool auto_create) {
    if (fd == -1)
        return nullptr;
    RWMutexType::ReadLock Rlock(m_mutex);
    if (fd >= (int) m_datas.size()) {
        if (!auto_create)
            return nullptr;
    } else {
        if (m_datas[fd] || !auto_create) {
            return m_datas[fd];
        }
    }
    Rlock.unlock();

    RWMutexType::WriteLock Wlock(m_mutex);
    FdCtx::ptr ctx(new FdCtx(fd));
    if (fd >= static_cast<double >(m_datas.size()))
        m_datas.resize(static_cast<unsigned long>((fd * 1.5)));
    m_datas[fd] = ctx;
    return ctx;
}

void FDManger::del(int fd) {
    RWMutexType::WriteLock lock(m_mutex);
    if (m_datas.size() <= fd)
        return;
    m_datas[fd].reset();
}
} // hyn