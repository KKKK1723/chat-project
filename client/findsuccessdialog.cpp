#include "findsuccessdialog.h"
#include "ui_findsuccessdialog.h"
#include <QDir>

FindSuccessDialog::FindSuccessDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::FindSuccessDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setWindowTitle(tr("添加"));

    // 当前路径
    QString app_path = QCoreApplication::applicationDirPath();
    QString pix_path = QDir::toNativeSeparators(app_path + QDir::separator() + "chat_img" + QDir::separator() + "boy.png");

    QPixmap head_pix(pix_path);
    head_pix = head_pix.scaled(ui->head_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->head_lb->setPixmap(head_pix);

    ui->add_friend_btn->SetState("normal", "hover", "press");

    this->setModal(true);
}

FindSuccessDialog::~FindSuccessDialog()
{
    delete ui;
}

void FindSuccessDialog::SetSearchInfo(std::shared_ptr<SearchInfo> si)
{
    ui->name_lb->setText(si->_name);
    _si = si;
}

void FindSuccessDialog::on_add_friend_btn_clicked()
{
    // this->hide();
    // //弹出加好友界面
    // auto applyFriend = new ApplyFriend(_parent);
    // applyFriend->SetSearchInfo(_si);
    // applyFriend->setModal(true);
    // applyFriend->show();
}
