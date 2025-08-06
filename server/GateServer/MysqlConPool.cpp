#include "MysqlConPool.h"

MysqlConPool::MysqlConPool(const std::string& url, const std::string& user, const std::string& password,
	const std::string& sqlname, int poolsize):_url(url),_user(user),_password(password),_sqlname(sqlname),
	_poolsize(poolsize),_b_stop(false)
{
	try
	{
		for (int i = 0; i < _poolsize; i++)
		{
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
			auto* connection = driver->connect(_url, _user, _password);
			connection->setSchema(_sqlname);

			auto currentTime = std::chrono::system_clock::now().time_since_epoch();
			long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
			_sql_queue.push(std::make_unique<SqlConnection>(connection, timestamp));
		}

		_check_thread = std::thread([this]() {
			while (!_b_stop)
			{
				CheckConnection();
				std::this_thread::sleep_for(std::chrono::seconds(60));
			}
			
		});
		_check_thread.detach();
	}
	catch(sql::SQLException& e)
	{
		std::cout << "mysql pool init failed, error is " << e.what() << std::endl;
	}
}


std::unique_ptr<SqlConnection> MysqlConPool::GetConnection()
{
	std::unique_lock<std::mutex>lock(_mutex);
	_conv.wait(lock, [this]() {
		if (_b_stop)
		{
			return true;
		}
		return !_sql_queue.empty();
	});

	if (_b_stop)
	{
		return nullptr;
	}

	auto connection = std::move(_sql_queue.front());
	_sql_queue.pop();
	return connection;
}


void MysqlConPool::ReturnConnection(std::unique_ptr<SqlConnection>connection)
{
	std::unique_lock<std::mutex>lock(_mutex);
	_sql_queue.push(std::move(connection));
	_conv.notify_one();
}


void  MysqlConPool::CheckConnection()
{
	std::lock_guard<std::mutex>lock(_mutex);
	int nowsize = _sql_queue.size();

	auto currentTime = std::chrono::system_clock::now().time_since_epoch();
	long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();

	//依次健康检查
	for (int i = 0; i < nowsize; i++)
	{
		auto connection = std::move(_sql_queue.front());
		_sql_queue.pop();

		//自动回收
		Defer defer([this,&connection]() {
			_sql_queue.push(std::move(connection));
			});

		if (timestamp - connection->_lasttime < 5)continue;

		try
		{
			std::unique_ptr<sql::Statement> stmt(connection->_connection->createStatement());
			stmt->executeQuery("SELECT 1");
			connection->_lasttime = timestamp;
		}
		catch (sql::SQLException& e)
		{
			std::cout << "Error keeping connection alive: " << e.what() << std::endl;
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
			auto* newcon = driver->connect(_url, _user, _password);
			newcon->setSchema(_sqlname);
			connection->_connection.reset(newcon);
			connection->_lasttime = timestamp;
		}

	}
}

void MysqlConPool::Close() {
	_b_stop = true;
	_conv.notify_all();
}

MysqlConPool::~MysqlConPool() {

	if (_check_thread.joinable()) {
		_check_thread.join();
	}

	std::unique_lock<std::mutex> lock(_mutex);
	while (!_sql_queue.empty()) {
		_sql_queue.pop();
	}
}