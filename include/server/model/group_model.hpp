#ifndef GROUP_MODEL_HPP_
#define GROUP_MODEL_HPP_

#include "group.hpp"

class GroupModel
{
public:
    // 创建群组
    bool CreatGroup(Group &group);
    // 加入群组
    void AddGroup(int user_id, int group_id, string role);
    // 查询用户 所在群组消息
    vector<Group> QueryGrouup(int user_id);
    // 查询除了userid外的指定群组的其他成员id，方便转发消息
    vector<int> QueryGroupUsers(int user_id, int group_id);
};

#endif // GROUP_MODEL_HPP_