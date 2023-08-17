#ifndef CHATSERVICE_H
#define CHATSERVICE_H
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <muduo/net/TcpConnection.h>
#include <muduo/base/Timestamp.h>
#include "public.hpp"
#include "json.hpp"
#include "usermodel.hpp"

#include <mutex>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using nlohmann::json;

using MsgHandler = function<void(const TcpConnectionPtr &conn, json &js, Timestamp time)>;

//聊天服务器业务类

class ChatService
{
public:
    //单例模式
    static ChatService* instance();

    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    MsgHandler getHandler(MSGID msgid);

    void clientCloseException(const TcpConnectionPtr &conn);

private:
    ChatService();//阻止构造
    unordered_map<MSGID, MsgHandler> _msgHandlerMap;
    UserModel _userModel;

    //因为是一个聊天服务器集群，
    //所以服务器要分发消息给各个客户端，故要记录在线的客户端的连接，以便于发送
    //记录客户端id和连接的映射
    unordered_map<int, TcpConnectionPtr> _userConnMap;

    //定义一把互斥锁，保证线程安全
    mutex _connMutex;
    
};



#endif