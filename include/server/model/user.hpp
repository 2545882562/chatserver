#ifndef USER_HPP_
#define USER_HPP_

#include <string>
using namespace std;
// User表的ORM模型

class User
{
public:
    User(int id = -1, string name = "", string state = "offline")
    {
        id_ = id;
        name_ = name;
        state_ = state;
    }
    void SetID(int id) { id_ = id; }
    void SetName(string name) { name_ = name; }
    void SetPassword(string password) { password_ = password; }
    void SetState(string state) { state_ = state; }

    int GetID() { return id_; }
    string GetName() { return name_; }
    string GetPassword() { return password_; }
    string GetState() { return state_; }

protected:
    int id_;
    string name_;
    string state_;

private:
    string password_;
};

#endif // USER_HPP_