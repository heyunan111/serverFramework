#include <iostream>
#include "hyn.h"
#include "examples/echo_server.h"

using namespace std;

int main() {
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->open("/home/hyn/test_log.log");
    test("-t");
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->close();
}