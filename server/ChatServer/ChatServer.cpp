#include"ConfigMgr.h"
#include"AsioIOServicePool.h"
#include"CServer.h"
#include <iostream>
#include"RedisMgr.h"
#include"ChatServiceImpl.h"

int main()
{
    auto& Inst = ConfigMgr::Inst();
    auto server_name = Inst["SelfServer"]["Name"];
    try
    {
        auto pool = AsioIOServicePool::GetInstance();
        //将登录数量设置为0
        RedisMgr::GetInstance()->HSet(LOGIN_COUNT, server_name, "0");


        std::string server_address(Inst["SelfServer"]["Host"] + ":" + Inst["SelfServer"]["RPCPort"]);
        ChatServiceImpl service;
        grpc::ServerBuilder builder;
        //监听端口 添加服务
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        //启动grpc服务器
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        std::cout << "RPC Server lisening on " << server_address << std::endl;

        std::thread grpc_server_thread([&server]() {
            server->Wait();
        });

        boost::asio::io_context ioc;
        boost::asio::signal_set signal(ioc, SIGINT, SIGTERM);
        signal.async_wait([&ioc,&pool,&server](auto,auto) {
            ioc.stop();
            pool->Stop();
            server->Shutdown();
            });
        
        auto port_str = Inst["SelfServer"]["Port"];
        unsigned short port = static_cast<short>(std::stoi(port_str));
        CServer s(ioc, port);
        std::cout << "hello world!!!!!!!!!!!!!!!!!!!!!!!!!";
        ioc.run();
        
        RedisMgr::GetInstance()->HDel(LOGIN_COUNT, server_name);
        RedisMgr::GetInstance()->Close();
        grpc_server_thread.join();
    }
    catch (std::exception& e)
    {
        std::cout << "exception erroer is" << e.what() << std::endl;
    }
}


