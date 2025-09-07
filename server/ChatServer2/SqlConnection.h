#pragma once
#include"const.h"

class SqlConnection
{
public:
	SqlConnection(sql::Connection* connection,int64_t lasttime);
	std::unique_ptr<sql::Connection> _connection;
	int64_t _lasttime;
};

