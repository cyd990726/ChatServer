#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include "user.hpp"
#include <vector>
#include "groupmodel.hpp"
#include "group.hpp"

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
    _msgHandlerMap.insert({MSGID::ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});
    _msgHandlerMap.insert({MSGID::CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({MSGID::ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({MSGID::GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});

    //连接redis服务器
    if(_redis.connect()){
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubcribeMessage, this, _1, _2));
    }

}

//登陆逻辑
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    //登陆业务代码
    int id = js["id"].get<int>();
    string pwd = js["password"];

    //查询用户信息
    User user = _repoProxy.queryUser(id);
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
            //向redis订阅channel
            _redis.subcribe(id);

            //更新状态信息
            user.setState("online");
            _repoProxy.updateUserState(user);
            reponse["msgid"] = LOG_MSG_ACK;
            reponse["errno"] = 0;
            reponse["id"] = user.getId();
            reponse["name"] = user.getName();

            //查询用户是否有离线消息
            vector<string>vec = _repoProxy.queryOfflineMsg(user.getId());
            if(!vec.empty()){
                reponse["offlinemsg"] = vec;//把整个列表存储如json中
                _repoProxy.removeOfflineMsg(id);
            }

            //查询用户的好友信息并返回，放在friends字段里面
            vector<User> userVec =  _repoProxy.queryFriends(id);
            if(!userVec.empty()){
                vector<string> vec2;
                for(auto &e : userVec){
                    json tmpJs;
                    tmpJs["id"] = e.getId();
                    tmpJs["name"] = e.getName();
                    tmpJs["state"] = e.getState();
                    vec2.push_back(tmpJs.dump());
                }
                reponse["friends"] = vec2;
            }

            //查询用户的群组信息并返回，放在groups字段里面
            vector<Group> groups = _repoProxy.queryGroups(id);
            vector<string> groupStrs;
            for(auto & group : groups){
                json js1;
                js1["id"] = group.getId();
                js1["groupname"] = group.getName();
                js1["groupdesc"] = group.getDesc();
                
                vector<string> userStrs;
                for(auto & user : group.getUsers()){
                    json js2;
                    js2["id"] = user.getId();
                    js2["name"] = user.getName();
                    js2["state"] = user.getState();
                    js2["role"] = user.getRole();
                    userStrs.push_back(js2.dump());
                }
                js1["users"] = userStrs;
                groupStrs.push_back(js1.dump());
            }
            reponse["groups"] = groupStrs;
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

    bool state = _repoProxy.insertUser(user);

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

//一对一聊天业务，注意，这里直接根据msgid将json转发或存储到数据库中即可
//因为这个json本来就是应该由客户端去解析的。
//我们服务端要做的只是根据对方是否在线进行一下转发。
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int toid = js["toid"].get<int>();
    //连在本机上，直接转发
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if(it != _userConnMap.end()){
            //在线,发送消息
            it->second->send(js.dump());
            return;
        }
    }
    //再看一下是否在线
    User user = _repoProxy.queryUser(toid);

    if(user.getId()!=-1 && user.getState() == "online"){
        _redis.publish(user.getId(), js.dump());
        return;
    }
    if(user.getId() != -1){
        //离线，存储离线消息
        _repoProxy.insertOfflineMsg(toid, js.dump());
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
    //取消订阅
    _redis.unsubcribe(user.getId());
    //更新状态信息
    if(user.getId() != -1){
        user.setState("offline");
        _repoProxy.updateUserState(user);
    }
}

void ChatService::reset(){
    _repoProxy.resetUserState();
}

//添加好友简单业务，可以根据自己需求调整
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    //存储好友信息
    _repoProxy.insertFriend(userid, friendid);

}


//创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    //存储新建的群组消息
    Group group(-1, name, desc);
    if(_repoProxy.createGroup(group)){
        //存储群组创建人信息
        _repoProxy.addGroup(userid, group.getId(), "creator");
    }

}
//加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    //存储群组成员信息
    _repoProxy.addGroup(userid, groupid, "normal");
}
//群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    //
    //查询群组的其他成员
    vector<int> useridVec = _repoProxy.queryGroupUsers(userid, groupid);
    
    lock_guard<mutex> lock(_connMutex);
    for(int id : useridVec){
        auto it = _userConnMap.find(id);
        if(it != _userConnMap.end()){
            it->second->send(js.dump());
        }else{
            //查看toid是否在线
            User user = _repoProxy.queryUser(id);
            if(user.getState() == "online"){
                _redis.publish(id, js.dump());
            }else{
                //存储离线群消息
                _repoProxy.insertOfflineMsg(id, js.dump());
            }
        }
    }
}

void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if(it != _userConnMap.end()){
            _userConnMap.erase(it);
        }
    }
    //用户注销，取消订阅通道
     _redis.unsubcribe(userid);
    //更新用户状态信息
    User user(userid, "", "", "offline");
    _repoProxy.updateUserState(user);

}


//从redis消息队列中获取订阅消息
//拿到userid和msg后发送给userid对应的客户端连接。
 void ChatService::handleRedisSubcribeMessage(int userid, string msg){
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if(it != _userConnMap.end()){
        it->second->send(msg);
        return;
    }
    //存储用户离线消息
    _repoProxy.insertOfflineMsg(userid, msg);
 }