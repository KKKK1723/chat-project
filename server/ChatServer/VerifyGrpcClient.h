#pragma once
#include <grpcpp/grpcpp.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"
#include "GrpcConnectionPool.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class VerifyGrpcClient : public Singleton<VerifyGrpcClient>
{
    friend class Singleton<VerifyGrpcClient>;

public:
    GetVarifyRsp GetVerifyCode(std::string email);

private:
    VerifyGrpcClient();
    std::unique_ptr<GrpcConnectionPool> pool_;
};
