#pragma once
#include <grpcpp/grpcpp.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class RPConPool
{
public:
    RPConPool(size_t poolSize, std::string host, std::string port);
    ~RPConPool();
    std::unique_ptr<VarifyService::Stub> getConnection();

    void returnConnection(std::unique_ptr<VarifyService::Stub> context);
    void Close();

private:
    std::atomic<bool> b_stop_;
    size_t poolSize_;
    std::string host_;
    std::string port_;
    std::queue<std::unique_ptr<VarifyService::Stub>> connections_;
    std::mutex mutex_;
    std::condition_variable cond_;
};
