#ifndef GROUP_USER_HPP_
#define GROUP_USER_HPP_

#include "user.hpp"

//群组User，继承User并添加在群组的身份
class GroupUser : public User
{
public:
    void SetRole(string role) { role_ = role; }
    string GetRole() { return role_; }

private:
    string role_; // 所在群组身份（管理者creator，普通normal）
};

#endif // GROUP_USER_HPP_