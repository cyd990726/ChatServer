#pragma once

#include "mprpcchannel.h"
#include "user_repo_service.pb.h"
#include "friend_repo_service.pb.h"
#include "group_repo_service.pb.h"
#include "offline_message_repo_service.pb.h"
#include "user.hpp"
#include "group.hpp"
#include "mprpccontroller.h"

class RepoProxy
{
private:
    // 创建一个channel
    MprpcChannel * _channelPtr;
    //创建controler
    MprpcController * _controllerPtr;
    // 创建stub
    fixbug::user_model_Stub * _userModelStubPtr;
    fixbug::friend_model_Stub * _friendModelStubPtr;
    fixbug::group_model_Stub * _groupModelStubPtr;
    fixbug::offline_message_model_Stub * _offlineMsgModelStubPtr;

public:
    RepoProxy();
    ~RepoProxy();

    //userModel
    bool insertUser(User &user);
    User queryUser(int id);
    bool updateUserState(User &user);
    void resetUserState();
    //friendModel
    void insertFriend(int userid, int friendid);
    std::vector<User> queryFriends(int userid);
    //groupModel
    bool createGroup(Group &group);
    void addGroup(int userid, int groupid, std::string role);
    std::vector<Group> queryGroups(int userid);
    std::vector<int> queryGroupUsers(int userid, int groupid);
    //offlineMsgModel
    void insertOfflineMsg(int userid, std::string msg);
    void removeOfflineMsg(int userid);
    std::vector<std::string> queryOfflineMsg(int userid);


};