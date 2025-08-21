#include "tcpmgr.h"
#include "QJsonDocument"
#include "usermgr.h"
TcpMgr::TcpMgr() : _head_is_over(false), _message_id(0), _message_len(0)
{
    connect(&_socket, &QTcpSocket::connected, this, [this]()
            {
        qDebug()<<"Tcp connect success";
        emit sig_con_success(true); });

    connect(&_socket, &QTcpSocket::readyRead, this, [this]()
            {
        _buffer.append(_socket.readAll());

        forever{
            QDataStream stream(&_buffer,QIODevice::ReadOnly);
            stream.setVersion(QDataStream::Qt_6_7);
            if(!_head_is_over)
            {
                if(_buffer.size()<static_cast<int>(sizeof(uint16_t)*2))
                {
                    return;
                }

                stream>>_message_id>>_message_len;
                _buffer = _buffer.mid(sizeof(quint16) * 2);;
                qDebug()<<"message_id="<<_message_id<<" message_len="<<_message_len;

            }

            if(_buffer.size()<_message_len)
            {
                _head_is_over=true;
                return;
            }

            _head_is_over=false;
            QByteArray message_body=_buffer.mid(0,_message_len);
            qDebug()<<"message_body="<<message_body;

            _buffer=_buffer.mid(_message_len);
            handleMsg(ReqId(_message_id),_message_len,message_body);
        } });

    QObject::connect(&_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), [&](QAbstractSocket::SocketError socketError)
                     {
                       Q_UNUSED(socketError)
                       qDebug() << "Error:" << _socket.errorString(); });

    connect(&_socket, &QTcpSocket::disconnected, this, [this]()
            { qDebug() << "Tcp disconnect"; });

    connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_send_data);
    initHandlers();
}

void TcpMgr::handleMsg(ReqId id, int len, QByteArray data)
{
    if (_handlers.find(id) == _handlers.end())
    {
        qDebug() << "_handlers is not have this " << id;
        return;
    }

    _handlers[id](id, len, data);
}

void TcpMgr::initHandlers()
{
    _handlers.insert(ID_CHAT_LOGIN_RSP, [this](ReqId id, uint16_t message_len, QByteArray buffer)
                     {
        Q_UNUSED(message_len);
        qDebug()<< "handle id is "<< id ;

        QJsonDocument document=QJsonDocument::fromJson(buffer);
        if(document.isNull())
        {
            qDebug()<<"ID_CHAT_LOGIN_RSP Json is null";
            return;
        }

        QJsonObject object=document.object();
        if(!object.contains("error"))
        {
            qDebug()<<"object.error is null";
            emit sig_login_faild(ErrorCodes::ERR_JSON);
            return;
        }

        int error=object["error"].toInt();
        if(error!=ErrorCodes::SUCCESS)
        {
            qDebug()<<"object.error is "<<error;
            emit sig_login_faild(error);
            return;
        }

        UserMgr::GetInstance()->SetName(object["name"].toString());
        UserMgr::GetInstance()->SetUid(object["uid"].toInt());
        UserMgr::GetInstance()->SetToken(object["token"].toString());
        qDebug()<<"Login success";
        emit sig_switch_chatdialog(); });
}

TcpMgr::~TcpMgr()
{
    qDebug() << "TcpMgr destruct";
}

void TcpMgr::slot_send_data(ReqId id, QByteArray data)
{
    uint16_t _len = static_cast<uint16_t>(data.size());
    uint16_t _id = static_cast<uint16_t>(id);

    QByteArray res;
    QDataStream stream(&res, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    stream << _id << _len;

    res.append(data);

    _socket.write(res);
}

void TcpMgr::slot_tcp_connect(ServerInfo si)
{
    qDebug() << "receive tcp connect signal";
    qDebug() << "Connecting to server...";
    _host = si.Host;
    _port = static_cast<uint16_t>(si.Port.toUInt());
    _socket.connectToHost(si.Host, _port);
}
