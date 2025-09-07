#include "mydb.hpp"
#include <muduo/base/Logging.h>

using namespace muduo;


MY_SQL::MY_SQL(){
    _conn = mysql_init(nullptr);
}
MY_SQL::~MY_SQL(){
    if(_conn!=nullptr){
        mysql_close(_conn);
    }
}
bool MY_SQL::connect(){
    MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(), password.c_str(), dbname.c_str(), 3306, nullptr, 0);
    if(p!=nullptr){
        mysql_query(_conn, "set names gbk");
    }
    return p;
}
bool MY_SQL::update(string sql){
    if(mysql_query(_conn, sql.c_str())){
        LOG_INFO << __FILE__ << ":" <<__LINE__ <<":" << sql << "更新失败!";
        return false;
    }
    return true;
}

MYSQL_RES * MY_SQL::query(string sql){
    if(mysql_query(_conn,sql.c_str())){
        LOG_INFO<<__FILE__<<":"<<__LINE__<<":"<<sql<<"查询失败!";
        return nullptr;
    }
    return mysql_use_result(_conn);
}

MYSQL *MY_SQL::getConnection(){
    return this->_conn;
}