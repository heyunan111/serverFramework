cmake_minimum_required(VERSION 3.20)
project(serverFramework)
set(CMAKE_CXX_STANDARD 20)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(/usr/include/openssl)

add_executable(serverFramework main.cpp src/_Singleton.h src/Address.cpp src/Address.h src/endian.h src/exceptdef.h
        src/FDManger.cpp src/FDManger.h src/fiber.cpp src/fiber.h src/Hook.cpp src/Hook.h src/iniFile.cpp src/iniFile.h
        src/IOManager.cpp src/IOManager.h src/Logger.h src/Logger.cpp src/mutex.cpp src/mutex.h src/Scheduler.cpp
        src/Scheduler.h src/singleton.h src/Socket.cpp src/Socket.h src/thread.h src/thread.cpp src/Timer.cpp src/Timer.h
        src/util.h src/util.cpp

        test/test_fiber.h test/test_hook.h test/test_iomanager.h test/test_scheduler.h test/thread_test.h test/util_test.h
        test/test_address.h test/test_Socket.h

        hyn.h main.cpp src/Bytearray.cpp src/ByteArray.h)

target_link_libraries(serverFramework ${SOURCE_FILES})
target_link_libraries(serverFramework PRIVATE OpenSSL::SSL OpenSSL::Crypto)

