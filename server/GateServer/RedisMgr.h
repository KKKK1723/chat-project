#include "const.h"
#include "RedisConPool.h"
#include "Singleton.h"
#include "ConfigMgr.h"

class RedisMgr : public Singleton<RedisMgr>, public std::enable_shared_from_this<RedisMgr>
{
	friend class Singleton<RedisMgr>;

public:
	~RedisMgr();
	void Close();
	std::string Get(const std::string &key); // 接口
private:
	RedisMgr();
	std::unique_ptr<sw::redis::Redis> _redis;
	std::unique_ptr<RedisConPool> _RedisConPool;
};
