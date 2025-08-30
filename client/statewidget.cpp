#include "statewidget.h"
#include <QPaintEvent>
#include <QStyleOption>
#include <QPainter>
#include <QLabel>
#include <QVBoxLayout>
#include<QCursor>

StateWidget::StateWidget(QWidget *parent):QWidget(parent),_curstate(ClickLbState::Normal)
{
    setCursor(Qt::PointingHandCursor);

    AddRedPoint();
}

void StateWidget::SetState(QString normal, QString hover, QString press,
                           QString select, QString select_hover, QString select_press)
{
    _normal = normal;
    _normal_hover = hover;
    _normal_press = press;

    _selected = select;
    _selected_hover = select_hover;
    _selected_press = select_press;

    setProperty("state",_normal);
    repolish(this);
}

void StateWidget::AddRedPoint()
{
    _red_point=new QLabel();
    _red_point->setObjectName("red_point");
    _red_point->setAlignment(Qt::AlignCenter);//水平居中

    QPixmap red_mp(":/chat_img/red_point.png");
    QPixmap red_mpp=red_mp.scaled(_red_point->size(),Qt::KeepAspectRatio);
    _red_point->setScaledContents(true);
    _red_point->setPixmap(red_mpp);

    QVBoxLayout *layout=new QVBoxLayout();
    layout->addWidget(_red_point);
    layout->setContentsMargins(0,0,0,0);

    this->setLayout(layout);
    _red_point->setVisible(false);
}

ClickLbState StateWidget::GetCurState(){
    return _curstate;
}

void StateWidget::ShowRedPoint(bool show)
{
    _red_point->setVisible(true);
}

void StateWidget::SetSelected(bool s)
{
    if(s)
    {
        _curstate = ClickLbState::Selected;
        setProperty("state",_selected);
        repolish(this);
        update();
    }
    else
    {
        _curstate = ClickLbState::Normal;
        setProperty("state",_normal);
        repolish(this);
        update();
    }


}


void StateWidget::ClearState()
{
    _curstate = ClickLbState::Normal;
    setProperty("state",_normal);
    repolish(this);
    update();
}

void StateWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if(_curstate == ClickLbState::Selected){
            qDebug()<<"PressEvent , already to selected press: "<< _selected_press;
            QWidget::mousePressEvent(event);
            return;
        }

        if(_curstate == ClickLbState::Normal){
             qDebug()<<"PressEvent , change to selected press: "<< _selected_press;
            _curstate = ClickLbState::Selected;
            setProperty("state",_selected_press);
            repolish(this);
            update();
        }

        return;
    }

    QWidget::mousePressEvent(event);
}

void StateWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if(_curstate == ClickLbState::Normal){
            setProperty("state",_normal_hover);
            repolish(this);
            update();

        }else{
            setProperty("state",_selected_hover);
            repolish(this);
            update();
        }
        emit clicked();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void StateWidget::enterEvent(QEnterEvent *event)
{
    // 在这里处理鼠标悬停进入的逻辑
    if(_curstate == ClickLbState::Normal){
        qDebug()<<"enter , change to normal hover: "<< _normal_hover;
        setProperty("state",_normal_hover);
        repolish(this);
        update();

    }else{
        qDebug()<<"enter , change to selected hover: "<< _selected_hover;
        setProperty("state",_selected_hover);
        repolish(this);
        update();
    }

    QWidget::enterEvent(event);
}

void StateWidget::leaveEvent(QEvent *event)
{
    if(_curstate == ClickLbState::Normal){
        // qDebug()<<"leave , change to normal : "<< _normal;
        setProperty("state",_normal);
        repolish(this);
        update();

    }else{
        // qDebug()<<"leave , change to select normal : "<< _selected;
        setProperty("state",_selected);
        repolish(this);
        update();
    }
    QWidget::leaveEvent(event);
}

void StateWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    return;
}
