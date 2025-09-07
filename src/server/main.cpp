#include "chatserver.hpp"
#include "chatservice.hpp"
#include "signal.h"
#include "mprpcapplication.h"

void resetHandler(int sig){
    //重置用户状态
    ChatService::instance()->reset();

    exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
    if(argc <2){
        cout<<"param error!ep:./ChatServer 6000 \n";
        return 0;
    }
    //调用rpc框架初始化函数
    MprpcApplication::Init(argc, argv);
    
    //注册信号处理
    signal(SIGINT, &resetHandler);//自己ctrl+c
    signal(SIGABRT, &resetHandler);//客户端发过来的数据不合法会abort掉进程

    EventLoop loop;
    InetAddress addr(StringArg("127.0.0.1"), atoi(argv[1]));
    ChatServer server(&loop, addr, "CYD");
    server.start(); //启动服务。将listenfd通过epoll_ctl添加到上面
    loop.loop();//epoll_wait以阻塞的方式等待新用户的连接，已连接的用户的读写事件
    //loop.quit(); 这个是

    return 0;
}