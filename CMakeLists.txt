cmake_minimum_required(VERSION 3.20)
project(serverFramework)
set(CMAKE_CXX_STANDARD 20)


find_package(Boost REQUIRED COMPONENTS program_options)

include_directories(${Boost_INCLUDE_DIRS})


add_executable(serverFramework main.cpp src/_Singleton.h src/Address.cpp src/Address.h src/endian.h src/exceptdef.h
        src/FDManger.cpp src/FDManger.h src/fiber.cpp src/fiber.h src/Hook.cpp src/Hook.h src/iniFile.cpp src/iniFile.h
        src/IOManager.cpp src/IOManager.h src/Logger.h src/Logger.cpp src/mutex.cpp src/mutex.h src/Scheduler.cpp
        src/Scheduler.h src/singleton.h src/Socket.cpp src/Socket.h src/thread.h src/thread.cpp src/Timer.cpp src/Timer.h
        src/util.h src/util.cpp src/Bytearray.cpp src/ByteArray.h src/Http.cpp src/Http.h src/http11_common.h
        src/http11_parser.h src/httpclient_parser.h src/http11_parser.cpp src/httpclient_parser.cpp src/HttpParser.cpp
        src/HttpParser.h src/TcpServer.cpp src/TcpServer.h src/Stream.cpp src/Stream.h src/SocketStream.cpp src/SocketStream.h
        src/HttpSession.cpp src/HttpSession.h src/HttpServer.cpp src/HttpServer.h src/Servlet.cpp src/Servlet.h
        src/HttpConnection.cpp src/HttpConnection.h

        test/test_fiber.h test/test_hook.h test/test_iomanager.h test/test_scheduler.h test/thread_test.h test/util_test.h
        test/test_address.h test/test_Socket.h test/test_bytesArray.h test/test_http.h test/test_parser.h test/test_tcpserver.h
        test/test_http_server.h test/test_http_connection.h

        examples/echo_server.h

        main.cpp src/URI.h)

target_link_libraries(serverFramework ${SOURCE_FILES})


