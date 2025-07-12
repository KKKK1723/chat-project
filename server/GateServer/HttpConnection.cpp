#include "HttpConnection.h"
#include"LogicSystem.h"

HttpConnection::HttpConnection(tcp::socket socket):_socket(std::move(socket))
{

}


void HttpConnection::Start()
{
	auto self = shared_from_this();
	http::async_read(_socket, _buffer, _request, [self](boost::beast::error_code ec, std::size_t bytes_transferred) {
		try
		{
			if (ec)
			{
				std::cout << "http read err is" << ec.message() << std::endl;
				return;
			}

			boost::ignore_unused(bytes_transferred);//消除未使用变量的编译警告
			self->HandleReq();
			self->CheckDeadline();
		}
		catch(std::exception& e)
		{
			std::cout << "exception is" << e.what() << std::endl;
		}

	});
}

void HttpConnection::HandleReq()
{
	_response.version(_request.version());//设置版本
	_response.keep_alive(false);//不需要长连接
	if (_request.method() == http::verb::get)//如果是get请求
	{
		bool success=LogicSystem::GetInstance()->HandleGet(std::string(_request.target()), shared_from_this());
		if (!success)
		{
			_response.result(http::status::not_found);
			_response.set(http::field::content_type, "text/plain");//设置响应头 Content-Type
			beast::ostream(_response.body()) << "url not found\r\n";
			WriteResponse();
			return;
		}
		_response.result(http::status::ok);
		_response.set(http::field::server, "GateServer"); 
		_response.set(http::field::content_type, "text/plain"); 
		WriteResponse();
	}
}

void HttpConnection::WriteResponse()
{
	auto self = shared_from_this();
	_response.content_length(_response.body().size());
	http::async_write(_socket, _response, [self](boost::beast::error_code ec, std::size_t bytes_transferred) {
		
		self->_socket.shutdown(tcp::socket::shutdown_send, ec);//关闭发送端
		self->_deadline.cancel();//取消定时器
		
		});

}


void HttpConnection::CheckDeadline()
{
	auto self = shared_from_this();
	_deadline.async_wait([self](beast::error_code ec) {
		if (!ec)
		{
			self->_socket.close(ec);
		}
		});
}