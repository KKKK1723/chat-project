#include "loadingdlg.h"
#include "ui_loadingdlg.h"
#include <QLabel>
#include <QMovie>
#include <QGraphicsOpacityEffect>
#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>

LoadingDlg::LoadingDlg(QWidget *parent) : QDialog(parent),
                                          ui(new Ui::LoadingDlg)
{
    ui->setupUi(this);

    // setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground); // 设置背景透明
    // 获取屏幕尺寸
    setFixedSize(parent->size()); // 设置对话框为全屏尺寸

    QPixmap p(":/chat_img/loading.png");
    ui->label->setPixmap(p);

    
}

LoadingDlg::~LoadingDlg()
{
    delete ui;
}
