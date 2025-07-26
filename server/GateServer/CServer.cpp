#include "CServer.h"
#include"HttpConnection.h"
#include "AsioIOServicePool.h"
CServer::CServer(boost::asio::io_context& ioc, unsigned short& port) :_ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port))
{

}

void CServer::Start()
{
	auto self = shared_from_this();
	auto &ioc = AsioIOServicePool::GetInstance()->GetIOService();
	std::shared_ptr<HttpConnection> new_connection = std::make_shared<HttpConnection>(ioc);
	_acceptor.async_accept(new_connection->GetSocket(), [self, new_connection](beast::error_code ec)
						   {

		try
		{
			if (ec)//说明连接出错
			{
				self->Start();
				return;
			}

			// 继续监听
			self->Start();

			// 管理连接
			new_connection->Start();
		}
		catch (std::exception& e)
		{

		} });
}