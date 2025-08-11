#pragma once
#define CODEPREFIX "code_"
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include "Singleton.h"
#include <map>
#include <functional>
#include <unordered_map>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <hiredis/hiredis.h>
// 添加 MySQL C++ Connector 头文件
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wnoexcept"
#endif
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
// 添加时间相关头文件
#include <chrono>
#include <thread>

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

enum ErrorCodes
{
    Success = 0,
    Error_Json = 1001,      // JSON解析失败
    RPCFailed = 1002,       // RPC请求错误
    VerifyExpired = 1003,   // 验证码过期
    VerifyCodeErr = 1004,   // 验证码错误
    UserExist = 1005,       // 用户已存在
    PasswdErr = 1006,       // 密码错误
    EmailNotMatch = 1007,   // 邮箱不匹配
    PasswdUpDateErr = 1008, // 密码更新失败
    RPCGetFailed = 1009,//GRPC获取失败
};

struct UserInfo
{
    std::string name;
    std::string email;
    int uid;
    std::string pwd;
};