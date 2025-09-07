#include "SendNode.h"

SendNode::SendNode(const char* msg, short max_len, short msg_id):_msg_id(msg_id), MsgNode(max_len+HEAD_TOTAL_LEN)
{

}
