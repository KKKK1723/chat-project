#include "AsioIOServicePool.h"

AsioIOServicePool::AsioIOServicePool(std::size_t size) : _IOServices(size), _WorkGuards(size), _nextIOService(0)
{
    for (std::size_t i = 0; i < size; i++)
    {
        _WorkGuards[i] = std::unique_ptr<WorkGuard>(new WorkGuard(boost::asio::make_work_guard(_IOServices[i])));
    }

    for (std::size_t i = 0; i < size; i++)
    {
        _threads.emplace_back([this, i]()
                              { _IOServices[i].run(); });
    }
}

AsioIOServicePool::~AsioIOServicePool()
{
    Stop();
    std::cout << "AsioIOServicePool destruct" << std::endl;
}

boost::asio::io_context &AsioIOServicePool::GetIOService()
{
    boost::asio::io_context &ioc = _IOServices[_nextIOService];
    _nextIOService++;
    if (_nextIOService == _IOServices.size())
    {
        _nextIOService = 0;
    }
    return ioc;
}

void AsioIOServicePool::Stop()
{
    for (auto &t : _WorkGuards)
    {
        t.reset();
    }
    for (auto &t : _IOServices)
    {
        t.stop();
    }
    for (auto &t : _threads)
    {
        t.join();
    }
}