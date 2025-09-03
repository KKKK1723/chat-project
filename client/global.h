#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <functional>
#include <QStyle>
#include <QRegularExpression>
#include <memory>
#include <iostream>
#include <mutex>
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonObject>
#include <QDir>
#include <QSettings>
#include <QCryptographicHash>
#include <QMouseEvent>
#include <QTimer>
/*
    repolish 用来刷新qss
*/
extern std::function<void(QWidget *)> repolish;
extern std::function<QByteArray(const QByteArray &)> Hash;

enum ReqId
{
    ID_GET_VARIFY_CODE = 1001, // 获取验证码
    ID_REG_USER = 1002,        // 注册用户
    ID_RESET_PWD = 1003,       // 重置密码
    ID_LOGIN_USER = 1004,      // 用户登录
    ID_CHAT_LOGIN = 1005,      // 登陆聊天服务器
    ID_CHAT_LOGIN_RSP = 1006,  // 登录聊天服务器回包
};

enum Modules
{
    REGISTERMOD = 0,
    RESETMOD = 1,
    LOGINMOD = 2,
};

enum ErrorCodes
{
    SUCCESS = 0,
    ERR_JSON = 1,    // json 解析失败
    ERR_NETWORK = 2, // 网络错误
};

enum TipErr
{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR1 = 1,    // 邮箱格式 错误
    TIP_EMAIL_ERR2 = 2,    // 邮箱为空 错误
    TIP_VARIFY_ERR = 3,    // 验证码为空 错误
    TIP_USER_ERR1 = 4,     // 用户名包含非法字符 错误
    TIP_USER_ERR2 = 5,     // 用户名为空  错误
    TIP_USER_ERR3 = 6,     // 用户名过长  错误
    TIP_PWD_ERR1 = 7,      // 密码为空 错误
    TIP_PWD_ERR2 = 8,      // 密码范围 错误
    TIP_CONFIRM_ERR1 = 9,  // 确认密码为空 错误
    TIP_CONFIRM_ERR2 = 10, // 确认密码与密码不一致 错误
};

// 聊天界面几种模式
enum ChatUIMode
{
    SearchMode,   // 搜索模式
    ChatMode,     // 聊天模式
    ContactMode,  // 联系模式
    SettingsMode, // 设置模式
};

struct ServerInfo
{
    QString Host;
    QString Port;
    QString Token;
    int Uid;
};

// 自定义QListWidgetItem的几种类型
enum ListItemType
{
    CHAT_USER_ITEM,    // 聊天用户
    CONTACT_USER_ITEM, // 联系人用户
    SEARCH_USER_ITEM,  // 搜索到的用户
    ADD_USER_TIP_ITEM, // 提示添加用户
    INVALID_ITEM,      // 不可点击条目
    GROUP_TIP_ITEM,    // 分组提示条目
    LINE_ITEM,         // 分割线
    APPLY_FRIEND_ITEM, // 好友申请
};

enum ChatRole
{
    Self,
    Other,
};

struct MsgInfo
{
    QString msgFlag; //"text,image,file"
    QString content; // 表示文件和图像的url,文本信息
    QPixmap pixmap;  // 文件和图片的缩略图
};

enum ClickLbState
{
    Normal = 0,
    Selected = 1,
};

extern QString gate_url_prefix;
// 申请好友标签输入框最低长度
const int MIN_APPLY_LABEL_ED_LEN = 40;

const QString add_prefix = "添加标签 ";

const int tip_offset = 5;

const int CHAT_COUNT_PER_PAGE = 13;

const std::vector<QString> strs = {"hello world !",
                                   "nice to meet u",
                                   "New year，new life",
                                   "You have to love yourself",
                                   "My love is written in the wind ever since the whole world is you"};
const std::vector<QString> heads = {
    ":/chat_img/20210411125716_d7ebc.jpeg",
    ":/chat_img/20210411125716_75b2e.jpeg",
    ":/chat_img/20201126170549_c29c9.jpeg",
    ":/chat_img/20201126170544_6afe1.jpeg",
    ":/chat_img/20200520083558_ZNiBV.jpeg",
};
const std::vector<QString> names = {
    "小白熊",
    "小黑熊",
    "一二",
    "布布",
    "灰灰",
    "蜜桃",
    "小懒",
    "胖熊"};

#endif // GLOBAL_H
