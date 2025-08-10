#include "resetdialog.h"
#include "ui_resetdialog.h"

ResetDialog::ResetDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ResetDialog)
{
    ui->setupUi(this);
    _timer = new QTimer(this);
    initHttpHandlers();

    // 隐藏
    QIcon eyeIcon(":/chat_img/eyebrow (2).png");
    // 展示
    QIcon dpeyeIcon(":/chat_img/eye (2).png");

    // 为密码框添加图标
    QAction *togglePwdAction = new QAction(eyeIcon, "显示/隐藏密码", this);
    togglePwdAction->setCheckable(true);
    ui->pwd_edit->addAction(togglePwdAction, QLineEdit::TrailingPosition);

    connect(togglePwdAction, &QAction::toggled, this, [this, togglePwdAction, eyeIcon, dpeyeIcon](bool checked)
            {
        ui->pwd_edit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
        togglePwdAction->setIcon(checked ? dpeyeIcon : eyeIcon); });

    connect(ui->reset2_btn_, &QPushButton::clicked, this, [this]()
            { emit SwitchLogin(); });

    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this]()
            { CheckEmail(); });
    connect(ui->verifycode_edit, &QLineEdit::editingFinished, this, [this]()
            { CheckVarify(); });
    connect(ui->name_edit, &QLineEdit::editingFinished, this, [this]()
            { CheckUserName(); });
    connect(ui->pwd_edit, &QLineEdit::editingFinished, this, [this]()
            { CheckUserPwd(); });

    connect(ui->email_edit, &QLineEdit::textChanged, this, &ResetDialog::onEmailTextChanged);
    connect(_timer, &QTimer::timeout, this, [this]()
            {
        emit SwitchLogin();
        _timer->stop(); });

    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reset_mod_finish, this, &ResetDialog::slot_reset_mod_finish);
}

ResetDialog::~ResetDialog()
{
    delete ui;
}

bool ResetDialog::CheckUserName()
{
    QString user = ui->name_edit->text();

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

bool ResetDialog::CheckEmail()
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

bool ResetDialog::CheckVarify()
{
    if (ui->verifycode_edit->text().isEmpty())
    {
        adderr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }

    deerr(TipErr::TIP_VARIFY_ERR);
    return true;
}

bool ResetDialog::CheckUserPwd()
{
    QString pwd = ui->pwd_edit->text();
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

void ResetDialog::adderr(TipErr err, QString s)
{
    _errmap.insert(err, s);
    showTip(s, false);
}

void ResetDialog::deerr(TipErr err)
{
    _errmap.remove(err);
    if (_errmap.empty())
    {
        showTip("", true);
        return;
    }
    showTip(_errmap.first(), false);
}

void ResetDialog::showTip(QString str, bool b_ok)
{
    if (b_ok)
    {
        ui->errtips->setProperty("state", "normal");
    }
    else
    {
        ui->errtips->setProperty("state", "err");
    }

    ui->errtips->setText(str);

    repolish(ui->errtips);
}

void ResetDialog::onEmailTextChanged(const QString &text)
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
            disconnect(ui->email_edit, &QLineEdit::textChanged, this, &ResetDialog::onEmailTextChanged);
            ui->email_edit->setText(newText);
            connect(ui->email_edit, &QLineEdit::textChanged, this, &ResetDialog::onEmailTextChanged);
            ui->email_edit->setCursorPosition(baseText.length());
        }
    }
    // 正常情况：添加@qq.com
    else if (!text.isEmpty() && !text.endsWith("@qq.com"))
    {
        QString newText = text + "@qq.com";
        disconnect(ui->email_edit, &QLineEdit::textChanged, this, &ResetDialog::onEmailTextChanged);
        ui->email_edit->setText(newText);
        connect(ui->email_edit, &QLineEdit::textChanged, this, &ResetDialog::onEmailTextChanged);
        ui->email_edit->setCursorPosition(text.length());
    }

    lastText = text;
}

// 验证码回包
void ResetDialog::initHttpHandlers()
{
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](const QJsonObject &jsonObj)
                     {
                         int error = jsonObj["error"].toInt();
                         if (error != ErrorCodes::SUCCESS)
                         {
                             showTip(tr("网络请求错误"), false);
                             return;
                         }

                         auto email = jsonObj["email"].toString();
                         showTip(tr("验证码已经发送到邮箱，注意查收"), true);
                         qDebug() << "email is " << email;
                     });

    _handlers.insert(ReqId::ID_RESET_PWD, [this](const QJsonObject &jsonObj)
                     {
        int error=jsonObj["error"].toInt();
        if(error!=ErrorCodes::SUCCESS)
        {
            showTip(tr("注册失败"),false);
            return;
        }
        auto email=jsonObj["email"].toString();
        showTip(tr("注册成功~即将返回登录"),true);
        _timer->start(3000);
        qDebug()<<"email is "<<email; });
}

void ResetDialog::on_verifycode_btn_clicked()
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
                                            json_obj, ReqId::ID_GET_VARIFY_CODE, Modules::RESETMOD);
    }
}

void ResetDialog::slot_reset_mod_finish(ReqId id, QString res, ErrorCodes err)
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

void ResetDialog::on_reset_btn_clicked()
{
    if (!CheckEmail() || !CheckVarify() || !CheckUserPwd() || !CheckUserName())
        return;

    QJsonObject json_obj;
    json_obj["user"] = ui->name_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = QString((Hash(ui->pwd_edit->text().toUtf8())).toHex());
    json_obj["verifycode"] = ui->verifycode_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/reset_pwd"),
                                        json_obj, ReqId::ID_RESET_PWD, Modules::RESETMOD);
}
