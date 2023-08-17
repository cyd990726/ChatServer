#ifndef MYDB_H
#define MYDB_H
#include <string>
#include<mysql/mysql.h>

using namespace std;

static string server = "127.0.0.1";
static string user = "root";
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