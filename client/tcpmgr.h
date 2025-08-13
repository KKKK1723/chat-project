#ifndef TCPMGR_H
#define TCPMGR_H
#include "global.h"
#include "singleton.h"
#include <QTcpSocket>

class TcpMgr : public QObject, public Singleton<TcpMgr>, public std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT
    friend class Singleton<TcpMgr>;

public:
    ~TcpMgr();

private:
    TcpMgr();
    void handleMsg(ReqId id, int len, QByteArray data);
    void initHandlers();

private:
    QTcpSocket _socket;
    QByteArray _buffer;
    QString _host;
    uint16_t _port;
    uint16_t _message_id;
    uint16_t _message_len;
    std::atomic<bool> _head_is_over;
    QMap<ReqId, std::function<void(ReqId, uint16_t message_len, QByteArray buffer)>> _handlers;

signals:
    void sig_send_data(ReqId id, QByteArray data);
    void sig_con_success(bool v);
    void sig_login_faild(int);
    void sig_switch_chatdialog();
public slots:
    void slot_send_data(ReqId id, QByteArray data);
    void slot_tcp_connect(ServerInfo s);
};

#endif // TCPMGR_H
