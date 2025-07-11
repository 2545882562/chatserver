#include "chat_service.hpp"
#include "muduo/base/Logging.h"
// 获取单例类
ChatService &ChatService::GetInstance()
{
    static ChatService instance;
    return instance;
}

ChatService::ChatService()
{
    msg_handle_map_.insert({LOGIN_MSG, std::bind(&ChatService::Login, this, _1, _2, _3)});
    msg_handle_map_.insert({REGISTER_MSG, std::bind(&ChatService::Register, this, _1, _2, _3)});
    msg_handle_map_.insert({LOGIN_OUT_MSG, std::bind(&ChatService::ClientCloseEvent, this, _1, _2, _3)});
    msg_handle_map_.insert({CHAT_ONE_MSG, std::bind(&ChatService::ChatWithOne, this, _1, _2, _3)});
    msg_handle_map_.insert({ADD_FRIEND_MSG, std::bind(&ChatService::AddFriend, this, _1, _2, _3)});
    msg_handle_map_.insert({CREATE_GROUP_MSG, std::bind(&ChatService::CreateGroup, this, _1, _2, _3)});
    msg_handle_map_.insert({ADD_GROUP_MSG, std::bind(&ChatService::AddGroup, this, _1, _2, _3)});
    msg_handle_map_.insert({GROUP_CHAT_MSG, std::bind(&ChatService::ChatWithGroup, this, _1, _2, _3)});

    // 连接redis服务器
    if (redis_.Connect())
    {
        redis_.InitNotifyHandler(std::bind(&ChatService::DealNotifyMessage, this, _1, _2));
    }
}

// 登录处理
void ChatService::Login(const TcpConnectionPtr &conn, json &js, Timestamp &time)
{
    string name = js["name"];
    string pwd = js["password"];
    User user = user_model_.query(name);
    json respond;
    respond["msgid"] = LOGIN_MSG_ACK;
    // 表明用户存在且密码正确
    if (user.GetID() != -1 && user.GetPassword() == pwd)
    {
        // 判断用户是否已经登录
        if (user.GetState() == "online")
        {
            respond["errno"] = 2;
            respond["errmsg"] = "该用户已经登录！";
        }
        else
        {
            // 保存登录用户的conn，方便调用，得加锁，因为多线程回调调用ChatService里面的资源
            {
                lock_guard<mutex> lock(mutex_);
                conn_map_.insert({user.GetID(), conn});
            }
            user.SetState("Online");
            user_model_.UpdateState(user);
            respond["errno"] = 0;
            respond["id"] = user.GetID();
            respond["name"] = user.GetName();
            respond["state"] = user.GetState();
            // 订阅redis消息
            redis_.Subscribe(user.GetID());
            // 获取离线消息，如果不为空则发送过去
            vector<string> vec = offline_message_model_.Query(user.GetID());
            if (!vec.empty())
            {
                respond["offline_message"] = vec;
                offline_message_model_.Remove(user.GetID());
            }
            // 获取好友信息，如果不为空则发送过去
            vector<User> vec_user = friend_model_.Query(user.GetID());
            if (!vec_user.empty())
            {
                vector<string> temp;
                for (auto &user : vec_user)
                {
                    json user_json;
                    user_json["id"] = user.GetID();
                    user_json["name"] = user.GetName();
                    user_json["state"] = user.GetState();
                    temp.push_back(user_json.dump());
                }
                respond["friends"] = temp;
            }
            vector<Group> vec_group = group_model_.QueryGrouup(user.GetID());
            if (!vec_group.empty())
            {
                vector<string> groupV;
                for (Group &group : vec_group)
                {
                    json grpjson;
                    grpjson["id"] = group.GetID();
                    grpjson["groupname"] = group.GetName();
                    grpjson["groupdesc"] = group.GetDesc();
                    vector<string> userV;
                    for (GroupUser &user : group.GetUsers())
                    {
                        json js;
                        js["id"] = user.GetID();
                        js["name"] = user.GetName();
                        js["state"] = user.GetState();
                        js["role"] = user.GetRole();
                        userV.push_back(js.dump());
                    }
                    grpjson["users"] = userV;
                    groupV.push_back(grpjson.dump());
                }
                respond["groups"] = groupV;
            }
        }
    }
    else
    {
        respond["errno"] = 1;
        respond["errmsg"] = "用户名或密码错误!";
    }
    conn->send(respond.dump());
}
// 注册处理
void ChatService::Register(const TcpConnectionPtr &conn, json &js, Timestamp &time)
{
    string name = js["name"];
    string pwd = js["password"];
    User new_user;
    new_user.SetName(name);
    new_user.SetPassword(pwd);
    bool state = user_model_.Insert(new_user);
    json respond;
    respond["msgid"] = REGISTER_MSG_ACK;
    if (state)
    {
        // 注册成功，返回用户ID
        int id = new_user.GetID();
        respond["id"] = id;
        // 0表示注册成功
        respond["errno"] = 0;
    }
    else
    {
        // 1表示注册失败
        respond["errno"] = 1;
    }
    // 给json序列化
    conn->send(respond.dump());
}

