/**
  ******************************************************************************
  * @file           : ByteArray.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/15
  ******************************************************************************
  */
#pragma

#include <memory>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>

#include "endian.h"

namespace hyn {

/**
 *@brief ByteArray
 *@note 序列化
 */
class ByteArray {
    /**
     *@brief Bytearray的存储节点
     */
    struct Node {
        /**
         *@brief 构造指定大小的内存块
         *@param s 内存块字节数
         */
        explicit Node(size_t s);

        /**
         *@brief 无参构造，全部初始化为nullptr/0
         */
        Node();

        /**
         *@brief 析构，释放内存
         */
        ~Node();

        ///内存块地址
        char *ptr{nullptr};
        ///下一个内存块
        Node *next{nullptr};
        ///内存块大小
        size_t size{0};
    };

    /**
     *@brief 使用指定长度的内存构造ByteArray
     *@param 内存块大小
     */
    explicit ByteArray(size_t base_size = 4096);

    ~ByteArray();

    /**
     *@brief 写入固定长度的数据（大端/小端）
     *@note int8_t,uint8_t,int16_t,uint16_t,int32_t,uint32_t,int64_t,uint64_t,
     */
    template<typename T>
    void writeFixation(T value);

    /**
     * @brief 写入有符号Varint32类型的数据
     * @note 采用Varint编码
     */
    void writeInt32(int32_t value);

    /**
     * @brief 写入无符号Varint32类型的数据
     * @note 采用Varint编码
     */
    void writeUint32(uint32_t value);

    /**
     * @brief 写入有符号Varint64类型的数据
     * @note 采用Varint编码
     */
    void writeInt64(int64_t value);

    /**
     * @brief 写入无符号Varint64类型的数据
     * @note 采用Varint编码
     */
    void writeUint64(uint64_t value);

    /**
     * @brief 写入float类型的数据
     */
    void writeFloat(float value);

    /**
     * @brief 写入double类型的数据
     */
    void writeDouble(double value);


    /**
     * @brief 写入std::string类型的数据,用uint16_t作为长度类型
     */
    void writeStringF16(const std::string &value);

    /**
     * @brief 写入std::string类型的数据,用uint32_t作为长度类型
     */
    void writeStringF32(const std::string &value);

    /**
     * @brief 写入std::string类型的数据,用uint64_t作为长度类型
     */
    void writeStringF64(const std::string &value);

    /**
     * @brief 写入std::string类型的数据,用无符号Varint64作为长度类型
     */
    void writeStringVint(const std::string &value);

    /**
     * @brief 写入std::string类型的数据,无长度
     */
    void writeStringWithoutLength(const std::string &value);

    /**
    *@brief 写入size长度的数据
    *@param buff 内存缓冲区指针
    *@param size 数据大小
    *@note m_pos += size,if:m_pos > m_size then:m_size = m_pos
    */
    void write(const void *buff, size_t size);

    /**
     *@brief 读取size长度的数据
     *@param [out] buff 内存缓冲区指针
     *@param size 数据大小
     *@note m_pos += size,if:m_pos > m_size then:m_size = m_pos
     *@exception if getReadSize < size then THROW_RUN_TIME_ERROR
     */
    void read(void *buff, size_t size);

    /**
    * @brief 读取size长度的数据
    * @param[out] buf 内存缓存指针
    * @param[in] size 数据大小
    * @param[in] position 读取开始位置
    * @exception 如果 (m_size - position) < size 则抛出 std::out_of_range
    */
    void read(void *buff, size_t size, size_t position) const;

    /**
     *@brief 把ByteArray写入文件
     *@param fileName 文件名
     *@return 是否成功
     */
    bool writeToFile(const std::string &fileName);

    /**
     *@brief 从文件中读取数据
     *@param fileName 文件名
     *@return 是否成功
     */
    bool readFromFile(const std::string &fileName);

    /**
    * @brief 返回ByteArray当前位置
    */
    [[nodiscard]] size_t getPos() const;

