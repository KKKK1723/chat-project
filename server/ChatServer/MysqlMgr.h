#pragma once
#include "const.h"
#include "Singleton.h"
#include "MysqlDao.h"
#include "data.h"
class MysqlMgr:public Singleton<MysqlMgr>,public std::enable_shared_from_this<MysqlMgr>
{
	friend class Singleton<MysqlMgr>;
public:
	~MysqlMgr();
	int RegisterUser(const std::string& name, const std::string& email, const std::string& pwd);
	bool CheckEmail(const std::string& name, const std::string& email);
	bool UpdatePwd(const std::string& name, const std::string& pwd);
	bool CheckPwd(const std::string& name, const std::string& pwd, UserInfo& userinfo);
	std::shared_ptr<UserInfo> GetUser(std::string name);
	std::shared_ptr<UserInfo> GetUser(int uid);

private:
	MysqlMgr();
	MysqlDao  _dao;
};

