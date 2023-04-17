#include <iostream>
#include "hyn.h"
#include "test/test_bytesArray.h"

using namespace std;

int main() {
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->open("/home/hyn/test.log");

    test_bytearrya();

    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->close();
}