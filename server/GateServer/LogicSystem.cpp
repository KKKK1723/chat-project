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
		beast::ostream(connection->_response.body()) << "receive get_test req"<<std::endl;
		
		int i = 0;
		for (auto& elem : connection->_get_params)
		{
			i++;
			beast::ostream(connection->_response.body()) << "param " << i << "  key is " << elem.first<<std::endl;
			beast::ostream(connection->_response.body()) << "param " << i << "  value is " << elem.second << std::endl;
		}
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