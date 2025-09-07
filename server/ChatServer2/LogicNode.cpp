#include "LogicNode.h"

LogicNode::LogicNode(std::shared_ptr<CSession>session, std::shared_ptr<RecvNode>recv_msg_node):_session(session),_recv_msg_node(recv_msg_node)
{

}