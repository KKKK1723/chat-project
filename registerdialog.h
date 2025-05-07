#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private:
    void showTip(QString str,bool b_ok);
    Ui::RegisterDialog *ui;

signals:
    void SwitchLogin();//通知主界面要从注册页面更新为主页面
private slots:
    void on_varify_btn_clicked();
};

#endif // REGISTERDIALOG_H
