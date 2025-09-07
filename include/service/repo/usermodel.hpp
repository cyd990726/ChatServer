#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"
#include "user_repo_service.pb.h"
/**User表的数据操作类*/
class UserModel: public fixbug::user_model{

public:
    void insert(::google::protobuf::RpcController* controller,
                       const ::fixbug::InsertUserRequest* request,
                       ::fixbug::InsertUserResponse* response,
                       ::google::protobuf::Closure* done) override;
    void query(::google::protobuf::RpcController* controller,
                       const ::fixbug::queryUserRequest* request,
                       ::fixbug::queryUserResponse* response,
                       ::google::protobuf::Closure* done) override;
    void updateState(::google::protobuf::RpcController* controller,
                       const ::fixbug::UpdateStateRequest* request,
                       ::fixbug::UpdateStateResponse* response,
                       ::google::protobuf::Closure* done) override;
    void resetState(::google::protobuf::RpcController* controller,
                       const ::fixbug::ResetStateRequest* request,
                       ::fixbug::ResetStateResponse* response,
                       ::google::protobuf::Closure* done) override;
private:
    bool insert(User &user);
    User query(int id);
    bool updateState(User &user);
    void resetState();

};

#endif