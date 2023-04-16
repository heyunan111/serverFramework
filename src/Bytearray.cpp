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

namespace hyn {
ByteArray::Node::Node(size_t s) : ptr(new char[s]), size(0) {
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
        auto len = read_size > m_baseSize ? m_baseSize : read_size - diff;
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
    if (size <= 0)
        return;
    addCapacity(size);
    size_t npos = m_pos % m_baseSize, ncap = m_cur->size - npos, bpos = 0;
    while (size > 0) {
        if (ncap >= size) {
            memcpy(m_cur->ptr + npos, static_cast<const char *>(buff) + bpos, size);
            if (m_cur->size == (npos + size))
                m_cur = m_cur->next;
            m_pos += size;
            bpos += size;
            size = 0;
        } else {
            memcpy(m_cur->ptr + npos, static_cast<const char *>(buff) + bpos, ncap);
            m_pos += ncap;
            bpos += ncap;
            size -= ncap;
            m_cur = m_cur->next;
            ncap = m_cur->size;
            npos = 0;
        }
    }
    if (m_pos > m_size)
        m_size = m_pos;
}

void ByteArray::addCapacity(size_t size) {
    if (size == 0)
        return;
    size_t old_cap = getCapacity();
    if (old_cap > size)
        return;
    size -= old_cap;
    size_t count = size / m_baseSize + 1;
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
    THROW_OUT_OF_RANGE_IF(size > m_size - m_pos, "not enough len");
    size_t npos = m_pos % m_baseSize, ncap = m_capacity - m_pos, bpos = 0;
    while (size > 0) {
        if (ncap >= size) {
            memcpy(static_cast<char *>(buff), m_cur->ptr + npos, size);
            m_pos += size;
            bpos += size;
            size = 0;
        } else {
            memcpy(static_cast<char *>(buff) + bpos, m_cur->ptr + npos, ncap);
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
    THROW_OUT_OF_RANGE_IF(size > m_size - position, "not enough len");
    size_t npos = position % m_baseSize, ncap = m_capacity - npos, bpos = 0;
    Node *cur = m_cur;
    while (size > 0) {
        if (ncap >= size) {
            memcpy(static_cast<char *>(buff), cur->ptr + npos, size);
            if (cur->size == (npos + size))
                cur = cur->next;
            position += size;
            bpos += size;
            size = 0;
        } else {
            memcpy(static_cast<char *>(buff) + bpos, cur->ptr + npos, ncap);
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
    return std::string();
}

std::string ByteArray::toHexString() const {
    return std::string();
}

uint64_t ByteArray::getReadBuffers(std::vector<iovec> &buffers, uint64_t len) const {
    return 0;
}

uint64_t ByteArray::getReadBuffers(std::vector<iovec> &buffers, uint64_t len, uint64_t position) const {
    return 0;
}

uint64_t ByteArray::getWriteBuffers(std::vector<iovec> &buffers, uint64_t len) {
    return 0;
}


} // hyn