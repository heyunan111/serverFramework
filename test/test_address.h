/**
  ******************************************************************************
  * @file           : test_address.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/14
  ******************************************************************************
  */



#pragma once

#include "../hyn.h"
#include <vector>

void test() {
    std::vector<hyn::Address::ptr> addrss;
    bool v = hyn::Address::Lookup(addrss, "www.baidu.com");
    if (!v) {
        info("error");
        return;
    }
    for (size_t i = 0; i < addrss.size(); ++i) {
        info("%d - %s", i, addrss[i]->toString().c_str());
    }
}

void test_ifce() {
    std::multimap<std::string, std::pair<hyn::Address::ptr, uint32_t>> res;
    bool v = hyn::Address::GetInterfaceAddresses(res);
    if (!v) {
        info("error");
        return;
    }
    for (auto &i: res) {
        info("%s - %s - %s", i.first.c_str(), i.second.first->toString().c_str(), i.second.second);
    }
}