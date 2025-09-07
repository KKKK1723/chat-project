#include "ChatConPool.h"

ChatConPool::ChatConPool(std::size_t poolsize, std::string host, std::string port) : _poolsize(poolsize), _host(host), _port(port), stop(false)
{
    for (int i = 0; i < _poolsize; i++)
    {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(_host + ":" + _port, grpc::InsecureChannelCredentials());
        connections.push(ChatService::NewStub(channel));
    }
}

ChatConPool::~ChatConPool()
{
    std::lock_guard<std::mutex> lock(_mutex);
    // Close();
    while (!connections.empty())
    {
        connections.pop();
    }
}

std::unique_ptr<ChatService::Stub> ChatConPool::getConnection()
{
    std::unique_lock<std::mutex> lock(_mutex);
    cond.wait(lock, [this]()
              {
		if (stop)return true;
		return !connections.empty(); });

    if (stop)
        return nullptr;

    auto t = std::move(connections.front());
    connections.pop();
    return t;
}

void ChatConPool::returnConnection(std::unique_ptr<ChatService::Stub> t)
{
    std::unique_lock<std::mutex> lock(_mutex);
    if (stop)
        return;
    connections.push(std::move(t));
    cond.notify_one();
}

void ChatConPool::Close()
{
    stop = true;
    cond.notify_all();
}