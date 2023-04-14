#include <iostream>
#include "hyn.h"
#include "test/test_address.h"

using namespace std;

int main() {
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->open("/home/hyn/test.log");

    test_ipv4();

    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->close();
}