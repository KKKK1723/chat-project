#include "httpmgr.h"

HttpMgr::HttpMgr()
{
    connect(this,&HttpMgr::sig_http_finish,this,&HttpMgr::slot_http_finish);

}

HttpMgr::~HttpMgr()
{
    std::cout<<"this is HttpMgr destruct"<<std::endl;
}

void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    qDebug()<<"地址："<<url;
    QByteArray data=QJsonDocument(json).toJson();// JSON 对象序列化为 JSON 字符串（字节流）
    QNetworkRequest request(url);//创建请求对象并绑定地址
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");//在头部中设置发送类型为json
    request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(data.length()));//在头部中设置发送长度

    QNetworkReply * reply=_manager.post(request,data);

    auto self=shared_from_this();
    connect(reply,&QNetworkReply::finished,[self,reply,req_id,mod]()
    {
        //处理错误情况
        if(reply->error()!=QNetworkReply::NoError)
        {

            qDebug()<<reply->errorString();
            //发送信号 通知完成
            emit self->sig_http_finish(req_id,"",ErrorCodes::ERR_NETWORK,mod);
            reply->deleteLater();//回收
            return ;
        }


        //无错误
        QString res=reply->readAll();
        //发送信号 通知完成
        emit self->sig_http_finish(req_id,res,ErrorCodes::SUCCESS,mod);

    });
}

void HttpMgr::slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod)
{

    if(mod==Modules::REGISTERMOD)//注册模块
    {
        //发送信号通知指定模块http的响应结束
        emit sig_reg_mod_finish(id,res,err);
    }
    else if(mod==Modules::RESETMOD)//重置模块
    {
        emit sig_reset_mod_finish(id,res,err);
    }
    else if(mod==Modules::LOGINMOD)
    {
        emit sig_login_mod_finish(id,res,err);
    }
}

