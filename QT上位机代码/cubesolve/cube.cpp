#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "solution.h"
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QFileDialog>
#include <QMouseEvent>
#include <QDebug>
#include "cube.h"
#include <QMessageBox>

Cube::Cube(Axis * a, QWidget * parent)
{
    // 变量绑定
    axis = a;
    mwin = parent;

    // 变量初始化
    isEnableRotating = true;
    isRotating = false;
    isBlockInit = false;
    rotatingLayer[0] = -5;
    rotatingLayer[1] = -5;
    rotatingLayer[2] = -5;
    moment = 0;

    // 创建"旋转动画"定时器
    timer = new QTimer();
    connect(timer, &QTimer::timeout, [=](){
        // 一次旋转开始
        static moveType currentMove;
        if (0 == moment)
        {
            // 取出首个待转步骤
            currentMove = movingList.at(0);
            movingList.removeFirst();
            // 解析信息，筛选旋转方块，并绑定旋转方法
            getRotatingBlocks(currentMove);
        }
        // 持续旋转方块
        foreach (CubeBlock * block, waitingForRotate)
        {
            block->rotate(currentMove, moment);
        }
        moment++;
        // 一次旋转完毕
        if (CubeValue::CUBE_ROTATE_FPS < moment)
        {
            moment = 0;
            // 如果旋转列表中没有旋转步骤则停止旋转
            if (movingList.isEmpty())
            {
                timer->stop();
                isRotating = false;
                emit endRotating();
            }
            else emit updatePattern();
        }
        // 更新显示
        mwin->update();
    });
    // 设置绘图画笔
    pen.setBrush(Qt::black);
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(1);
    // 生成魔方
    reset(true);
}

Cube::~Cube()
{
    delete timer;
    for (int i = 0; i < 27; i++) delete blockList[i];
}

bool compareBlock(CubeBlock * block1, CubeBlock * block2)
{
    return block1->paintTag < block2->paintTag;
}

// 获取需要旋转的方块列表
void Cube::getRotatingBlocks(moveType move)
{
    // 旋转类型
    RotateType type = CubeValue::MOVE2TYPE[move];
    // 旋转面
    int face = move / 3;
    // 旋转方法
    void (*func)(float, Point3&) = CubeValue::ROTATING_FUNC[type / 3];
    // 旋转角
    float angle = CubeValue::ROTATE_ANGLE[type % 3];
    // 旋转层(可能多层旋转)
    int axisRange[3] = { -5 };

    // 解析旋转层
    if (1 == face || 2 == face || 4 == face)
    {
        axisRange[0] = -1, axisRange[1] = -5, axisRange[2] = -5;
    }
    else if (12 == face || 13 == face || 14 == face)
    {
        axisRange[0] = 0, axisRange[1] = -5, axisRange[2] = -5;
    }
    else if (0 == face || 3 == face || 5 == face)
    {
        axisRange[0] = 1, axisRange[1] = -5, axisRange[2] = -5;
    }
    else if (7 == face || 8 == face || 10 == face)
    {
        axisRange[0] = -1, axisRange[1] = 0, axisRange[2] = -5;
    }
    else if (6 == face || 9 == face || 11 == face)
    {
        axisRange[0] = 0, axisRange[1] = 1, axisRange[2] = -5;
    }
    else if (15 == face || 16 == face || 17 == face)
    {
        axisRange[0] = -1, axisRange[1] = 0, axisRange[2] = 1;
    }

    // 解析旋转轴与旋转次序
    rotatingAxis = CubeRotateAxis(type / 3);
    rotatingLayer[0] = (rotatingAxis == YAxis)? 1:-1;
    rotatingLayer[1] = 0;
    rotatingLayer[2] = -rotatingLayer[0];

    // 寻找需要旋转的方块，并绑定旋转方法与旋转角
    waitingForRotate.clear();
    foreach (CubeBlock * block, blockList)
    {
        int p = block->pos[rotatingAxis];
        if (p == axisRange[0] || p == axisRange[1] || p == axisRange[2])
        {
            block->isRotating = true;
            block->setRotatingMesg(func, angle / CubeValue::CUBE_ROTATE_FPS);
            waitingForRotate.append(block);
        }
        else block->isRotating = false;
    }
}

// 是否可解
bool Cube::isSoluteble()
{
    // 12个棱块的编号+8个角块的编号
    int series[20] = {0};
    // 棱块/角块编号序列逆序性
    int s_edge = 0, s_corner = 0;
    // 棱块/角块方向值总和
    int d_edge = 0, d_corner = 0;
    foreach (CubeBlock * block, blockList)
    {
        // 按照位置顺序记录棱块的角块的编号
        int i = block->position;
        if (block->positionType != CentralBlock) series[i] = block->number;
        // 累加棱块/角块方向值
        int d = block->getDelta();
        if (block->positionType == EdgeBlock) d_edge += d;
        else if (block->positionType == CornerBlock) d_corner += d;
    }
    // 要求1、2：棱块方向值总和是偶数，角块方向值总和是3的倍数
    if (0 != d_edge % 2 || 0 != d_corner % 3) return false;
    // 计算棱块编号序列逆序性
    for (int i = 0; i < 12; i++) for (int j = i + 1; j < 12; j++) s_edge ^= series[i] > series[j];
    // 计算角块编号序列逆序性
    for (int i = 12; i < 20; i++) for (int j = i + 1; j < 20; j++) s_corner ^= series[i] > series[j];
    // 要求3：棱块编号序列逆序性与角块编号序列逆序性一致
    if (s_edge != s_corner) return false;
    return true;
}

// 是否复原
bool Cube::isInResetStatus()
{
    foreach (CubeBlock * block, blockList)
    {
        if (!block->isInCorrectPosition()) return false;
    }
    return true;
}

