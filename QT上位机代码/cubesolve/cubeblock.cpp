#include "cubevalue.h"
#include "cubeblock.h"

// 辅助表a1[type][i]：表示方块各面颜色索引的转移表，表示在旋转type下，第i面将置换为第j=a1[type][i]面
const int a1[9][6] =
{
    { 2, 3, 1, 0, 4, 5},
    { 3, 2, 0, 1, 4, 5},
    { 1, 0, 3, 2, 4, 5},
    { 5, 4, 2, 3, 0, 1},
    { 4, 5, 2, 3, 1, 0},
    { 1, 0, 2, 3, 5, 4},
    { 0, 1, 5, 4, 2, 3},
    { 0, 1, 4, 5, 3, 2},
    { 0, 1, 3, 2, 5, 4}
};
// 辅助表a2[type][i]：表示方块坐标的转移符号
const int a2[9][3] =
{
    { 1, 1,-1},
    { 1,-1, 1},
    { 1,-1,-1},
    {-1, 1, 1},
    { 1, 1,-1},
    {-1, 1,-1},
    { 1,-1, 1},
    {-1, 1, 1},
    {-1,-1, 1}
};
// 辅助表a3[type][i]：表示方块坐标分量的转移表，表示在旋转type下，第i分量将置换为a2[type][i] * 第j=a3[type][i]分量
const int a3[9][3] =
{
    { 0, 2, 1},
    { 0, 2, 1},
    { 0, 1, 2},
    { 2, 1, 0},
    { 2, 1, 0},
    { 0, 1, 2},
    { 1, 0, 2},
    { 1, 0, 2},
    { 0, 1, 2}
};

CubeBlock::CubeBlock(int xx, int yy, int zz, Axis * a)
{
    // 计算、绑定变量
    axis = a;
    correct[0] = xx, correct[1] = yy, correct[2] = zz;
    if (zz == 1) correctColorIndex[0] = 1;
    else correctColorIndex[0] = 0;
    if (zz == -1) correctColorIndex[1] = 2;
    else correctColorIndex[1] = 0;
    if (yy == -1) correctColorIndex[2] = 3;
    else correctColorIndex[2] = 0;
    if (yy == 1) correctColorIndex[3] = 4;
    else correctColorIndex[3] = 0;
    if (xx == -1) correctColorIndex[4] = 5;
    else correctColorIndex[4] = 0;
    if (xx == 1) correctColorIndex[5] = 6;
    else correctColorIndex[5] = 0;

    // 变量初始化
    isRotating = false;
    rotatingFunc = nullptr;
    singleRotatingAngle = 0.0;

    // 确定位置类型
    int i = qAbs(xx) + qAbs(yy) + qAbs(zz);
    if (3 == i) positionType = CornerBlock;
    else if (2 == i) positionType = EdgeBlock;
    else positionType = CentralBlock;

    // 绘图次序，编号，位置
    paintTag = 3 * xx - yy + 9 * zz + 14;
    number = CubeValue::CUBE_POSNUM[paintTag - 1] + (CornerBlock == positionType) * 12;
    position = number;
    reset();
}

// 重算标签
void CubeBlock::recalculateTag()
{
    // 重算绘图次序
    paintTag = 3 * pos[0] - pos[1] + 9 * pos[2] + 14;
    // 重算位置
    position = CubeValue::CUBE_POSNUM[paintTag - 1] + (CornerBlock == positionType) * 12;
}

// 重算标准位置下的方块顶点坐标
void CubeBlock::recalculatePoint()
{
    float x1, y1, z1, x2, y2, z2;
    // 标准位置下的坐标
    x1 = (pos[0] - 0.5) * CubeValue::CUBE_BLOCK_SIZE;
    x2 = (pos[0] + 0.5) * CubeValue::CUBE_BLOCK_SIZE;
    y1 = (pos[1] - 0.5) * CubeValue::CUBE_BLOCK_SIZE;
    y2 = (pos[1] + 0.5) * CubeValue::CUBE_BLOCK_SIZE;
    z1 = (pos[2] - 0.5) * CubeValue::CUBE_BLOCK_SIZE;
    z2 = (pos[2] + 0.5) * CubeValue::CUBE_BLOCK_SIZE;
    // 方块顶点的空间坐标
    point[0].set(x1, y1, z2);
    point[1].set(x2, y1, z2);
    point[2].set(x1, y1, z1);
    point[3].set(x2, y1, z1);
    point[4].set(x1, y2, z2);
    point[5].set(x2, y2, z2);
    point[6].set(x1, y2, z1);
    point[7].set(x2, y2, z1);
}

