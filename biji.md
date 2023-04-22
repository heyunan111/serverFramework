## 日志

---

## 配置

---

## 线程

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