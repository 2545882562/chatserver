#include "chat_server.hpp"
#include"chat_service.hpp"
#include "functional"
#include"json.hpp"
#include"string"
using namespace placeholders;
using json=nlohmann::json;
using namespace std;

ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listen_addr,
                       const string &name) : chat_server_(loop, listen_addr, name),
                                             loop_(loop)
{
    // 设置连接回调
    chat_server_.setConnectionCallback(std::bind(&ChatServer::OnConnection, this, _1));
    // 设置消息回调
    chat_server_.setMessageCallback(std::bind(&ChatServer::OnMessage, this, _1, _2, _3));
    // 设置线程数量，3个工作线程一个IO线程,工作线程处理已连接用户发送消息，IO线程处理新用户连接
    chat_server_.setThreadNum(4);
}

void ChatServer::Start()
{
    chat_server_.start();
}

// 回调函数，处理客户端连接断开
void ChatServer::OnConnection(const TcpConnectionPtr &tcp_connect)
{
    //客户端断开连接则释放这个连接
    if(!tcp_connect->connected())
    {
        //处理异常退出客户端
        ChatService::GetInstance().ClientCloseException(tcp_connect);
        tcp_connect->shutdown();
    }
}
// 回调函数，处理已连接用户的信息,这个是多线程调用的
void ChatServer::OnMessage(const TcpConnectionPtr &tcp_connect,
                           Buffer *buff,
                           Timestamp time)
{
    string buf=buff->retrieveAllAsString();
    //从string中反序列化为json
    json js=json::parse(buf);
    //接下来准备解耦网络和业务逻辑，根据id来回调相应操作
    //get为模板方法，在模板中输入想要的类型即可变更为相应类型
    auto handler=ChatService::GetInstance().GetHandler(js["msgid"].get<int>());
    handler(tcp_connect,js,time);
}