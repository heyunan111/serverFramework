/**
  ******************************************************************************
  * @file           : Stream.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/21
  ******************************************************************************
  */
#pragma once

#include <memory>
#include "ByteArray.h"


namespace hyn {

/**
 *@brief 流结构
 */
class Stream {
public:
    using ptr = std::shared_ptr<Stream>;

    virtual ~Stream() = default;

    /**
     *@brief 读数据
     *@param [out] buff 接收数据的内存
     *@param len 接收数据的内存大小
     *@return
     *          >0:接收到数据的实际大小
     *          =0:被关闭
     *          <0:出现流错误
     */
    virtual int read(void *buff, size_t len) = 0;

    /**
     *@brief 读数据
     *@param [out] ba 接收数据的ByteArray
     *@param len 接收数据的内存大小
     *@return
     *          >0:接收到数据的实际大小
     *          =0:被关闭
     *          <0:出现流错误
     */
    virtual int read(ByteArray::ptr ba, size_t len) = 0;

    /**
     *@brief 读固定长度数据
     *@param [out] buff 接收数据的内存
     *@param len 接收数据的内存大小
     *@return
     *          >0:接收到数据的实际大小
     *          =0:被关闭
     *          <0:出现流错误
     */
    virtual int readFixSize(void *buff, size_t len);

    /**
    *@brief 读固定长度数据
    *@param [out] ba 接收数据的ByteArray
    *@param len 接收数据的内存大小
    *@return
    *          >0:接收到数据的实际大小
    *          =0:被关闭
    *          <0:出现流错误
    */
    virtual int readFixSize(ByteArray::ptr ba, size_t len);

    /**
     *@brief 写数据
     *@param buff 写入数据的内存
     *@param len 写入数据的内存大小
     *@return
     *          >0:写入到数据的实际大小
     *          =0:被关闭
     *          <0:出现流错误
     */
    virtual int write(void *buff, size_t len) = 0;

    /**
     *@brief 写数据
     *@param ba 写入数据的ByteArray
     *@param len 写入数据的内存大小
     *@return
     *          >0:写入到数据的实际大小
     *          =0:被关闭
     *          <0:出现流错误
     */
    virtual int write(ByteArray::ptr ba, size_t len) = 0;

    /**
    *@brief 写固定大小数据
    *@param buff 写入数据的内存
    *@param len 写入数据的内存大小
    *@return
    *          >0:写入到数据的实际大小
    *          =0:被关闭
    *          <0:出现流错误
    */
    virtual int writeFixSize(void *buff, size_t len);

    /**
    *@brief 写固定大小数据
    *@param ba 写入数据的ByteArray
    *@param len 写入数据的内存大小
    *@return
    *          >0:写入到数据的实际大小
    *          =0:被关闭
    *          <0:出现流错误
    */
    virtual int writeFixSize(ByteArray::ptr ba, size_t len);

    /**
     *@brief 关闭流
     */
    virtual void close() = 0;
};

} // hyn

