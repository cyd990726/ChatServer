#include "repo_proxy.h"

RepoProxy::RepoProxy()
{
    _channelPtr = new MprpcChannel();
    _controllerPtr = new MprpcController();
    _userModelStubPtr = new fixbug::user_model_Stub(_channelPtr);
    _friendModelStubPtr = new fixbug::friend_model_Stub(_channelPtr);
    _groupModelStubPtr = new fixbug::group_model_Stub(_channelPtr);
    _offlineMsgModelStubPtr = new fixbug::offline_message_model_Stub(_channelPtr);
}

RepoProxy::~RepoProxy()
{
    delete _channelPtr;
    delete _controllerPtr;
    delete _userModelStubPtr;
    delete _friendModelStubPtr;
    delete _groupModelStubPtr;
    delete _offlineMsgModelStubPtr;
}

bool RepoProxy::insertUser(User &user){
    //填充request
    fixbug::InsertUserRequest request;
    request.mutable_user()->set_id(user.getId());
    request.mutable_user()->set_name(user.getName());
    request.mutable_user()->set_password(user.getPassword());
    request.mutable_user()->set_state(user.getState());

    //定义接受response
    fixbug::InsertUserResponse response;
    //调用rpc方法
    _userModelStubPtr->insert(_controllerPtr, &request, &response, nullptr);
    if (_controllerPtr->Failed())
    {
        std::cout << _controllerPtr->ErrorText() << std::endl;
        return false;
    }
    return response.result().errcode() == 0;
}

User RepoProxy::queryUser(int id){
    //填充request
    fixbug::queryUserRequest request;
    request.set_userid(id);
    //定义接受response
    fixbug::queryUserResponse response;
    //调用rpc方法
    _userModelStubPtr->query(_controllerPtr, &request, &response, nullptr);

    User user;
    if (_controllerPtr->Failed())
    {
        std::cout << _controllerPtr->ErrorText() << std::endl;
        return user;
    }
    if (response.result().errcode() == 0)
    {
        user.setId(response.user().id());
        user.setName(response.user().name());
        user.setPassword(response.user().password());
        user.setState(response.user().state());
    }
    return user;
}
bool RepoProxy::updateUserState(User &user){
    //填充request
    fixbug::UpdateStateRequest request;
    request.mutable_user()->set_id(user.getId());
    request.mutable_user()->set_state(user.getState());
    request.mutable_user()->set_name(user.getName());
    request.mutable_user()->set_password(user.getPassword());
    //定义接受response
    fixbug::UpdateStateResponse response;
    //调用rpc方法
    _userModelStubPtr->updateState(_controllerPtr, &request, &response, nullptr);
    if (_controllerPtr->Failed())
    {
        std::cout << _controllerPtr->ErrorText() << std::endl;
        return false;
    }
    return response.result().errcode() == 0;
}

void RepoProxy::resetUserState(){
    //填充request
    fixbug::ResetStateRequest request;
    request.set_userid(rand()%100); //这里随便填个id，用不到
    //定义接受response
    fixbug::ResetStateResponse response;
    //调用rpc方法
    _userModelStubPtr->resetState(_controllerPtr, &request, &response, nullptr);
    if (_controllerPtr->Failed())
    {
        std::cout << _controllerPtr->ErrorText() << std::endl;
        return;
    }
    if (response.result().errcode() == 0)
    {
        std::cout << "reset user state success" << std::endl;
    }
    else
    {
        std::cout << "reset user state failed" << std::endl;
    }
}


void RepoProxy::insertFriend(int userid, int friendid){
    //填充request
    fixbug::InsertFriendRequest request;
    request.set_userid(userid);
    request.set_friendid(friendid);
    //定义接受response
    fixbug::InsertFriendResponse response;
    //调用rpc方法
    _friendModelStubPtr->insert(_controllerPtr, &request, &response, nullptr);
    if (_controllerPtr->Failed())
    {
        std::cout << _controllerPtr->ErrorText() << std::endl;
        return;
    }
    if (response.result().errcode() == 0)
    {
        std::cout << "insert friend success" << std::endl;
    }
    else
    {
        std::cout << "insert friend failed" << std::endl;
    }
}
std::vector<User> RepoProxy::queryFriends(int userid){
    //填充request
    fixbug::queryFriendsRequest request;
    request.set_userid(userid);
    //定义接受response
    fixbug::queryFriendsResponse response;
    //调用rpc方法
    _friendModelStubPtr->query(_controllerPtr, &request, &response, nullptr);
    if (_controllerPtr->Failed())
    {
        std::cout << _controllerPtr->ErrorText() << std::endl;
        return {};
    }
    std::vector<User> friends;
    for (const auto &frd : response.friends())
    {
        User user;
        user.setId(frd.id());
        user.setName(frd.name());
        user.setPassword(frd.password());
        user.setState(frd.state());
        friends.push_back(user);
    }
    return friends;
}


