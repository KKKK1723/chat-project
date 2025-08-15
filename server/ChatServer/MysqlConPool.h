#pragma once
#include "const.h"
#include "SqlConnection.h"
#include "Defer.h"

class MysqlConPool
{
public:
    MysqlConPool(const std::string &url, const std::string &user, const std::string &password,
                 const std::string &sqlname, int _poolsize);
    ~MysqlConPool();
    std::unique_ptr<SqlConnection> GetConnection();
    void ReturnConnection(std::unique_ptr<SqlConnection> connection);
    void CheckConnection();
    void Close();

private:
    std::string _url;
    std::string _user;
    std::string _password;
    std::string _sqlname; // 数据库名称
    int _poolsize;
    std::queue<std::unique_ptr<SqlConnection>> _sql_queue;
    std::thread _check_thread;
    std::atomic<bool> _b_stop;
    std::mutex _mutex;
    std::condition_variable _conv;
};
