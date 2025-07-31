#include "RedisConPool.h"

RedisConPool::RedisConPool(std::size_t PoolSize, const std::string& host, const std::string& port, const std::string& password):_host(host),_port(port),
_password(password), _bstop(false), _poolsize(PoolSize)
{
	for (std::size_t i = 0; i < _poolsize; i++)
	{
		try
		{
			std::string path = "tcp://" + host + ":" + port;

			auto _redis = std::make_unique<sw::redis::Redis>(path);

			if (!_password.empty())
			{
				_redis->auth(_password);
			}

			std::cout << "redis尝试连接" << std::endl;
			_redis->ping();
			_redis_queue.push(std::move(_redis));
			std::cout << "redis成功放入队列" << std::endl;

		}
		catch (const sw::redis::Error& e)
		{
			std::cout << "error in RedisConPool catch and  " ;
			std::cout << "error is" <<e.what() << std::endl;
			continue;
		}
	}
}

RedisConPool::~RedisConPool()
{
	Close();
	std::lock_guard<std::mutex> lock(_mutex);
	while (!_redis_queue.empty())
	{
		_redis_queue.pop();
	}
}

void RedisConPool::Close()
{
	_bstop = true;
	_cond.notify_all();
}


std::unique_ptr<sw::redis::Redis> RedisConPool::GetConnection()
{
	if (_bstop)return nullptr;
	std::unique_lock<std::mutex> lock(_mutex);

	_cond.wait(lock, [this]() {

		if (_bstop)return false;

		return !_redis_queue.empty();

		});

	if (_bstop)return nullptr;

	auto redis =std::move( _redis_queue.front());
	_redis_queue.pop();
	return redis;
}


void RedisConPool::ReturnConnection(std::unique_ptr<sw::redis::Redis> redis)
{
	if (redis == nullptr)return;

	std::lock_guard<std::mutex> lock(_mutex);
	if (_bstop)return;

	try
	{
		redis->ping();
		_redis_queue.push(std::move(redis));
		_cond.notify_one();
	}
	catch(const sw::redis::Error& e)
	{
		std::cout << "返回无效，丢弃连接: " << e.what() << std::endl;
	}
}