/**
 * server和client的公共文件
*/

#ifndef PUBLIC_H
#define PUBLIC_H
#include <iostream>
using namespace std;
enum MSGID{
    LOG_MSG = 0,//登陆消息
    LOG_MSG_ACK,//登陆响应消息
    REG_MSG,//注册消息
    REG_MSG_ACK,//注册响应消息

    ONE_CHAT_MSG//聊天消息
};


#endif