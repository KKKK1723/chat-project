#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"

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
    GetVarifyRsp GetVerifyCode(std::string email)
    {
        GetVarifyReq request;
        GetVarifyRsp reply;
        ClientContext context;

        request.set_email(email);

        Status status = _stub->GetVarifyCode(&context, request, &reply);

        if (status.ok())
        {
            return reply;
        }
        else
        {
            reply.set_error(ErrorCodes::RPCFailed);
            return reply;
        }
    }

private:
    VerifyGrpcClient()
    {
        std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());
        _stub = VarifyService::NewStub(channel);
    }
    std::unique_ptr<VarifyService::Stub> _stub;
};
