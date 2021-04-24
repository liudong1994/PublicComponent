## PublicComponent

平常写代码过程中需要用到的一些公共代码组件

使用方法:

```shell
1 make mkdir
2 make
3 ./bin/***
```





## 介绍：

### 一、Code

#### 1.RingBuf

环形缓冲区代码实现以及使用方法



#### 2.Cache

类redis内存缓冲区



#### 3.Util

​    3.1 url_util    url编码 解码类

​    3.2 hex2byte    16进制数据转换为字节流（可用于pb文件处理）



#### 4.RabbitmqClient

Rabbitmq客户端代码（机器需要安装rabbitmq-c库 github:https://github.com/alanxz/rabbitmq-c）



#### 5.DirMonitor        Linux目录变化监控

​    5.1 DM-nonblock     内部启动线程，文件通知统一接收，另一线程统一处理

​    5.2 DM-block        内部无线程，文件通知处理后再接收下一个文件通知



#### 6.QueueThreadSafe

读写队列(线程安全)

​	6.1 queue_1         pthread_*相关函数实现

​    6.2 queue_2         C++11实现



#### 7 ListThreadSafe

读写列表（线程安全）



#### 8 AsyncTask

异步任务处理（内部实现依赖 7 ListThreadSafe）



#### 9 ConsistentHash

一致性哈希简单实现



#### 10 TcpSend

简单的tcp发送接收工具，可模拟tcp及基于tcp的应用层协议（如HTTP）



#### 11 TimerTask

函数模板定时器





### 二、DB：

#### 1.Redis

Redis数据库(无集群)交互代码实现以及使用方法





### 三、Linux：

#### 1.StartShell

CentOS6/7下的开机自启脚本安装go



