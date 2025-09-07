#ifndef GROUPUSER_H
#define GROUPUSER_H


#include "user.hpp"

//groupuser表
class GroupUser: public User{
public:
    
    void setRole(string role){
        this->_role = role;
    }
    string getRole(){
        return this->_role;
    }


private:
    //群聊中的身份
    string _role;

};

#endif