// 获取方块信息
void CubeBlock::getFaceInfo()
{
    // 方块面中心的空间坐标
    faceCentral[0].setMiddle(point[0], point[5]);
    faceCentral[1].setMiddle(point[3], point[6]);
    faceCentral[2].setMiddle(point[0], point[3]);
    faceCentral[3].setMiddle(point[5], point[6]);
    faceCentral[4].setMiddle(point[0], point[6]);
    faceCentral[5].setMiddle(point[3], point[5]);
    // 方块体中心的空间坐标
    valumnCentral.setMiddle(point[0], point[7]);
    for (int i = 0; i < 6; i++)
    {
        // 方块面向量
        faceVector[i].set(faceCentral[i], valumnCentral);
        // 获取当前魔方方块六个面的可视情况
        projectedFaceVisible[i] = (0.0 < axis->view * faceVector[i]);
        // 重置方块面的投影四边形
        projectedFace[i].clear();
    }
    // 方块顶点的投影坐标
    for (int i = 0; i < 8; i++)
    {
        projectedPoint[i] = axis->getProject(point[i]);
    }
    // 方块面的投影四边形
    projectedFace[0] << projectedPoint[0] << projectedPoint[4] << projectedPoint[5] << projectedPoint[1];
    projectedFace[1] << projectedPoint[2] << projectedPoint[6] << projectedPoint[7] << projectedPoint[3];
    projectedFace[2] << projectedPoint[0] << projectedPoint[1] << projectedPoint[3] << projectedPoint[2];
    projectedFace[3] << projectedPoint[4] << projectedPoint[5] << projectedPoint[7] << projectedPoint[6];
    projectedFace[4] << projectedPoint[0] << projectedPoint[4] << projectedPoint[6] << projectedPoint[2];
    projectedFace[5] << projectedPoint[1] << projectedPoint[5] << projectedPoint[7] << projectedPoint[3];
}

// 获取下一旋转位置
void CubeBlock::getNextPosition(RotateType rotateType)
{
    // 辅助表a1和a2，作出位置的置换表
    for (int i = 0; i < 3; i++)
    {
        int s = a2[rotateType][i];
        int j = a3[rotateType][i];
        next[i] = s * pos[j];
    }
    // 辅助表a3，作出色面的置换表
    for (int i = 0; i < 6; i++)
    {
        int j = a1[rotateType][i];
        nextColorIndex[i] = colorIndex[j];
    }
}

// 设置为下一位置
void CubeBlock::setNextPosition()
{
    pos[0] = next[0], pos[1] = next[1], pos[2] = next[2];
    for (int i = 0; i < 6; i++) colorIndex[i] = nextColorIndex[i];
}

// 重设当前方块信息
void CubeBlock::attachPAS(pas &msg)
{
    // 位置，坐标
    position = msg.p;
    next[0] = pos[0] = msg.x;
    next[1] = pos[1] = msg.y;
    next[2] = pos[2] = msg.z;
    // 色面
    for (int i = 0; i < 6; i++) nextColorIndex[i] = colorIndex[i] = msg.cd[i];
    // 绘图次序
    paintTag = 3 * pos[0] - pos[1] + 9 * pos[2] + 14;
    // 其他坐标
    recalculatePoint();
}

// 获取方向值
int CubeBlock::getDelta()
{
    if (CentralBlock == positionType) return 0;
    // 位置下的颜色记号
    QString s = CubeValue::CUBE_GOAL[position];
    // 当前的颜色记号
    QString v = s;
    for (int i = 0; i < s.length(); i++)
    {
        int index = CubeValue::CUBE_FACE.indexOf(s[i]);
        v[i] = CubeValue::CUBE_FACE[colorIndex[index] - 1];
    }
    // 复原状态下的颜色记号
    QString standard = CubeValue::CUBE_GOAL[number];
    // 计算方向值
    int d = 0;
    while (v != standard && d < 3)
    {
        v = v.right(v.length() - 1) + v[0];
        d++;
    }
    return d;
}

// 是否处于正确位置
bool CubeBlock::isInCorrectPosition()
{
    for (int i = 0; i < 6; i++)
    {
        if (colorIndex[i] != correctColorIndex[i]) return false;
    }
    return true;
}

// 链接旋转信息
void CubeBlock::setRotatingMesg(void (*func)(float, Point3 &), float singleAngle)
{
    rotatingFunc = func;
    singleRotatingAngle = singleAngle;
}

// 旋转
void CubeBlock::rotate(moveType move, int moment)
{
    // 旋转初时刻：不作旋转，获取下一位置信息
    if (0 == moment)
    {
        getNextPosition(CubeValue::MOVE2TYPE[move]);
        return;
    }
    // 旋转末时刻：位置转移，重算信息
    else if (CubeValue::CUBE_ROTATE_FPS == moment)
    {
        setNextPosition();
        recalculateTag();
        recalculatePoint();
        return;
    }
    // 旋转过程
    for (int i = 0; i < 8; i++) rotatingFunc(singleRotatingAngle, point[i]);
}

// 复位
void CubeBlock::reset()
{
    // 重置位置
    pos[0] = correct[0], pos[1] = correct[1], pos[2] = correct[2];
    next[0] = correct[0], next[1] = correct[1], next[2] = correct[2];
    // 重置色面
    for (int i = 0; i < 6; i++)
    {
        colorIndex[i] = correctColorIndex[i];
        nextColorIndex[i] = correctColorIndex[i];
    }
    // 重置编号
    position = number;
    recalculateTag();
    // 重算坐标
    recalculatePoint();
    getFaceInfo();
}

// 绘图
void CubeBlock::draw(QPainter *painter, int paintingType)
{
    int g = CubeValue::CUBE_COLOR_GROUP;
    // 获取坐标，可视面等
    getFaceInfo();
    for (int i = 0; i < 6; i++)
    {
        if (projectedFaceVisible[i])
        {
            // 绘制底色
            if (((paintingType & 0x01) && colorIndex[i] == 0) ||
            // 绘制表色
                ((paintingType & 0x02) && colorIndex[i] != 0))
            {
                painter->setBrush(CubeValue::CUBE_COLOR[g][colorIndex[i]]);
                painter->drawPolygon(projectedFace[i]);
            }
        }
    }
}

