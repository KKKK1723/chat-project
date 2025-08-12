#pragma once
#include "const.h"
#include "Singleton.h"

class AsioIOServicePool : public Singleton<AsioIOServicePool>
{
    friend class Singleton<AsioIOServicePool>;

public:
    using IOService = boost::asio::io_context;
    using WorkGuard = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    using WorkGuardPtr = std::unique_ptr<WorkGuard>;

    AsioIOServicePool(const AsioIOServicePool &t) = delete;
    AsioIOServicePool &operator=(const AsioIOServicePool &t) = delete;
    ~AsioIOServicePool();

    void Stop();
    boost::asio::io_context &GetIOService();

private:
    AsioIOServicePool(std::size_t size = 2);
    std::vector<IOService> _IOServices;
    std::vector<WorkGuardPtr> _WorkGuards;
    std::vector<std::thread> _threads;
    std::size_t _nextIOService;
};
