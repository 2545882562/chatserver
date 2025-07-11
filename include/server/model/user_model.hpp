#ifndef USER_MODEL_HPP_
#define USER_MODEL_HPP_

#include "user.hpp"

//实际操作User表类，主要涉及数据库操作
class UserModel
{
public:
    //插入用户表
    bool Insert(User &user);
    //根据用户名查询用户
    User query(string name);
    //根据用户id查询用户
    User queryID(int id);
    //更新用户状态
    bool UpdateState(User user);
    //设置所有的用户为下线状态
    void UpdateAllState();
private:
};

#endif // USER_MODEL_HPP_