
#include "hyn.h"
#include "test/test_http.h"

using namespace std;

int main() {
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->open("/home/hyn/test.log");

    //test_req();
    test_resp();
    hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->close();
}