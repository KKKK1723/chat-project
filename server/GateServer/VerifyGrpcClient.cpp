#include "VerifyGrpcClient.h"
#include"ConfigMgr.h"
#include"GrpcConnectionPool.h"

VerifyGrpcClient::VerifyGrpcClient() 
{
	auto& gCfgMgr = ConfigMgr::Inst();
	std::string host = gCfgMgr["VarifyServer"]["Host"];
	std::string port = gCfgMgr["VarifyServer"]["Port"];
    pool_.reset(new GrpcConnectionPool(5, host, port));
}

GetVarifyRsp VerifyGrpcClient::GetVerifyCode(std::string email)
{
	GetVarifyReq request;
	GetVarifyRsp reply;
	ClientContext context;

	request.set_email(email);

	auto stub = pool_->getConnection();
	Status status = stub->GetVarifyCode(&context, request, &reply);

	if (status.ok())
	{
		pool_->returnConnection(std::move(stub));
		return reply;
	}
	else
	{
		reply.set_error(ErrorCodes::RPCFailed);
		pool_->returnConnection(std::move(stub));
		return reply;
	}
}

