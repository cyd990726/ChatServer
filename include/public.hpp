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

    ONE_CHAT_MSG,//一对一聊天消息
    ADD_FRIEND_MSG,//添加好友消息

    CREATE_GROUP_MSG, //创建群组
    ADD_GROUP_MSG, //加入群组
    GROUP_CHAT_MSG,//群聊天

    LOGOUT_MSG
};


#endif
