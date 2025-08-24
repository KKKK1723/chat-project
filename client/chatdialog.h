#ifndef CHATDIALOG_H
#define CHATDIALOG_H
#include "global.h"
#include <QDialog>

namespace Ui
{
    class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    bool ShowSearch(bool b);
    void addChatUserList();

private:
    Ui::ChatDialog *ui;
    ChatUIMode _mode;
    ChatUIMode _state;
    bool _b_loading;

private slots:
    void slot_loading_chat_user();
};

#endif // CHATDIALOG_H
