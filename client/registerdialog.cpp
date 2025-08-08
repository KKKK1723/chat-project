#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmgr.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    connect(ui->cancel_btn, &QPushButton::clicked, this, &RegisterDialog::SwitchLogin);

    // 输入密码时进行遮盖
    ui->userpwd_edit->setEchoMode(QLineEdit::Password);
    ui->lineEdit->setEchoMode(QLineEdit::Password);

    // 对提示词添加两种样式
    ui->rtips_label->setProperty("state", "normal");
    repolish(ui->rtips_label);
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);
    initHttpHandlers();

    // 隐藏
    QIcon eyeIcon(":/chat_img/eyebrow (1).png");
    // 展示
    QIcon dpeyeIcon(":/chat_img/eye (1).png");

    // 为第一个密码框添加图标
    QAction *togglePwd1Action = new QAction(eyeIcon, "显示/隐藏密码", this);
    togglePwd1Action->setCheckable(true);
    ui->userpwd_edit->addAction(togglePwd1Action, QLineEdit::TrailingPosition);

    // 为第二个密码框添加图标
    QAction *togglePwd2Action = new QAction(eyeIcon, "显示/隐藏密码", this);
    togglePwd2Action->setCheckable(true);
    ui->lineEdit->addAction(togglePwd2Action, QLineEdit::TrailingPosition);

    connect(togglePwd1Action, &QAction::toggled, this, [this, togglePwd1Action, eyeIcon, dpeyeIcon](bool checked)
            {
        ui->userpwd_edit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
        togglePwd1Action->setIcon(checked ? dpeyeIcon : eyeIcon); });

    connect(togglePwd2Action, &QAction::toggled, this, [this, togglePwd2Action, eyeIcon, dpeyeIcon](bool checked)
            {
        ui->lineEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
        togglePwd2Action->setIcon(checked ? dpeyeIcon : eyeIcon); });

    connect(ui->email_edit, &QLineEdit::textChanged, this, &RegisterDialog::onEmailTextChanged);
    connect(ui->varify_btn, &TimerBtn::CheckVerifyIsEmpty, this, &RegisterDialog::OnCheckVerifyIsEmpty);

    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this]()
            { CheckEmail(); });
    connect(ui->varify_edit, &QLineEdit::editingFinished, this, [this]()
            { CheckVarify(); });
    connect(ui->username_edit, &QLineEdit::editingFinished, this, [this]()
            { CheckUserName(); });
    connect(ui->userpwd_edit, &QLineEdit::editingFinished, this, [this]()
            { CheckUserPwd(); });
    connect(ui->lineEdit, &QLineEdit::editingFinished, this, [this]()
            { CheckUserPwdS(); });
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

// 登录和注册页面提示词
void RegisterDialog::showTip(QString str, bool b_ok)
{
    if (b_ok)
    {
        ui->rtips_label->setProperty("state", "normal");
    }
    else
    {
        ui->rtips_label->setProperty("state", "err");
    }

    ui->rtips_label->setText(str);

    repolish(ui->rtips_label);
}

void RegisterDialog::adderr(TipErr err, QString s)
{
    _errmap.insert(err, s);
    showTip(s, false);
}

void RegisterDialog::deerr(TipErr err)
{
    _errmap.remove(err);
    if (_errmap.empty())
    {
        showTip("", true);
        return;
    }
    showTip(_errmap.first(), false);
}

// 验证码回包 存入map
void RegisterDialog::initHttpHandlers()
{
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](const QJsonObject &jsonObj)
                     {
                         int error = jsonObj["error"].toInt();
                         if (error != ErrorCodes::SUCCESS)
                         {
                             showTip(tr("参数错误"), false);
                             return;
                         }

                         auto email = jsonObj["email"].toString();
                         showTip(tr("验证码已经发送到邮箱，注意查收"), true);
                         qDebug() << "email is " << email;
                     });

    _handlers.insert(ReqId::ID_REG_USER, [this](const QJsonObject &jsonObj)
                     {
        int error=jsonObj["error"].toInt();
        if(error!=ErrorCodes::SUCCESS)
        {
            showTip(tr("注册失败"),false);
            return;
        }
        auto email=jsonObj["email"].toString();
        showTip(tr("注册成功"),true);
        qDebug()<<"email is "<<email; });
}

bool RegisterDialog::CheckEmail()
{
    auto email = ui->email_edit->text();

    if (email.isEmpty())
    {
        adderr(TipErr::TIP_EMAIL_ERR2, tr("邮箱不能为空"));
        return false;
    }

    if (!email.endsWith("@qq.com"))
    {
        adderr(TipErr::TIP_EMAIL_ERR1, tr("邮箱地址必须以@qq.com结尾"));
        return false;
    }

    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@qq\.com)");
    bool match = regex.match(email).hasMatch();
    if (match)
    {
        deerr(TipErr::TIP_EMAIL_ERR1);
        deerr(TipErr::TIP_EMAIL_ERR2);
        return true;
    }
}

bool RegisterDialog::CheckVarify()
{
    if (ui->varify_edit->text().isEmpty())
    {
        adderr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }

    deerr(TipErr::TIP_VARIFY_ERR);
    return true;
}

