/**
  ******************************************************************************
  * @file           : FDManger.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/10
  ******************************************************************************
  */
#pragma once


#include <boost/container/detail/singleton.hpp>
#include <sys/stat.h>
#include <cstdint>
#include <memory>

namespace hyn {

/**
 * @作用 文件句柄上下文类
 * @参数 管理文件句柄类型(是否socket)
 * @参数 是否阻塞,是否关闭,读/写超时时间
 */
class FdCtx : public std::enable_shared_from_this<FdCtx> {
public:
    explicit FdCtx(int fd);

private:
    bool init();

    ///是否初始化
    bool m_isInit: 1;
    ///是否socket
    bool m_isSocket: 1;
    ///是否system nonblock
    bool m_isSysNonblock: 1;
    ///是否usr nonblock
    bool m_isUsrNonblock: 1;
    ///是否关闭
    bool m_isClose;
    ///fd
    int m_fd;
    ///读超时时间
    uint64_t m_recvTimeOut;
    ///写超时时间
    uint64_t m_sendTimeOut;
};

class FDManger {

};

} // hyn

