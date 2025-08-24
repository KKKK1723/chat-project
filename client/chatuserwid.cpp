#include "chatuserwid.h"
#include "ui_chatuserwid.h"
#include <QFontMetrics>
#include <QResizeEvent>
ChatUserWid::ChatUserWid(QWidget *parent) : ListItemBase(parent),
                                            ui(new Ui::ChatUserWid)
{
    ui->setupUi(this);
    SetItemType(ListItemType::CHAT_USER_ITEM);
}

ChatUserWid::~ChatUserWid()
{
    delete ui;
}

void ChatUserWid::SetInfo(QString name, QString head, QString msg)
{
    _name = name;
    _head = head;
    _msg = msg;
    // 加载图片
    QPixmap pixmap(_head);
    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);
    ui->user_name_lb->setText(_name);
    updateElidedText();
}

void ChatUserWid::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateElidedText();
}

void ChatUserWid::updateElidedText()
{
    if (!_msg.isEmpty())
    {
        QFontMetrics fm(ui->user_chat_lb->font());
        int labelWidth = ui->user_chat_lb->width();
        QString elidedText = fm.elidedText(_msg, Qt::ElideRight, labelWidth);
        ui->user_chat_lb->setText(elidedText);
    }
}
