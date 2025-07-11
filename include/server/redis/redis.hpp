#ifndef REDIS_HPP_
#define REDIS_HPP_

#include <hiredis/hiredis.h>
#include <thread>
#include <functional>
using namespace std;

class Redis
{
public:
    Redis();
    ~Redis();

    // 连接Redis服务器
    bool Connect();

    // 向Redis指定通道channel发送消息
    bool Publish(int channel, string message);

    // 向redis指定的通道subscribe订阅消息
    bool Subscribe(int channel);

    // 取消订阅
    bool UnSubscribe(int channel);

    // 在独立线程中接收订阅通道的消息，避免阻塞主线程
    void ObserverChannelMessage();

    // 初始化业务层上报消息（通知相应服务器消息）的回调函数
    void InitNotifyHandler(function<void(int, string)> handler);

private:
    // hiredis同步上下文对象，也就是一个连接对象，负责publish消息
    redisContext *publish_context_;
    // hiredis同步上下文对象，也就是一个连接对象，负责subscribe消息
    redisContext *subscribe_context_;

    // 回调函数
    function<void(int, string)> notify_message_handler_;
};

#endif // REDIS_HPP_