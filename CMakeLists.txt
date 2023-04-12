cmake_minimum_required(VERSION 3.20)
project(serverFramework)
set(CMAKE_CXX_STANDARD 20)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(/usr/include/openssl)

add_executable(serverFramework main.cpp src/Logger.h src/exceptdef.h src/singleton.h src/Logger.cpp
        src/util.h src/util.cpp test/util_test.h src/fiber.h src/thread.h src/thread.cpp test/thread_test.h
        hyn.h src/mutex.cpp src/mutex.h src/fiber.cpp src/iniFile.h src/iniFile.cpp
        test/test_fiber.h src/Scheduler.cpp src/Scheduler.h test/test_scheduler.h src/IOManager.cpp src/IOManager.h include/learn/Lfiber.h test/test_iomanager.h src/Timer.h src/Timer.cpp src/Hook.cpp src/Hook.h test/test_hook.h src/FDManger.cpp src/FDManger.h src/_Singleton.h src/Socket.cpp src/Socket.h src/Address.cpp src/Address.h)

target_link_libraries(serverFramework ${SOURCE_FILES})
target_link_libraries(serverFramework PRIVATE OpenSSL::SSL OpenSSL::Crypto)

