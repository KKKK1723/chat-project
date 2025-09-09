#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QIcon>
#include <QLineEdit>
#include <QRandomGenerator>
#include "chatuserwid.h"
#include <QPixmap>
#include "statewidget.h"
#include <QMouseEvent>
#include <QTimer>

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ChatDialog), _b_loading(false), _mode(ChatUIMode::ChatMode), _state(ChatUIMode::ChatMode), _last_widget(nullptr), _cur_chat_uid(0)
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

    ui->side_chat_lb->setProperty("state", "normal");
    ui->side_chat_lb->SetState("normal", "hover", "pressed", "selected_normal", "selected_hover", "selected_pressed");
    ui->side_chat_lb->setProperty("state", "selected_normal");

    ui->side_contact_lb->SetState("normal", "hover", "pressed", "selected_normal", "selected_hover", "selected_pressed");

    QPixmap mp(":/chat_img/boy.png");
    QPixmap mpp = mp.scaled(ui->side_head_lb->size(), Qt::KeepAspectRatio);
    ui->side_head_lb->setPixmap(mpp);
    ui->side_head_lb->setScaledContents(true);

    AddLBGroup(ui->side_chat_lb);
    AddLBGroup(ui->side_contact_lb);

    connect(ui->side_chat_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_chat);
    connect(ui->side_contact_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_contact);

    // 连接清除搜索框操作
    // connect(ui->friend_apply_page, &ApplyFriendPage::sig_show_search, this, &ChatDialog::slot_show_search);

    // 设置当前页面
    ui->stackedWidget->setCurrentWidget(ui->chat_page);

    // 设置搜索列表的搜索编辑框
    ui->search_list->SetSearchEdit(ui->search_edit);

    // 显示搜索列表
    connect(ui->search_edit, &QLineEdit::textChanged, this, [this]()
            {
        if(!ui->search_edit->text().isEmpty())
        {
            ClearAllLabelState();
            ShowSearch(true);
        } });

    this->installEventFilter(this);

    qDebug() << "stack idx =" << ui->stackedWidget->currentIndex()
             << "current =" << ui->stackedWidget->currentWidget()->objectName()
             << "chat_page type =" << ui->chat_page->metaObject()->className()
             << "friend_apply_page type =" << ui->friend_apply_page->metaObject()->className();
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

void ChatDialog::AddLBGroup(StateWidget *lb)
{
    lb_vector.emplace_back(lb);
}

bool ChatDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *e = static_cast<QMouseEvent *>(event);
        handleGlobalMousePress(e);
    }

    return QDialog::eventFilter(watched, event);
}

void ChatDialog::handleGlobalMousePress(QMouseEvent *event)
{
    if (_mode != ChatUIMode::SearchMode)
    {
        return;
    }

    QPoint posInSearchList = (ui->search_list->mapFromGlobal(event->globalPosition())).toPoint();

    if (!ui->search_list->rect().contains(posInSearchList))
    {
        ui->search_edit->clear();
        ShowSearch(false);
    }
}

void ChatDialog::slot_loading_chat_user()
{
    addChatUserList();
}

void ChatDialog::slot_side_chat()
{
    qDebug() << "receive side chat clicked";
    ClearLabelState(ui->side_chat_lb);
    ui->stackedWidget->setCurrentWidget(ui->chat_page);
    _state = ChatUIMode::ChatMode;
    ShowSearch(false);
}

void ChatDialog::slot_side_contact()
{
    qDebug() << "receive side contact clicked";
    ClearLabelState(ui->side_contact_lb);
    ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
    _state = ChatUIMode::ContactMode;
    ShowSearch(false);
}

void ChatDialog::ClearLabelState(StateWidget *lb)
{
    for (auto &ele : lb_vector)
    {
        if (ele == lb)
        {
            continue;
        }
        ele->ClearState();
    }
}

void ChatDialog::ClearAllLabelState()
{
    for (auto &ele : lb_vector)
    {
        ele->ClearState();
    }
}

void ChatDialog::slot_side_setting()
{
    // qDebug()<< "receive side setting clicked";
    // ClearLabelState(ui->side_settings_lb);
    // _state = ChatUIMode::SettingsMode;
    // ShowSearch(false);
}

void ChatDialog::slot_text_changed(const QString &str)
{
    if (!str.isEmpty())
    {
        ShowSearch(true);
    }
}

void ChatDialog::slot_focus_out()
{
    qDebug() << "receive focus out signal";
    ShowSearch(false);
}

void ChatDialog::slot_loading_contact_user()
{
    qDebug() << "slot loading contact user";
}

void ChatDialog::slot_switch_apply_friend_page()
{
    // qDebug()<<"receive switch apply friend page sig";
    // _last_widget = ui->friend_apply_page;
    // ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
}

void ChatDialog::slot_show_search(bool show)
{
    ShowSearch(show);
}
