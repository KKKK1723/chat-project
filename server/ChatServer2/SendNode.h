#pragma once
#include "MsgNode.h"

class SendNode : public MsgNode
{
    // friend class LogicSystem;
public:
    SendNode(const char *msg, short max_len, short msg_id);

private:
    short _msg_id;
};
