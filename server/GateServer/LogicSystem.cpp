#include"LogicSystem.h"
#include"HttpConnection.h"

//注册Get请求
void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
	_get_handlers.insert({ url,handler });
}


LogicSystem::LogicSystem()
{
	RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
		beast::ostream(connection->_response.body()) << "receive get_test req";
		});
}

LogicSystem::~LogicSystem()
{

}

bool LogicSystem::HandleGet(std::string url, std::shared_ptr<HttpConnection>connection)
{
	if (_get_handlers.find(url) == _get_handlers.end())
	{
		return false;
	}

	_get_handlers[url](connection);
	return true;
}