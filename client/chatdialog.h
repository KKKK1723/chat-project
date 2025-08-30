#ifndef CHATDIALOG_H
#define CHATDIALOG_H
#include"global.h"
#include <QDialog>
#include<QVector>

namespace Ui {
class ChatDialog;
}

class StateWidget;
class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    bool ShowSearch(bool b);
    void addChatUserList();
    QVector<StateWidget*> lb_vector;
    void AddLBGroup(StateWidget* lb);
private:
    Ui::ChatDialog *ui;
    ChatUIMode _mode;
    ChatUIMode _state;
    bool _b_loading;

private slots:
    void slot_loading_chat_user();
    void slot_side_chat();
    void slot_side_contact();
    void ClearLabelState(StateWidget *lb);
    void ClearAllLabelState();
};

#endif // CHATDIALOG_H
