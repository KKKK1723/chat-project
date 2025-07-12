#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"logindialog.h"
#include"registerdialog.h"
#include<QStackedLayout>
/*
 *  file:          mainwindow.h
 *  function:      主窗口
*/



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void SlotSwitchRegister();//进入注册页面
    void SlotSwitchLogin();//返回主页面

private:
    Ui::MainWindow *ui;
    QWidget* _centralWidget;//一个容器 装入登录和注册两个页面
    QStackedLayout* _stackedLayout;//堆叠布局 方便来回切换两个页面
    LoginDialog *_login_dialog;
    RegisterDialog *_register_dialog;
};
#endif // MAINWINDOW_H