// 重构魔方
void Cube::reset(bool isClearHistroy)
{
    if (isClearHistroy) history.clear();

    // 重排魔方
    if (isBlockInit)
    {
        foreach (CubeBlock * block, blockList) block->reset();
    }
    // 新建魔方
    else
    {
        for (int x = -1; x <= 1; x++)
        {
            for (int y = -1; y <= 1; y++)
            {
                for (int z = -1; z <= 1; z++)
                {
                    CubeBlock * block = new CubeBlock(x, y, z, axis);
                    blockList.append(block);
                }
            }
        }
        isBlockInit = true;
    }
}

// 打乱魔方
QStringList Cube::disorder(int steps)
{
    int length = steps, move = -1, face = -1;
    // 随机步长
    if (-1 == length) length = 17 + qrand() % 3;
    QStringList list;
    // 生成打乱步骤，并保证相邻两个步骤不可逆
    for (int i = 0; i < length;)
    {
        move = qrand() % 18;
        // 是否固定顶层
        if (1 == CubeValue::CUBE_TOPLAYER_FIX && 0 <= move && move < 3) continue;
        if (face == move / 3) continue;
        face = move / 3;
        list << CubeValue::CUBE_ROTATE_TYPE[move];
        i++;
    }
    // 执行步骤
//    for (int i = 0; i < list.length(); i++)
//        rotate(list[i]);

    return list;
}

// 旋转
void Cube::rotate(const QString & rotateStr)
{
    moveType move = CubeValue::ROTATE2MOVE[rotateStr];
    // 呈现旋转动画
    if (isEnableRotating)
    {
        // 旋转操作的列表
        movingList.append(move);
        // 如果没有启动旋转定时器，便启动
        if (!isRotating)
        {
            isRotating = true;
            moment = 0;
            timer->start(CubeValue::CUBE_ROTATE_FPS_TIME);
        }
    }
    // 禁止旋转动画
    else
    {
        // 获取旋转的方块，并绑定旋转方法与旋转角
        getRotatingBlocks(move);
        // 应用旋转
        foreach (CubeBlock * block, waitingForRotate)
        {
            block->rotate(move, 0);
            block->rotate(move, CubeValue::CUBE_ROTATE_FPS);
        }
    }
    history.append(rotateStr);

    /********************************/
//    if(MainWindow::isPortEnable)
//    {
//        QByteArray* temp_array;
//        temp_array = MainWindow::convert_command(rotateStr);
//        serialport->write(*temp_array);
//        MainWindow::sleep(1000);//延时
//        delete temp_array;
//        qDebug()<<"命令发送到下位机";
//    }
}

// 绘图
void Cube::draw(QPainter * painter)
{
    // 按照绘图顺序进行排序
    std::sort(blockList.begin(), blockList.end(), &compareBlock);
    painter->setPen(pen);
    if (isRotating)
    {
        // 按层绘制方块
        int count = 0;
        for (int x = 0; x < 3 && count < 27; x++)
        {
            int p = rotatingLayer[x];
            // 绘制方块底色
            for (int n = 0; n < 27 && count < 27; n++)
            {
                CubeBlock * block = blockList[n];
                if (p == block->pos[rotatingAxis])
                {
                    block->draw(painter, 0x01);
                }
            }
            // 绘制方块表色
            for (int n = 0; n < 27 && count < 27; n++)
            {
                CubeBlock * block = blockList[n];
                if (p == block->pos[rotatingAxis])
                {
                    block->draw(painter, 0x02);
                    count++;
                }
            }
        }
    }
    else
    {
        for (int n = 0; n < 27; n++)
        {
            CubeBlock * block = blockList[n];
            block->draw(painter, 0x03);
        }
    }
}

// 生成标准状态值
stateType Cube::state()
{
    stateType standardState(20);
    for (int n = 0; n < 27; n++)
    {
        CubeBlock * block = blockList[n];
        if (CentralBlock == block->positionType) continue;
        // 魔方的位置p处是原本s的方块
        int p = block->position;
        standardState[p].s.pos = block->number;
        int d = block->getDelta();
        if (d == 3) return {}; // 无效的方向值
        standardState[p].s.det = d;
    }
    return standardState;
}

// 获取颜色信息表
void Cube::getPattern(patternType &pattern)
{
    int index, x, y, z, d;
    foreach (CubeBlock* block, blockList)
    {
        x = block->pos[0];
        y = block->pos[1];
        z = block->pos[2];
        d = block->getDelta();
        // 获取block的某个面在standardState中的索引，存入当前的面的标识
        if (z == 1)
        {
            index = x - 3 * y + 4; // index=[0:9)
            pattern[index / 3][index % 3 + 3] = patternElementType(d, block->colorIndex[0]);
        }
        else if (z == -1)
        {
            index = x + 3 * y + 49; // index=[45:54)
            pattern[index / 3 - 9][index % 3 + 3] = patternElementType(d, block->colorIndex[1]);
        }
        if (y == -1)
        {
            index = x - 3 * z + 22; // index=[18:27)
            pattern[index / 3 - 3][index % 3 + 3] = patternElementType(d, block->colorIndex[2]);
        }
        else if (y == 1)
        {
            index = 40 - x - 3 * z; // index=[36:45)
            pattern[index / 3 - 9][index % 3 + 9] = patternElementType(d, block->colorIndex[3]);
        }
        if (x == -1)
        {
            index = 13 - y - 3 * z; // index=[9:18)
            pattern[index / 3][index % 3] = patternElementType(d, block->colorIndex[4]);
        }
        else if (x == 1)
        {
            index = y - 3 * z + 31; // index=[27:36)
            pattern[index / 3 - 6][index % 3 + 6] = patternElementType(d, block->colorIndex[5]);
        }
    }
}
