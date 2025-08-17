#pragma once
#include"const.h"
#include"CSession.h"


class CServer :public std::enable_shared_from_this<CServer>
{
public:
	CServer(boost::asio::io_context& ioc, int port);//上下文+端口号
	~CServer();
	void StartAccept();
	void HandleAccept(std::shared_ptr<CSession> session, boost::system::error_code ec);
	void ClearSession(std::string uuid);
	bool CheckVaild(std::string uuid);
private:
	tcp::acceptor _acceptor;
	net::io_context& _ioc;
	int _port;
	std::map<std::string,std::shared_ptr<CSession>> _sessions;
	std::mutex _mutex;
};

