/**
  ******************************************************************************
  * @file           : ByteArray.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/15
  ******************************************************************************
  */


#include "Logger.h"
#include "ByteArray.h"
#include <cstring>
#include <iomanip>
#include <cmath>

namespace hyn {

ByteArray::Node::Node(size_t s) : ptr(new char[s]), size(s) {
}

ByteArray::Node::Node() = default;

ByteArray::Node::~Node() {
    delete[] ptr;
}

ByteArray::ByteArray(size_t base_size) : m_baseSize(base_size), m_capacity(base_size), m_root(new Node(base_size)),
                                         m_cur(m_root) {
}

ByteArray::~ByteArray() {
    Node *temp = m_root;
    while (temp != nullptr) {
        m_cur = temp;
        temp = temp->next;
        delete m_cur;
    }
}

void ByteArray::writeFint8(int8_t value) {
    write(&value, sizeof(value));
}

void ByteArray::writeFuint8(uint8_t value) {
    write(&value, sizeof(value));
}

void ByteArray::writeFint16(int16_t value) {
    if (m_endian != hyn_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFuint16(uint16_t value) {
    if (m_endian != hyn_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFint32(int32_t value) {
    if (m_endian != hyn_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFuint32(uint32_t value) {
    if (m_endian != hyn_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFint64(int64_t value) {
    if (m_endian != hyn_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFuint64(uint64_t value) {
    if (m_endian != hyn_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

size_t ByteArray::getPos() const {
    return m_pos;
}

void ByteArray::setPos(size_t mPos) {
    THROW_OUT_OF_RANGE_IF(mPos > m_capacity, "setpos out of range");
    m_pos = mPos;
    m_size = std::max(m_pos, m_size);
    m_cur = m_root;
    while (mPos > m_cur->size) {
        mPos -= m_cur->size;
        m_cur = m_cur->next;
    }
    if (m_cur->size == mPos)
        m_cur = m_cur->next;
}

bool ByteArray::writeToFile(const std::string &fileName) {
    std::ofstream ofs;
    ofs.open(fileName, std::ios::trunc | std::ios::binary);
    if (!ofs) {
        error("write to file %s error,errno:%d,errstr:%s", fileName.c_str(), errno, strerror(errno));
        return false;
    }
    auto read_size = getReadSize(), pos = m_pos;
    Node *cur = m_cur;
    while (read_size > 0) {
        auto diff = pos % m_baseSize;
        auto len = (read_size > m_baseSize ? m_baseSize : read_size) - diff;
        ofs.write(cur->ptr, static_cast<long>(len));
        cur = cur->next;
        pos += len;
        read_size -= len;
    }
    return true;
}

void ByteArray::clear() {
    m_pos = 0;
    m_capacity = m_baseSize;
    m_size = 0;
    Node *temp = m_root->next;
    while (temp) {
        m_cur = temp;
        temp = temp->next;
        delete m_cur;
    }
    m_cur = m_root;
    m_root->next = nullptr;
}

void ByteArray::write(const void *buff, size_t size) {
    if (size == 0) {
        return;
    }
    addCapacity(size);

    size_t npos = m_pos % m_baseSize;
    size_t ncap = m_cur->size - npos;
    size_t bpos = 0;

    while (size > 0) {
        if (ncap >= size) {
            memcpy(m_cur->ptr + npos, (const char *) buff + bpos, size);
            if (m_cur->size == (npos + size)) {
                m_cur = m_cur->next;
            }
            m_pos += size;
            bpos += size;
            size = 0;
        } else {
            memcpy(m_cur->ptr + npos, (const char *) buff + bpos, ncap);
            m_pos += ncap;
            bpos += ncap;
            size -= ncap;
            m_cur = m_cur->next;
            ncap = m_cur->size;
            npos = 0;
        }
    }

    if (m_pos > m_size) {
        m_size = m_pos;
    }
}

void ByteArray::addCapacity(size_t size) {
    if (size == 0)
        return;
    size_t old_cap = getCapacity();
    if (old_cap >= size)
        return;
    size -= old_cap;
    size_t count = ceil(1.0 * size / m_baseSize);
    Node *first = nullptr, *last = m_root;
    while (last->next != nullptr)
        last = last->next;
    for (size_t i = 0; i < count; ++i) {
        last->next = new Node(m_baseSize);
        if (first == nullptr)
            first = last->next;
        last = last->next;
        m_capacity += m_baseSize;
    }
    if (old_cap == 0)
        m_cur = first;
}

void ByteArray::read(void *buff, size_t size) {
    if (size > getReadSize()) {
        throw std::out_of_range("not enough len");
    }

    size_t npos = m_pos % m_baseSize;
    size_t ncap = m_cur->size - npos;
    size_t bpos = 0;
    while (size > 0) {
        if (ncap >= size) {
            memcpy((char *) buff + bpos, m_cur->ptr + npos, size);
            if (m_cur->size == (npos + size)) {
                m_cur = m_cur->next;
            }
            m_pos += size;
            bpos += size;
            size = 0;
        } else {
            memcpy((char *) buff + bpos, m_cur->ptr + npos, ncap);
            m_pos += ncap;
            bpos += ncap;
            size -= ncap;
            m_cur = m_cur->next;
            ncap = m_cur->size;
            npos = 0;
        }
    }
}

void ByteArray::read(void *buff, size_t size, size_t position) const {
    THROW_OUT_OF_RANGE_IF(size > (m_size - position), "not enough len");
    size_t npos = position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    size_t bpos = 0;
    Node *cur = m_cur;
    while (size > 0) {
        if (ncap >= size) {
            memcpy((char *) buff + bpos, cur->ptr + npos, size);
            if (cur->size == (npos + size)) {
                cur = cur->next;
            }
            position += size;
            bpos += size;
            size = 0;
        } else {
            memcpy((char *) buff + bpos, cur->ptr + npos, ncap);
            position += ncap;
            bpos += ncap;
            size -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
    }
}

bool ByteArray::readFromFile(const std::string &fileName) {
    std::ifstream ifs;
    ifs.open(fileName);
    if (!ifs) {
        error("read from file %s error,errno:%d,errstr:%s", fileName.c_str(), errno, strerror(errno));
        return false;
    }
    std::shared_ptr<char> buff(new char[m_baseSize], [](char *ptr) { delete[] ptr; });
    while (!ifs.eof()) {
        ifs.read(buff.get(), static_cast<long >(m_baseSize));
        write(buff.get(), ifs.gcount());
    }
    return true;
}

bool ByteArray::isLittleEndian() const {
    return m_endian == hyn_LITTLE_ENDIAN;
}

void ByteArray::setIsLittleEndian(bool is) {
    if (is) {
        m_endian = hyn_LITTLE_ENDIAN;
    } else {
        m_endian = hyn_BIG_ENDIAN;
    }
}

std::string ByteArray::toString() const {
    std::string str;
    str.resize(getReadSize());
    if (str.empty())
        return str;
    read(&str[0], str.size(), m_pos);
    return str;
}


std::string ByteArray::toHexString() const {
    std::string str = toString();
    std::stringstream ss;

    for (size_t i = 0; i < str.size(); ++i) {
        if (i > 0 && i % 32 == 0) {
            ss << std::endl;
        }
        ss << std::setw(2) << std::setfill('0') << std::hex
           << (int) (uint8_t) str[i] << " ";
    }

    return ss.str();
}


uint64_t ByteArray::getReadBuffers(std::vector<iovec> &buffers, uint64_t len) const {
    len = len > getReadSize() ? getReadSize() : len;
    if (len <= 0)
        return 0;
    uint64_t size = len;
    size_t npos = m_pos % m_baseSize, ncap = m_cur->size - npos;
    iovec iov{};
    Node *cur = m_cur;
    while (size > 0) {
        if (ncap >= size) {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = size;
            size = 0;
        } else {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = ncap;
            size -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
        buffers.push_back(iov);
    }
    return len;
}

uint64_t ByteArray::getReadBuffers(std::vector<iovec> &buffers, uint64_t len, uint64_t position) const {
    len = len > getReadSize() ? getReadSize() : len;
    if (len <= 0)
        return 0;
    uint64_t size = len;
    size_t npos = position % m_baseSize;
    size_t count = position / m_baseSize;
    Node *cur = m_cur;
    while (count > 0) {
        cur = cur->next;
        --count;
    }
    size_t ncap = cur->size - npos;
    iovec iov{};
    while (size > 0) {
        if (ncap >= size) {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = size;
            size = 0;
        } else {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = ncap;
            size -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
        buffers.push_back(iov);
    }
    return len;
}

uint64_t ByteArray::getWriteBuffers(std::vector<iovec> &buffers, uint64_t len) {
    if (len == 0) {
        return 0;
    }
    addCapacity(len);
    uint64_t size = len;

    size_t npos = m_pos % m_baseSize;
    size_t ncap = m_cur->size - npos;
    iovec iov{};
    Node *cur = m_cur;
    while (len > 0) {
        if (ncap >= len) {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = len;
            len = 0;
        } else {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = ncap;

            len -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
        buffers.push_back(iov);
    }
    return size;
}

uint32_t ByteArray::EncodeZigzag32(const int32_t &value) {
    if (value < 0) {
        return (static_cast<uint32_t>(-value)) * 2 - 1;
    } else {
        return value * 2;
    }
}

uint64_t ByteArray::EncodeZigzag64(const int64_t &value) {
    if (value < 0) {
        return (static_cast<uint64_t>(-value)) * 2 - 1;
    } else {
        return value * 2;
    }
}

int32_t ByteArray::DecodeZigzag32(uint32_t value) {
    return (value >> 1) ^ (-(value & 1));
}

int64_t ByteArray::DecodeZigzag64(uint64_t value) {
    return (value >> 1) ^ (-(value & 1));
}

void ByteArray::writeInt32(int32_t value) {
    writeUint32(EncodeZigzag32(value));
}

void ByteArray::writeUint32(uint32_t value) {
    uint8_t temp[5];
    uint8_t i = 0;
    while (value >= 0x80) {
        temp[i++] = (value & 0x7F) | 0x80;
        value >>= 7;
    }
    temp[i++] = value;
    write(temp, i);
}

void ByteArray::writeInt64(int64_t value) {
    writeUint64(EncodeZigzag64(value));
}

void ByteArray::writeUint64(uint64_t value) {
    uint8_t temp[10];
    uint8_t i = 0;
    while (value >= 0x80) {
        temp[i++] = (value & 0x7F) | 0x80;
        value >>= 7;
    }
    temp[i++] = value;
    write(temp, i);
}

void ByteArray::writeFloat(float value) {
    uint32_t v;
    memcpy(&v, &value, sizeof(value));
    writeFuint32(v);
}

void ByteArray::writeDouble(double value) {
    uint64_t v;
    memcpy(&v, &value, sizeof(value));
    writeFuint64(v);
}

void ByteArray::writeStringF16(const std::string &value) {
    writeFuint16(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringF32(const std::string &value) {
    writeFuint32(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringF64(const std::string &value) {
    writeFuint64(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringVint(const std::string &value) {
    writeUint64(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringWithoutLength(const std::string &value) {
    write(value.c_str(), value.size());
}

int8_t ByteArray::readFint8() {
    int8_t v;
    read(&v, sizeof(v));
    return v;
}

uint8_t ByteArray::readFuint8() {
    uint8_t v;
    read(&v, sizeof(v));
    return v;
}


int16_t ByteArray::readFint16() {
    int16_t v;
    read(&v, sizeof(v));
    if (m_endian == hyn_BYTE_ORDER) {
        return v;
    } else {
        return byteswap(v);
    }
}

uint16_t ByteArray::readFuint16() {
    uint16_t v;
    read(&v, sizeof(v));
    if (m_endian == hyn_BYTE_ORDER) {
        return v;
    } else {
        return byteswap(v);
    }
}

int32_t ByteArray::readFint32() {
    int32_t v;
    read(&v, sizeof(v));
    if (m_endian == hyn_BYTE_ORDER) {
        return v;
    } else {
        return byteswap(v);
    }
}

uint32_t ByteArray::readFuint32() {
    uint32_t v;
    read(&v, sizeof(v));
    if (m_endian == hyn_BYTE_ORDER) {
        return v;
    } else {
        return byteswap(v);
    }
}

int64_t ByteArray::readFint64() {
    int64_t v;
    read(&v, sizeof(v));
    if (m_endian == hyn_BYTE_ORDER) {
        return v;
    } else {
        return byteswap(v);
    }
}

uint64_t ByteArray::readFuint64() {
    uint64_t v;
    read(&v, sizeof(v));
    if (m_endian == hyn_BYTE_ORDER) {
        return v;
    } else {
        return byteswap(v);
    }
}


int32_t ByteArray::readInt32() {
    return DecodeZigzag32(readUint32());
}

uint32_t ByteArray::readUint32() {
    uint32_t res = 0;
    for (int i = 0; i < 32; i += 7) {
        uint8_t bytes = readFuint8();
        if (bytes < 0x80) {
            res |= static_cast<uint32_t>(bytes) << i;
            break;
        } else {
            res |= static_cast<uint32_t>(bytes & 0x7f) << i;
        }
    }
    return res;
}

int64_t ByteArray::readInt64() {
    return DecodeZigzag64(readUint64());
}

uint64_t ByteArray::readUint64() {
    uint64_t res = 0;
    for (int i = 0; i < 64; i += 7) {
        uint8_t bytes = readFuint8();
        if (bytes < 0x80) {
            res |= static_cast<uint64_t>(bytes) << i;
            break;
        } else {
            res |= static_cast<uint64_t>(bytes & 0x7f) << i;
        }
    }
    return res;
}

float ByteArray::readFloat() {
    auto v = readFuint32();
    float value;
    memcpy(&value, &v, sizeof(v));
    return value;
}

double ByteArray::readDouble() {
    auto v = readFuint64();
    double value;
    memcpy(&value, &v, sizeof(v));
    return value;
}

std::string ByteArray::readStringF16() {
    auto len = readFuint16();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

std::string ByteArray::readStringF32() {
    auto len = readFuint32();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

std::string ByteArray::readStringF64() {
    auto len = readFuint64();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

std::string ByteArray::readStringVint() {
    auto len = readFuint64();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}


} // hyn