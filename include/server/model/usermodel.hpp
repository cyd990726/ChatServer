#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"
/**User表的数据操作类*/
class UserModel{

public:
    bool insert(User &user);
    User query(int id);
    bool updateState(User &user);
    void resetState();
private:

};

#endif