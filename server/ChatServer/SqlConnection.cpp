#include "SqlConnection.h"

SqlConnection::SqlConnection(sql::Connection *connection, int64_t lasttime) : _connection(connection), _lasttime(lasttime)
{
}