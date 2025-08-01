#include "RedisMgr.h"

RedisMgr::RedisMgr()
{
	auto &gCfgMgr = ConfigMgr::Inst();
	auto host = gCfgMgr["Redis"]["Host"];
	auto port = gCfgMgr["Redis"]["Port"];
	auto pwd = gCfgMgr["Redis"]["PassWord"];
	_RedisConPool.reset(new RedisConPool(5, host, port, pwd));
}

RedisMgr::~RedisMgr()
{
	Close();
}

void RedisMgr::Close()
{
	_RedisConPool->Close();
}

std::string RedisMgr::Get(const std::string &key)
{
	auto redis_tmp = _RedisConPool->GetConnection();
	auto value = redis_tmp->get(key);
	_RedisConPool->ReturnConnection(move(redis_tmp));
	if (value)
	{
		return value.value();
	}
	return "";
}