// 获取事件处理器，进行回调操作
MsgHandlerFunc ChatService::GetHandler(int msgid)
{
    auto it = msg_handle_map_.find(msgid);
    if (it == msg_handle_map_.end())
    {
        // 返回空操作
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp &time)
        {
            LOG_ERROR << "msgid:" << msgid << "can't find";
        };
    }
    else
    {
        return msg_handle_map_[msgid];
    }
}
void ChatService::ClientCloseEvent(const TcpConnectionPtr &conn, json &js, Timestamp &time)
{
    int id = js["id"].get<int>();
    {
        lock_guard<mutex> lock(mutex_);
        auto it = conn_map_.find(id);
        conn_map_.erase(it);
    }
    // 取消redis订阅消息
    redis_.UnSubscribe(id);
    User user(id);
    user_model_.UpdateState(user);
}

void ChatService::ClientCloseException(TcpConnectionPtr conn)
{
    User user;
    {
        lock_guard<mutex> lock(mutex_);
        for (auto it = conn_map_.begin(); it != conn_map_.end(); it++)
        {
            if (it->second == conn)
            {
                user.SetID(it->first);
                conn_map_.erase(it);
                break;
            }
        }
    }
    // 取消订阅消息
    redis_.UnSubscribe(user.GetID());
    user.SetState("offline");
    user_model_.UpdateState(user);
}

void ChatService::ChatWithOne(const TcpConnectionPtr &conn, json &js, Timestamp &time)
{
    /*格式：
    msgid,fromid,fromname,toid,msg
    */
    {
        lock_guard<mutex> lock(mutex_);
        auto it = conn_map_.find(js["toid"].get<int>());
        // 处理在线用户，转发操作
        if (it != conn_map_.end())
        {
            it->second->send(js.dump());
        }
    }
    // 检查用户是否在线，在线则说明不在此服务器上，因此需要转发消息，即redis的publish
    User user = user_model_.queryID(js["toid"].get<int>());
    if (user.GetState() == "online")
    {
        redis_.Publish(user.GetID(), js.dump());
        return;
    }
    // 不在线用户则存在离线消息表里面
    offline_message_model_.Insert(js["toid"], js.dump());
    return;
}

void ChatService::ServerCloseEvent()
{
    user_model_.UpdateAllState();
}

void ChatService::AddFriend(const TcpConnectionPtr &conn, json &js, Timestamp &time)
{
    /*id,friendid
     */
    int id = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    friend_model_.AddFriend(id, friendid);
    return;
}

// 创建群组操作
void ChatService::CreateGroup(const TcpConnectionPtr &conn, json &js, Timestamp &time)
{
    int user_id = js["id"].get<int>();
    string group_name = js["name"];
    string group_desc = js["desc"];
    Group group;
    group.SetDesc(group_desc);
    group.SetName(group_name);
    if (group_model_.CreatGroup(group))
    {
        group_model_.AddGroup(user_id, group.GetID(), "creator");
    }
}

// 加入群组操作
void ChatService::AddGroup(const TcpConnectionPtr &conn, json &js, Timestamp &time)
{
    int user_id = js["userid"].get<int>();
    int group_id = js["groupid"].get<int>();
    group_model_.AddGroup(user_id, group_id, "normal");
}

// 群组聊天操作
void ChatService::ChatWithGroup(const TcpConnectionPtr &conn, json &js, Timestamp &time)
{
    int user_id = js["userid"].get<int>();
    int group_id = js["groupid"].get<int>();
    // 查询该群除了自己以外的所有用户id
    vector<int> users = group_model_.QueryGroupUsers(user_id, group_id);

    lock_guard<mutex> lock(mutex_);
    for (int id : users)
    {
        auto it = conn_map_.find(id);
        // 在线则转发，不在线则存入离线信息表
        if (it != conn_map_.end())
        {
            it->second->send(js.dump());
        }
        else
        {
            // 检查用户是否在线，在线则说明不在此服务器上，因此需要转发消息，即redis的publish
            User user = user_model_.queryID(id);
            if (user.GetState() == "online")
            {
                redis_.Publish(user.GetID(), js.dump());
                return;
            }
            offline_message_model_.Insert(id, js.dump());
        }
    }
}

void ChatService::DealNotifyMessage(int channel, string message)
{
    lock_guard<mutex> lock(mutex_);
    auto it = conn_map_.find(channel);
    // 用户在线
    if (it != conn_map_.end())
    {
        it->second->send(message);
    }
    else
    {
        offline_message_model_.Insert(channel, message);
    }
}