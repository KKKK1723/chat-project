#pragma once
#include <grpcpp/grpcpp.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"
#include "ConfigMgr.h"
#include "Defer.h"
#include "StatusConPool.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;

class StatusGrpcClient : public Singleton<StatusGrpcClient>
{
    friend class Singleton<StatusGrpcClient>;

public:
    ~StatusGrpcClient()
    {
    }
    GetChatServerRsp GetChatServer(int uid);

private:
    StatusGrpcClient();
    std::unique_ptr<StatusConPool> pool_;
};
