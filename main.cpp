#include <iostream>
#include "hyn.h"


using namespace std;

int main() {
    Config f;
    error("test");
    int a = hyn::singleton::Singleton<hyn::ini::IniFile>::get_instance()->get("TCP", "timeout");
    auto b = static_cast<uint64_t>(a);
    cout << a << '\n' << b << '\n';
    int c = hyn::singleton::Singleton<hyn::ini::IniFile>::get_instance()->get("HTTP", "request_max_body_size");
    cout << c;
    int d = 64 * 1024 * 1024;
    cout << '\n' << d;
}