bool RepoProxy::createGroup(Group &group){
    //填充request
    
    fixbug::CreateGroupRequest request;
    request.mutable_group()->set_id(group.getId());
    request.mutable_group()->set_name(group.getName());
    request.mutable_group()->set_desc(group.getDesc());
    for (auto &groupuser : group.getUsers())
    {
        request.mutable_group()->add_users()->mutable_user()->set_id(groupuser.getId());
        request.mutable_group()->add_users()->mutable_user()->set_name(groupuser.getName());
        request.mutable_group()->add_users()->mutable_user()->set_password(groupuser.getPassword());
        request.mutable_group()->add_users()->mutable_user()->set_state(groupuser.getState());
        request.mutable_group()->add_users()->set_role(groupuser.getRole().c_str());
    }   
   
    //定义接受response
    fixbug::CreateGroupResponse response;
    //调用rpc方法
    _groupModelStubPtr->createGroup(_controllerPtr, &request, &response, nullptr);
    if (_controllerPtr->Failed())
    {
        std::cout << _controllerPtr->ErrorText() << std::endl;
        return false;
    }
    return response.result().errcode() == 0;
}
void RepoProxy::addGroup(int userid, int groupid, std::string role){
    //填充request
    fixbug::AddGroupRequest request;
    request.set_userid(userid);
    request.set_groupid(groupid);
    request.set_role(role);
    //定义接受response
    fixbug::AddGroupResponse response;
    //调用rpc方法
    _groupModelStubPtr->addGroup(_controllerPtr, &request, &response, nullptr);
    if (_controllerPtr->Failed())
    {
        std::cout << _controllerPtr->ErrorText() << std::endl;
        return;
    }
    if (response.result().errcode() == 0)
    {
        std::cout << "add group success" << std::endl;
    }
    else
    {
        std::cout << "add group failed" << std::endl;
    }
}
std::vector<Group> RepoProxy::queryGroups(int userid){
    //填充request
    fixbug::queryGroupsRequest request;
    request.set_userid(userid);
    //定义接受response
    fixbug::queryGroupsResponse response;
    //调用rpc方法
    _groupModelStubPtr->queryGroups(_controllerPtr, &request, &response, nullptr);
    if (_controllerPtr->Failed())
    {
        std::cout << _controllerPtr->ErrorText() << std::endl;
        return {};
    }
    std::vector<Group> groups;
    for (const auto &group : response.groups())
    {
        Group grp;
        grp.setId(group.id());
        grp.setName(group.name());
        grp.setDesc(group.desc());
        groups.push_back(grp);
    }
    return groups;
}
std::vector<int> RepoProxy::queryGroupUsers(int userid, int groupid){
    //填充request
    fixbug::queryGroupUsersRequest request;
    request.set_userid(userid);
    request.set_groupid(groupid);
    //定义接受response
    fixbug::queryGroupUsersResponse response;
    //调用rpc方法
    _groupModelStubPtr->queryGroupUsers(_controllerPtr, &request, &response, nullptr);
    if (_controllerPtr->Failed())
    {
        std::cout << _controllerPtr->ErrorText() << std::endl;
        return {};
    }
    std::vector<int> userids;
    for (const auto &userid : response.userids())
    {
        userids.push_back(userid);
    }
    return userids;
}

void RepoProxy::insertOfflineMsg(int userid, std::string msg){
    //填充request
    fixbug::InsertOfflineMessageRequest request;
    request.set_userid(userid);
    request.set_msg(msg);
    //定义接受response
    fixbug::InsertOfflineMessageResponse response;
    //调用rpc方法
    _offlineMsgModelStubPtr->insert(_controllerPtr, &request, &response, nullptr);
    if (_controllerPtr->Failed())
    {
        std::cout << _controllerPtr->ErrorText() << std::endl;
        return;
    }
    if (response.result().errcode() == 0)
    {
        std::cout << "insert offline msg success" << std::endl;
    }
    else
    {
        std::cout << "insert offline msg failed" << std::endl;
    }
}
void RepoProxy::removeOfflineMsg(int userid){
    //填充request
    fixbug::RemoveOfflineMessageRequest request;
    request.set_userid(userid);
    //定义接受response
    fixbug::RemoveOfflineMessageResponse response;
    //调用rpc方法
    _offlineMsgModelStubPtr->remove(_controllerPtr, &request, &response, nullptr);
    if (_controllerPtr->Failed())
    {
        std::cout << _controllerPtr->ErrorText() << std::endl;
        return;
    }
    if (response.result().errcode() == 0)
    {
        std::cout << "remove offline msg success" << std::endl;
    }
    else
    {
        std::cout << "remove offline msg failed" << std::endl;
    }
}
std::vector<std::string> RepoProxy::queryOfflineMsg(int userid){
    //填充request
    fixbug::QueryOfflineMessageRequest request;
    request.set_userid(userid);
    //定义接受response
    fixbug::QueryOfflineMessageResponse response;
    //调用rpc方法
    _offlineMsgModelStubPtr->query(_controllerPtr, &request, &response, nullptr);
    if (_controllerPtr->Failed())
    {
        std::cout << _controllerPtr->ErrorText() << std::endl;
        return {};
    }
    std::vector<std::string> msgs;
    for (const auto &msg : response.msgs())
    {
        msgs.push_back(msg);
    }
    return msgs;
}
