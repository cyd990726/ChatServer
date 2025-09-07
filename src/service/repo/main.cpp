#include "user_repo_service.pb.h"
#include "group_repo_service.pb.h"
#include "offline_message_repo_service.pb.h"
#include "friend_repo_service.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "friendmodel.hpp"
#include "groupmodel.hpp"
#include "offlinemessagemodel.hpp"
#include "usermodel.hpp"

int main(int argc, char** argv){
    MprpcApplication::Init(argc, argv);
    // provider是一个rpc网络服务对象。把UserService对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new FriendModel());
    provider.NotifyService(new GroupModel());
    provider.NotifyService(new OfflineMsgModel());
    provider.NotifyService(new UserModel());

    // 启动一个rpc服务发布节点   Run以后，进程进入阻塞状态，等待远程的rpc调用请求
    provider.Run();

    return 0;
}