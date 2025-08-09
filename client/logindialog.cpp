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

}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_forgetpwd_btn_clicked()
{
    emit SwithReset();
}

