#pragma once
#include"const.h"
#include"Singleton.h"
#include "MysqlDao.h"
class MysqlMgr:public Singleton<MysqlMgr>,public std::enable_shared_from_this<MysqlMgr>
{
	friend class Singleton<MysqlMgr>;
public:
	~MysqlMgr();
	int RegisterUser(const std::string& name, const std::string& email, const std::string& pwd);
private:
	MysqlMgr();
	MysqlDao  _dao;
};

