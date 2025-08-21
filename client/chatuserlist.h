#ifndef CHATUSERLIST_H
#define CHATUSERLIST_H
#include "global.h"
#include <QWheelEvent>
#include <QEvent>
#include <QScrollBar>
#include <QListWidget>

class ChatUserList : public QListWidget
{
    Q_OBJECT
public:
    ChatUserList(QWidget *parent = nullptr);
    ~ChatUserList();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void sig_loading_chat_user();
};

#endif // CHATUSERLIST_H
