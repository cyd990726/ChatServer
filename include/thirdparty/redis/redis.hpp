#ifndef REDIS_H
#define REDIS_H

#include <string>
#include <hiredis/hiredis.h>
#include <functional>
using namespace std;

class Redis{
public:
    Redis();
    ~Redis();
    
    bool connect();

    bool publish(int channel, string message);

    bool subcribe(int channel);
    bool unsubcribe(int channel);

    void observer_channel_message();

    void init_notify_handler(function<void(int, string)> fn);

private:
    //hiredis同步上下文对象，负责publish消息
    redisContext *_publish_context;

    //hiredis同步上下文对象，负责subcribe消息。
    redisContext *_subcribe_context;

    //回调操作，收到订阅消息，给service层上报
    function<void(int, string)> _notify_message_handler;


};

#endif