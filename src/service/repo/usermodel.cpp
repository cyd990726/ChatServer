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


void UserModel::insert(::google::protobuf::RpcController* controller,
                   const ::fixbug::InsertUserRequest* request,
                   ::fixbug::InsertUserResponse* response,
                   ::google::protobuf::Closure* done) {
    //获取请求数据
    User user;
    user.setId(request->user().id());
    user.setName(request->user().name());
    user.setPassword(request->user().password());
    user.setState(request->user().state());
    //插入用户
    bool ret = insert(user);
    if(ret){
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("success");
    }
    else{
        response->mutable_result()->set_errcode(1);
        response->mutable_result()->set_errmsg("failed");
    }
    //把数据返回给调用方               
    done->Run();
}

void UserModel::query(::google::protobuf::RpcController* controller,
                   const ::fixbug::queryUserRequest* request,
                   ::fixbug::queryUserResponse* response,
                   ::google::protobuf::Closure* done) {
    //获取请求数据
    int id = request->userid();
    //查询用户
    User user = query(id);
    if(user.getId() != -1){
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("success");
        response->mutable_user()->set_id(user.getId());
        response->mutable_user()->set_name(user.getName());
        response->mutable_user()->set_password(user.getPassword());
        response->mutable_user()->set_state(user.getState());
    }
    else{
        response->mutable_result()->set_errcode(1);
        response->mutable_result()->set_errmsg("failed");
    }
    //把数据返回给调用方               
    done->Run();
}

void UserModel::updateState(::google::protobuf::RpcController* controller,
                   const ::fixbug::UpdateStateRequest* request,
                   ::fixbug::UpdateStateResponse* response,
                   ::google::protobuf::Closure* done) {
    //获取请求数据
    User user;
    user.setId(request->user().id());
    user.setState(request->user().state());
    //更新状态
    bool ret = updateState(user);
    if(ret){
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("success");
    }
    else{
        response->mutable_result()->set_errcode(1);
        response->mutable_result()->set_errmsg("failed");
    }
    //把数据返回给调用方               
    done->Run();
}