#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QPushButton>

class MyPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit MyPushButton(QWidget *parent = Q_NULLPTR);

signals:
    // 新建点击信号
    void myClicked(const QString & text);

public slots:
    // 接管原点击信号
    void clickedSlot(bool checked);

};

#endif // MYPUSHBUTTON_H
