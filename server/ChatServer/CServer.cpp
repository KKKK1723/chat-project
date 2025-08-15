#include "CServer.h"
#include "AsioIOServicePool.h"

CServer::CServer(boost::asio::io_context &ioc, int port) : _ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port)), _port(port)
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
    auto &ioc = AsioIOServicePool::GetInstance()->GetIOService();
    auto new_csession = std::make_shared<CSession>(ioc, this);
    _acceptor.async_accept(new_csession->GetSocket(), std::bind(&CServer::HandleAccept, this, new_csession, std::placeholders::_1));
}

void CServer::HandleAccept(std::shared_ptr<CSession> session, boost::system::error_code ec)
{
}