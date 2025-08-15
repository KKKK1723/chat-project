#include "CSession.h"

CSession::CSession(boost::asio::io_context& ioc, CServer* server):_socket(ioc),_server(server)
{
	boost::uuids::uuid  a_uuid = boost::uuids::random_generator()();
	_uid = boost::uuids::to_string(a_uuid);
}

boost::asio::ip::tcp::socket& CSession::GetSocket()
{
	return _socket;
}

std::string CSession::GetUid()
{
	return _uid;
}