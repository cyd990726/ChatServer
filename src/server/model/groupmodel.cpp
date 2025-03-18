#include "groupmodel.hpp"
#include "mydb.hpp"
#include <string.h>

//创建群组
bool GroupModel::createGroup(Group &group)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into allgroup(groupname, groupdesc) values('%s', '%s')", group.getName().c_str(), group.getDesc().c_str());

    MY_SQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            //获取插入数据的id并设置在group中
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}
//加入群组
void GroupModel::addGroup(int userid, int groupid, string role){
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser values(%d, %d, '%s')", groupid, userid,role.c_str());

    MY_SQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }
}
//查询用户所在群组信息
vector<Group> GroupModel::querGroups(int userid){
    char sql[1024] = {0};
    //allgroup和groupuser联合查询
    sprintf(sql, "select a.id, a.groupname, a.groupdesc from allgroup a inner join groupuser b on a.id = b.groupid where b.userid = %d", userid);

    vector<Group> groupVec;
    MY_SQL mysql;
    
    //查询用户所在的群的信息，放在groupVec中
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr){
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupVec.push_back(group);
            }
            mysql_free_result(res);
        }
    }
    //根据groupVec中存储的群组的信息，查询各个群组的用户信息
    for(Group &group : groupVec){
        memset(sql, 0x00, sizeof(sql));
        sprintf(sql, "select a.id, a.name, a.state, b.grouprole from user a inner join groupuser b on b.userid = a.id where b.groupid = %d", group.getId());
        MYSQL_RES *res = mysql.query(sql);
        if(res!=nullptr){
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                GroupUser groupUser;
                groupUser.setId(atoi(row[0]));
                groupUser.setName(row[1]);
                groupUser.setState(row[2]);
                groupUser.setPassword(row[3]);
                group.getUsers().push_back(groupUser);
            }
            mysql_free_result(res);
        }
    }
    return groupVec;

}
//根据指定的groupid查询群组用户id列表，除userid自己，主要用户群聊业务给群组其他成员发消息
vector<int> GroupModel::queryGroupUSers(int userid, int groupid){
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where groupid = %d and userid != %d", groupid, userid);
    MY_SQL mysql;
    vector<int> vec;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr){
            //查询成功，加入列表返回
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                vec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return vec;
}
