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
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

enum ErrorCodes
{
    Success = 0,
    Error_Json = 1001,       // Json解析错误
    RPCFailed = 1002,        // RPC请求错误
    VarifyExpired = 1003,    // 验证码过期
    VarifyCodeErr = 1004,    // 验证码错误
    UserExist = 1005,        // 用户已经存在
    PasswdErr = 1006,        // 密码错误
    EmailNotMatch = 1007,    // 邮箱不匹配
    PasswdUpFailed = 1008,   // 更新密码失败
    PasswdInvalid = 1009,    // 密码更新失败
    TokenInvalid = 1010,     // Token失效
    UidInvalid = 1011,       // uid无效
    GetBaseInfoError = 1012, // 未在数据库中找到对应信息
};

#define MAX_LENGTH 1024 * 2
#define HEAD_TOTAL_LEN 4
#define HEAD_ID_LEN 2
#define HEAD_DATA_LEN 2

enum MSG_IDS
{
    MSG_CHAT_LOGIN = 1005,              // 用户登陆
    MSG_CHAT_LOGIN_RSP = 1006,          // 用户登陆回包
    ID_SEARCH_USER_REQ = 1007,          // 用户搜索请求
    ID_SEARCH_USER_RSP = 1008,          // 搜索用户回包
    ID_ADD_FRIEND_REQ = 1009,           // 申请添加好友请求
    ID_ADD_FRIEND_RSP = 1010,           // 申请添加好友回复
    ID_NOTIFY_ADD_FRIEND_REQ = 1011,    // 通知用户添加好友申请
    ID_AUTH_FRIEND_REQ = 1013,          // 认证好友请求
    ID_AUTH_FRIEND_RSP = 1014,          // 认证好友回复
    ID_NOTIFY_AUTH_FRIEND_REQ = 1015,   // 通知用户认证好友申请
    ID_TEXT_CHAT_MSG_REQ = 1017,        // 文本聊天信息请求
    ID_TEXT_CHAT_MSG_RSP = 1018,        // 文本聊天信息回复
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, // 通知用户文本聊天信息
    ID_NOTIFY_OFF_LINE_REQ = 1021,      // 通知用户下线
    ID_HEART_BEAT_REQ = 1023,           // 心跳请求
    ID_HEARTBEAT_RSP = 1024,            // 心跳回复
};

#define USERIPPREFIX  "uip_"
#define USERTOKENPREFIX  "utoken_"
#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT  "logincount"
#define NAME_INFO  "nameinfo_"
#define LOCK_PREFIX "lock_"
#define USER_SESSION_PREFIX "usession_"
#define LOCK_COUNT "lockcount"