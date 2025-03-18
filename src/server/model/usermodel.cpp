#include "user.hpp"
#include "usermodel.hpp"
#include "mydb.hpp"
#include <muduo/base/Logging.h>
#include <iostream>
#include <sstream>
using namespace std;
using namespace muduo;

//将组装好的用户对象插入数据库
bool UserModel::insert(User &user){
    char sqlBuf[1024] = {0};
    //组装sql
    sprintf(sqlBuf, "insert into user(name, password, state) values('%s', '%s', '%s')", user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());
    MY_SQL mysql;
    if(mysql.connect()){
        if(mysql.update(sqlBuf)){
            //获取插入成功的用户数据生成的主键
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    
    return false;

}

//根据id查询数据库并将其封装成对象返回
User UserModel::query(int id){
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = %d", id);
    MY_SQL mysql;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        //从res中提取数据
        if(res!=nullptr){
            //查到了
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row!=nullptr){
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPassword(row[2]);
                user.setState(row[3]);
                return user;
            }
        }
    }
    return User();//注意，User默认id值是-1。表示没找到。
    
}

bool UserModel::updateState(User &user){
    char sql[1024] = {0};
    sprintf(sql, "update user set state = '%s' where id = %d", user.getState().c_str(), user.getId());
    MY_SQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            return true;
        }  
    }
    return false;
}

void UserModel::resetState(){
    char sql[1024] = {0};
    sprintf(sql, "update user set state = 'offline' where state = 'online'");
    MY_SQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }
}   