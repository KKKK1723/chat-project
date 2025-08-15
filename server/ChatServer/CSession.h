#pragma once
#include "const.h"

class CServer;
class CSession
{
public:
    CSession(boost::asio::io_context &ioc, CServer *server);
    boost::asio::ip::tcp::socket &GetSocket();
    std::string GetUid();

private:
    std::string _uid;
    CServer *_server;
    boost::asio::ip::tcp::socket _socket;
};
