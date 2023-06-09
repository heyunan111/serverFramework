## 代码量统计

```
     163 text files.
     145 unique files.
      56 files ignored.

github.com/AlDanial/cloc v 1.96  T=0.44 s (328.0 files/s, 106384.2 lines/s)
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C++                             28           1548            582          21579
D                               26              0              0           9542
C/C++ Header                    46           1088           2382           2507
JSON                             7              0              0           2483
C                                2            277            123           1269
XML                             12              0              0           1017
make                             1            196            123            537
YAML                             1             28              4            510
CMake                           11            111             29            453
Text                             5             78              0            361
Markdown                         4             68              0            119
INI                              1              0              0              9
TypeScript                       1              0              0              2
-------------------------------------------------------------------------------
SUM:                           145           3394           3243          40388
-------------------------------------------------------------------------------
```

---  

## 简单介绍

### 日志与配置

一个简单的单例日志实现，支持日志级别，日志过滤，日志滚动等功能，但未能考虑性能优化，后续可以改进

> 2023-05-14 16:18:44 ERROR /mnt/c/Users/hyn/CLionProjects/serverFramework/main.cpp:28 test log

配置默认采用ini格式，同时提供了简单的XML，JSON解析器，可以根据需求修改配置文件格式

### 线程与协程

通过对pthread进行简单的封装，提供较为便捷的API，如：使用RAII技术实现了与C++11线程库提供的 `std::unique_lock()`
`std::lock_guard()`功能相同的智能锁。  
使用ucontext_t封装实现协程，使用线程池实现N-M协程调度器以支持多个协程在多个线程中运行。

### IO协程调度

继承于协程调度器与时间管理器，封装了epoll，支持Scoket读写时间的添加删除取消，支持一次性定时器，循环定时器

### Hook

使用hook技术hook socket，socket io，sleep系列的API，以实现异步，提高性能

### Socket

封装socket类，提供socket API功能，统一封装IPV4，TPV6地址，提供了域名，IP地址解析功能

### 序列化与反序列化

通过二进制序列化支持int8_t,int16_t,int32_t等基础类型，以及varint和std::string，支持对文件内容的序列化与反序列化。

### TcpServer

封装了一个简单的TcpServer，实现基础功能，可以简化服务器开发

### HTTP

使用ragel通过有限状态机生成HTTP解析器，实现超高性能http的解析，仅支持1.1/1.0

---

## 项目优化  


