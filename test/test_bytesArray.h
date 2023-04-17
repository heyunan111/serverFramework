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
#include "../hyn.h"

/*
template<typename T>
void test_byarr(int len, int bas_len) {
    std::vector<T> vec;
    vec.reserve(len);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
    for (int i = 0; i < len; ++i) {
        vec.emplace_back(dis(gen));
    }
    hyn::ByteArray::ptr ba(new hyn::ByteArray(bas_len));
    for (auto &i: vec) {
        ba->writeFixation<T>(i);
    }
    ba->setPos(0);
    for (auto &i: vec) {
        auto v = ba->readFixation<T>();
        assert(v == i);
    }
    assert(ba->getReadSize() == 0);
}*/

void test_bytearrya() {
    /* test_byarr<int8_t>(100, 1);
     test_byarr<uint8_t>(100, 1);
     test_byarr<int16_t>(100, 1);
     test_byarr<uint16_t>(100, 1);*/
}


#endif //SERVERFRAMEWORK_TEST_BYTESARRAY_H
