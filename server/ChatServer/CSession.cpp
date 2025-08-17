#include "CSession.h"
#include "CServer.h"
#include"LogicNode.h"

CSession::CSession(boost::asio::io_context& ioc, CServer* server):_socket(ioc),_server(server), _b_close(false)
{
	_recv_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
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

void CSession::Start()
{
	AsyncReadHead(HEAD_TOTAL_LEN);
}

void CSession::AsyncReadHead(int total_len)
{
	auto self = shared_from_this();
	AsyncReadFull(HEAD_TOTAL_LEN, [this, self](const boost::system::error_code& ec, std::size_t bytes_transfered) {
			
		try
		{
			if (ec)
			{
				std::cout << "handle read failed, error is " << ec.what() << std::endl;
				Close();
				return;
			}

			if (bytes_transfered < HEAD_TOTAL_LEN)
			{
				std::cout << "read head len is error,len is" << bytes_transfered << ", head total len is" << HEAD_TOTAL_LEN << std::endl;
				_server->ClearSession(_uid);
				Close();
				return;
			}

			if (!_server->CheckVaild(_uid))
			{
				std::cout << "this uuid " << _uid << " session is disconnect" << std::endl;
				Close();
				return;
			}

			_recv_head_node->Clear();
			memcpy(_recv_head_node->_data, _data, bytes_transfered);


			short msg_id = 0;
			memcpy(&msg_id, _recv_head_node->_data, HEAD_ID_LEN);
			msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
			std::cout << "msg_id is " << msg_id << std::endl;


			if (msg_id > MAX_LENGTH)
			{
				std::cout << "msg_id is invalid , error msg_id is" << msg_id << std::endl;
				_server->ClearSession(_uid);
				return;
			}


			short msg_len = 0;
			memcpy(&msg_len, _recv_head_node->_data + HEAD_ID_LEN, HEAD_DATA_LEN);
			msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);
			std::cout << "msg_len is " << msg_len << std::endl;

			if (msg_len > MAX_LENGTH)
			{
				std::cout << "msg_len is invalid , error msg_len is" << msg_len << std::endl;
				_server->ClearSession(_uid);
				return;
			}

			_recv_msg_node = std::make_shared<RecvNode>(msg_len, msg_id);
			AsyncReadBody(msg_len);
		}
		catch (std::exception& e)
		{
			std::cout << "AsyncReadHead exception error,error is" << e.what() << std::endl;
		}
	});
}

void CSession::AsyncReadFull(int max_len, std::function<void(const boost::system::error_code& ec, std::size_t bytes_transfered)>handler)
{
	::memset(_data, 0, sizeof(_data));
	AsyncReadLen(0, max_len, handler);
}

void CSession::AsyncReadLen(int  read_len, int total_len, std::function<void(const boost::system::error_code& ec, std::size_t bytes_transfered)> handler)
{
	auto self = shared_from_this();
	_socket.async_read_some(boost::asio::buffer(_data+read_len,total_len-read_len),
		[self, this,read_len,handler,total_len](const boost::system::error_code& ec, std::size_t bytes_transfered) {
			
			if (ec)
			{
				std::cout << "AsyncReadLen is error,error is" << ec.what() << std::endl;
				handler(ec, read_len + bytes_transfered);
				return;
			}

			if (read_len + bytes_transfered >= total_len)//足够
			{
				std::cout << "AsyncReadLen is enough" << std::endl;
				handler(ec, read_len + bytes_transfered);
				return;
			}
			else
			{
				std::cout << "AsyncReadLen is not enough" <<std::endl;
				AsyncReadLen(read_len + bytes_transfered, total_len, handler);
			}
		});
}


void CSession::AsyncReadBody(int length)
{
	auto self = shared_from_this();
	AsyncReadFull(length, [self,this, length](const boost::system::error_code& ec, std::size_t bytes_transfered) {

		try
		{
			if (ec)
			{
				std::cout << "Body read failed, error is " << ec.what() << std::endl;
				Close();
				return;
			}

			if (bytes_transfered < length)
			{
				std::cout << "read body len is error,len is" << bytes_transfered << ", body total len is" << length << std::endl;
				_server->ClearSession(_uid);
				Close();
				return;
			}

			if (!_server->CheckVaild(_uid))
			{
				std::cout << "this uuid " << _uid << " session is disconnect" << std::endl;
				Close();
				return;
			}

			memcpy(_recv_msg_node->_data, _data, bytes_transfered);
			_recv_msg_node->_cur_len += bytes_transfered;
			_recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';
			std::cout << "receive data is " << _recv_msg_node->_data << std::endl;

			//逻辑层
			LogicSystem::GetInstance()->PostMsgToQue(std::make_shared<LogicNode>(shared_from_this(), _recv_msg_node));
			AsyncReadHead(HEAD_TOTAL_LEN);
		}
		catch (std::exception& e)
		{
			std::cout << "AsyncReadBody exception error, error is " << e.what() << std::endl;
		}
	});
}

void CSession::Close()
{
	std::lock_guard<std::mutex> lock(_mutex);
	_socket.close();
	_b_close = true;
}


void CSession::Send(const char* msg, short total_len, short msg_id)
{
	std::lock_guard<std::mutex> lock(_mutex);
	int queue_size = _send_queue.size();
	if (queue_size > MAX_LENGTH)
	{
		std::cout << "Send_queue is not have enough space" << std::endl;
		return;
	}

	std::shared_ptr<SendNode> send_msg=std::make_shared<SendNode>(msg, total_len, msg_id);
	_send_queue.push(send_msg);
	if (queue_size > 0)
	{
		std::cout << "There are other data being sent in the queue. waiting......" << std::endl;
		return;
	}


	auto& msg_node = _send_queue.front();
	boost::asio::async_write(_socket, boost::asio::buffer(msg_node->_data, msg_node->_total_len),
		std::bind(&CSession::HandleWrite, this, std::placeholders::_1, shared_from_this()));
}


void CSession::HandleWrite(const boost::system::error_code& ec, std::shared_ptr<CSession> self)
{
	try
	{
		if (!ec)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_send_queue.pop();
			if (_send_queue.size() > 0)
			{
				auto& msg_node = _send_queue.front();
				boost::asio::async_write(_socket, boost::asio::buffer(msg_node->_data, msg_node->_total_len),
					std::bind(&CSession::HandleWrite, this, std::placeholders::_1, shared_from_this()));
			}
		}
		else
		{
			std::cout << "HandleWrite error_code error,error is" << ec.what() << std::endl;
			Close();
		}
	}
	catch(std::exception& e)
	{
		std::cout << "HandleWrite exception error,error is" << e.what() << std::endl;
	}
}