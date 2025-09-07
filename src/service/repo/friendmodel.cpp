#include "friendmodel.hpp"
#include "mydb.hpp"


void FriendModel::insert(int userid, int friendid){
    char sql[1024] = {0};
    sprintf(sql, "insert into friend(userid, friendid) values(%d, %d)", userid, friendid);
    MY_SQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }
}
vector<User> FriendModel::query(int userid){
    vector<User> vec;
    char sql[1024] = {0};
    sprintf(sql, "select a.id AS ID, a.name, a.state from user a inner join friend b on b.friendid = a.id where b.userid = %d UNION DISTINCT select a.id AS ID, a.name, a.state from user a inner join friend b on b.userid = a.id where b.friendid = %d", userid, userid);
    MY_SQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row;
            User user;
            while((row = mysql_fetch_row(res)) != nullptr)
            {
                user.setId(atoi(row[0]));
        
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }   
            mysql_free_result(res);
            return vec; 
        }
    }
    return vec;
}

void FriendModel::insert(::google::protobuf::RpcController* controller,
                   const ::fixbug::InsertFriendRequest* request,
                   ::fixbug::InsertFriendResponse* response,
                   ::google::protobuf::Closure* done) {
    int userid = request->userid();
    int friendid = request->friendid();
    //执行逻辑代码
    insert(userid, friendid);
    //返回结果
    response->mutable_result()->set_errcode(0);
    response->mutable_result()->set_errmsg("success");
    done->Run();
}

void FriendModel::query(::google::protobuf::RpcController* controller,
                   const ::fixbug::queryFriendsRequest* request,
                   ::fixbug::queryFriendsResponse* response,
                   ::google::protobuf::Closure* done) {
    int userid = request->userid();
    response->mutable_result()->set_errcode(0);
    response->mutable_result()->set_errmsg("success");
    vector<User> vec = query(userid);
    
    for(User &user : vec){
        fixbug::User *p = response->add_friends();
        p->set_id(user.getId());
        p->set_name(user.getName());
        p->set_state(user.getState());
    }
    done->Run();
}