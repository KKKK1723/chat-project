#pragma once
#include "const.h"
#include "MysqlConPool.h"
#include "ConfigMgr.h"
#include "data.h"

class MysqlDao
{
public:
    MysqlDao();
    ~MysqlDao();
    int RegisterUser(const std::string &name, const std::string &email, const std::string &password);
    bool CheckEmail(const std::string &name, const std::string &email);
    bool UpdatePwd(const std::string &name, const std::string &pwd);
    bool CheckPwd(const std::string &name, const std::string &pwd, UserInfo &userinfo);

private:
    std::unique_ptr<MysqlConPool> _mysqlpool;
};
