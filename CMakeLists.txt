cmake_minimum_required(VERSION 3.20)
project(serverFramework)
set(CMAKE_CXX_STANDARD 20)

include_directories(/usr/include/boost)
link_directories(/usr/lib64)

add_executable(serverFramework main.cpp include/Logger.h include/exceptdef.h include/singleton.h src/Logger.cpp include/config.h test/Config_test.h include/macro.h include/util.h src/util.cpp test/util_test.h include/fiber.h include/thread.h src/thread.cpp test/thread_test.h include/hyn_include.h src/mutex.cpp include/mutex.h)

target_link_libraries(serverFramework ${SOURCE_FILES})