bool RegisterDialog::CheckUserName()
{
    QString user = ui->username_edit->text();

    if (user.isEmpty())
    {
        adderr(TipErr::TIP_USER_ERR2, tr("用户名不能为空"));
        return false;
    }
    else if (user.size() > 8)
    {
        adderr(TipErr::TIP_USER_ERR3, tr("用户名请小于8字符"));
        return false;
    }
    else if (!QRegularExpression("^[\\p{Han}A-Za-z0-9]+$").match(user).hasMatch())
    {
        adderr(TipErr::TIP_USER_ERR1, tr("用户名包含非法字符"));
        return false;
    }

    deerr(TipErr::TIP_USER_ERR1);
    deerr(TipErr::TIP_USER_ERR2);
    deerr(TipErr::TIP_USER_ERR3);
    return true;
}

bool RegisterDialog::CheckUserPwd()
{
    QString pwd = ui->userpwd_edit->text();
    if (pwd.isEmpty())
    {
        adderr(TipErr::TIP_PWD_ERR1, tr("密码不能为空"));
        return false;
    }
    else if (pwd.size() < 5 || pwd.size() > 18)
    {
        adderr(TipErr::TIP_PWD_ERR2, tr("密码长度应在5~18"));
        return false;
    }

    deerr(TipErr::TIP_PWD_ERR1);
    deerr(TipErr::TIP_PWD_ERR2);
    return true;
}

bool RegisterDialog::CheckUserPwdS()
{
    QString pwd = ui->lineEdit->text();
    if (pwd.isEmpty())
    {
        adderr(TipErr::TIP_CONFIRM_ERR1, tr("请输入确认密码"));
        return false;
    }
    else if (pwd != ui->userpwd_edit->text())
    {
        adderr(TipErr::TIP_CONFIRM_ERR2, tr("两次密码不一致"));
        return false;
    }

    deerr(TipErr::TIP_CONFIRM_ERR1);
    deerr(TipErr::TIP_CONFIRM_ERR2);
    return true;
}

void RegisterDialog::onEmailTextChanged(const QString &text)
{
    static QString lastText;

    // 如果用户手动输入了@，不进行自动补全
    if (text.contains('@') && !text.endsWith("@qq.com"))
    {
        lastText = text;
        return;
    }

    // 如果用户删除了@qq.com部分，重新添加
    if (text.length() < lastText.length() && lastText.endsWith("@qq.com"))
    {
        QString baseText = text;
        if (!baseText.contains('@'))
        {
            QString newText = baseText + "@qq.com";
            disconnect(ui->email_edit, &QLineEdit::textChanged, this, &RegisterDialog::onEmailTextChanged);
            ui->email_edit->setText(newText);
            connect(ui->email_edit, &QLineEdit::textChanged, this, &RegisterDialog::onEmailTextChanged);
            ui->email_edit->setCursorPosition(baseText.length());
        }
    }
    // 正常情况：添加@qq.com
    else if (!text.isEmpty() && !text.endsWith("@qq.com"))
    {
        QString newText = text + "@qq.com";
        disconnect(ui->email_edit, &QLineEdit::textChanged, this, &RegisterDialog::onEmailTextChanged);
        ui->email_edit->setText(newText);
        connect(ui->email_edit, &QLineEdit::textChanged, this, &RegisterDialog::onEmailTextChanged);
        ui->email_edit->setCursorPosition(text.length());
    }

    lastText = text;
}

void RegisterDialog::OnCheckVerifyIsEmpty()
{
    auto email = ui->email_edit->text();

    if (email.isEmpty())
    {
        showTip(tr("邮箱不能为空"), false);
        return;
    }

    if (!email.endsWith("@qq.com"))
    {
        showTip(tr("邮箱地址必须以@qq.com结尾"), false);
        return;
    }
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@qq\.com)");
    bool match = regex.match(email).hasMatch();
    if (match)
    {
        ui->varify_btn->_timer->start(1000);
    }
}

void RegisterDialog::on_varify_btn_clicked() // 点击获取验证码
{
    auto email = ui->email_edit->text();

    if (email.isEmpty())
    {
        showTip(tr("邮箱不能为空"), false);
        return;
    }

    if (!email.endsWith("@qq.com"))
    {
        showTip(tr("邮箱地址必须以@qq.com结尾"), false);
        return;
    }
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@qq\.com)");
    bool match = regex.match(email).hasMatch();
    if (match)
    {
        // 发送验证码
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/get_varifycode"),
                                            json_obj, ReqId::ID_GET_VARIFY_CODE, Modules::REGISTERMOD);
    }
}

// 判断验证码是否成功发送
void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if (err != ErrorCodes::SUCCESS)
    {
        showTip(tr("网络请求错误"), false);
    }

    // 反序列化
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    // json解析失败
    if (jsonDoc.isNull())
    {
        showTip(tr("json解析失败1"), false);
        return;
    }
    // json解析错误
    if (!jsonDoc.isObject())
    {
        showTip(tr("json解析失败2"), false);
        return;
    }

    _handlers[id](jsonDoc.object());
    return;
}

void RegisterDialog::on_confirm_btn_clicked()
{

    if (!CheckEmail() || !CheckVarify() || !CheckUserPwd() || !CheckUserName() || !CheckUserPwdS())
        return;

    QJsonObject json_obj;
    json_obj["user"] = ui->username_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = QString((Hash(ui->userpwd_edit->text().toUtf8())).toHex());
    json_obj["confirm"] = QString(Hash(ui->lineEdit->text().toUtf8()).toHex());
    json_obj["verifycode"] = ui->varify_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/user_register"),
                                        json_obj, ReqId::ID_REG_USER, Modules::REGISTERMOD);
}
