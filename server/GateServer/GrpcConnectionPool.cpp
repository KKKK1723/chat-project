#include "GrpcConnectionPool.h"

GrpcConnectionPool::GrpcConnectionPool(size_t poolSize, std::string host, std::string port)
    : poolSize_(poolSize), host_(host), port_(port), b_stop_(false)
{
    for (size_t i = 0; i < poolSize_; ++i)
    {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port, grpc::InsecureChannelCredentials());
        connections_.push(VarifyService::NewStub(channel));
    }
}

GrpcConnectionPool::~GrpcConnectionPool()
{
    std::lock_guard<std::mutex> lock(mutex_);
    Close();
    while (!connections_.empty())
    {
        connections_.pop();
    }
}

std::unique_ptr<VarifyService::Stub> GrpcConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this]
               {
        if (b_stop_) {
            return true;
        }
        return !connections_.empty(); });
    // 如果停止则直接返回空指针
    if (b_stop_)
    {
        return nullptr;
    }
    auto context = std::move(connections_.front());
    connections_.pop();
    return context;
}

void GrpcConnectionPool::returnConnection(std::unique_ptr<VarifyService::Stub> context)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (b_stop_)
    {
        return;
    }
    connections_.push(std::move(context));
    cond_.notify_one();
}

void GrpcConnectionPool::Close()
{
    b_stop_ = true;
    cond_.notify_all();
}
