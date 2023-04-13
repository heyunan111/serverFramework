/**
  ******************************************************************************
  * @file           : endian.h
  * @author         : hyn
  * @brief          : 字节序大小端转换
  * @attention      : None
  * @date           : 2023/4/13
  ******************************************************************************
  */



#pragma once

#define hyn_LITTLE_ENDIAN 1
#define hyn_BIG_ENDIAN 2

#include <byteswap.h>
#include <cstdint>
#include <type_traits>

/**
 * @brief 8字节类型的字节序转化
 */
template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
byteswap(T value) {
    return (T) bswap_64((uint64_t) value);
}

/**
 * @brief 4字节类型的字节序转化
 */
template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
byteswap(T value) {
    return (T) bswap_32((uint32_t) value);
}

/**
 * @brief 2字节类型的字节序转化
 */
template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
byteswap(T value) {
    return (T) bswap_16((uint16_t) value);
}

#if BYTE_ORDER == BIG_ENDIAN
#define hyn_BYTE_ORDER hyn_BIG_ENDIAN
#else
#define hyn_BYTE_ORDER hyn_LITTLE_ENDIAN
#endif

#if hyn_BYTE_ORDER == hyn_BIG_ENDIAN

/**
 * @brief 只在小端机器上执行byteswap, 在大端机器上什么都不做
 */
template<class T>
T byteswapOnLittleEndian(T t) {
    return t;
}

/**
 * @brief 只在大端机器上执行byteswap, 在小端机器上什么都不做
 */
template<class T>
T byteswapOnBigEndian(T t) {
    return byteswap(t);
}

#else

/**
 * @brief 只在小端机器上执行byteswap, 在大端机器上什么都不做
 */
template<class T>
T byteswapOnLittleEndian(T t) {
    return byteswap(t);
}

/**
 * @brief 只在大端机器上执行byteswap, 在小端机器上什么都不做
 */
template<class T>
T byteswapOnBigEndian(T t) {
    return t;
}
#endif
