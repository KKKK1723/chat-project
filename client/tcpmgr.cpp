#include "tcpmgr.h"
#include "QJsonDocument"
#include "usermgr.h"
#include <QNetworkProxy>
TcpMgr::TcpMgr() : _head_is_over(false), _message_id(0), _message_len(0)
{
    connect(&_socket, &QTcpSocket::connected, this, [this]()
            {
        qDebug()<<"Tcp connect success";
        emit sig_con_success(true); });

    connect(&_socket, &QTcpSocket::readyRead, this, [this]()
            {
        QByteArray newData = _socket.readAll();
        qDebug() << "=== 接收到新数据 ===";
        qDebug() << "新数据大小：" << newData.size();
        qDebug() << "新数据内容（hex）：" << newData.toHex();
        _buffer.append(newData);
        qDebug() << "总缓冲区大小：" << _buffer.size();

        forever{
            QDataStream stream(&_buffer,QIODevice::ReadOnly);
            stream.setVersion(QDataStream::Qt_6_7);
            stream.setByteOrder(QDataStream::BigEndian);  // 明确设置大端序
            if(!_head_is_over)
            {
                if(_buffer.size()<static_cast<int>(sizeof(uint16_t)*2))
                {
                    qDebug() << "数据不够解析包头，需要:" << sizeof(uint16_t)*2 << "实际:" << _buffer.size();
                    return;
                }

                qDebug() << "解析包头，缓冲区前8字节：" << _buffer.left(8).toHex();
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

        // 创建 UserInfo 对象
        int uid = object["uid"].toInt();
        QString name = object["name"].toString();
        qDebug() << "创建 UserInfo 对象, uid:" << uid << ", name:" << name;
        
        auto user_info = std::make_shared<UserInfo>(
            uid,
            name, 
            ":/chat_img/boy.png" // 默认头像
        );
        
        qDebug() << "UserInfo 创建完成, user_info 是否为空:" << (user_info ? "valid" : "null");
        if (user_info) {
            qDebug() << "UserInfo 内容 - uid:" << user_info->_uid << ", name:" << user_info->_name;
        }
        
        // 设置到 UserMgr
        qDebug() << "设置 UserInfo 到 UserMgr";
        UserMgr::GetInstance()->SetUserInfo(user_info);
        UserMgr::GetInstance()->SetToken(object["token"].toString());
        
        // 验证设置是否成功
        auto stored_info = UserMgr::GetInstance()->GetUserInfo();
        qDebug() << "UserMgr 中的 UserInfo 是否为空:" << (stored_info ? "valid" : "null");
        if (stored_info) {
            qDebug() << "存储的 UserInfo - uid:" << stored_info->_uid << ", name:" << stored_info->_name;
        }
        qDebug()<<"Login success";
        emit sig_switch_chatdialog(); });

    _handlers.insert(ID_SEARCH_USER_RSP, [this](ReqId id, uint16_t message_len, QByteArray buffer)
                     {
        Q_UNUSED(message_len);
        qDebug()<< "=== 收到搜索响应 ID_SEARCH_USER_RSP ===" ;
        qDebug()<< "响应数据：" << buffer;
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
            emit sig_user_search(nullptr);
            return;
        }

        int error=object["error"].toInt();
        if(error!=ErrorCodes::SUCCESS)
        {
            qDebug()<<"object.error is "<<error;
            emit sig_user_search(nullptr);
            return;
        }

        auto search_info=std::make_shared<SearchInfo>(object["uid"].toInt(),object["name"].toString(),"测试","湖南",0,":/chat_img/boy.png");

        emit sig_user_search(search_info); });
}

TcpMgr::~TcpMgr()
{
    qDebug() << "TcpMgr destruct";
}

void TcpMgr::slot_send_data(ReqId id, QByteArray data)
{
    qDebug() << "=== 发送数据 ===";
    qDebug() << "消息ID：" << (int)id;
    qDebug() << "数据内容：" << data;

    uint16_t _len = static_cast<uint16_t>(data.size());
    uint16_t _id = static_cast<uint16_t>(id);

    QByteArray res;
    QDataStream stream(&res, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    stream << _id << _len;

    res.append(data);

    qDebug() << "发送的完整数据包（hex）：" << res.toHex();
    qDebug() << "发送数据包大小：" << res.size();

    auto written = _socket.write(res);
    qDebug() << "实际写入字节数：" << written;
    qDebug() << "Socket状态：" << _socket.state();
}

void TcpMgr::slot_tcp_connect(ServerInfo si)
{
    qDebug() << "receive tcp connect signal";
    qDebug() << "Connecting to server...";
    _host = si.Host;
    _port = static_cast<uint16_t>(si.Port.toUInt());

    // 禁用代理，避免代理相关错误
    _socket.setProxy(QNetworkProxy::NoProxy);
    qDebug() << "Proxy disabled, connecting to" << si.Host << ":" << _port;
    
    _socket.connectToHost(si.Host, _port);
}