    /**
     * @brief 设置ByteArray当前位置
     * @post 如果m_position > m_size 则 m_size = m_position
     * @exception 如果m_position > m_capacity 则抛出 std::out_of_range
     */
    void setPos(size_t mPos);

    /**
     *@brief 返回内存块大小
     */
    [[nodiscard]] bool getBaseSize() const { return m_baseSize; }

    /**
     *@brief 返回可读取数据的大小
     */
    [[nodiscard]] size_t getReadSize() const { return m_size - m_pos; }

    /**
     *@brief 是否小端
     */
    [[nodiscard]] bool isLittleEndian() const;

    /**
     *@brief 设置是否小端
     */
    void setIsLittleEndian(bool is);

    /**
     *@brief 将ByteArray里的数据转成string
     *@note [m_pos,_size)
     */
    [[nodiscard]] std::string toString() const;

    /**
     *@brief 将ByteArray里的数据转成16进制string
     *@note [m_pos,_size)
     */
    [[nodiscard]] std::string toHexString() const;

    /**
     * @brief 获取可读取的缓存,保存成iovec数组
     * @param[out] buffers 保存可读取数据的iovec数组
     * @param[in] len 读取数据的长度,如果len > getReadSize() 则 len = getReadSize()
     * @return 返回实际数据的长度
     */
    uint64_t getReadBuffers(std::vector<iovec> &buffers, uint64_t len = ~0ull) const;

    /**
     * @brief 获取可读取的缓存,保存成iovec数组,从position位置开始
     * @param[out] buffers 保存可读取数据的iovec数组
     * @param[in] len 读取数据的长度,如果len > getReadSize() 则 len = getReadSize()
     * @param[in] position 读取数据的位置
     * @return 返回实际数据的长度
     */
    uint64_t getReadBuffers(std::vector<iovec> &buffers, uint64_t len, uint64_t position) const;

    /**
     * @brief 获取可写入的缓存,保存成iovec数组
     * @param[out] buffers 保存可写入的内存的iovec数组
     * @param[in] len 写入的长度
     * @return 返回实际的长度
     * @post 如果(m_position + len) > m_capacity 则 m_capacity扩容N个节点以容纳len长度
     */
    uint64_t getWriteBuffers(std::vector<iovec> &buffers, uint64_t len);


    /**
     *@brief 清空ByteArray
     */
    void clear();

    /**
     *@brief 返回当前数据的大小
     *@note 不包括空的
     */
    [[nodiscard]] size_t getSize() const { return m_size; }

private:
    /**
     *@brief 采用Zigzag编码将32位有符号整数编码为无符号整数
     */
    static uint32_t EncodeZigzag32(const uint32_t &value);

    /**
     *@brief 采用Zigzag编码将64位有符号整数编码为无符号整数
     */
    static uint64_t EncodeZigzag64(const uint64_t &value);

    /**
     *@brief 采用Zigzag编码解码32位有符号整数
     */
    static int32_t DecodeZigzag32(int32_t value);

    /**
    *@brief 采用Zigzag编码解码64位有符号整数
    */
    static int64_t DecodeZigzag64(int64_t value);

    /**
     *@brief 如果不能容纳size，则扩容ByteArray
     *@param size
     */
    void addCapacity(size_t size);

    /**
     *@brief 获取当前的可写入数据大小
     *@return 大小
     */
    [[nodiscard]] size_t getCapacity() const { return m_capacity - m_pos; }

    ///内存总大小
    size_t m_baseSize;
    ///当前操作位置
    size_t m_pos{0};
    ///当前总容量
    size_t m_capacity;
    ///当前数据大小
    size_t m_size{0};
    ///字节序（默认大端）
    int8_t m_endian{hyn_BIG_ENDIAN};
    ///第一个内存块指针
    Node *m_root;
    ///当前操作的内存块指针
    Node *m_cur;
};


template<typename T>
void ByteArray::writeFixation(T value) {
    if (m_endian != hyn_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

template<>
void ByteArray::writeFixation(uint8_t value) {
    write(&value, sizeof(value));
}

template<>
void ByteArray::writeFixation(int8_t value) {
    write(&value, sizeof(value));
}


} // hyn


