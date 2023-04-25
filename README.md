## 日志

### 1.等级

提供了`DEBUG` `INFO` `WARN` `ERROR` `FATAL`
五个等级可选择，并且可以设置筛选功能，即关闭某个等级以下的日志，比如调试完毕之后可以关闭`DEBUG` 和 `INFO` 。

### 2.输出

> 2023-04-24 23:41:16 INFO /home/hyn/tmp/tmp.sfTbGjgjyE/main.cpp:46 req:GET /blog/HTTP/1.1

输出了时间（精确到秒），等级，打印这条日志的文件以及行号，最后是日志内容。  
时间：使用标准库函数`time()`获得，用`localtime()`转换为本地时间，使用`strftime()`格式化。  
文件以及行号：使用宏`__FILE__`和` __LINE__`。

### 3."自动翻页“

可以配置日志文件最大文件大小，当log文件超过该大小之后会在原名之后加上最后一条日志的时间，并使用原名称创建一个新的日志文件继续打印日志  
例如：
> test_log.log.2023-04-25_16-27-08 //原名称：test_log.log

### 4.实现为单例模式

使用*Meyers' Singleton*实现线程安全的单例模式,即*Scott Myers在其著名的《Effective C++》一书中提出了一种经典的单例模式实现*
,在静态成员函数中返回一个静态局部变量的引用，因为静态局部变量的初始化只会在第一次调用时进行，因此可以保证只有一个实例被创建。同时，由于静态局部变量只会在函数内部可见，因此可以保证单例对象的访问控制.

### 5.简单的调用方式

将调用封装成c风格输出方式的宏，例如：
> error("thread join fail,rt = %d,name = %s", rt, m_name.c_str())

尽管对比c++风格流式输出存在可读性、类型安全、扩展性、面向对象方面的缺点，但是为了***性能和方便的精确控制输出格式***
， 性能是指避免了C++流式输出中频繁调用流操作符<<的开销，但是我不确定这点性能是否重要，毕竟打不了几个日志

---

## 配置

配置模块使用*ini*
文件进行配置，内置了简单的ini解析器，读取ini配置文件之后使用`std::map<std::string, std::map<std::string, Value>>`存储。   
值得注意的是`Value`类，该类用于表示不同类型的值，Value类提供了多个构造函数，分别用于从不同类型的值构造一个Value对象；还提供
了多个赋值运算符和类型转换函数，可以将Value对象转换为不同的基本类型或者字符串。
这种设计方式在实现一个配置文件读取、解析或者存储的功能时非常有用，可以方便地将不同类型的数据转换为统一的类型进行处理。

### 保证在程序启动时进行一些初始化操作

在之后的各个需要配置文件进行配置的地方，使用静态变量的初始化函数，这是线程安全的，在C++11标准中，规定了静态局部变量的初始化过程必须是线程
安全的。具体来说，当多个线程同时访问同一个静态局部变量时，编译器必须确保只有一个线程对该变量进行初始化操作。这通常是通过使用互斥锁来实现的，编译器会在静态局部变量的初始化代码周围插入一段互斥锁的代码，保证只有一个线程能够进入初始化代码区域

---

## 线程

因为在Linux平台下，C++11的多线程库通常使用POSIX线程库（pthread）实现，所以封装C++11线程库还不如直接封装pthread

### 1.Semaphore(信号量)

在多线程编程中，拷贝可能会导致资源的多次释放，从而导致程序崩溃或其他问题。因此，继承了`boost::noncopyable`

### 2.mutex(互斥锁)

### 3.Thread(线程)

---

## 协程

---

## 协程调度

---

## IO协程调度

IOManager相当于线程池


---

## Socket

---

## Hook

---

## 序列化

---

## HTTP/1.1

HttpRequest

> GET /HTTP/1.1  
> connection: close  
> host: www.qq.com  
> connection-length: 11  
> hello world

HttpResponse

> HTTP/1.1 400 Bad Request  
> X-X: qq  
> connection: keep-alive  
> content-length: 11  
> hello world

Ragel 实现HTTP协议解析  
Ragel:基于状态机的编译器，生成高效的、可预测的代码，特别适合用于解析和生成具有特定格式的文本
TcpServer封装

## Stream 文件/socket封装

read,readFixSize,write,writeFixSize

## HttpSession/HttpConnection

httpSession: Server端accept返回的socket  
httpConnection: Client端connect返回的socket