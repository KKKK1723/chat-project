#pragma once
#include "const.h"
#include "MysqlConPool.h"
#include "ConfigMgr.h"

class MysqlDao
{
public:
    MysqlDao();
    ~MysqlDao();
    int RegisterUser(const std::string &name, const std::string &email, const std::string &password);

private:
    std::unique_ptr<MysqlConPool> _mysqlpool;
};
