#pragma once
#include <string>
#include "const.h"
#include <iostream>
#include <boost/asio.hpp>

class MsgNode
{
public:
    MsgNode(short max_len);

    ~MsgNode();

    void Clear();

    short _cur_len;
    short _total_len;
    char *_data;
};
