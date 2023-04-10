cmake_minimum_required(VERSION 3.20)
project(serverFramework)
set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS program_options)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(serverFramework main.cpp include/Logger.h include/exceptdef.h include/singleton.h src/Logger.cpp
        include/util.h src/util.cpp test/util_test.h include/fiber.h include/thread.h src/thread.cpp test/thread_test.h
        hyn_include.h src/mutex.cpp include/mutex.h src/fiber.cpp include/iniFile.h src/iniFile.cpp
        test/test_fiber.h src/Scheduler.cpp include/Scheduler.h test/test_scheduler.h src/IOManager.cpp include/IOManager.h include/learn/Lfiber.h test/test_iomanager.h include/Timer.h src/Timer.cpp src/Hook.cpp include/Hook.h test/test_hook.h)

target_link_libraries(serverFramework ${SOURCE_FILES})
