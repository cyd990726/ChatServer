#include "chatserver.hpp"
#include "chatservice.hpp"
#include "json.hpp"
#include <iostream>

using namespace muduo;
using namespace muduo::net;
using namespace std;
using nlohmann::json;


ChatServer::ChatServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg)
      :_loop(loop),_server(loop, listenAddr, nameArg)      
{
     //给服务器注册用户连接的创建和断开回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, std::placeholders::_1));
    //给服务器注册用户读写时间回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    //设置线程数量
    _server.setThreadNum(4);
}

void ChatServer::start(){
    _server.start();
}

//专门处理用户的连接和断开的回调函数
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    if(!conn->connected()){
        ChatService::instance()->clientCloseException(conn);
        //说明用户断开了连接。我们需要shutdown这个连接,回收资源
        conn->shutdown(); //等于close(fd)
        
    }
}
//专门处理用户的读写事件
void ChatServer::onMessage(const TcpConnectionPtr& conn,//连接
                Buffer* buffer,//缓冲区
                Timestamp time)//接收到数据的时间信息
{
    string buf = buffer->retrieveAllAsString();
    //数据的反序列化
    json js = json::parse(buf);
    //下面就是我们要使用的服务，要实现服务模块与网络模块的解耦合
    /**
     * 思路：在消息里面存一个字段msgid，保存这条消息所需要的服务
     * 通过js["msgid"]来获取事先绑定好的服务msghandler
     * 直接传入约定好的参数(const TcpConnectionPtr& conn,json &js, Timestamp time)，
     * 就可以获取到相应的服务啦。
     * 这样我们就不需要再网络模块里面写业务代码。
     * 完全实现解耦合！
     * */
    auto handler = ChatService::instance()->getHandler(js["msgid"].get<MSGID>());
    //执行回调
    handler(conn, js, time);
    
}