#include "chatserver.hpp"

int main(){
    EventLoop loop;
    InetAddress addr(StringArg("127.0.0.1"), 1234);
    ChatServer server(&loop, addr, "CYD");
    server.start(); //启动服务。将listenfd通过epoll_ctl添加到上面
    loop.loop();//epoll_wait以阻塞的方式等待新用户的连接，已连接的用户的读写事件
    //loop.quit(); 这个是

    return 0;
}