#include "timerbtn.h"
#include "global.h"

TimerBtn::TimerBtn(QWidget *parent) : QPushButton(parent), _dex(10)
{
    _timer = new QTimer(this);

    connect(_timer, &QTimer::timeout, [this]()
            {
        if(_dex>0)
        {
            this->setText(QString::number(_dex));
            this->setEnabled(false);
            _dex--;
        }
        else
        {
            _timer->stop();
            _dex=10;
            this->setEnabled(true);
            this->setText(tr("获取"));
            return;
        } });
}

TimerBtn::~TimerBtn()
{
    _timer->stop();
}

void TimerBtn::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        //_timer->start(1000);
        emit CheckVerifyIsEmpty();
        emit clicked();
    }
    QPushButton::mouseReleaseEvent(e);
}
