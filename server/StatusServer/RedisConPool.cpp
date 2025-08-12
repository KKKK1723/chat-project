#include "RedisConPool.h"

RedisConPool::RedisConPool(std::size_t PoolSize, const std::string &host, const std::string &port, const std::string &password) : _host(host), _port(port),
																																  _password(password), _bstop(false), _poolsize(PoolSize)
{
	for (std::size_t i = 0; i < _poolsize; i++)
	{
		try
		{
			// std::string path = "tcp://" + host + ":" + port;
			// auto _redis = std::make_unique<sw::redis::Redis>(path);
			redisContext *_redis = redisConnect(host.c_str(), std::stoi(port));

			if (_redis == nullptr || _redis->err)
			{
				if (_redis)
				{
					std::cout << "redis连接错误: " << _redis->errstr << std::endl;
					redisFree(_redis);
				}
				else
				{
					std::cout << "redis连接错误: 无法分配redis上下文" << std::endl;
				}
				continue;
			}

			if (!_password.empty())
			{
				// _redis->auth(_password);
				redisReply *reply = (redisReply *)redisCommand(_redis, "AUTH %s", _password.c_str());
				if (reply)
				{
					if (reply->type == REDIS_REPLY_ERROR)
					{
						std::cout << "redis认证失败: " << reply->str << std::endl;
						freeReplyObject(reply);
						redisFree(_redis);
						continue;
					}
					freeReplyObject(reply);
				}
			}

			std::cout << "redis尝试连接" << std::endl;
			// _redis->ping();
			redisReply *reply = (redisReply *)redisCommand(_redis, "PING");
			if (reply)
			{
				if (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "PONG") == 0)
				{
					std::cout << "redis连接成功" << std::endl;
					_redis_queue.push(_redis);
					std::cout << "redis成功放入队列" << std::endl;
				}
				else
				{
					std::cout << "redis ping失败" << std::endl;
					redisFree(_redis);
				}
				freeReplyObject(reply);
			}
			else
			{
				std::cout << "redis ping失败" << std::endl;
				redisFree(_redis);
			}
		}
		catch (const std::exception &e)
		{
			std::cout << "error in RedisConPool catch and  ";
			std::cout << "error is" << e.what() << std::endl;
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
		redisContext *redis = _redis_queue.front();
		_redis_queue.pop();
		if (redis)
		{
			redisFree(redis);
		}
	}
}

void RedisConPool::Close()
{
	_bstop = true;
	_cond.notify_all();
}

// std::unique_ptr<sw::redis::Redis> RedisConPool::GetConnection()
redisContext *RedisConPool::GetConnection()
{
	if (_bstop)
		return nullptr;
	std::unique_lock<std::mutex> lock(_mutex);

	_cond.wait(lock, [this]()
			   {
				   if (_bstop)
					   return false;

				   return !_redis_queue.empty();
			   });

	if (_bstop)
		return nullptr;

	// auto redis =std::move( _redis_queue.front());
	redisContext *redis = _redis_queue.front();
	_redis_queue.pop();
	return redis;
}

// void RedisConPool::ReturnConnection(std::unique_ptr<sw::redis::Redis> redis)
void RedisConPool::ReturnConnection(redisContext *redis)
{
	if (redis == nullptr)
		return;

	std::lock_guard<std::mutex> lock(_mutex);
	if (_bstop)
		return;

	try
	{
		// redis->ping();
		redisReply *reply = (redisReply *)redisCommand(redis, "PING");
		if (reply)
		{
			if (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "PONG") == 0)
			{
				// _redis_queue.push(std::move(redis));
				_redis_queue.push(redis);
				_cond.notify_one();
			}
			else
			{
				std::cout << "返回无效，丢弃连接: " << reply->str << std::endl;
				redisFree(redis);
			}
			freeReplyObject(reply);
		}
		else
		{
			std::cout << "返回无效，丢弃连接: ping失败" << std::endl;
			redisFree(redis);
		}
	}
	catch (const std::exception &e)
	{
		std::cout << "返回无效，丢弃连接: " << e.what() << std::endl;
		redisFree(redis);
	}
}