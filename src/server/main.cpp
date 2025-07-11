#include"chat_server.hpp"
#include"chat_service.hpp"
#include"signal.h"
#include<iostream>
using namespace std;
//处理服务器Citl+C退出后业务逻辑更正（如用户状态设为下线）
void HandleQuit(int)
{
    ChatService::GetInstance().ServerCloseEvent();
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cerr << "command invalid! example: ./ChatServer 127.0.0.1 6000" << endl;
        exit(-1);
    }
    // 解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    signal(SIGINT, HandleQuit);
    EventLoop loop;
    InetAddress adress(ip,port);
    ChatServer chat_server(&loop,adress,"ChatServer");
    //启动服务器
    chat_server.Start();
    //开始事件循环，阻塞
    loop.loop();
    return 0;
}