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
#include <vector>
#include <memory>
#include <fcntl.h>
#include "mutex.h"
#include "_Singleton.h"


namespace hyn {

/**
 * @作用 文件句柄上下文类
 * @参数 管理文件句柄类型(是否socket)
 * @参数 是否阻塞,是否关闭,读/写超时时间
 */
class FdCtx : public std::enable_shared_from_this<FdCtx> {
public:
    typedef std::shared_ptr<FdCtx> ptr;

    explicit FdCtx(int fd);

    bool is_init() const { return m_isInit; }

    bool is_socket() const { return m_isSocket; }

    bool is_close() const { return m_isClose; }

    bool is_sys_nonblock() const;

    void set_is_sys_nonblock(bool mIsSysNonblock);

    bool is_usr_nonblock() const;

    void set_is_usr_nonblock(bool mIsUsrNonblock);

    void set_time(int type, uint64_t time);

    uint64_t get_time(int type) const;

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

public:
    typedef mutex::RWMutex RWMutexType;

    FDManger();

    ~FDManger() = default;

    /**
     *@作用：获取文件句柄类,fd不存在的情况下是否自动创建
     *@参数：文件句柄
     *@参数：fd不存在的情况下是否自动创建
     */
    FdCtx::ptr get(int fd, bool auto_create = false);

    /**
     *@作用：删除文件句柄类
     *@参数：fd
     */
    void del(int fd);

private:
    RWMutexType m_mutex;
    ///文件句柄集合
    std::vector<FdCtx::ptr> m_datas;
};

typedef Singleton <FDManger> FdMgr;
} // hyn

