#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QIcon>
#include <QLineEdit>
#include <QRandomGenerator>
#include "chatuserwid.h"
ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ChatDialog), _b_loading(false), _mode(ChatUIMode::ChatMode), _state(ChatUIMode::ChatMode)
{
    ui->setupUi(this);

    ui->search_edit->setPlaceholderText(QStringLiteral("搜索"));
    ui->search_edit->addAction(QIcon(":/chat_img/search (1).png"), QLineEdit::LeadingPosition);
    QAction *ClearEdit = ui->search_edit->addAction(QIcon(":/chat_img/cross1.png"), QLineEdit::TrailingPosition);
    ClearEdit->setCheckable(true);
    ClearEdit->setVisible(false);
    connect(ui->search_edit, &QLineEdit::textChanged, this, [this, ClearEdit]()
            {
        if(!ui->search_edit->text().isEmpty())
        {
            ClearEdit->setVisible(true);
        }
        else
        {
            ClearEdit->setVisible(false);
        } });
    connect(ClearEdit, &QAction::toggled, this, [this]()
            {
        if(!ui->search_edit->text().isEmpty())
        {
            ShowSearch(false);
            ui->search_edit->clear();
        } });
    ui->add_btn->setText("+");

    connect(ui->chat_user_list, &ChatUserList::sig_loading_chat_user, this, &ChatDialog::slot_loading_chat_user);
    ShowSearch(false);
    addChatUserList();
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

bool ChatDialog::ShowSearch(bool b)
{
    if (b)
    {
        ui->chat_user_list->hide();
        ui->con_user_list->hide();
        ui->search_list->show();
        _mode = ChatUIMode::SearchMode;
    }
    else if (_state == ChatUIMode::ChatMode)
    {
        ui->chat_user_list->show();
        ui->con_user_list->hide();
        ui->search_list->hide();
        _mode = ChatUIMode::ChatMode;
    }
    else if (_state == ChatUIMode::ContactMode)
    {
        ui->chat_user_list->hide();
        ui->search_list->hide();
        ui->con_user_list->show();
        _mode = ChatUIMode::ContactMode;
    }
}

std::vector<QString> strs = {"hello world !",
                             "nice to meet u",
                             "New year，new life",
                             "You have to love yourself",
                             "My love is written in the wind ever since the whole world is you"};
std::vector<QString> heads = {
    ":/chat_img/20210411125716_d7ebc.jpeg",
    ":/chat_img/20210411125716_75b2e.jpeg",
    ":/chat_img/20201126170549_c29c9.jpeg",
    ":/chat_img/20201126170544_6afe1.jpeg",
    ":/chat_img/20200520083558_ZNiBV.jpeg",
};
std::vector<QString> names = {
    "小白熊",
    "小黑熊",
    "一二",
    "布布",
    "灰灰",
    "蜜桃",
    "小懒",
    "胖熊"};

void ChatDialog::addChatUserList()
{
    // 创建QListWidgetItem，并设置自定义的widget
    for (int i = 0; i < 13; i++)
    {
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue % strs.size();
        int head_i = randomValue % heads.size();
        int name_i = randomValue % names.size();

        auto *chat_user_wid = new ChatUserWid();
        chat_user_wid->SetInfo(names[name_i], heads[head_i], strs[str_i]);
        QListWidgetItem *item = new QListWidgetItem;
        // qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(chat_user_wid->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chat_user_wid);
    }
}

void ChatDialog::slot_loading_chat_user()
{
    addChatUserList();
}
