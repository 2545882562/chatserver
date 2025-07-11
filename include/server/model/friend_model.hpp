#ifndef FRIEND_MODEL_HPP_
#define FRIEND_MODEL_HPP_
#include <vector>
#include "user.hpp"
using namespace std;
class FriendModel
{
public:
    // 添加好友
    void AddFriend(int id, int friendid);
    // 查询用户好友
    vector<User> Query(int id);

private:
};

#endif // FRIEND_MODEL_HPP_