#include<iostream>
#include<json/json.h>
#include<json/value.h>
#include<json/reader.h>
#include"CServer.h"
#include"ConfigMgr.h"
#include"VerifyGrpcClient.h"

int main()
{
    auto &gCfgMgr = ConfigMgr::Inst();
    std::string gate_port_str = gCfgMgr["GateServer"]["Port"];
    unsigned short gate_port = atoi(gate_port_str.c_str());
    try
    {
        net::io_context ioc;
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](boost::system::error_code e, int signal_number) {
            if (e)
            {
                return;
            }
            ioc.stop();
            });
        auto p = std::make_shared<CServer>(ioc, gate_port);
        p->Start();
        ioc.run();
    }
    catch (std::exception const& e)
    {
        std::cout << "error=" << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}


