#include "group_model.hpp"
#include "db.h"

// 创建群组
bool GroupModel::CreatGroup(Group &group)
{
    char sql[1024];
    sprintf(sql, "insert into allgroup(groupname,groupdesc) values('%s','%s')",
            group.GetName().c_str(), group.GetDesc().c_str());
    Mysql mysql;
    if (mysql.ConnectToDB())
    {
        if (mysql.Update(sql))
        {
            // 获取插入成功后群组的id
            group.SetID(mysql_insert_id(mysql.GetConn()));
            return true;
        }
    }
    return false;
}

// 加入群组
void GroupModel::AddGroup(int user_id, int group_id, string role)
{
    char sql[1024];
    sprintf(sql, "insert into groupuser values(%d,%d,'%s')",
            group_id, user_id, role.c_str());
    Mysql mysql;
    if (mysql.ConnectToDB())
    {
        mysql.Update(sql);
    }
}

// 查询用户所在群组消息
vector<Group> GroupModel::QueryGrouup(int user_id)
{
    vector<Group> groups;
    //根据用户id查询所在群组的名称，id，描述
    char sql[1024];
    sprintf(sql,
            "select g.id,g.groupname,g.groupdesc from allgroup g inner join groupuser u "
            "on u.groupid=g.id where u.userid=%d",user_id);
    Mysql mysql;
    if (mysql.ConnectToDB())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                Group group;
                group.SetID(stoi(row[0]));
                group.SetName(row[1]);
                group.SetDesc(row[2]);
                groups.push_back(group);
            }
            mysql_free_result(res);
        }
    }
    //根据群组id查询群组的所有成员
    for(auto& group:groups)
    {
        sprintf(sql,
            "select u.id,u.name,u.state,g.grouprole from user u inner join groupuser g "
            "on u.id=g.userid where g.groupid=%d",group.GetID());
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                GroupUser gu;
                gu.SetID(stoi(row[0]));
                gu.SetName(row[1]);
                gu.SetState(row[2]);
                gu.SetRole(row[3]);
                group.GetUsers().push_back(gu);
            }
            mysql_free_result(res);
        }
    }
    return groups;
}

// 查询除了userid外的指定群组的其他成员id，方便转发消息
vector<int> GroupModel::QueryGroupUsers(int user_id, int group_id)
{
    char sql[1024];
    sprintf(sql,
            "select userid from groupuser where groupid=%d and userid!=%d",group_id,user_id);
    Mysql mysql;
    vector<int> vec;
    if (mysql.ConnectToDB())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
               vec.push_back(stoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return vec;
}