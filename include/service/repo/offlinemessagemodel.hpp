#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H

#include <string>
#include <vector>
#include "offline_message_repo_service.pb.h"

using namespace std;

class OfflineMsgModel : public fixbug::offline_message_model{
public:
    void insert(::google::protobuf::RpcController* controller,
                       const ::fixbug::InsertOfflineMessageRequest* request,
                       ::fixbug::InsertOfflineMessageResponse* response,
                       ::google::protobuf::Closure* done) override;
    void remove(::google::protobuf::RpcController* controller,
                       const ::fixbug::RemoveOfflineMessageRequest* request,
                       ::fixbug::RemoveOfflineMessageResponse* response,
                       ::google::protobuf::Closure* done) override;
    void query(::google::protobuf::RpcController* controller,
                       const ::fixbug::QueryOfflineMessageRequest* request,
                       ::fixbug::QueryOfflineMessageResponse* response,
                       ::google::protobuf::Closure* done) override;

private:
    void insert(int userid, string msg);

    void remove(int userid);

    vector<string> query(int userid);

};


#endif