/**
  ******************************************************************************
  * @file           : SocketStream.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/22
  ******************************************************************************
  */
#pragma once

#include "Stream.h"
#include "Socket.h"

namespace hyn {
/**
 *@brief SocketStream
 *@note
 */
class SocketStream : public Stream {
public:
    using ptr = std::shared_ptr<SocketStream>;

    /**
     *@brief 构造函数
     *@param sock Socket类
     *@param owner 是否完全控制（是的话结束之后会关闭socket）
     */
    explicit SocketStream(Socket::ptr sock, bool owner = true);

    /**
     *@brief 析构函数
     *@note owner = true，关闭socket
     */
    ~SocketStream() override;

    /**
     * @brief 读取数据
     * @param[out] buffer 待接收数据的内存
     * @param[in] length 待接收数据的内存长度
     * @return
     *      @retval >0 返回实际接收到的数据长度
     *      @retval =0 socket被远端关闭
     *      @retval <0 socket错误
     */
    int read(void *buffer, size_t length) override;

    /**
     * @brief 读取数据
     * @param[out] ba 接收数据的ByteArray
     * @param[in] length 待接收数据的内存长度
     * @return
     *      @retval >0 返回实际接收到的数据长度
     *      @retval =0 socket被远端关闭
     *      @retval <0 socket错误
     */
    int read(ByteArray::ptr ba, size_t length) override;

    /**
     * @brief 写入数据
     * @param[in] buffer 待发送数据的内存
     * @param[in] length 待发送数据的内存长度
     * @return
     *      @retval >0 返回实际接收到的数据长度
     *      @retval =0 socket被远端关闭
     *      @retval <0 socket错误
     */
    int write(const void *buffer, size_t length) override;

    /**
     * @brief 写入数据
     * @param[in] ba 待发送数据的ByteArray
     * @param[in] length 待发送数据的内存长度
     * @return
     *      @retval >0 返回实际接收到的数据长度
     *      @retval =0 socket被远端关闭
     *      @retval <0 socket错误
     */
    int write(ByteArray::ptr ba, size_t length) override;

    /**
     * @brief 关闭socket
     */
    void close() override;

    /**
    * @brief 返回Socket类
    */
    [[nodiscard]] Socket::ptr getSocket() const { return m_socket; }

    /**
     * @brief 返回是否连接
     */
    [[nodiscard]] bool isConnected() const;

private:
    Socket::ptr m_socket;
    bool m_owner;
};


}//hyn