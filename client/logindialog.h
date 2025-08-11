#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QPixmap>
#include "httpmgr.h"
namespace Ui
{
    class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private:
    Ui::LoginDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;
    int _uid;
    QString _token;

private:
    void showTip(QString str, bool b_ok);
    void initHttpHandlers();

signals:
    void SwithRegister(); // 发送信号通知主界面进入注册页面
    void SwithReset();    // 发送信号通知主界面进入重置密码页面
    void sig_connect_tcp(ServerInfo);
private slots:
    void on_forgetpwd_btn_clicked();
    void on_login_btn_clicked();
    void slot_log_mod_finish(ReqId id, QString res, ErrorCodes err);
};

#endif // LOGINDIALOG_H
