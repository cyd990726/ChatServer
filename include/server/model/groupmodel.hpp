#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.hpp"

//维护群组信息的操作接口方法,操作allgroup和groupuser这两张表
class GroupModel{
public:
    //创建群组
    bool createGroup(Group &group);
    //加入群组
    void addGroup(int userid, int groupid, string role);
    //查询用户所在群组信息
    vector<Group> querGroups(int userid);
    //根据指定的groupid查询群组用户id列表，除userid自己，主要用户群聊业务给群组其他成员发消息
    vector<int> queryGroupUSers(int userid, int groupid);
    
private:

};





#endif