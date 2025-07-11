#ifndef CHAT_SERVICE_HPP_
#define CHAT_SERVICE_HPP_

#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"
#include "json.hpp"
#include"public.hpp"
#include "unordered_map"
#include"user_model.hpp"
#include"offline_message_model.hpp"
#include"friend_model.hpp"
#include"group_model.hpp"
#include"redis.hpp"
#include<mutex>
using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;
using namespace std;

// 聊天服务器业务逻辑类
using MsgHandlerFunc = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp &time)>;
class ChatService
{
public:
    ChatService(const ChatService&)=delete;
    ChatService& operator=(const ChatService&)=delete;
    // 获取单例类
    static ChatService &GetInstance();
    // 登录处理
    void Login(const TcpConnectionPtr &conn, json &js, Timestamp &time);
    // 注册处理
    void Register(const TcpConnectionPtr &conn, json &js, Timestamp &time);
    //1V1聊天处理
    void ChatWithOne(const TcpConnectionPtr &conn, json &js, Timestamp &time);
    //添加好友操作
    void AddFriend(const TcpConnectionPtr &conn, json &js, Timestamp &time);
    //创建群组操作
    void CreateGroup(const TcpConnectionPtr &conn, json &js, Timestamp &time);
    //加入群组操作
    void AddGroup(const TcpConnectionPtr &conn, json &js, Timestamp &time);
    //群组聊天操作
    void ChatWithGroup(const TcpConnectionPtr &conn, json &js, Timestamp &time);
    //客户端正常退出操作
    void ClientCloseEvent(const TcpConnectionPtr &conn, json &js, Timestamp &time);
    //处理跨服务器通信的回调通知函数
    void DealNotifyMessage(int channel,string message);
    //获取事件处理器，进行回调操作
    MsgHandlerFunc GetHandler(int mes_id);
    //处理客户端异常退出操作
    void ClientCloseException(TcpConnectionPtr conn);
    //处理服务器正常退出的业务修复操作
    void ServerCloseEvent();
private:
    ChatService();
    unordered_map<int, MsgHandlerFunc> msg_handle_map_;
    //操作用户的对象数据库逻辑
    UserModel user_model_;
    //操作离线聊天信息对象
    OfflineMessageModel offline_message_model_;
    //操作好友信息对象
    FriendModel friend_model_;
    // 操作群组对象
    GroupModel group_model_;
    //操作redis对象
    Redis redis_;
    //加锁保证线程安全
    mutex mutex_;
    //存储所有在线用户的conn，方便调用
    unordered_map<int,TcpConnectionPtr> conn_map_;
};

#endif // CHAT_SERVICE_HPP_