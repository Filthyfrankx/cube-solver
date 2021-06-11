#ifndef CUBE_H
#define CUBE_H

#include "axis.h"
#include "cubevalue.h"
#include "cubeblock.h"
#include "mypushbutton.h"
#include "log.h"
#include <QMainWindow>
#include <QThread>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTime>

class Cube : public QObject
{
    Q_OBJECT
public:
    explicit Cube(Axis * a, QWidget *parent = nullptr);
    ~Cube();

public:
    // 主窗口
    QWidget * mwin;
    // 坐标系
    Axis * axis;

    // 历史记录
    QStringList history;
    // 魔方方块列表
    bool isBlockInit; // 是否进行了初始化
    cubeBlockList blockList;

    // 等待旋转列表
    moveList movingList;
    cubeBlockList waitingForRotate;

    // 当前旋转信息
    bool isEnableRotating;
    bool isRotating; // 是否正在旋转
    CubeRotateAxis rotatingAxis; // 旋转轴
    int rotatingLayer[3]; // 绘图(旋转)次序
    QTimer * timer; // 旋转定时器(线程)
    int moment; // 旋转时刻

    // 画笔
    QPen pen;


private:
    // 获取需要旋转的方块列表
    void getRotatingBlocks(moveType move);

    QSerialPort *serialport;//串口相关

public:
    // 是否可解
    bool isSoluteble();
    // 是否处于复原状态
    bool isInResetStatus();
    // 重构魔方
    void reset(bool isClearHistroy);
    // 打乱魔方
    QStringList disorder(int steps = -1);
    // 旋转
    void rotate(const QString & rotateStr);
    // 绘图
    void draw(QPainter * painter);
    // 生成标准状态值
    stateType state();
    // 获取颜色信息表
    void getPattern(patternType & pattern);

signals:
    // 更新颜色状态表
    void updatePattern();
    // 停止旋转
    void endRotating();

public slots:
};

#endif // CUBE_H
