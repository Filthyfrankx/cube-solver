#include "axis.h"
#include <QtMath>

Axis::Axis(QPoint point): offset(point), view(-1.0f, 1.0f, -1.0f)
{
    zoom = 1.20f;
    angle = 0.0f;
    dashPen.setStyle(Qt::CustomDashLine);
    QVector<qreal> dashes;
    dashes << 1 << 4 << 5 << 4;
    dashPen.setDashPattern(dashes);
    solidPen.setStyle(Qt::SolidLine);
    brush.setColor(Qt::blue);
    brush.setStyle(Qt::SolidPattern);
}

void Axis::setOffset(const QPoint &value)
{
    offset = value;
}

void Axis::setZoom(float value)
{
    zoom = value;
}

void Axis::setView(const Vector &value)
{
    view = value;
}

// 获取投影点坐标
QPoint Axis::getProject(const Point3 & point)
{
    float x, y, z, ox, oy, oz;
    float L, Lxy;
    float sinA, cosA, sinB, cosB, sinC, cosC;
    float x1, y1, z1, x2, z2, fx, fz, ex, ey;
    x = point.x;
    y = point.y;
    z = point.z;
    ox = view.x;
    oy = view.y;
    oz = view.z;
    Lxy = sqrtf(ox * ox + oy * oy);
    L = sqrtf(Lxy * Lxy + oz * oz);
    cosA = oy / Lxy;
    sinA = -ox / Lxy;
    cosB = Lxy / L;
    sinB = oz / L;
    x1 = cosA * x + sinA * y;
    y1 = cosA * y - sinA * x;
    z1 = z;
    x2 = x1;
    z2 = cosB * z1 - sinB * y1;
    fx = x2 * zoom;
    fz = z2 * zoom;
    cosC = cosf(angle);
    sinC = sinf(angle);
    ex = cosC * fx + sinC * fz;
    ey = cosC * fz - sinC * fx;
    return QPoint(offset.x() + ex, offset.y() - ey);
}

// 绘制坐标系
void Axis::draw(QPainter *painter)
{
    int w = painter->device()->width();
    int h = painter->device()->height();
    int maxLength = qMax(w, h) << 1;
    maxLength = int(maxLength * zoom);
    QPoint X = getProject(Point3(maxLength, 0, 0));
    QPoint Y = getProject(Point3(0, maxLength, 0));
    QPoint Z = getProject(Point3(0, 0, maxLength));

    // 绘制坐标轴
    painter->setPen(dashPen);
    painter->drawLine(offset, X);
    painter->drawLine(offset, Y);
    painter->drawLine(offset, Z);
}
