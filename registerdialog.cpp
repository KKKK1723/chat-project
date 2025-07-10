#include "registerdialog.h"
#include "ui_registerdialog.h"
#include"global.h"
#include"httpmgr.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    connect(ui->cancel_btn,&QPushButton::clicked,this,&RegisterDialog::SwitchLogin);

    //输入密码时进行遮盖
    ui->userpwd_edit->setEchoMode(QLineEdit::Password);

    //对提示词添加两种样式
    ui->rtips_label->setProperty("state","normal");
    repolish(ui->rtips_label);
    connect(HttpMgr::GetInstance().get(),&HttpMgr::sig_reg_mod_finish,this,&RegisterDialog::slot_reg_mod_finish);
    initHttpHandlers();

}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

//登录和注册页面提示词
void RegisterDialog::showTip(QString str,bool b_ok)
{
    if(b_ok)
    {
        ui->rtips_label->setProperty("state","normal");
    }
    else
    {
        ui->rtips_label->setProperty("state","err");
    }
    ui->rtips_label->setText(str);
    repolish(ui->rtips_label);
}

void RegisterDialog::initHttpHandlers()
{
    //注册获取验证码回包的逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE,[this](const QJsonObject& jsonObj){
        int error=jsonObj["error"].toInt();
        if(error!=ErrorCodes::SUCCESS)
        {
            showTip(tr("参数错误"),false);
            return;
        }

        auto email=jsonObj["email"].toString();
        showTip(tr("验证码已经发送到邮箱，注意查收"),true);
        qDebug()<<"email is "<<email;

    });
}



void RegisterDialog::on_varify_btn_clicked()//点击获取验证码
{
    auto email=ui->email_edit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match=regex.match(email).hasMatch();
    if(match)
    {
        //发送验证码
        qDebug()<<"ok";
        showTip(tr("已发送"),true);
    }
    else
    {
        showTip(tr("邮箱地址不正确"),false);
    }
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err!=ErrorCodes::SUCCESS)
    {
        showTip(tr("网络请求错误"),false);
    }


    //反序列化
    QJsonDocument jsonDoc=QJsonDocument::fromJson(res.toUtf8());
    //json解析失败
    if(jsonDoc.isNull())
    {
        showTip(tr("json解析失败"),false);
        return;
    }
    //json解析错误
    if(!jsonDoc.isObject())
    {
        showTip(tr("json解析失败"),false);
        return;
    }

    _handlers[id](jsonDoc.object());
    return ;

}

