#include "searchlist.h"
#include <QScrollBar>
#include "tcpmgr.h"
#include "logindialog.h"
#include "userdata.h"
#include "usermgr.h"
#include "listitembase.h"
#include "adduseritem.h"
#include "findsuccessdialog.h"
#include "CustomizeEdit.h"
#include <QJsonDocument>
#include "findfaildlg.h"
#include "findsuccessdialog.h"
#include "loadingdlg.h"

SearchList::SearchList(QWidget *parent) : QListWidget(parent), _find_dlg(nullptr), _search_edit(nullptr), _send_pending(false)
{
    Q_UNUSED(parent);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    this->viewport()->installEventFilter(this);
    // 连接点击的信号和槽
    connect(this, &QListWidget::itemClicked, this, &SearchList::slot_item_clicked);
    // 添加条目
    addTipItem();
    // 连接搜索结果
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_user_search, this, &SearchList::slot_user_search);
}

void SearchList::CloseFindDlg()
{
    if (_find_dlg)
    {
        _find_dlg->hide();
        _find_dlg = nullptr;
    }
}

void SearchList::SetSearchEdit(QWidget *edit)
{
    _search_edit = edit;
}

void SearchList::waitPending(bool pending)
{
    qDebug() << "=== waitPending 被调用，pending=" << pending << " ===";
    if (pending)
    {
        qDebug() << "显示加载对话框";
        _loadingDialog = new LoadingDlg(this);
        _loadingDialog->setModal(true);
        _loadingDialog->show();
        _send_pending = pending;
    }
    else
    {
        qDebug() << "隐藏加载对话框";
        _loadingDialog->hide();
        _loadingDialog->deleteLater();
        _send_pending = pending;
    }
}

void SearchList::addTipItem()
{
    auto *invalid_item = new QWidget();
    QListWidgetItem *item_tmp = new QListWidgetItem;
    // qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item_tmp->setSizeHint(QSize(250, 10));
    this->addItem(item_tmp);
    invalid_item->setObjectName("invalid_item");
    this->setItemWidget(item_tmp, invalid_item);
    item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable);

    auto *add_user_item = new AddUserItem();
    QListWidgetItem *item = new QListWidgetItem;
    // qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(add_user_item->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, add_user_item);
}

void SearchList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = this->itemWidget(item); // 获取自定义widget对象
    if (!widget)
    {
        qDebug() << "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase *>(widget);
    if (!customItem)
    {
        qDebug() << "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if (itemType == ListItemType::INVALID_ITEM)
    {
        qDebug() << "slot invalid item clicked ";
        return;
    }

    if (itemType == ListItemType::ADD_USER_TIP_ITEM)
    {

        if (_send_pending)
        {
            return;
        }

        if (!_search_edit)
        {
            qDebug() << "slot_item_clicked _search_edit is false";
            return;
        }
        waitPending(true);
        auto search_edit = dynamic_cast<CustomizeEdit *>(_search_edit);
        if (!search_edit)
        {
            qDebug() << "dynamic_cast failed for search_edit";
            waitPending(false);
            return;
        }
        auto uid_str = search_edit->text();

        // 发送请求给server
        QJsonObject jsonObj;
        jsonObj["uid"] = uid_str;

        // 压缩
        QJsonDocument doc(jsonObj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

        // 发送tcp请求给chat server
        emit TcpMgr::GetInstance() -> sig_send_data(ReqId::ID_SEARCH_USER_REQ, jsonData);

        return;
    }
    // 创建一个测试用的 SearchInfo
    auto testSearchInfo = std::make_shared<SearchInfo>(123, "测试用户", "测试昵称", "测试描述", 1, ":/chat_img/boy.png");

    // 先关闭之前的对话框
    // if (_find_dlg) {
    //     _find_dlg->hide();
    //     _find_dlg = nullptr;
    // }

    // qDebug() << "Creating new FindSuccessDialog...";
    // _find_dlg = std::make_shared<FindSuccessDialog>(this);

    // if (!_find_dlg) {
    //     qDebug() << "ERROR: Failed to create FindSuccessDialog!";
    //     return;
    // }

    // qDebug() << "FindSuccessDialog created successfully";

    // // 使用类型转换调用 SetSearchInfo
    // auto findDialog = std::dynamic_pointer_cast<FindSuccessDialog>(_find_dlg);
    // if (findDialog) {
    //     qDebug() << "Type cast successful, calling SetSearchInfo...";
    //     findDialog->SetSearchInfo(testSearchInfo);
    //     qDebug() << "SetSearchInfo completed, showing dialog...";
    //     _find_dlg->show();
    //     qDebug() << "Dialog shown successfully";
    // } else {
    //     qDebug() << "ERROR: Type cast failed!";
    //     _find_dlg = nullptr;
    // }
    //      return;
    //  }

    // //清除弹出框
    CloseFindDlg();
}

void SearchList::slot_user_search(std::shared_ptr<SearchInfo> si)
{
    qDebug() << "=== slot_user_search 被调用 ===";
    qDebug() << "SearchInfo是否为空：" << (si == nullptr);
    if (si)
    {
        qDebug() << "用户名：" << si->_name;
        qDebug() << "用户ID：" << si->_uid;
    }

    waitPending(false);
    if (si == nullptr)
    {
        qDebug() << "创建FindFailDlg失败对话框";
        _find_dlg = std::make_shared<FindFailDlg>(this);
    }
    else
    {
        qDebug() << "创建FindSuccessDialog成功对话框";
        // 如果是自己，暂且先直接返回，以后看逻辑扩充
        //  auto self_uid = UserMgr::GetInstance()->GetUid();
        //  if (si->_uid == self_uid) {
        //      return;
        //  }
        //  //此处分两种情况，一种是搜多到已经是自己的朋友了，一种是未添加好友
        //  //查找是否已经是好友
        //  bool bExist = UserMgr::GetInstance()->CheckFriendById(si->_uid);
        //  if(bExist){
        //      //此处处理已经添加的好友，实现页面跳转
        //      //跳转到聊天界面指定的item中
        //      emit sig_jump_chat_item(si);
        //      return;
        //  }
        //  //此处先处理为添加的好友
        _find_dlg = std::make_shared<FindSuccessDialog>(this);
        std::dynamic_pointer_cast<FindSuccessDialog>(_find_dlg)->SetSearchInfo(si);
    }
    qDebug() << "准备显示对话框";
    _find_dlg->show();
    qDebug() << "对话框已显示";
}
bool SearchList::eventFilter(QObject *watched, QEvent *event)
{
    // 检查事件是否是鼠标悬浮进入或离开
    if (watched == this->viewport())
    {
        if (event->type() == QEvent::Enter)
        {
            // 鼠标悬浮，显示滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        else if (event->type() == QEvent::Leave)
        {
            // 鼠标离开，隐藏滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    // 检查事件是否是鼠标滚轮事件
    if (watched == this->viewport() && event->type() == QEvent::Wheel)
    {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15; // 计算滚动步数

        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

        return true; // 停止事件传递
    }

    return QListWidget::eventFilter(watched, event);
}
