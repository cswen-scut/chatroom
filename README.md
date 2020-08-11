# 基于C++和epoll实现的聊天室

- 客户端涉及到的技术点
   - 常用QT控件(QWidget, QListWidget, QLabel, QPushButton)
   - QT信号与槽
   - QJsonObject完成json数据的解析
   - QT多线程
   - QTcpSocket连接服务器
   
- 服务端涉及到的技术点
   - epoll多路IO转接机制
   - 常用STL(vector, map)
   - 文件读写(fstream)
   - jsoncpp解析json数据
   - MySQL基本操作
   
- 实现的功能
   - 注册
   - 单点登录
   - 登出
   - 群聊(支持文本和图片的传送)
   - 上线下线公告
   - 在线用户记录

- 客户端使用方式：
  - ./client/source/文件夹是客户端源码，若安装了QT，则可打开chatroom.pro，若在QT中构建项目，请在构建
  的文件夹下新建一个image文件夹用于保存图片，另外新建一个config文件夹，再在config文件夹中新建一个server.conf用于配置服务端IP和
  端口，server.conf的格式按照./client/source/server.conf填写
  - ./client/exec是编译后的可执行文件以及依赖的库，请先修改./client/exec/config/server.conf，
  然后将chatroom.exe所在的文件夹路径加入环境变量，再直接运行chatroom.exe
  
- 服务端使用方式
  - 1.安装jsoncpp库, 详情请见https://github.com/open-source-parsers/jsoncpp
    - 安装完后将./vcpkg/installed/x64-linux/include/json拷贝到/usr/local/include/
    - 将./vcpkg/installed/x64-linux/lib/libjsoncpp.a拷贝到/usr/local/include
    - 在/etc/profile里追加
        - export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
        - export LIBRARY_PATH=$LIBRARY_PATH:/usr/local/lib
    - 最后source /etc/profile
  - 2.安装MySQL连接库
    - ubuntu使用命令apt-get install libmysqlclient-dev
    - centos使用命令yum install mysql-devel -y
  - 3.利用./sql/user.sql在MySQL中创建一张user表
  - 4.进入./server/config目录，将服务器的IP和端口等配置信息换成适合自己的
  - 5.进入./server/, 然后make即可
  - 6.运行步骤5生成的可执行文件
 
- 通信协议
  - 开始1B表示这个数据包是登录请求、发送请求、登出请求等(详情请见./server/ProtocolHead/protocolmsg.h)
  - 接下来2B表示用户的账号
  - 接下来1B表示数据包的数据格式，文本或图片
  - 接下来4B表示数据的大小
  - 最后就是真实数据了
  
- 遇到的问题及解决方案
  - 服务端端口被占用问题。服务端强制关闭后TCP连接进入TIME_WAIT状态，此状态持续2MSL(大概40多秒)，由于端口被占用，若此时若再次启动服务端，会失败
    - 通过setsockopt函数实现端口复用
  - 数据包"粘包"问题。TCP是流式协议，所传输的数据没有明确的界线，需要用户自己区分。
    - 在接收数据时，先解析协议头部，根据头部数据大小字段来决定接收多少数据，然后循环接收数据，直到接收完该数据大小的包后再接收下一个包
  - QT readyRead信号丢失问题，若服务端发送给客户端的数据较大，且发送速度较快，客户端可能来不及接收完本次readyRead信号的bytesAvailable大小的数据，此时服务端却在不停地发，可能会造成客户端readyRead信号的丢失，从而导致接收数据不全
    - 使用确认机制，即客户端每读完一个readyRead信号所携带的数据后，给服务端发送一个确认包，告诉服务端自己已经接收了多少数据，然后服务端再接着发下一部分的数据，直到接收完数据
  - 客户端发送数据与接收数据的冲突问题。若客户端与服务端仅建立一个连接，那么当客户端在接收数据时，若客户端此时向服务端发送聊天数据，服务端本应接收客户端的确认包，但是却接收了客户端的聊天数据，从而造成确认信息异常
    - 客户端与服务端建立两个连接，一个用于写数据，一个用于读数据
  - 客户端强制退出问题。客户端在接收数据时，强制退出了，服务端由于发给客户端的数据没有收到确认，在read确认包时进入了阻塞状态。
    - 每次向客户端发送数据时，先用getsockopt获取客户端的连接状态，若客户端的连接状态不是ESTABLISHED，则直接结束发送，并取消监听客户端的fd
   
- 总结
   - 本项目的主要目的是熟悉C++和Linux网络编程，目前还有很多不完善之处，例如服务端仅采用单线程实现，这样效率会很低，后续考虑引入线程池。

