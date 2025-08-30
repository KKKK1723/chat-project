#ifndef STATEWIDGET_H
#define STATEWIDGET_H
#include <QWidget>
#include <QLabel>
#include "global.h"

class StateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StateWidget(QWidget *parent = nullptr);

    void SetState(QString normal = "", QString hover = "", QString press = "",
                  QString select = "", QString select_hover = "", QString select_press = "");
    void AddRedPoint();
    ClickLbState GetCurState();
    void ShowRedPoint(bool show);
    void SetSelected(bool s);
    void ClearState();
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

signals:
    void clicked(void);

private:
    QString _normal;
    QString _normal_hover;
    QString _normal_press;

    QString _selected;
    QString _selected_hover;
    QString _selected_press;

    ClickLbState _curstate;
    QLabel *_red_point;
};

#endif // STATEWIDGET_H
