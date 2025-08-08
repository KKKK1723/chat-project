#ifndef TIMERBTN_H
#define TIMERBTN_H
#include<QPushButton>
#include<QTimer>

class RegisterDialog;
class TimerBtn:public QPushButton
{
    Q_OBJECT
public:
    TimerBtn(QWidget *parent = nullptr);
    ~TimerBtn();
    void mouseReleaseEvent(QMouseEvent *e) override;
    QTimer *_timer;
    int _dex;

signals:
    void CheckVerifyIsEmpty();
};

#endif // TIMERBTN_H
