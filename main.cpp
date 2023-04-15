#include <iostream>
#include "hyn.h"
#include "test/test_Socket.h"

using namespace std;

int main() {
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->open("/home/hyn/test.log");

    test_sososs();

    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->close();
}