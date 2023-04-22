/**
  ******************************************************************************
  * @file           : test_bytesArray.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/17
  ******************************************************************************
  */



#ifndef SERVERFRAMEWORK_TEST_BYTESARRAY_H
#define SERVERFRAMEWORK_TEST_BYTESARRAY_H

#include <random>
#include "../src/ByteArray.h"
#include "../src/Logger.h"

void test_bytearrya() {

#define test_b(type, len, write_fun, read_fun, base_len) {      \
    std::vector<type> vec;                                      \
    vec.resize(len);                                            \
    for(int i = 0;i<len;++i){                                   \
        vec.push_back(rand());                                  \
    }                                                           \
    hyn::ByteArray::ptr ba(new hyn::ByteArray(base_len));       \
    for(auto &i:vec){                                           \
        ba->write_fun(i);                                       \
    }                                                           \
    ba->setPos(0);                                              \
    for (size_t i= 0;i < vec.size();++i){                       \
        type v = ba->read_fun();                                \
        assert(v == vec[i]);                                    \
    }                                                           \
}

    test_b(int8_t, 100, writeFint8, readFint8, 1);
    test_b(uint8_t, 100, writeFuint8, readFuint8, 1);
    test_b(int16_t, 100, writeFint16, readFint16, 1);
    test_b(uint16_t, 100, writeFuint16, readFuint16, 1);
    test_b(int32_t, 100, writeFint32, readFint32, 1);
    test_b(uint32_t, 100, writeFuint32, readFuint32, 1);
    test_b(int64_t, 100, writeFint64, readFint64, 1);
    test_b(uint64_t, 100, writeFuint64, readFuint64, 1);

    test_b(int32_t, 100, writeInt32, readInt32, 1);
    test_b(uint32_t, 100, writeUint32, readUint32, 1);
    test_b(int64_t, 100, writeInt64, readInt64, 1);
    test_b(uint64_t, 100, writeUint64, readUint64, 1);
#undef test_b

#define test_b(type, len, write_fun, read_fun, base_len) {      \
    std::vector<type> vec;                                      \
    vec.resize(len);                                            \
    for(int i = 0;i<len;++i){                                   \
        vec.push_back(rand());                                  \
    }                                                           \
    hyn::ByteArray::ptr ba(new hyn::ByteArray(base_len));       \
    for(auto &i:vec){                                           \
        ba->write_fun(i);                                       \
    }                                                           \
    ba->setPos(0);                                              \
    for (size_t i= 0;i < vec.size();++i){                       \
        type v = ba->read_fun();                                \
        assert(v == vec[i]);                                    \
    }                                                           \
    assert(ba->getReadSize() == 0);                             \
    ba->setPos(0);                                              \
    assert(ba->writeToFile(#type "_" #len "_" #read_fun".dat"));      \
    hyn::ByteArray::ptr ba2(new hyn::ByteArray(base_len * 2));  \
    assert(ba2->readFromFile(#type "_" #len "_" #read_fun ".dat"));    \
    ba2->setPos(0);                                             \
    assert(ba->toString() == ba2->toString());                  \
    assert(ba->getPos() == 0);                                  \
    assert(ba2->getPos() == 0);                                 \
}

    test_b(int8_t, 100, writeFint8, readFint8, 1);
    test_b(uint8_t, 100, writeFuint8, readFuint8, 1);
    test_b(int16_t, 100, writeFint16, readFint16, 1);
    test_b(uint16_t, 100, writeFuint16, readFuint16, 1);
    test_b(int32_t, 100, writeFint32, readFint32, 1);
    test_b(uint32_t, 100, writeFuint32, readFuint32, 1);
    test_b(int64_t, 100, writeFint64, readFint64, 1);
    test_b(uint64_t, 100, writeFuint64, readFuint64, 1);

    test_b(int32_t, 100, writeInt32, readInt32, 1);
    test_b(uint32_t, 100, writeUint32, readUint32, 1);
    test_b(int64_t, 100, writeInt64, readInt64, 1);
    test_b(uint64_t, 100, writeUint64, readUint64, 1);
#undef test_b

}


#endif //SERVERFRAMEWORK_TEST_BYTESARRAY_H
