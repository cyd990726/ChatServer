#ifndef GROUP_H
#define GROUP_H
#include "groupuser.hpp"
#include <string>
#include <vector>

using namespace std;
//allgroup表，存储群聊信息。
class Group{
public:
    Group(int id = -1, string name = "", string desc = ""){
        this->_id = id;
        this->_name = name;
        this->_desc = desc;
    }

    void setId(int id){
        this->_id = id;
    }
    void setName(string name){
        this->_name = name;
    }
    void setDesc(string desc){
        this->_desc = desc;
    }
    int getId() const{
        return this->_id;
    }
    string getName() const{
        return this->_name;
    }
    string getDesc()const{
        return this->_desc;
    }

    //以引用的方式返回users
    vector<GroupUser> &getUsers(){
        return _users;
    }
private:

    int _id;
    //群聊名字
    string _name;
    //群描述信息
    string _desc; 
    //群里的人
    vector<GroupUser> _users;
};


#endif