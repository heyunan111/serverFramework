**项目名称：serverFramework**

[![GitHub](https://img.shields.io/badge/GitHub-serverFramework-green)](https://github.com/heyunan111/serverFramework.git)

**项目简介**

serverFramework是一个高性能的Linux协程网络库，旨在为开发者提供可靠、高效的网络编程解决方案。该项目的开发时间为2023年3月至2023年5月。

**特点**

- **日志与配置**：实现了单例日志，支持日志级别、过滤和滚动功能。

- **线程与协程**：封装pthread，提供便捷的API和智能锁。基于ucontext_t实现协程，支持N-M协程调度器，使得多协程可以在多线程中运行。

- **IO协程调度**：基于epoll的封装，支持Socket事件的管理，同时支持定时器的添加、删除和取消。

- **Hook技术**：使用hook技术实现异步IO，提高性能。特别针对socket、socket IO和sleep系列API进行hook优化。

- **Socket**：统一封装Socket API功能，支持IPV4和IPV6地址以及域名解析。

- **序列化与反序列化**：采用与protobuf相同的序列化格式，进行高效二进制序列化。

- **TcpServer**：简化服务器开发的封装，支持监听、处理客户端连接、数据的收发和处理。

- **HTTP**：使用ragel实现超高性能的HTTP解析，支持HTTP 1.1/1.0。

**快速开始**

1. 克隆项目：`git clone https://github.com/heyunan111/serverFramework.git`
2. 进入项目目录：`cd serverFramework`
3. 编译项目：`makdir build && cd build; cmake .. ;make`


**问题与反馈**

如果您在使用本项目时遇到任何问题，请在 [Issues](https://github.com/heyunan111/serverFramework/issues) 页面提交问题报告，我们将积极解决。

**许可证**

本项目采用 [Apache License 2.0](./LICENSE) 进行授权。

**鸣谢**

感谢所有为该项目作出贡献的开发者和开源社区，您们的支持和贡献是项目成功的关键。

---