#include "db.h"
#include <muduo/base/Logging.h>

static string user = "root";
static string password = "123456";
static string dbname = "chat";
static string server = "127.0.0.1";

// 初始化数据库连接
Mysql::Mysql()
{
    // 初始化连接
    conn_ = mysql_init(nullptr);
}
// 关闭数据库释放连接
Mysql::~Mysql()
{
    if (conn_ != nullptr)
    {
        mysql_close(conn_);
    }
}
// 连接数据库
bool Mysql::ConnectToDB()
{
    MYSQL *p = mysql_real_connect(conn_,server.c_str(),user.c_str(),password.c_str(),dbname.c_str(),3306,nullptr,0);
    if (p != nullptr)
    {
        // 更改编码方式，C/C++采用ASCII码表示，不设置从MYSQL拉下来的数据就会显示？？？
        mysql_query(conn_, "set names gbk");
        LOG_INFO << "连接数据库成功！";
    }
    return p;
}
// 更新操作
bool Mysql::Update(string sql)
{
    if (mysql_query(conn_, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << "更新失败";
        return false;
    }
    return true;
}
// 查询操作
MYSQL_RES *Mysql::query(string sql)
{
    if (mysql_query(conn_, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << "查询失败";
        return nullptr;
    }
    return mysql_use_result(conn_);
}

MYSQL *Mysql::GetConn()
{
    return conn_;
}