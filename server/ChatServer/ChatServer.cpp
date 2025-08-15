#include "ConfigMgr.h"
#include "AsioIOServicePool.h"
#include "CServer.h"
#include <iostream>

int main()
{
    try
    {
        auto &Inst = ConfigMgr::Inst();
        auto pool = AsioIOServicePool::GetInstance();
        boost::asio::io_context ioc;
        boost::asio::signal_set signal(ioc, SIGINT, SIGTERM);
        signal.async_wait([&ioc, &pool](auto, auto)
                          {
            ioc.stop();
            pool->Stop(); });

        auto port_str = Inst["SelfServer"]["Port"];
        int port = static_cast<int>(std::stoi(port_str));
        CServer s(ioc, port);
        std::cout << "hello world！！！！！！！！！！！！！";
        ioc.run();
    }
    catch (std::exception &e)
    {
        std::cout << "exception errer is" << e.what() << std::endl;
    }
}
