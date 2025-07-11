#include "user_model.hpp"
#include "db.h"
#include <iostream>
using namespace std;

bool UserModel::Insert(User &user)
{
    char sql[1024];
    sprintf(sql, "insert into user(name,password,state) values('%s','%s','%s')",
            user.GetName().c_str(), user.GetPassword().c_str(), user.GetState().c_str());
    Mysql mysql;
    if (mysql.ConnectToDB())
    {
        if (mysql.Update(sql))
        {
            // 获取插入成功后用户的id
            user.SetID(mysql_insert_id(mysql.GetConn()));
            return true;
        }
    }
    return false;
}

User UserModel::query(string name)
{
    char sql[1024];
    sprintf(sql, "select* from user where name='%s'", name.c_str());
    Mysql mysql;
    if (mysql.ConnectToDB())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.SetID(stoi(row[0]));
                user.SetName(row[1]);
                user.SetPassword(row[2]);
                user.SetState(row[3]);
                return user;
            }
            mysql_free_result(res);
        }
    }
    return User();
}

User UserModel::queryID(int id)
{
    char sql[1024];
    sprintf(sql, "select* from user where id=%d", id);
    Mysql mysql;
    if (mysql.ConnectToDB())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.SetID(stoi(row[0]));
                user.SetName(row[1]);
                user.SetPassword(row[2]);
                user.SetState(row[3]);
                return user;
            }
            mysql_free_result(res);
        }
    }
    return User();
}

bool UserModel::UpdateState(User user)
{
    char sql[1024];
    sprintf(sql, "update user set state='%s' where id=%d", user.GetState().c_str(), user.GetID());
    Mysql mysql;
    if (mysql.ConnectToDB())
    {
        if (mysql.Update(sql))
        {
            return true;
        }
    }
    return false;
}

void UserModel::UpdateAllState()
{
    string sql="update user set state='offline'";
    Mysql mysql;
    if (mysql.ConnectToDB())
    {
        mysql.Update(sql);  
    }
}