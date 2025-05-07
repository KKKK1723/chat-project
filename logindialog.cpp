#include "logindialog.h"
#include "ui_logindialog.h"
#include"global.h"

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


}

LoginDialog::~LoginDialog()
{
    delete ui;
}
