#include "friend_model.hpp"
#include"db.h"

// 添加好友
void FriendModel::AddFriend(int id, int friendid)
{
    char sql[1024];
    sprintf(sql, "insert into friend values(%d,%d)", id, friendid);
    Mysql mysql;
    if (mysql.ConnectToDB())
    {
        mysql.Update(sql);
    }
}

// 查询用户好友
vector<User> FriendModel::Query(int id)
{
    char sql[1024];
    sprintf(sql,
            "SELECT u.id, u.name, u.state FROM friend f JOIN user u ON f.friendid = u.id WHERE f.userid = %d "
            "UNION "
            "SELECT u.id, u.name, u.state FROM friend f JOIN user u ON f.userid = u.id WHERE f.friendid = %d",
            id, id);
    Mysql mysql;
    vector<User> vec;
    if (mysql.ConnectToDB())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.SetID(stoi(row[0]));
                user.SetName(row[1]);
                user.SetState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}
