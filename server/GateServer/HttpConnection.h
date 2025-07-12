#pragma once
#include"const.h"

class HttpConnection:public std::enable_shared_from_this<HttpConnection>
{
public:
	HttpConnection(tcp::socket socket);
	void Start();
private:
	friend class LogicSystem;
	void CheckDeadline();//检查是否掉线
	void WriteResponse();//应答
	void HandleReq();

	tcp::socket _socket;
	beast::flat_buffer _buffer{ 8192 };//数据缓存区域
	http::request<http::dynamic_body> _request;//解析请求
	http::response<http::dynamic_body> _response;//回应客户端
	//定时器判断请求是否超时
	net::steady_timer _deadline{
		_socket.get_executor(),
		std::chrono::seconds(60)
	};

};

