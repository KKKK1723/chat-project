#include "registerdialog.h"
#include "ui_registerdialog.h"
#include"global.h"
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



void RegisterDialog::on_varify_btn_clicked()//点击获取验证码
{
    auto email=ui->email_edit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match=regex.match(email).hasMatch();
    if(match)
    {
        //发送验证码
        qDebug()<<"ok";
        showTip(tr("邮箱地址正确"),true);
    }
    else
    {
        showTip(tr("邮箱地址不正确"),false);
    }
}

