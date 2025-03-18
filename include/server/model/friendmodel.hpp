#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H
#include <string>
#include <vector>
#include "user.hpp"
//维护好友信息的操作接口方法

class FriendModel{
public:
    //添加好友关系
    void insert(int userid, int friendid);
    vector<User> query(int userid);

private:


};



#endif