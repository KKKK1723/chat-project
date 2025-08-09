#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>
#include "global.h"
#include "httpmgr.h"

namespace Ui
{
    class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();

private:
    Ui::ResetDialog *ui;
    QMap<TipErr, QString> _errmap;
    QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;
    QTimer *_timer;

private:
    bool CheckUserName();
    bool CheckEmail();
    bool CheckVarify();
    bool CheckUserPwd();

    void adderr(TipErr err, QString s);
    void deerr(TipErr err);
    void showTip(QString str, bool b_ok);
    void onEmailTextChanged(const QString &text);
    void initHttpHandlers();
signals:
    void SwitchLogin();

private slots:
    void on_verifycode_btn_clicked();
    void slot_reset_mod_finish(ReqId id, QString res, ErrorCodes err);
    void on_reset_btn_clicked();
};

#endif // RESETDIALOG_H
