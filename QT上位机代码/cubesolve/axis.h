#ifndef AXIS_H
#define AXIS_H

#include "cubevalue.h"

// 坐标系
class Axis
{
public:
    Axis(QPoint point);

public:
    // 原点偏移
    QPoint offset;
    // 放缩系数
    float zoom;
    // 投影方向
    Vector view;
    // 旋转角度
    float angle;
    // 画笔
    QPen dashPen;
    QPen solidPen;
    QBrush brush;

public:
    // 获取投影点坐标
    QPoint getProject(const Point3 & point);
    // 绘制坐标系
    void draw(QPainter * painter);
    void setView(const Vector &value);
    void setZoom(float value);
    void setOffset(const QPoint &value);
};

#endif // AXIS_H
