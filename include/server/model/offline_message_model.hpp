#ifndef OFFLINE_MESSAGE_MODEL_HPP_
#define OFFLINE_MESSAGE_MODEL_HPP_

#include <string>
#include <vector>
using namespace std;

class OfflineMessageModel
{

public:
    // 插入离线信息表
    void Insert(int id, string msg);
    // 删除用户id的离线消息表的内容
    void Remove(int id);
    // 查找用户id的离线消息表的内容
    vector<string> Query(int id);

private:
};

#endif