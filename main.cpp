#include <iostream>
#include <format>
int main() {
    std::string name = "Tom";
    int age = 20;
    double height = 1.75;

    // 使用 format 函数进行格式化输出
    std::string str = std::format("My name is {}, I'm {} years old, and my height is {:.2f} meters.", name, age,
                                  height);
    std::cout << str << std::endl;

    return 0;
}
