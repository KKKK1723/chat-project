#ifndef CHATDIALOG_H
#define CHATDIALOG_H
#include "global.h"
#include <QDialog>
#include <QVector>
#include <QList>
#include "statewidget.h"
#include <memory>
#include "userdata.h"
#include <QListWidgetItem>
#include <QTimer>

namespace Ui
{
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
    QVector<StateWidget *> lb_vector;
    void AddLBGroup(StateWidget *lb);
    void handleGlobalMousePress(QMouseEvent *event);

private:
    Ui::ChatDialog *ui;
    ChatUIMode _mode;
    ChatUIMode _state;
    bool _b_loading;
    QWidget *_last_widget;
    QList<StateWidget *> _lb_list;
    QMap<int, QListWidgetItem *> _chat_items_added;
    int _cur_chat_uid;
    QTimer *_timer;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private slots:
    void slot_loading_chat_user();
    void slot_side_chat();
    void slot_side_contact();
    void slot_side_setting();
    void slot_text_changed(const QString &str);
    void slot_focus_out();
    void slot_loading_contact_user();
    void slot_switch_apply_friend_page();
    void slot_show_search(bool show);
    void ClearLabelState(StateWidget *lb);
    void ClearAllLabelState();
};

#endif // CHATDIALOG_H
