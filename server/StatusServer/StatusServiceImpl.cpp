#include "StatusServiceImpl.h"
#include "ConfigMgr.h"
#include "const.h"
#include "RedisMgr.h"
std::string generate_unique_string()
{
    // 创建UUID对象
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    // 将UUID转换为字符串
    std::string unique_string = to_string(uuid);
    return unique_string;
}
Status StatusServiceImpl::GetChatServer(ServerContext *context, const GetChatServerReq *request, GetChatServerRsp *reply)
{
    std::string prefix("llfc status server has received :  ");
    const auto &server = getChatServer();
    reply->set_host(server.host);
    reply->set_port(server.port);
    reply->set_error(ErrorCodes::Success);
    reply->set_token(generate_unique_string());
    insertToken(request->uid(), reply->token());
    return Status::OK;
}
Status StatusServiceImpl::Login(ServerContext *context, const LoginReq *request, LoginRsp *reply)
{
    auto uid = request->uid();
    auto token = request->token();
    auto iter = _tokens.find(uid);
    if (iter == _tokens.end())
    {
        reply->set_error(ErrorCodes::UidInvalid);
        return Status::OK;
    }
    if (iter->second != token)
    {
        reply->set_error(ErrorCodes::TokenInvalid);
        return Status::OK;
    }
    reply->set_uid(uid);
    reply->set_token(token);
    reply->set_error(ErrorCodes::Success);
    return Status::OK;
}

StatusServiceImpl::StatusServiceImpl()
{
    auto &cfg = ConfigMgr::Inst();
    auto server_list = cfg["chatservers"]["Name"];

    std::vector<std::string> words;

    std::stringstream ss(server_list);
    std::string word;

    while (std::getline(ss, word, ','))
    {
        words.push_back(word);
    }

    for (auto &word : words)
    {
        if (cfg[word]["Name"].empty())
        {
            continue;
        }

        ChatServer server;
        server.port = cfg[word]["Port"];
        server.host = cfg[word]["Host"];
        server.name = cfg[word]["Name"];
        _servers[server.name] = server;
    }
}

ChatServer StatusServiceImpl::getChatServer()
{
    std::lock_guard<std::mutex> guard(_server_mtx);


    if (_servers.empty())
    {
        return ChatServer{};
    }

    auto it = _servers.begin();
    ChatServer minServer = it->second;

    // 从Redis读取实际连接数
    auto count_str = RedisMgr::GetInstance()->HGet("logincount", minServer.name);
    int min_count = count_str.empty() ? 0 : std::stoi(count_str);

    ++it;

    for (; it != _servers.end(); ++it)
    {
        auto current_count_str = RedisMgr::GetInstance()->HGet("logincount", it->second.name);
        int current_count = current_count_str.empty() ? 0 : std::stoi(current_count_str);

        if (current_count < min_count)
        {
            min_count = current_count;
            minServer = it->second;
        }
    }

    return minServer;
}

void StatusServiceImpl::insertToken(int uid, std::string token)
{
    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPREFIX + uid_str;
    RedisMgr::GetInstance()->Set(token_key, token);
    
}