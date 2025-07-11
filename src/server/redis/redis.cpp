#include "redis.hpp"
#include<iostream>
using namespace std;

Redis::Redis():subscribe_context_(nullptr),publish_context_(nullptr)
{

}

Redis::~Redis()
{
    if(subscribe_context_!=nullptr)
    {
        //释放RedisContext对象
        redisFree(subscribe_context_);
    }
    if(publish_context_!=nullptr)
    {
        redisFree(publish_context_);
    }
}


// 连接Redis服务器
bool Redis::Connect()
{
    //负责上下文对象的连接
    publish_context_=redisConnect("127.0.0.1",6379);
    if(publish_context_==nullptr)
    {
        cerr<<"Redis connect false!";
        return false;
    }
    subscribe_context_=redisConnect("127.0.0.1",6379);
    if(subscribe_context_==nullptr)
    {
        cerr<<"Redis connect false!";
        return false;
    }

    //创建一个新线程用来接收subscribe的消息，因为subscribe是阻塞等待消息传来的
    //但是设计成不阻塞的，直接执行，然后在这个线程中接收subscibe中的消息，就可以使主线程不阻塞
    thread get_subscibe_message([&](){
        ObserverChannelMessage();
    });
    //设计为线程分离，避免主线程阻塞
    get_subscibe_message.detach();

    cout<<"Redis connect true!"<<endl;
    return true;

}

// 向Redis指定通道channel发送消息
bool Redis::Publish(int channel, string message)
{
    //执行publish发布命令
    //执行redisCommand的三个步骤，第一个是缓存到本地redisAppendCommand
    //第二个将命令发送到redis上，redisBufferWrite
    //第三个则是接收reply，以阻塞的形式，redisGetReply
    redisReply* reply=(redisReply*)redisCommand(publish_context_,"PUBLISH %d %s",channel,message.c_str());
    if(reply==nullptr)
    {
        cerr<<"publish is false!";
        return false;
    }
    //释放资源
    freeReplyObject(reply);
    return true;
}

// 向redis指定的通道subscribe订阅消息
bool Redis::Subscribe(int channel)
{
    //为了实现不阻塞的状态，即只订阅通道，所以只执行缓存到本地，发送到redis上
    //接收阻塞的消息则只在单独线程ObserverChannelMessage函数中
    if(REDIS_ERR==redisAppendCommand(subscribe_context_,"SUBSCRIBE %d",channel))
    {
        cerr<<"subscribe is false!";
        return false;
    }
    //redisBufferWrite可以循环发送至缓冲区，直到缓冲区发送完毕（done被置为1）
    int done=0;
    while(!done)
    {
        if(redisBufferWrite(subscribe_context_,&done)==REDIS_ERR)
        {
            cerr<<"subscribe is false!";
            return false;
        }
    }
    return true;
}

// 取消订阅
bool Redis::UnSubscribe(int channel)
{
    //理论上如果取消了所有通道的订阅后，不会阻塞线程，但是如果只取消了部分通道的订阅，仍然会阻塞线程
    //因此只执行缓存到本地，发送到redis上
     if(REDIS_ERR==redisAppendCommand(subscribe_context_,"UNSUBSCRIBE %d",channel))
    {
        cerr<<"unsubscribe is false!";
        return false;
    }
    //redisBufferWrite可以循环发送至缓冲区，直到缓冲区发送完毕（done被置为1）
    int done=0;
    while(!done)
    {
        if(redisBufferWrite(subscribe_context_,&done)==REDIS_ERR)
        {
            cerr<<"unsubscribe is false!";
            return false;
        }
    }
    return true;

}

// 在独立线程中接收订阅通道的消息，避免阻塞主线程
void Redis::ObserverChannelMessage()
{
    redisReply* reply=nullptr;
    //循环等待subscribe订阅的消息
    while(redisGetReply(subscribe_context_,(void **)&reply)==REDIS_OK)
    {
        //如果订阅消息不为空且为三个元素的数组，则表示这是正常的订阅消息
        if(reply!=nullptr&&reply->element[2]!=nullptr&&reply->element[2]->str!=nullptr)
        {
            notify_message_handler_(atoi(reply->element[1]->str),reply->element[2]->str);
        } 
        freeReplyObject(reply);
    }
}

// 初始化业务层上报消息（通知相应服务器消息）的回调函数
void Redis::InitNotifyHandler(function<void(int, string)> handler)
{
    notify_message_handler_=handler;
}