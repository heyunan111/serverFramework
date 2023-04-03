cmake_minimum_required(VERSION 3.20)
project(serverFramework)
set(CMAKE_CXX_STANDARD 20)

include_directories(/usr/include/boost)
link_directories(/usr/lib64)

add_executable(serverFramework main.cpp include/Logger.h include/exceptdef.h include/singleton.h src/Logger.cpp
        include/util.h src/util.cpp test/util_test.h include/fiber.h include/thread.h src/thread.cpp test/thread_test.h
        hyn_include.h src/mutex.cpp include/mutex.h src/fiber.cpp include/iniFile.h src/iniFile.cpp
        test/test_fiber.h src/Scheduler.cpp include/Scheduler.h test/test_scheduler.h)

target_link_libraries(serverFramework ${SOURCE_FILES})