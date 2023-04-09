
#include "hyn_include.h"
#include "test/test_iomanager.h"

int main() {
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->open("/home/hyn/test.log");
    test();
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->close();
}
