#ifndef DB_H_
#define DB_H_

#include <mysql/mysql.h>
#include <string>
using namespace std;

class Mysql
{
public:
    // 初始化数据库连接
    Mysql();
    // 关闭数据库释放连接
    ~Mysql();
    // 连接数据库
    bool ConnectToDB();
    // 更新操作
    bool Update(string sql);
    // 查询操作
    MYSQL_RES *query(string sql);
    //获取mysql连接
    MYSQL* GetConn();

private:
    MYSQL* conn_;
};

#endif // DB_H_