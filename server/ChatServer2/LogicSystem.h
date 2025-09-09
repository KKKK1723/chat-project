#pragma once
#include "const.h"
#include "Singleton.h"
#include "LogicNode.h"

struct UserInfo;
class CSession;
typedef std::function<void(std::shared_ptr<CSession>, const short &msg_id, const std::string &msg_data)> FunCallBack;
class LogicSystem : public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;

public:
	void PostMsgToQue(std::shared_ptr<LogicNode> msg);
	void DealQueMsg();
	void RegisterCallBacks();
	void LoginHandler(std::shared_ptr<CSession> session, const short &msg_id, const std::string &msg_data);
	void SearchInfo(std::shared_ptr<CSession>, const short &uid, const std::string &msg_data);
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo> &userinfo);
	bool isPureDigit(const std::string tmp);
	void GetUserByName(std::string name, Json::Value &rtvalue);
	void GetUserByUid(std::string uid_str, Json::Value &rtvalue);
	~LogicSystem();

private:
	LogicSystem();
	std::thread _work_thread;
	std::queue<std::shared_ptr<LogicNode>> _msg_queue;
	std::mutex _mutex;
	std::condition_variable _cond;
	std::map<short, FunCallBack> _fun_callbacks;
	std::atomic<bool> _b_stop;
};
