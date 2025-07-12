#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    //布局中加入两个页面
    _stackedLayout->addWidget(_login_dialog);
    _stackedLayout->addWidget(_register_dialog);

    //容器放入主窗口中
    setCentralWidget(_centralWidget);
    _stackedLayout->setCurrentWidget(_login_dialog);

    connect(_login_dialog,&LoginDialog::SwithRegister,this,&MainWindow::SlotSwitchRegister);//主页面转注册页面
    connect(_register_dialog,&RegisterDialog::SwitchLogin,this,&MainWindow::SlotSwitchLogin);//注册页面转主页面

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
    _stackedLayout->setCurrentWidget(_register_dialog);
}

void MainWindow::SlotSwitchLogin()//注册页面切换到登录页面
{
    _stackedLayout->setCurrentWidget(_login_dialog);
}
