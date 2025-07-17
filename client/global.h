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

/*
    repolish 用来刷新qss
*/
extern std::function<void(QWidget*)>repolish;

enum ReqId
{
    ID_GET_VARIFY_CODE=1001,//或者验证码
    ID_REG_USER=1002,//注册用户
};


enum Modules
{
    REGISTERMOD=0,
};

enum ErrorCodes
{
    SUCCESS=0,
    ERR_JSON=1,//json 解析失败
    ERR_NETWORK,//网络错误
};

#endif // GLOBAL_H
