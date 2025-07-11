#ifndef CHAT_SERVER_HPP_
#define CHAT_SERVER_HPP_

#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"
using namespace std;
using namespace muduo;
using namespace muduo::net;

class ChatServer
{
public:
    // 初始化服务器和事件循环
    ChatServer(EventLoop *loop,
               const InetAddress &listen_addr,
               const string &name);
    // 启动服务器
    void Start();

private:
    // 回调函数，处理客户端连接断开
    void OnConnection(const TcpConnectionPtr &tcp_connect);
    // 回调函数，处理已连接用户的信息
    void OnMessage(const TcpConnectionPtr &tcp_connect,
                   Buffer *buff,
                   Timestamp time);

private:
    TcpServer chat_server_;
    EventLoop *loop_;
};

#endif // CHAT_SERVER_HPP_