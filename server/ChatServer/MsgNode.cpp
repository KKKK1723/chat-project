#include "MsgNode.h"

MsgNode::MsgNode(short max_len) : _total_len(max_len), _cur_len(0)
{
    _data = new char[_total_len + 1]();
    _data[_total_len] = '\0';
}

MsgNode::~MsgNode()
{
    std::cout << "destruct MsgNode" << std::endl;
    delete[] _data;
}

void MsgNode::Clear()
{
    ::memset(_data, 0, _total_len);
    _cur_len = 0;
}