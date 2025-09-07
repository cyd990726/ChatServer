#include "redis.hpp"
#include "muduo/base/Logging.h"
#include <thread>
#include <iostream>

using namespace std;

using namespace muduo;


Redis::Redis():_publish_context(nullptr), _subcribe_context(nullptr){

}
Redis::~Redis(){
    if(_publish_context != nullptr){
        redisFree(_publish_context);
    }
    if(_subcribe_context != nullptr){
        redisFree(_subcribe_context);
    }
}

bool Redis::connect(){
    _publish_context = redisConnect("chenyandon.xyz", 6379);
    if(nullptr == _publish_context){
        LOG_ERROR << "redis connect failed";
        return false;
    }
    _subcribe_context = redisConnect("chenyandon.xyz", 6379);
    if(nullptr == _subcribe_context){
        LOG_ERROR << "redis connect failed";
        return false;
    }

    //进行授权
    redisReply *auth = (redisReply *)redisCommand(_publish_context, "AUTH %s", "cyd19990726");
    if(auth->type == REDIS_REPLY_ERROR){
        LOG_ERROR << "redis Authentication failed";
    }
    freeReplyObject(auth);
    auth = (redisReply *)redisCommand(_subcribe_context, "AUTH %s", "cyd19990726");
    if(auth->type == REDIS_REPLY_ERROR){
        LOG_ERROR << "redis Authentication failed";
    }
    freeReplyObject(auth);

    //订阅上下文在单独的线程中监听通道上的事件,有消息给业务层上报
    thread t([&](){
        observer_channel_message();
    });
    t.detach();
    LOG_INFO << "connect redis sucess";
    return true;
}

bool Redis::publish(int channel, string message){
    //发送命令发布消息
    
    redisReply *reply = (redisReply *)redisCommand(_publish_context, "PUBLISH %d %s", channel, message.c_str());
    if(nullptr == reply){
        LOG_ERROR << "publish command failed";
        return false;
    }
    freeReplyObject(reply);
    return true;
}

bool Redis::subcribe(int channel){
    //因为subcribe会阻塞，所以这里只订阅通道，不接收通道消息
    //注意：redisCommand=redisAppCommand + redisBufferWrite+redisGetReply
    //redisAppCommand：把命令存储到本地缓存
    //redisBufferWrite：把命令发送到redis
    //redisGetReply：获取命令的响应消息
    if(REDIS_ERR == redisAppendCommand(this->_subcribe_context, "SUBSCRIBE %d", channel)){
        LOG_ERROR << "subscribe command failed";
        return false;
    }

    int done = 0;
    while (!done)
    {
        if(REDIS_ERR == redisBufferWrite(this->_subcribe_context, &done)){
            LOG_ERROR << "subscribe command failed";
            return false;
        }
    }
    return true;
}

bool Redis::unsubcribe(int channel){
    if(REDIS_ERR == redisAppendCommand(this->_subcribe_context, "UNSUBSCRIBE %d", channel)){
        LOG_ERROR << "unsubscribe command failed";
        return false;
    }

    int done = 0;
    while (!done)
    {
        if(REDIS_ERR == redisBufferWrite(this->_subcribe_context, &done)){
            LOG_ERROR << "unsubscribe command failed";
            return false;
        }
    }
    return true;
}
void Redis::observer_channel_message(){
    redisReply *reply = nullptr;
    while(REDIS_OK == redisGetReply(this->_subcribe_context, (void **)&reply)){
        if(reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr){
            //给业务层上报消息
            _notify_message_handler(atoi(reply->element[1]->str), reply->element[2]->str);
        }
        freeReplyObject(reply);
    }
    LOG_ERROR <<">>>>>>>>>>>>> observer_channel_message quit <<<<<<<<<<<<<";
}

void Redis::init_notify_handler(function<void(int, string)> fn){
    this->_notify_message_handler = fn;
}