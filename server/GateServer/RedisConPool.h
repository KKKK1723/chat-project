#pragma once
#include"const.h"

class RedisConPool
{
public:
	RedisConPool(std::size_t PoolSize, const std::string& host, const std::string& port, const std::string& password);
	~RedisConPool();
	std::unique_ptr<sw::redis::Redis> GetConnection();
	void ReturnConnection(std::unique_ptr<sw::redis::Redis> redis);
	void Close();

private:
	std::size_t _poolsize;
	std::string _host;
	std::string _port;
	std::string _password;
	std::queue<std::unique_ptr<sw::redis::Redis>> _redis_queue;
	std::condition_variable _cond;
	std::atomic<bool> _bstop;
	std::mutex _mutex;
};

