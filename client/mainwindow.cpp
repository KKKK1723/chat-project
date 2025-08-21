#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"tcpmgr.h"
#include <QSplitter>
#include <algorithm>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("YQchat"));
    setWindowIcon(QIcon(":/chat_img/20220502135506_b02ee.jpeg"));

    //初始一个装页面的容器  布局为堆叠布局
    _centralWidget=new QWidget(this);
    _stackedLayout = new QStackedLayout(_centralWidget);

    //页面初始化
    _login_dialog=new LoginDialog();
    _register_dialog=new RegisterDialog();
    _reset_dialog=new ResetDialog();
    _chat_dialog=new ChatDialog();

    //页面放入容器
    _stackedLayout->addWidget(_login_dialog);
    _stackedLayout->addWidget(_register_dialog);
    _stackedLayout->addWidget(_reset_dialog);
    _stackedLayout->addWidget(_chat_dialog);

    //容器放入主窗口中
    setCentralWidget(_centralWidget);
    _stackedLayout->setCurrentWidget(_login_dialog);

    connect(_login_dialog,&LoginDialog::SwithRegister,this,&MainWindow::SlotSwitchRegister);//登录页面转注册页面
    connect(_register_dialog,&RegisterDialog::SwitchLogin,this,&MainWindow::SlotSwitchLogin);//注册页面转登录页面
    connect(_login_dialog,&LoginDialog::SwithReset,this,&MainWindow::SlotSwitchReset);//登录界面转重置密码界面
    connect(_reset_dialog,&ResetDialog::SwitchLogin,this,&MainWindow::SlotSwitchLogin);//重置密码页面转登录页面
    connect(TcpMgr::GetInstance().get(),&TcpMgr::sig_switch_chatdialog,this,&MainWindow::SlotSwitchChat);//登录成功后进入聊天页面
    emit TcpMgr::GetInstance()->sig_switch_chatdialog();


    initChatSplitter();


}

void MainWindow::initChatSplitter()
{
    QWidget *user = _chat_dialog->findChild<QWidget*>("chat_user_wid");
    QWidget *data = _chat_dialog->findChild<QWidget*>("chat_data_wid");
    QHBoxLayout *root = _chat_dialog->findChild<QHBoxLayout*>("horizontalLayout");
    if (!(user && data && root)) return;

    QLayoutItem *item1 = root->takeAt(1);
    QLayoutItem *item2 = root->takeAt(1);
    if (item1) delete item1;
    if (item2) delete item2;

    QSplitter *splitter = new QSplitter(Qt::Horizontal, _chat_dialog);
    splitter->setChildrenCollapsible(false);
    splitter->setHandleWidth(4);
    splitter->addWidget(user);
    splitter->addWidget(data);

    root->insertWidget(1, splitter);
    user->setMinimumWidth(260);
    user->setMaximumWidth(QWIDGETSIZE_MAX);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    QList<int> sizes; sizes << 260 << 900;
    splitter->setSizes(sizes);

    // 限制左侧最大占比为 30%，同时不小于最小宽
    QObject::connect(splitter, &QSplitter::splitterMoved, _chat_dialog,
                     [splitter, user](int, int) {
        QList<int> sz = splitter->sizes();
        if (sz.size() < 2) return;
        const int total = sz[0] + sz[1];
        int minLeft = user->minimumWidth();
        int maxLeft = static_cast<int>(total * 0.7); // 左侧最多70%
        if (maxLeft < minLeft) maxLeft = minLeft;
        int left = sz[0];
        int clamped = std::clamp(left, minLeft, maxLeft);
        if (clamped != left) {
            sz[0] = clamped;
            sz[1] = total - clamped;
            splitter->setSizes(sz);
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
    if(_login_dialog)
    {
        delete _login_dialog;
    }
    if(_register_dialog)
    {
        delete _register_dialog;
    }
}

void MainWindow::SlotSwitchRegister()//登录页面切换到注册页面
{
    resize(370,450);
    setMinimumSize(370,450);
    _stackedLayout->setCurrentWidget(_register_dialog);
}

void MainWindow::SlotSwitchLogin()//注册页面切换到登录页面
{
    resize(370,450);
    setMinimumSize(370,450);
    _stackedLayout->setCurrentWidget(_login_dialog);
}

void MainWindow::SlotSwitchReset()
{
    resize(370,450);
    setMinimumSize(370,450);
    _stackedLayout->setCurrentWidget(_reset_dialog);
}

void MainWindow::SlotSwitchChat()
{
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    resize(1200,800);
    setMinimumSize(960,720);
    _stackedLayout->setCurrentWidget(_chat_dialog);
}
