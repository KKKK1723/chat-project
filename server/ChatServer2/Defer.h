#pragma once
#include "const.h"

class Defer
{
public:
    Defer(std::function<void()> func);
    ~Defer();

private:
    std::function<void()> _func;
};
