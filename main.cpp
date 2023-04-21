#include <iostream>
#include "hyn.h"


using namespace std;

int main() {
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->open("/home/hyn/test_log.log");

    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->close();
}