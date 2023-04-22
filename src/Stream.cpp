/**
  ******************************************************************************
  * @file           : Stream.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/21
  ******************************************************************************
  */



#include "Stream.h"

namespace hyn {

int Stream::readFixSize(void *buff, size_t len) {
    size_t offset = 0;
    auto left = static_cast<int64_t>(len);
    while (left > 0) {
        int64_t length = read(static_cast<char *>(buff) + offset, left);
        if (length <= 0)
            return (int) length;
        offset += length;
        left -= length;
    }
    return (int) len;
}

int Stream::readFixSize(ByteArray::ptr ba, size_t len) {
    auto left = static_cast<int64_t>(len);
    while (left > 0) {
        int64_t length = read(ba, left);
        if (length <= 0)
            return (int) length;
        left -= length;
    }
    return (int) len;
}

int Stream::writeFixSize(void *buff, size_t len) {
    size_t offset = 0;
    auto left = static_cast<int64_t>(len);
    while (left > 0) {
        int64_t length = write(static_cast<char *>(buff) + offset, left);
        if (length <= 0)
            return (int) length;
        offset += length;
        left -= length;
    }
    return (int) len;
}

int Stream::writeFixSize(ByteArray::ptr ba, size_t len) {
    auto left = static_cast<int64_t>(len);
    while (left > 0) {
        int64_t length = write(ba, left);
        if (length <= 0)
            return (int) length;
        left -= length;
    }
    return (int) len;
}
} // hyn