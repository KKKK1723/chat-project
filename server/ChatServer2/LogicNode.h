#pragma once
#include"const.h"

class RecvNode;
class CSession;
class LogicNode
{
public:
	LogicNode(std::shared_ptr<CSession>session, std::shared_ptr<RecvNode>recv_msg_node);
	std::shared_ptr<CSession> _session;
	std::shared_ptr<RecvNode> _recv_msg_node;
};

