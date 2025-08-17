#pragma once
#include"MsgNode.h"

class RecvNode :public MsgNode {
	//friend class LogicSystem;
public:
	RecvNode(short max_len, short msg_id);
	short _msg_id;
};

