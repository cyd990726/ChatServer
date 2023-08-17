#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

using namespace muduo;
using namespace muduo::net;

class ChatServer{
public:
    //初始化服务器对象
    ChatServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg);
        
    void start();

private:
    //专门处理用户的连接和断开的回调函数
    void onConnection(const TcpConnectionPtr &conn);

    //专门处理用户的读写事件的回调函数
    void onMessage(const TcpConnectionPtr& conn,//连接
                    Buffer* buffer,//缓冲区
                    Timestamp time);//接收到数据的时间信息
    TcpServer _server;
    EventLoop *_loop;

};

#endif