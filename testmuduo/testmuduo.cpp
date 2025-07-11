#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"
#include <iostream>
#include<string>
#include <functional>

/*
基于muduo网络库开发服务器程序
1.组合TcpServer对象，内置muduo：：net：：tcpserver
2.创建EventLoop时间循环对象的指针，类似于epoll的作用
3.明确TcpSever的构造函数
4.在当前服务器类的构造函数中，处理连接和断开连接，用户读写事件的回调函数，绑定在tcpserver对象上
4.设置合适的服务器端线程数量，muduo库会自己分配IO线程和work线程数

*/


class ChatServer
{

public:
    ChatServer(muduo::net::EventLoop *loop,
               const muduo::net::InetAddress &listenAddr,
               const std::string &nameArg)
        : tcp_server_(loop, listenAddr, nameArg),
          event_loop_(loop)
    {
        // 绑定新用户连接和断开连接事件，回调函数
        // setConnectCallback即为muduo库中处理连接和断开的函数,当底层监听到相关操作就会回调OnConnection函数
        tcp_server_.setConnectionCallback(std::bind(&ChatServer::OnConnection, this, std::placeholders::_1));
        // 绑定用户进行文件读写事件，回调函数
        tcp_server_.setMessageCallback(std::bind(&ChatServer::OnMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
        tcp_server_.setThreadNum(4);//设置线程数，1个IO，3个work线程
    }

    void Start(){
        tcp_server_.start();
    }//启动服务器

private:
    muduo::net::TcpServer tcp_server_;  // 服务器监听
    muduo::net::EventLoop *event_loop_; // 事件循环，处理各种事件，绑定在server上
    // 上述均为muduo库初始化必备参数

    void OnConnection(const muduo::net::TcpConnectionPtr &conn) // 连接和断开连接的回调
    // 参数为内置参数
    {
        if(conn->connected()){
            std::cout<<conn->peerAddress().toIpPort()<<"->"<<conn->localAddress().toIpPort()<<"  online"<<std::endl;
        }
        else
        {
            std::cout<<conn->peerAddress().toIpPort()<<"->"<<conn->localAddress().toIpPort()<<"  offonline"<<std::endl;
            conn->shutdown();//类似free(conn)，释放这个连接
            //event_loop_->quit();//断开事件循环，关闭整个服务器
        }
    }

    void OnMessage(const muduo::net::TcpConnectionPtr &conn,//客户端连接
                   muduo::net::Buffer *buff,//缓冲区
                   muduo::Timestamp time)//接收时间
    {
        std::string buf=buff->retrieveAllAsString();
        std::cout<<"data:"<<buf<<"time:"<<time.toString()<<std::endl;
    }
};

int main(){
    muduo::net::EventLoop loop;
    muduo::net::InetAddress adress("127.0.0.1",6000);
    ChatServer server(&loop,adress,"ChatServer");
    server.Start();//开启服务器
    loop.loop();//开启事件循环，类似epoll_wait阻塞住，开始监听客户端连接
    return 0;
}