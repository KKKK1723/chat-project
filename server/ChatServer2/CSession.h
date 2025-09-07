#pragma once
#include"const.h"
#include"LogicSystem.h"
#include"MsgNode.h"
#include"RecvNode.h"
#include"SendNode.h"

class CServer;
class CSession :public std::enable_shared_from_this<CSession>
{
public:
	CSession(boost::asio::io_context& ioc,CServer* server);
	boost::asio::ip::tcp::socket& GetSocket();
	std::string& GetSessionUid();
	void SetUserId(int uid);
	int GetUserId();
	void Start();
	void AsyncReadHead(int total_len);
	void AsyncReadBody(int length);
	void AsyncReadFull(int max_len, std::function<void(const boost::system::error_code& ec, std::size_t bytes_transfered)>handler);
	void AsyncReadLen(int  read_len, int total_len,std::function<void(const boost::system::error_code& ec, std::size_t bytes_transfered)> handler);
	void Close();

	void Send(const char* msg,short total_len, short msg_id);
	void Send(std::string msg, short msgid);
	void HandleWrite(const boost::system::error_code& ec,std::shared_ptr<CSession> self);
private:
	std::string _session_uid;
	CServer* _server;
	boost::asio::ip::tcp::socket _socket;
	char _data[MAX_LENGTH];
	std::mutex _mutex;
	std::atomic<bool>_b_close;
	std::shared_ptr<MsgNode> _recv_head_node;
	std::shared_ptr<RecvNode> _recv_msg_node;
	std::queue<std::shared_ptr<SendNode>> _send_queue;
	int _user_id;
};

