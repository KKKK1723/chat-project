#include "clicklabel.h"

ClickLabel::ClickLabel(QWidget *parent) : QLabel(parent)
{
}

void ClickLabel::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        qDebug() << "点击";
    }
    QLabel::mousePressEvent(ev);
}
