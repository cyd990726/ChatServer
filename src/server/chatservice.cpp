#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include "user.hpp"

using namespace muduo;
    
ChatService* ChatService::instance()
{
    
    static ChatService service;
    return &service;

}

//注册消息以及对应的回调操作
ChatService::ChatService(){
    _msgHandlerMap.insert({MSGID::LOG_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({MSGID::REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({MSGID::ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});

}

//登陆逻辑
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    //注册业务代码
    int id = js["id"].get<int>();
    string pwd = js["password"];

    User user = _userModel.query(id);
    json reponse;
    if(user.getId() == id && user.getPassword() == pwd){
        if(user.getState()=="online"){
            //用户已经登陆，不允许重复登陆
            reponse["msgid"] = LOG_MSG_ACK;
            reponse["errno"] = 2;
            reponse["errmsg"] = "该用户已经登陆，请重新输入账号!";
        }else{
            
            {
                lock_guard<mutex> lock(_connMutex);//上锁

                //登陆成功,记录下来。
                _userConnMap.insert({id, conn});
            }

            //更新状态信息
            user.setState("online");
            _userModel.updateState(user);
            reponse["msgid"] = LOG_MSG_ACK;
            reponse["errno"] = 0;
            reponse["id"] = user.getId();
            reponse["name"] = user.getName();
        }
    }else{
        //失败
        reponse["msgid"] = LOG_MSG_ACK;
        reponse["errno"] = 1;
        reponse["errmsg"] = "用户名或密码错误!";
    }
    //将信息回复给客户端
    conn->send(reponse.dump());

}
//注册逻辑
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    //注册业务代码
    string name = js["name"];
    string pwd = js["password"];

    //封装成对象
    User user;
    user.setName(name);
    user.setPassword(pwd);

    bool state = _userModel.insert(user);

    json reponse;
    if(state){
        //注册成功
        reponse["msgid"] = REG_MSG_ACK;
        reponse["errno"] = 0;
        reponse["id"] = user.getId();
    }else{
        //注册失败
        reponse["msgid"] = REG_MSG_ACK;
        reponse["errno"] = 1;
    }
    conn->send(reponse.dump());

    
}

//一对一聊天业务
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int toid = js["to"].get<int>();
    auto it = _userConnMap.find(toid);
    if(it == _userConnMap.end()){
        //不在线,存储离线消息
        
    }else{
        //在线，将消息转发给用户
        it->second->send(js.dump());
    }
}

//获取消息对应的事件处理器
MsgHandler ChatService::getHandler(MSGID msgid){
    auto it = _msgHandlerMap.find(msgid);
    if(it == _msgHandlerMap.end()){
        //为了防止出bug，这里给它返回一个空的处理器，只打印一行错误日志
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time){
            LOG_ERROR << "msgid:" << msgid << "can not find handler!";
        };
    }
    return it->second;
}

void ChatService::clientCloseException(const TcpConnectionPtr &conn){
    User user;
    {
        lock_guard<mutex> lock(_connMutex);//上锁
        for(auto it = _userConnMap.begin(); it!=_userConnMap.end(); it++){
            if(it->second == conn){
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }
    //更新状态信息
    if(user.getId() != -1){
        user.setState("offline");
        _userModel.updateState(user);
    }
}