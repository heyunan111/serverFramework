#include "test/test_http_server.h"

int main() {
  hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->open(
      "/home/hyn/test_log.log");
  test();
  hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->close();
  return 0;
}