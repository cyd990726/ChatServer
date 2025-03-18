#include "offlinemessagemodel.hpp"
#include "mydb.hpp"

void OfflineMsgModel::insert(int userid, string msg)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage values('%d', '%s')", userid, msg.c_str());
    MY_SQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }
}

void OfflineMsgModel::remove(int userid)
{
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid = %d", userid);

    MY_SQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }
}

vector<string> OfflineMsgModel::query(int userid)
{
    vector<string> vec;
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid = %d", userid);
    MY_SQL mysql;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);//res指向一块堆内存，需要mysql_free_result来释放。
        MYSQL_ROW row;
        if(res!=nullptr){
            //把结果按行提取
            while((row = mysql_fetch_row(res))!=nullptr){
                vec.push_back(row[0]);
            }
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;

}




