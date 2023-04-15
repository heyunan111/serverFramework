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
 * @brief 文件句柄上下文类
 * @note 管理文件句柄类型(是否socket)
 * @note 是否阻塞,是否关闭,读/写超时时间
 */
class FdCtx : public std::enable_shared_from_this<FdCtx> {
public:
    typedef std::shared_ptr<FdCtx> ptr;

    explicit FdCtx(int fd);

    /**
     * @brief 是否初始化完成
     */
    bool is_init() const { return m_isInit; }

    /**
     * @brief 是否socket
     */
    bool is_socket() const { return m_isSocket; }

    /**
    * @brief 是否已关闭
    */
    bool is_close() const { return m_isClose; }

    /**
    * @brief 获取系统非阻塞
    */
    bool is_sys_nonblock() const;

    /**
     * @brief 设置系统非阻塞
     * @param[in] mIsSysNonblock 是否阻塞
     */
    void set_is_sys_nonblock(bool mIsSysNonblock);

    /**
     * @brief 获取是否用户主动设置的非阻塞
     */
    bool is_usr_nonblock() const;

    /**
     * @brief 设置用户主动设置非阻塞
     * @param[in] mIsUsrNonblock 是否阻塞
     */
    void set_is_usr_nonblock(bool mIsUsrNonblock);

    /**
    * @brief 设置超时时间
    * @param[in] type 类型SO_RCVTIMEO(读超时), SO_SNDTIMEO(写超时)
    * @param[in] v 时间毫秒
    */
    void set_time(int type, uint64_t time);

    /**
    * @brief 获取超时时间
    * @param[in] type 类型SO_RCVTIMEO(读超时), SO_SNDTIMEO(写超时)
    * @return 超时时间毫秒
    */
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
     *@brief：获取文件句柄类,fd不存在的情况下是否自动创建
     *@param：文件句柄
     *@param：fd不存在的情况下是否自动创建
     */
    FdCtx::ptr get(int fd, bool auto_create = false);

    /**
     *@brief：删除文件句柄类
     *@param：fd
     */
    void del(int fd);

private:
    RWMutexType m_mutex;
    ///文件句柄集合
    std::vector<FdCtx::ptr> m_datas;
};

typedef Singleton <FDManger> FdMgr;
} // hyn

