#ifndef CLICKLABEL_H
#define CLICKLABEL_H
#include "global.h"
#include <QLabel>
#include <QWidget>
#include <QMouseEvent>

class ClickLabel : public QLabel
{
    Q_OBJECT
public:
    ClickLabel(QWidget *parent = nullptr);

private:
    void mousePressEvent(QMouseEvent *ev) override;
};

#endif // CLICKLABEL_H
