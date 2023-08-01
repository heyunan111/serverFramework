**Project Name: serverFramework**

[![GitHub](https://img.shields.io/badge/GitHub-serverFramework-green)](https://github.com/heyunan111/serverFramework.git)

中文版链接：[中文README](./Chinese.md)

**Project Overview**

serverFramework is a high-performance Linux coroutine networking library designed to provide developers with a reliable and efficient networking solution. The development period for this project was from March 2023 to May 2023.

**Features**

- **Logging and Configuration**: Implements a singleton logging mechanism with support for log levels, filtering, and rolling functionality.

- **Threads and Coroutines**: Encapsulates pthread, providing convenient APIs and intelligent locks. It uses ucontext_t to implement coroutines, supporting N-M coroutine schedulers, allowing multiple coroutines to run within multiple threads.

- **IO Coroutine Scheduling**: Uses epoll-based encapsulation for managing Socket events and supports adding, removing, and canceling timers.

- **Hook Technology**: Utilizes hook technology to achieve asynchronous IO, enhancing performance. Specifically optimized for socket, socket IO, and sleep series APIs.

- **Socket**: Provides unified encapsulation of Socket API functionalities, supporting both IPV4 and IPV6 addresses, as well as domain name resolution.

- **Serialization and Deserialization**: Employs a serialization format similar to protobuf for efficient binary serialization.

- **TcpServer**: Simplifies server development by providing a wrapper supporting listening, handling client connections, data communication, and processing.

- **HTTP**: Utilizes ragel to achieve high-performance HTTP parsing, supporting HTTP 1.1/1.0.

**Getting Started**

1. Clone the project: `git clone https://github.com/heyunan111/serverFramework.git`
2. Navigate to the project directory: `cd serverFramework`
3. Build the project: `makdir build && cd build; cmake .. ;make`


**Issues and Feedback**

If you encounter any problems while using the project, please submit an issue report on the [Issues](https://github.com/heyunan111/serverFramework/issues) page, and we will actively address it.

**License**

This project is licensed under the [Apache License 2.0](./LICENSE).

**Acknowledgments**

We extend our heartfelt appreciation to all the developers and open-source communities who have contributed to this project. Your support and contributions have been key to the success of this project.