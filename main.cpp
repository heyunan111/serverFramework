#include <iostream>
#include "hyn.h"
#include "test/test_parser.h"

using namespace std;

int main() {
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->open("/home/hyn/test.log");
    test_request_parser();
    test_response_parser();
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->close();
}