#ifndef CUBEBLOCK_H
#define CUBEBLOCK_H

#include "axis.h"
#include "cubevalue.h"

class CubeBlock
{
public:
    CubeBlock(int xx, int yy, int zz, Axis * a);

public:
    // 当前位置
    int pos[3];
    int colorIndex[6]; // 最多含三个有效数据
    int position; // 状态位置
    // 正确位置：复原状态下的位置
    int correct[3];
    int correctColorIndex[6];
    int number;
    // 位置类型
    PositionType positionType;
    // 绘图排序标签
    int paintTag;

    // 坐标系
    Axis * axis;

    // 以下是动画过程变量
    // 下一位置
    int next[3];
    int nextColorIndex[6];

    // 方块顶点的空间坐标
    Point3 point[8];
    // 方块面中心的空间坐标
    Point3 faceCentral[6];
    // 方块体中心的空间坐标
    Point3 valumnCentral;
    // 方块面向量
    Vector faceVector[6];
    // 方块顶点的投影坐标
    QPoint projectedPoint[8];
    // 方块面的投影四边形
    QPolygon projectedFace[6];
    // 方块面的投影四边形可视性
    bool projectedFaceVisible[6];

    // 旋转信息
    // 是否正在旋转
    bool isRotating;
    // 旋转方法
    void (*rotatingFunc)(float, Point3 &);
    // 旋转刻度
    float singleRotatingAngle;

public:
    // 比较函数，用于重排魔方
    bool operator < (const CubeBlock & block) const;

private:
    // 重算标签
    void recalculateTag();
    // 重算标准位置下的方块顶点坐标
    void recalculatePoint();
    // 获取方块信息
    void getFaceInfo();
    // 获取下一旋转位置
    void getNextPosition(RotateType rotateType);
    // 设置为下一位置
    void setNextPosition();

public:
    // 重设当前方块信息
    void attachPAS(pas & msg);
    // 获取方向值
    int getDelta();
    // 是否处于正确位置
    bool isInCorrectPosition();
    // 链接旋转信息
    void setRotatingMesg(void (*func)(float, Point3 &), float singleAngle);
    // 旋转
    void rotate(moveType move, int moment);
    // 复位
    void reset();
    // 绘图
    void draw(QPainter * painter, int paintingType);
};

typedef QList<CubeBlock*> cubeBlockList;

#endif // CUBEBLOCK_H
