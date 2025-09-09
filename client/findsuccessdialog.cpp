#include "findsuccessdialog.h"
#include "ui_findsuccessdialog.h"
#include<QDir>
#include"applyfriend.h"

FindSuccessDialog::FindSuccessDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindSuccessDialog),_parent(parent)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()|Qt::FramelessWindowHint);
    setWindowTitle(tr("添加"));

    //当前路径
    QString app_path=QCoreApplication::applicationDirPath();
    QString pix_path=QDir::toNativeSeparators(app_path+QDir::separator()+"chat_img"+QDir::separator()+"boy.png");
    qDebug() << "图片路径：" << pix_path;
    qDebug() << "图片文件是否存在：" << QFile::exists(pix_path);

    QPixmap head_pix(pix_path);
    qDebug() << "图片是否加载成功：" << !head_pix.isNull();
    qDebug() << "head_lb尺寸：" << ui->head_lb->size();
    head_pix=head_pix.scaled(ui->head_lb->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->head_lb->setPixmap(head_pix);

    ui->add_friend_btn->SetState("normal","hover","press");

    this->setModal(true);
}

FindSuccessDialog::~FindSuccessDialog()
{
    delete ui;
}

void FindSuccessDialog::SetSearchInfo(std::shared_ptr<SearchInfo> si)
{
    qDebug() << "=== SetSearchInfo被调用 ===";
    qDebug() << "用户名：" << si->_name;
    qDebug() << "设置到UI之前";
    ui->name_lb->setText(si->_name);
    qDebug() << "设置到UI之后";
    qDebug() << "name_lb文本：" << ui->name_lb->text();
    _si = si;
}

void FindSuccessDialog::on_add_friend_btn_clicked()
{
    this->hide();
    //弹出加好友界面
    auto applyFriend = new ApplyFriend(_parent);
    applyFriend->SetSearchInfo(_si);
    applyFriend->setModal(true);
    applyFriend->show();
}
