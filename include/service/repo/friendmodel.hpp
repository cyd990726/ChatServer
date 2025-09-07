#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H
#include <string>
#include <vector>
#include "user.hpp"
#include "friend_repo_service.pb.h"
//维护好友信息的操作接口方法

class FriendModel : public fixbug::friend_model{
public:
    public:
    void insert(::google::protobuf::RpcController* controller,
                       const ::fixbug::InsertFriendRequest* request,
                       ::fixbug::InsertFriendResponse* response,
                       ::google::protobuf::Closure* done) override;
    void query(::google::protobuf::RpcController* controller,
                          const ::fixbug::queryFriendsRequest* request,
                          ::fixbug::queryFriendsResponse* response,
                          ::google::protobuf::Closure* done) override;

private:
    //添加好友关系
    void insert(int userid, int friendid);
    vector<User> query(int userid);
};



#endif