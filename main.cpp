#include <iostream>
#include <sys/socket.h>
#include "test/test_scheduler.h"
#include "test/test_fiber.h"

int main() {
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->open("/home/hyn/test.log");
    //test_fiber();
    test_sch();
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->close();
}
