# PublicComponent
平常写代码过程中需要用到的一些公共代码组件
使用方法:
1 make mkdir
2 make
3 ./bin/***


介绍：
Code
1.RingBuf           环形缓冲区代码实现以及使用方法
2.Cache             类redis内存缓冲区
3.Util
    3.1 url_util    url编码 解码类
    3.2 hex2byte    16进制数据转换为字节流（可用于pb文件处理）
4.RabbitmqClient    Rabbitmq客户端代码（机器需要安装rabbitmq-c库 github:https://github.com/alanxz/rabbitmq-c）
5.DirMonitor        Linux目录变化监控
6.QueueThreadSafe	读写队列(线程安全)
	6.1 1			pthread_*相关函数实现


DB：
1.Redis             Redis数据库(无集群)交互代码实现以及使用方法


Linux：
1.StartShell        CentOS6/7下的开机自启脚本

