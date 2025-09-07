#pragma once
#include <grpcpp/grpcpp.h> 
#include "message.grpc.pb.h"
#include "message.pb.h"
#include <queue>
#include "const.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::AddFriendReq;
using message::AddFriendRsp;

using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::GetChatServerRsp;
using message::LoginRsp;
using message::LoginReq;
using message::ChatService;

using message::TextChatMsgReq;
using message::TextChatMsgRsp;
using message::TextChatData;

class ChatConPool
{
public:
	ChatConPool(std::size_t poolsize, std::string host, std::string port);
	~ChatConPool();
	std::unique_ptr<ChatService::Stub> getConnection();
	void returnConnection(std::unique_ptr<ChatService::Stub> t);
	void Close();
private:
	std::size_t _poolsize;
	std::string _host;
	std::string _port;
	std::atomic<bool> stop;
	std::mutex _mutex;
	std::condition_variable cond;
	std::queue<std::unique_ptr<ChatService::Stub> > connections;

};

