#include "const.h"
#include "RedisConPool.h"
#include "Singleton.h"
#include "ConfigMgr.h"
#include "Defer.h"

// class RedisMgr : public Singleton<RedisMgr>, public std::enable_shared_from_this<RedisMgr>
// {
// 	friend class Singleton<RedisMgr>;

// public:
// 	~RedisMgr();
// 	void Close();
// 	std::string Get(const std::string &key); // 接口
// private:
// 	RedisMgr();
// 	std::unique_ptr<sw::redis::Redis> _redis;
// 	std::unique_ptr<RedisConPool> _RedisConPool;
// };

class RedisMgr : public Singleton<RedisMgr>,
                 public std::enable_shared_from_this<RedisMgr>
{
    friend class Singleton<RedisMgr>;

public:
    ~RedisMgr();
    bool Get(const std::string &key, std::string &value);
    bool Set(const std::string &key, const std::string &value);
    bool LPush(const std::string &key, const std::string &value);
    bool LPop(const std::string &key, std::string &value);
    bool RPush(const std::string &key, const std::string &value);
    bool RPop(const std::string &key, std::string &value);
    bool HSet(const std::string &key, const std::string &hkey, const std::string &value);
    bool HSet(const char *key, const char *hkey, const char *hvalue, size_t hvaluelen);
    std::string HGet(const std::string &key, const std::string &hkey);
    bool HDel(const std::string &key, const std::string &field);
    bool Del(const std::string &key);
    bool ExistsKey(const std::string &key);
    void Close();

private:
    RedisMgr();
    std::unique_ptr<RedisConPool> _con_pool;
};