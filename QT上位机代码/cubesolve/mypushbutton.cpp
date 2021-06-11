#include "mypushbutton.h"

MyPushButton::MyPushButton(QWidget *parent): QPushButton(parent)
{
    connect(this, &QPushButton::clicked, this, &MyPushButton::clickedSlot);
}

// 接管原点击信号
void MyPushButton::clickedSlot(bool checked)
{
    checked = checked;
    emit myClicked(this->text());
}
