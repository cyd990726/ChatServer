#ifndef MYDB_H
#define MYDB_H
#include <string>
#include<mysql/mysql.h>

//将数据库的基本操作方法封装成对象。方便使用。


using namespace std;

static string server = "chenyandon.xyz";
static string user = "cyd";
static string password = "Cyd19990726";
static string dbname = "chat";

class MY_SQL{

public:
    MY_SQL();
    ~MY_SQL();
    bool connect();
    bool update(string sql);
    MYSQL_RES *query(string sql);
    MYSQL *getConnection();

private:

    MYSQL *_conn;

};




#endif