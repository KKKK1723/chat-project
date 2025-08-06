#include "MysqlMgr.h"
MysqlMgr::~MysqlMgr()
{
}

int MysqlMgr::RegisterUser(const std::string &name, const std::string &email, const std::string &pwd)
{
    return _dao.RegisterUser(name, email, pwd);
}

MysqlMgr::MysqlMgr()
{
}