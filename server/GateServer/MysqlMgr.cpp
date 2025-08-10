#include "MysqlMgr.h"
MysqlMgr::~MysqlMgr()
{
}

int MysqlMgr::RegisterUser(const std::string &name, const std::string &email, const std::string &pwd)
{
    return _dao.RegisterUser(name, email, pwd);
}

bool MysqlMgr::CheckEmail(const std::string& name, const std::string& email)
{
    return _dao.CheckEmail(name, email);
}

bool MysqlMgr::UpdatePwd(const std::string& name, const std::string& pwd)
{
    return _dao.UpdatePwd(name, pwd);

}

MysqlMgr::MysqlMgr()
{
}