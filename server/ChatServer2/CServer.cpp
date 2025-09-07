#include "CServer.h"
#include "AsioIOServicePool.h"
#include "UserMgr.h"
CServer::CServer(boost::asio::io_context &ioc, unsigned short port) : _ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port)), _port(port)
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
		_sessions.insert(std::make_pair(session->GetSessionUid(), session));
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

void CServer::ClearSession(std::string session_uid)
{

	if (_sessions.find(session_uid) != _sessions.end())
	{
		// 移除用户和session的关联
		UserMgr::GetInstance()->RmvUserSession(_sessions[session_uid]->GetUserId());
	}

	{
		std::lock_guard<std::mutex> lock(_mutex);
		_sessions.erase(session_uid);
	}
}