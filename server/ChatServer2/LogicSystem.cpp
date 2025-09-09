#include "LogicSystem.h"
#include "CSession.h"
#include "StatusGrpcClient.h"
#include "RedisMgr.h"
#include "data.h"
#include "MysqlMgr.h"
#include "UserMgr.h"

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
	_fun_callbacks[ID_SEARCH_USER_REQ] = std::bind(&LogicSystem::SearchInfo, this,
												   std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

void LogicSystem::LoginHandler(std::shared_ptr<CSession> session, const short &msg_id, const std::string &msg_data)
{
	Json::Reader reader;
	Json::Value root;
	reader.parse(msg_data, root);
	auto uid = root["uid"].asInt();
	auto token = root["token"].asString();
	std::cout << "user login uid is  " << uid << " user token  is "
			  << token << std::endl;

	Json::Value rtvalue;
	Defer defer([this, &rtvalue, session]()
				{
		std::string return_str = rtvalue.toStyledString();
		session->Send(return_str, MSG_CHAT_LOGIN_RSP); });

	// 从redis获取用户token是否正确
	std::string uid_str = std::to_string(uid);
	std::string token_key = USERTOKENPREFIX + uid_str;
	std::string token_value = "";
	bool success = RedisMgr::GetInstance()->Get(token_key, token_value);
	if (!success)
	{
		rtvalue["error"] = ErrorCodes::UidInvalid;
		std::string return_str = rtvalue.toStyledString();
		session->Send(return_str, MSG_CHAT_LOGIN_RSP);
		return;
	}

	if (token_value != token)
	{
		rtvalue["error"] = ErrorCodes::TokenInvalid;
		std::string return_str = rtvalue.toStyledString();
		session->Send(return_str, MSG_CHAT_LOGIN_RSP);
		return;
	}

	rtvalue["error"] = ErrorCodes::Success;

	std::string base_key = USER_BASE_INFO + uid_str;
	auto user_info = std::make_shared<UserInfo>();
	bool b_base = GetBaseInfo(base_key, uid, user_info);
	if (!b_base)
	{
		rtvalue["error"] = ErrorCodes::UidInvalid;
		std::string return_str = rtvalue.toStyledString();
		session->Send(return_str, MSG_CHAT_LOGIN_RSP);
		return;
	}
	rtvalue["uid"] = uid;
	rtvalue["pwd"] = user_info->pwd;
	rtvalue["name"] = user_info->name;
	rtvalue["email"] = user_info->email;
	

	// 从数据库获取申请列表
	//...

	// 获取好友列表
	//...

	auto server_name = ConfigMgr::Inst().GetValue("SelfServer", "Name");
	// 登录设备增加
	auto rd_res = RedisMgr::GetInstance()->HGet(LOGIN_COUNT, server_name);
	int count = 0;
	if (rd_res != "")
	{
		count = std::stoi(rd_res);
	}

	count++;

	// 数量写入redis
	auto count_str = std::to_string(count);
	RedisMgr::GetInstance()->HSet(LOGIN_COUNT, server_name, count_str);

	// session绑定用户id
	session->SetUserId(uid);

	// 设置用户登录的server名字
	std::string ipkey = USERIPPREFIX + std::to_string(uid);
	RedisMgr::GetInstance()->Set(ipkey, server_name);
	
	// 踢人
	UserMgr::GetInstance()->SetUserSession(uid, session);

	std::string return_str = rtvalue.toStyledString();
	session->Send(return_str, MSG_CHAT_LOGIN_RSP);

	return;
}

bool LogicSystem::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo> &userinfo)
{
	// redis中找用户信息
	std::string info_str = "";
	bool b_base = RedisMgr::GetInstance()->Get(base_key, info_str);
	if (b_base)
	{
		Json::Reader reader;
		Json::Value root;
		reader.parse(info_str, root);
		userinfo->uid = root["uid"].asInt();
		userinfo->name = root["name"].asString();
		userinfo->pwd = root["pwd"].asString();
		userinfo->email = root["email"].asString();
		
		std::cout << "user login uid is  " << userinfo->uid << " name  is "
				  << userinfo->name << " pwd is " << userinfo->pwd << " email is " << userinfo->email << std::endl;
	}
	else
	{
		// mysql找
		std::shared_ptr<UserInfo> user_info = nullptr;
		user_info = MysqlMgr::GetInstance()->GetUser(uid);
		if (user_info == nullptr)
		{
			std::cout << "mysql与redis中未找到此用户" << std::endl;
			return false;
		}

		userinfo = user_info;

		// 将数据库内容写入redis缓存
		Json::Value redis_root;
		redis_root["uid"] = uid;
		redis_root["pwd"] = userinfo->pwd;
		redis_root["name"] = userinfo->name;
		redis_root["email"] = userinfo->email;

		RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());
	}

	return true;
}

