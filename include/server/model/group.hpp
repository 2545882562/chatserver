#ifndef GROUP_HPP_
#define GROUP_HPP_

#include "group_user.hpp"
#include <string>
#include <vector>
using namespace std;

class Group
{
public:
    Group(int id = -1, string name = "", string desc = "")
    {
        group_id_ = id;
        group_name_ = name;
        group_desc_ = desc;
    }

    void SetID(int id) { group_id_ = id; }
    void SetName(string name) { group_name_ = name; }
    void SetDesc(string desc) { group_desc_ = desc; }

    int GetID() { return group_id_; }
    string GetName() { return group_name_; }
    string GetDesc() { return group_desc_; }
    vector<GroupUser>& GetUsers() { return users_; }

private:
    int group_id_;
    string group_name_;
    string group_desc_;
    vector<GroupUser> users_;
};

#endif // GROUP_HPP_