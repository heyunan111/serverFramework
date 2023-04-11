
#include "hyn_include.h"
#include "test/test_hook.h"

int main() {
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->open("/home/hyn/test.log");
    test_sleep();
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->close();
}
