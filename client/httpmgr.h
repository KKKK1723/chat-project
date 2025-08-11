#ifndef HTTPMGR_H
#define HTTPMGR_H
#include"singleton.h"
#include"global.h"
#include<QString>
#include<QUrl>
#include<QObject>
#include <QNetworkAccessManager>
#include<QJsonObject>
#include<QJsonDocument>


class ResetDialog;
class HttpMgr: public QObject,public Singleton<HttpMgr>,
                public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT
public:
    ~HttpMgr();
    //发送请求函数
    void PostHttpReq(QUrl url,QJsonObject json,ReqId req_id,Modules mod);
private:
    friend class Singleton<HttpMgr>;//基类中需要访问子类的构造函数
    HttpMgr();//因为想要单例 所以构造函数不能公有
    QNetworkAccessManager _manager;

private slots:
    void slot_http_finish(ReqId id,QString res,ErrorCodes err,Modules mod);

signals:
    void sig_http_finish(ReqId id,QString res,ErrorCodes err,Modules mod);//通知信号
    void sig_reg_mod_finish(ReqId id,QString res,ErrorCodes err);
    void sig_reset_mod_finish(ReqId id,QString res,ErrorCodes err);
    void sig_login_mod_finish(ReqId id,QString res,ErrorCodes err);
};

#endif // HTTPMGR_H
