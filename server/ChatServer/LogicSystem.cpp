#include "LogicSystem.h"
#include"CSession.h"
#include"StatusGrpcClient.h"
LogicSystem::LogicSystem():_b_stop(false)
{
	RegisterCallBacks();
	_work_thread = std::thread(&LogicSystem::DealQueMsg, this);
}


LogicSystem::~LogicSystem()
{

}


void LogicSystem::PostMsgToQue(std::shared_ptr<LogicNode> msg)
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (_msg_queue.size() > MAX_LENGTH)
	{
		std::cout << "PostMsgToQue size not enough" << std::endl;
		return;
	}
	
	_msg_queue.push(msg);
	if (_msg_queue.size() == 1)
	{
		lock.unlock();
		_cond.notify_one();
	}

}


void LogicSystem::DealQueMsg()
{
	for (;;)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		while (_msg_queue.empty() && !_b_stop)
		{
			_cond.wait(lock);
		}

		if (_b_stop)
		{
			while (!_msg_queue.empty())
			{
				auto msg = _msg_queue.front();
				_msg_queue.pop();

				auto msg_id = msg->_recv_msg_node->_msg_id;
				if (_fun_callbacks.find(msg_id) == _fun_callbacks.end())
				{
					continue;
				}
				else
				{ 
					_fun_callbacks[msg_id](msg->_session, msg->_recv_msg_node->_msg_id, 
						std::string(msg->_recv_msg_node->_data,msg->_recv_msg_node->_cur_len));
				}
				
			}
			break;
		}

		//未停止 但队列也不为空
		auto msg = _msg_queue.front();
		_msg_queue.pop();

		auto msg_id = msg->_recv_msg_node->_msg_id;
		if (_fun_callbacks.find(msg_id) == _fun_callbacks.end())
		{
			continue;
		}
		else
		{
			_fun_callbacks[msg_id](msg->_session, msg->_recv_msg_node->_msg_id,
				std::string(msg->_recv_msg_node->_data, msg->_recv_msg_node->_cur_len));
		}

	}
}

void LogicSystem::RegisterCallBacks()
{
	_fun_callbacks[MSG_CHAT_LOGIN] = std::bind(&LogicSystem::LoginHandler, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

void LogicSystem::LoginHandler(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data)
{
	Json::Value root;
	Json::Reader reader;
	reader.parse(msg_data, root);
	auto uid = root["uid"].asInt();
	std::cout << "user login uid is" << uid << ", token is " << root["token"].asString() << std::endl;

	auto rsp = StatusGrpcClient::GetInstance()->Login(uid, root["token"].asString());

	if (rsp.error() != ErrorCodes::Success)
	{
		std::cout << "状态服务器uid和token与客户端不匹配,状态服务器uid=" << rsp.uid() << " 状态服务器token=" << rsp.token() << std::endl;
		return;
	}

	Json::Value rt;
	rt["error"] = rsp.error();
	rt["uid"] = rsp.uid();
	rt["token"] = rsp.token();
}