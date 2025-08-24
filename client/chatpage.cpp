#include "chatpage.h"
#include "ui_chatpage.h"
#include <QStyle>

ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::ChatPage)
{
    ui->setupUi(this);
}

ChatPage::~ChatPage()
{
    delete ui;
}