void LogicSystem::SearchInfo(std::shared_ptr<CSession> session, const short &uid, const std::string &msg_data)
{
	Json::Reader reader;
	Json::Value root;
	reader.parse(msg_data, root);
	auto uid_str = root["uid"].asString();
	std::cout << "user SearchInfo uid is  " << uid_str << std::endl;

	Json::Value rtvalue;

	Defer defer([this, &rtvalue, session]()
				{
					std::string return_str = rtvalue.toStyledString();
					session->Send(return_str, ID_SEARCH_USER_RSP);
				});

	bool b_digit = isPureDigit(uid_str);
	if (b_digit)
	{
		GetUserByUid(uid_str, rtvalue);
	}
	else
	{
		GetUserByName(uid_str, rtvalue);
	}
	return;
}

void LogicSystem::GetUserByUid(std::string uid_str, Json::Value &rtvalue)
{
	rtvalue["error"] = ErrorCodes::Success;

	std::string base_key = USER_BASE_INFO + uid_str;

	// 优先查redis中查询用户信息
	std::string info_str = "";
	bool b_base = RedisMgr::GetInstance()->Get(base_key, info_str);
	if (b_base)
	{
		Json::Reader reader;
		Json::Value root;
		reader.parse(info_str, root);
		auto uid = root["uid"].asInt();
		auto name = root["name"].asString();
		auto pwd = root["pwd"].asString();
		auto email = root["email"].asString();


		rtvalue["uid"] = uid;
		rtvalue["pwd"] = pwd;
		rtvalue["name"] = name;
		rtvalue["email"] = email;
		rtvalue["nick"] = "nick";
		rtvalue["desc"] = "desc";
		rtvalue["sex"] = "sex";
		rtvalue["icon"] = "icon";
		return;
	}

	auto uid = std::stoi(uid_str);
	
	//mysql中找
	std::shared_ptr<UserInfo> user_info = nullptr;
	user_info = MysqlMgr::GetInstance()->GetUser(uid);
	if (user_info == nullptr)
	{
		rtvalue["error"] = ErrorCodes::UserExist;
		return;
	}

	// 将数据库内容写入redis缓存
	Json::Value redis_root;
	redis_root["uid"] = user_info->uid;
	redis_root["pwd"] = user_info->pwd;
	redis_root["name"] = user_info->name;
	redis_root["email"] = user_info->email;
	

	RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());

	// 返回数据
	rtvalue["uid"] = user_info->uid;
	rtvalue["pwd"] = user_info->pwd;
	rtvalue["name"] = user_info->name;
	rtvalue["email"] = user_info->email;

}

void LogicSystem::GetUserByName(std::string name, Json::Value &rtvalue)
{
	rtvalue["error"] = ErrorCodes::Success;

	std::string base_key = NAME_INFO + name;

	// 优先查redis中查询用户信息
	std::string info_str = "";
	bool b_base = RedisMgr::GetInstance()->Get(base_key, info_str);
	if (b_base)
	{
		Json::Reader reader;
		Json::Value root;
		reader.parse(info_str, root);
		auto uid = root["uid"].asInt();
		auto name = root["name"].asString();
		auto pwd = root["pwd"].asString();
		auto email = root["email"].asString();
		

		rtvalue["uid"] = uid;
		rtvalue["pwd"] = pwd;
		rtvalue["name"] = name;
		rtvalue["email"] = email;
		rtvalue["nick"] = "nick";
		rtvalue["desc"] = "desc";
		rtvalue["sex"] = "sex";
		rtvalue["icon"] = "icon";
		return;
	}

	//
	//mysql中找
	std::shared_ptr<UserInfo> user_info = nullptr;
	user_info = MysqlMgr::GetInstance()->GetUser(name);
	if (user_info == nullptr)
	{
		rtvalue["error"] = ErrorCodes::UserExist;
		return;
	}

	// 将数据库内容写入redis缓存
	Json::Value redis_root;
	redis_root["uid"] = user_info->uid;
	redis_root["pwd"] = user_info->pwd;
	redis_root["name"] = user_info->name;
	redis_root["email"] = user_info->email;
	

	RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());

	// 返回数据
	rtvalue["uid"] = user_info->uid;
	rtvalue["pwd"] = user_info->pwd;
	rtvalue["name"] = user_info->name;
	rtvalue["email"] = user_info->email;
	
}

bool LogicSystem::isPureDigit(const std::string tmp)
{
	for (auto &c : tmp)
	{
		// 将char转换为unsigned char避免负值问题
		if (!std::isdigit(static_cast<unsigned char>(c)))
		{
			return false;
		}
	}
	return true;
}