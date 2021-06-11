#ifndef CUBEVALUE_H
#define CUBEVALUE_H

#include <QtMath>
#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QVector>
#include <QList>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QQueue>
#include <QPoint>
#include <QTime>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include <QException>

// 面类型
typedef int faceType;
// 步骤类型
typedef int moveType;
// 步骤列表
typedef QList<moveType> moveList;
// 状态元
typedef union BlockStatePositonDelta
{
    struct {
        unsigned char pos: 5;
        unsigned char det: 3;
    }s;
    unsigned char n;
    bool operator ==(const BlockStatePositonDelta & o) const
    {
        return n == o.n;
    }
}bspd;
// 状态类型
typedef QVector<bspd> stateType;

// id类型
typedef QVector<int> idType;

// 颜色状态表类型
typedef QVector<QVector<QPair<int, int>>> patternType;
typedef QVector<QPair<int, int>> patternSubType;
typedef QPair<int, int> patternElementType;
typedef struct patternAidStruct
{
    int x, y, z, p;
    int cd[6];
    patternAidStruct()
    {
        x = y = z = 0;
        p = -1;
        for (int i = 0; i < 6; i++) cd[i] = 0;
    }
}pas;

// 位置类型 { 心块, 边块, 角块 }
enum PositionType { CentralBlock, EdgeBlock, CornerBlock };
// 旋转类别 { 正转, 反转, 翻转 }
enum RotateType { X_P, X_N, X_D, Y_P, Y_N, Y_D, Z_P, Z_N, Z_D };
// 旋转轴 { X轴, Y轴, Z轴 }
enum CubeRotateAxis { XAxis = 0, YAxis = 1, ZAxis = 2};

// 程序错误码
enum ProgamErroCode { Normal, OverTime, UnSolveble };

// 坐标
class Point3
{
public:
    Point3(float xx = 0.0, float yy = 0.0, float zz = 0.0): x(xx), y(yy), z(zz) {}

public:
    float x, y, z;

public:
    // 设置坐标
    void set(float xx = 0.0, float yy = 0.0, float zz = 0.0);
    // 设置为中点
    void setMiddle(const Point3 & p1, const Point3 & p2);
};

// 旋转方法
typedef void (*pRotatingFunc)(float, Point3 &);

// 向量
class Vector
{
public:
    Vector(float xx = 1.0, float yy = 1.0, float zz = 1.0): x(xx), y(yy), z(zz) {}

public:
    float x, y, z;

public:
    // 设置向量坐标
    void set(const Point3 & from, const Point3 & to);

    // 内积
    float operator *(const Vector & v) const;
};

// 旋转
class Rotate
{
public:
    // 二维旋转：顺时针旋转(°)
    static void rotate(float angle, float & x, float & y);
    // 三维旋转X：顺时针旋转(°)
    static void rotateX(float angle, Point3 & p);
    static void rotateY(float angle, Point3 & p);
    static void rotateZ(float angle, Point3 & p);
};

// 旋转值
class CubeRotateValue
{
public:
    CubeRotateValue(CubeRotateAxis a, int i, quint8 v);

public:
    quint8 r[3];
    CubeRotateAxis cubeRotateAxis;

public:
    // 解析旋转值
    static void Analysis(moveType move, CubeRotateAxis & a, int & i, quint8 & v);
    // 列表转换
    static QList<CubeRotateValue> transfer(moveList moveValueList);
    static QStringList transfer(QList<CubeRotateValue> rotateValueList);
    // 化简旋转步骤列表
    static QStringList simplify(moveList &moveValueList);

public:
    // 追加旋转值
    void append(int i, quint8 v);
    // 获取旋转步骤列表
    QStringList getStepList();
};

// 常量
class CubeValue
{
public:
    static ProgamErroCode ERRO_CODE;
    const static int CUBE_BLOCK_SIZE;
    static int CUBE_ROTATE_TIME;
    static int CUBE_ROTATE_FPS;
    static int CUBE_ROTATE_FPS_TIME;
    const static QString CUBE_FACE;
    const static QColor CUBE_COLOR[2][7];
    static int CUBE_COLOR_GROUP;
    static int CUBE_TOPLAYER_FIX;
    static stateType CUBE_GOALSTATE;
    const static QStringList CUBE_GOAL;
    const static int CUBE_POSNUM[27];
    const static QString CUBE_ROTATE_TYPE[54];
    static QMap<QString, moveType> ROTATE2MOVE;
    const static RotateType MOVE2TYPE[54];
    const static pRotatingFunc ROTATING_FUNC[3];
    const static float ROTATE_ANGLE[3];
    const static int N2STATE[9][12];
    const static int STATE2N[20];
    const static int N2NEXT[108];

public:
    // 常量初始化
    static void initValue();
};

// 步骤的逆步骤
moveType inverse(moveType move);

#endif // CUBEVALUE_H
