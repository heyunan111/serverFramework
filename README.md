

## 线程

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