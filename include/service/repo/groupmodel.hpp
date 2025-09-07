#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.hpp"
#include "group_repo_service.pb.h"

//维护群组信息的操作接口方法,操作allgroup和groupuser这两张表
class GroupModel : public fixbug::group_model{
public:
    //创建群组
    void createGroup(::google::protobuf::RpcController* controller,
                       const ::fixbug::CreateGroupRequest* request,
                       ::fixbug::CreateGroupResponse* response,
                       ::google::protobuf::Closure* done) override;
    //加入群组
    void addGroup(::google::protobuf::RpcController* controller,
                       const ::fixbug::AddGroupRequest* request,
                       ::fixbug::AddGroupResponse* response,
                       ::google::protobuf::Closure* done) override;
    //查询用户所在群组信息
    void queryGroups(::google::protobuf::RpcController* controller,
                       const ::fixbug::queryGroupsRequest* request,
                       ::fixbug::queryGroupsResponse* response,
                       ::google::protobuf::Closure* done) override;
    //根据指定的groupid查询群组用户id列表，除userid自己，主要用户群聊业务给群组其他成员发消息
    void queryGroupUsers(::google::protobuf::RpcController* controller,
                       const ::fixbug::queryGroupUsersRequest* request,
                       ::fixbug::queryGroupUsersResponse* response,
                       ::google::protobuf::Closure* done) override;
private:
    //创建群组
    bool createGroup(Group &group);
    //加入群组
    void addGroup(int userid, int groupid, string role);
    //查询用户所在群组信息
    vector<Group> queryGroups(int userid);
    //根据指定的groupid查询群组用户id列表，除userid自己，主要用户群聊业务给群组其他成员发消息
    vector<int> queryGroupUSers(int userid, int groupid);

};





#endif