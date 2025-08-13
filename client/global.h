#ifndef GLOBAL_H
#define GLOBAL_H

#include<QWidget>
#include<functional>
#include<QStyle>
#include<QRegularExpression>
#include<memory>
#include<iostream>
#include<mutex>
#include<QByteArray>
#include<QNetworkReply>
#include<QJsonObject>
#include<QDir>
#include<QSettings>
#include <QCryptographicHash>
#include <QMouseEvent>
#include<QTimer>
/*
    repolish 用来刷新qss
*/
extern std::function<void(QWidget*)>repolish;
extern std::function<QByteArray(const QByteArray&)>Hash;

enum ReqId
{
    ID_GET_VARIFY_CODE=1001,//获取验证码
    ID_REG_USER = 1002, //注册用户
    ID_RESET_PWD = 1003, //重置密码
    ID_LOGIN_USER = 1004, //用户登录
    ID_CHAT_LOGIN = 1005, //登陆聊天服务器
        ID_CHAT_LOGIN_RSP=1006,//登录聊天服务器回包
};


enum Modules
{
    REGISTERMOD = 0,
    RESETMOD = 1,
    LOGINMOD = 2,
};

enum ErrorCodes
{
    SUCCESS=0,
    ERR_JSON=1,//json 解析失败
    ERR_NETWORK=2,//网络错误
};

enum TipErr{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR1 = 1,//邮箱格式 错误
    TIP_EMAIL_ERR2 =2,//邮箱为空 错误
    TIP_VARIFY_ERR = 3, //验证码为空 错误
    TIP_USER_ERR1 = 4,  //用户名包含非法字符 错误
    TIP_USER_ERR2 = 5,//用户名为空  错误
    TIP_USER_ERR3 =6,//用户名过长  错误
    TIP_PWD_ERR1 = 7,//密码为空 错误
    TIP_PWD_ERR2 = 8,//密码范围 错误
    TIP_CONFIRM_ERR1 = 9, //确认密码为空 错误
    TIP_CONFIRM_ERR2 = 10, //确认密码与密码不一致 错误
};


struct ServerInfo{
    QString Host;
    QString Port;
    QString Token;
    int Uid;
};

extern QString gate_url_prefix;

#endif // GLOBAL_H
