#include "chatpage.h"
#include "ui_chatpage.h"
#include<QStyle>
#include"global.h"
#include"chatitembase.h"
#include"textbubble.h"
#include"picturebubbble.h"

ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatPage)
{
    ui->setupUi(this);
    
    // 连接回车键发送信号
    connect(ui->chat_edit, &MessageTextEdit::send, this, &ChatPage::on_send_btn_clicked);
}

ChatPage::~ChatPage()
{
    delete ui;
}

void ChatPage::on_send_btn_clicked()
{
    auto pTextEdit = ui->chat_edit;
    ChatRole role = ChatRole::Self;
    QString userName = QStringLiteral("布布");
    QString userIcon = ":/chat_img/20200904151743_610f8.jpeg";

    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    for(int i=0; i<msgList.size(); ++i)
    {
        QString type = msgList[i].msgFlag;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(userName);
        pChatItem->setUserIcon(QPixmap(userIcon));
        QWidget *pBubble = nullptr;
        if(type == "text")
        {
            pBubble = new TextBubble(role, msgList[i].content);
        }
        else if(type == "image")
        {
            pBubble = new PictureBubble(QPixmap(msgList[i].content) , role);
        }
        else if(type == "file")
        {

        }
        if(pBubble != nullptr)
        {
            pChatItem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
        }
    }

     ui->chat_edit->setFocus();
}

