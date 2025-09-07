#include "logindialog.h"
#include "ui_logindialog.h"
#include"global.h"
#include"tcpmgr.h"
LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->image_label->setAlignment(Qt::AlignCenter); // 居中显示图片
    ui->user_pwd_edit->setEchoMode(QLineEdit::Password);

    connect(ui->register_btn,&QPushButton::clicked,this,&LoginDialog::SwithRegister);//接收点击注册信号  然后发送信号通知主界面

    ui->ltips_label->setProperty("state","normal");
    repolish(ui->ltips_label);

    //隐藏
    QIcon eyeIcon(":/chat_img/eyebrow (2).png");
    //展示
    QIcon dpeyeIcon(":/chat_img/eye (2).png");

    // 为密码框添加图标
    QAction* togglePwdAction = new QAction(eyeIcon, "显示/隐藏密码", this);
    togglePwdAction->setCheckable(true);
    ui->user_pwd_edit->addAction(togglePwdAction, QLineEdit::TrailingPosition);

    connect(togglePwdAction, &QAction::toggled, this, [this,togglePwdAction,eyeIcon,dpeyeIcon](bool checked) {
        ui->user_pwd_edit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
        togglePwdAction->setIcon(checked ? dpeyeIcon : eyeIcon);
    });

    connect(HttpMgr::GetInstance().get(),&HttpMgr::sig_login_mod_finish,this,&LoginDialog::slot_log_mod_finish);

    initHttpHandlers();

    connect(this, &LoginDialog::sig_connect_tcp, TcpMgr::GetInstance().get(), &TcpMgr::slot_tcp_connect);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_con_success, this, &LoginDialog::slot_tcp_con_finish);
    connect(TcpMgr::GetInstance().get(),&TcpMgr::sig_login_faild,this,&LoginDialog::slot_login_faild);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::showTip(QString str, bool b_ok)
{
    if(b_ok)
    {
        ui->ltips_label->setProperty("state","normal");
    }
    else
    {
        ui->ltips_label->setProperty("state","err");
    }

    ui->ltips_label->setText(str);

    repolish(ui->ltips_label);
}

void LoginDialog::initHttpHandlers()
{
    _handlers.insert(ReqId::ID_LOGIN_USER,[this](const QJsonObject& jsonObj){
        qDebug()<<"进入回调";
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();

        //发送信号通知tcpMgr发送长链接
        ServerInfo si;
        si.Uid = jsonObj["uid"].toInt();
        si.Host = jsonObj["host"].toString();
        si.Port = jsonObj["port"].toString();
        si.Token = jsonObj["token"].toString();

        _uid = si.Uid;
        _token = si.Token;
        qDebug()<< "email is " << email << " uid is " << si.Uid <<" host is "
                 << si.Host << " Port is " << si.Port << " Token is " << si.Token;
        emit sig_connect_tcp(si);
        showTip(tr("登录成功"),true);
    });
}

void LoginDialog::on_forgetpwd_btn_clicked()
{
    emit SwithReset();
}


void LoginDialog::on_login_btn_clicked()
{
    if(ui->user_account_edit->text()=="")
    {
        showTip("用户名不能为空",false);
        return;
    }
    else if(ui->user_pwd_edit->text()=="")
    {
        showTip("密码不能为空",false);
        return;
    }
    qDebug()<<"点击登录";
    QJsonObject json_obj;
    json_obj["user"] = ui->user_account_edit->text();
    json_obj["passwd"] = QString((Hash(ui->user_pwd_edit->text().toUtf8())).toHex());
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_login"),
                                        json_obj, ReqId::ID_LOGIN_USER,Modules::LOGINMOD);
}

void LoginDialog::slot_log_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    qDebug()<<"进入slot_log_mod_finis";
    qDebug()<<"id="<<id<<" res="<<res<<" err="<<err;
    if(err!=ErrorCodes::SUCCESS)
    {
        showTip(tr("网络请求错误"),false);
    }


    //反序列化
    QJsonDocument jsonDoc=QJsonDocument::fromJson(res.toUtf8());
    //json解析失败
    if(jsonDoc.isNull())
    {
        showTip(tr("json解析失败1"),false);
        return;
    }
    //json解析错误
    if(!jsonDoc.isObject())
    {
        showTip(tr("json解析失败2"),false);
        return;
    }

    _handlers[id](jsonDoc.object());
    return ;

}

void LoginDialog::slot_tcp_con_finish(bool t)
{
    if(t)
    {
        qDebug()<<"聊天服务连接成功，正在登录...";
        showTip(tr("Success"),true);
        QJsonObject object;
        object["uid"] = _uid;
        object["token"] = _token;

        QJsonDocument doc(object);
        QByteArray res=doc.toJson(QJsonDocument::Indented);

        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_CHAT_LOGIN,res);
        emit TcpMgr::GetInstance()->sig_switch_chatdialog();

    }
    else
    {
        qDebug()<<"网络异常";
        showTip(tr("网络异常"),false);
    }
}

void LoginDialog::slot_login_faild(int error)
{
    QString result=QString("登录失败");
    qDebug()<<"slot_login_faild error is"<<error;
    showTip(result,false);
}
