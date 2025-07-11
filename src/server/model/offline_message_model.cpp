#include "offline_message_model.hpp"
#include "db.h"

void OfflineMessageModel::Insert(int id, string msg)
{
    char sql[1024];
    sprintf(sql, "insert into offlinemessage values(%d,'%s')", id, msg.c_str());
    Mysql mysql;
    if (mysql.ConnectToDB())
    {
        mysql.Update(sql);
    }
    return;
}

// 删除用户id的离线消息表的内容
void OfflineMessageModel::Remove(int id)
{
    char sql[1024];
    sprintf(sql, "delete from offlinemessage where userid=%d", id);
    Mysql mysql;
    if (mysql.ConnectToDB())
    {
        mysql.Update(sql);
    }
    return;
}

// 查找用户id的离线消息表的内容
vector<string> OfflineMessageModel::Query(int id)
{
    char sql[1024];
    sprintf(sql, "select message from offlinemessage where userid=%d", id);
    Mysql mysql;
    vector<string> vec;
    if (mysql.ConnectToDB())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res))!= nullptr)
            {
                vec.push_back(row[0]);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}

/*
{"msgid":1,"name":"zhangsan","password":"123456"}
{"msgid":1,"name":"lisi","password":"123456"}
{"msgid":5,"fromid":22,"fromname":"zhangsan","toid":23,"msg":"hello1"}
{"msgid":5,"fromid":22,"fromname":"zhangsan","toid":23,"msg":"hello22"}
{"msgid":6,"id":22,"friendid":23}
*/