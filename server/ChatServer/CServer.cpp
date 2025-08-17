#include "CServer.h"
#include "AsioIOServicePool.h"

CServer::CServer(boost::asio::io_context& ioc, int port) :_ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port)),_port(port)
{
	std::cout << "Server start success, listen on port :" << _port << std::endl;
	StartAccept();
}

CServer::~CServer()
{
	std::cout << "CServer is destruct" << std::endl;
}

void CServer::StartAccept()
{
	auto& ioc = AsioIOServicePool::GetInstance()->GetIOService();
	auto new_csession = std::make_shared<CSession>(ioc, this);
	_acceptor.async_accept(new_csession->GetSocket(),std::bind(&CServer::HandleAccept,this,new_csession,std::placeholders::_1));
}

void CServer::HandleAccept(std::shared_ptr<CSession> session, boost::system::error_code ec)
{
	if (!ec)
	{
		session->Start();
		std::lock_guard<std::mutex> lock(_mutex);
		_sessions.insert(std::make_pair(session->GetUid(), session));
	}
	else
	{
		std::cout << "HandleAccept is error,error is" << ec.what() << std::endl;
	}
	StartAccept();
}


bool CServer::CheckVaild(std::string uuid)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_sessions.find(uuid) == _sessions.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

void CServer::ClearSession(std::string uuid)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_sessions.find(uuid) != _sessions.end())
	{
		_sessions.erase(uuid);
	}
}