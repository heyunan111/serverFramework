/**
  ******************************************************************************
  * @file           : FDManger.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/10
  ******************************************************************************
  */



#include <bits/fcntl-linux.h>
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
} // hyn