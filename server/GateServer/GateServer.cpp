#include<iostream>
#include<json/json.h>
#include<json/value.h>
#include<json/reader.h>
#include"CServer.h"

int main()
{
    try
    {
       
        unsigned short port = static_cast<unsigned short>(8080);
        net::io_context ioc{ 1 };
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](boost::system::error_code e, int signal_number) {
            if (e)
            {
                return;
            }
            ioc.stop();
        });

        auto t=std::make_shared<CServer>(ioc, port);
        t->Start();
        ioc.run();
        

    }
    catch(std::exception const & e)
    {
        std::cout << "error=" << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}


