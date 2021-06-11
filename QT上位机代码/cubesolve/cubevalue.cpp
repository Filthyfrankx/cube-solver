#include "cubevalue.h"

// 设置坐标
void Point3::set(float xx, float yy, float zz)
{
    x = xx, y = yy, z = zz;
}

// 设置为中点
void Point3::setMiddle(const Point3 & p1, const Point3 &p2)
{
    x = (p1.x + p2.x) * 0.5;
    y = (p1.y + p2.y) * 0.5;
    z = (p1.z + p2.z) * 0.5;
}

// 设置向量坐标
void Vector::set(const Point3 & from, const Point3 & to)
{
    this->x = to.x - from.x;
    this->y = to.y - from.y;
    this->z = to.z - from.z;
}

// 内积
float Vector::operator *(const Vector &v) const
{
    return this->x * v.x + this->y * v.y + this->z * v.z;
}

// 二维旋转：顺时针旋转(°)
void Rotate::rotate(float angle, float &x, float &y)
{
    float ox = x, oy = y;
    float cosA, sinA;
    cosA = qCos(qDegreesToRadians(angle));
    sinA = qSin(qDegreesToRadians(angle));
    x = cosA * ox + sinA * oy;
    y = cosA * oy - sinA * ox;
}

// 三维旋转X：顺时针旋转(°)
void Rotate::rotateX(float angle, Point3 &p)
{
    Rotate::rotate(angle, p.y, p.z);
}
void Rotate::rotateY(float angle, Point3 &p)
{
    Rotate::rotate(-angle, p.x, p.z);
}
void Rotate::rotateZ(float angle, Point3 &p)
{
    Rotate::rotate(angle, p.x, p.y);
}

// 程序错误码
ProgamErroCode CubeValue::ERRO_CODE = Normal;

// 魔方方块的边长
const int CubeValue::CUBE_BLOCK_SIZE = 100;

// 魔方旋转时间
int CubeValue::CUBE_ROTATE_TIME = 500;
// 魔方旋转帧数
int CubeValue::CUBE_ROTATE_FPS = 25;
// 魔方旋转帧长
int CubeValue::CUBE_ROTATE_FPS_TIME = 20;

// 魔方各面名称
const QString CubeValue::CUBE_FACE = "UDFBLR";

// 魔方方块绘图颜色
const QColor CubeValue::CUBE_COLOR[2][7] =
{{
    QColor(240, 240, 240),
    QColor(255, 255, 255),
    QColor(255, 255, 0),
    QColor(0, 255, 0),
    QColor(100, 100, 255),
    QColor(255, 97, 0),
    QColor(200, 0, 0)
},{
    QColor(240, 240, 240),
    QColor(255, 255, 255),
    QColor(254, 254, 254),
    QColor(0, 255, 0),
    QColor(0, 254, 0),
    QColor(200, 0, 0),
    QColor(201, 0, 0)
}};
int CubeValue::CUBE_COLOR_GROUP = 0;
int CubeValue::CUBE_TOPLAYER_FIX = 0;

// 魔方复原状态描述
stateType CubeValue::CUBE_GOALSTATE = stateType(20);
const QStringList CubeValue::CUBE_GOAL =
{
    "UF", "UR", "UB", "UL", "DF", "DR", "DB", "DL", "FR", "FL", "BR", "BL",
    "UFR", "URB", "UBL", "ULF", "DRF", "DFL", "DLB", "DBR"
};

// 魔方位置映射表：给定paintTag-1，求出position
const int CubeValue::CUBE_POSNUM[27] =
{
    6, 7, 5, 6, 0, 4, 7, 5, 4,
    11,0, 9, 0, 0, 0,10, 0, 8,
    2, 3, 3, 2, 0, 0, 1, 1, 0
};

// 魔方可执行旋转步骤
const QString CubeValue::CUBE_ROTATE_TYPE[54] =
{
    "U","U2","U'", "D","D2","D'",
    "F","F2","F'", "B","B2","B'",
    "L","L2","L'", "R","R2","R'",
    "u","u2","u'", "d","d2","d'",
    "f","f2","f'", "b","b2","b'",
    "l","l2","l'", "r","r2","r'",
    "E","E2","E'", "M","M2","M'", "S","S2","S'",
    "x","x2","x'", "y","y2","y'", "z","z2","z'"
};

// 步骤映射表
QMap<QString, moveType> CubeValue::ROTATE2MOVE;

// 魔方旋转类型转化表
const RotateType CubeValue::MOVE2TYPE[54] =
{
    Z_P,Z_D,Z_N,Z_N,Z_D,Z_P, Y_N,Y_D,Y_P,Y_P,Y_D,Y_N, X_N,X_D,X_P,X_P,X_D,X_N,
    Z_P,Z_D,Z_N,Z_N,Z_D,Z_P, Y_N,Y_D,Y_P,Y_P,Y_D,Y_N, X_N,X_D,X_P,X_P,X_D,X_N,
    Z_N,Z_D,Z_P,X_N,X_D,X_P, Y_N,Y_D,Y_P,X_P,Y_D,X_N, Y_P,Y_D,Y_N,Z_P,Z_D,Z_N
};

// 旋转方法与旋转角
const pRotatingFunc CubeValue::ROTATING_FUNC[3] = { &Rotate::rotateX, &Rotate::rotateY, &Rotate::rotateZ };
const float CubeValue::ROTATE_ANGLE[3] = { 90.0f, -90.0f, 180.0f };

// 颜色状态提取辅助表
const int CubeValue::N2STATE[9][12] =
{
   { 0, 0, 0, 14, 2,13,  0, 0, 0,  0, 0, 0 },
   { 0, 0, 0,  3, 0, 1,  0, 0, 0,  0, 0, 0 },
   { 0, 0, 0, 15, 0,12,  0, 0, 0,  0, 0, 0 },

   {14, 3,15, 15, 0,12, 12, 1,13, 13, 2,14 },
   {11, 0, 9,  9, 0, 8,  8, 0,10, 10, 0,11 },
   {18, 7,17, 17, 4,16, 16, 5,19, 19, 6,18 },

   { 0, 0, 0, 17, 4,16,  0, 0, 0,  0, 0, 0 },
   { 0, 0, 0,  7, 0, 5,  0, 0, 0,  0, 0, 0 },
   { 0, 0, 0, 18, 6,19,  0, 0, 0,  0, 0, 0 }
};
const int CubeValue::STATE2N[20] =
{
    28, 17, 4, 15, 76, 89, 100, 87, 53, 51, 57, 59,
    29, 5, 3, 27, 77, 75, 99, 101
};
const int CubeValue::N2NEXT[108] =
{
    0, 0, 0, 47,46,44,  0, 0, 0,  0, 0, 0,
    0, 0, 0, 37, 0,43,  0, 0, 0,  0, 0, 0,
    0, 0, 0, 38,40,41,  0, 0, 0,  0, 0, 0,

    3,15,39, 27,28,42, 29,17,45,  5, 4,36,
   59, 0,51, 50, 0,54, 53, 0,57, 56, 0,48,
   71,87,75, 62,76,77, 65,89,101,68,100,99,

    0, 0, 0, 63,64,66,  0, 0, 0,  0, 0, 0,
    0, 0, 0, 61, 0,67,  0, 0, 0,  0, 0, 0,
    0, 0, 0, 60,70,69,  0, 0, 0,  0, 0, 0
};

// 常量初始化
void CubeValue::initValue()
{
    qsrand(QTime::currentTime().msecsSinceStartOfDay());
    for (int i = 0; i < 54; i++) ROTATE2MOVE[CUBE_ROTATE_TYPE[i]] = i;
    //更改最终状态值
    for(int i=0;i<20;i++)CUBE_GOALSTATE[i].s.pos=i;

}

// 旋转值构造
CubeRotateValue::CubeRotateValue(CubeRotateAxis a, int i, quint8 v)
{
    cubeRotateAxis = a;
    r[0] = r[1] = r[2] = 0;
    r[i] = v;
}

// 解析旋转值
void CubeRotateValue::Analysis(moveType move, CubeRotateAxis & a, int & i, quint8 & v)
{
    int face = move / 3;
    // 解析旋转轴
    if (4 == face || 5 == face || 13 == face) a = XAxis;
    else if (2 == face || 3 == face || 14 == face) a = YAxis;
    else if (0 == face || 1 == face || 12 == face) a = ZAxis;
    // 解析旋转级别
    if (0 == face || 2 == face || 4 == face) i = 0;
    else if (12 == face || 13 == face || 14 == face) i = 1;
    else if (1 == face || 3 == face || 5 == face) i = 2;
    // 解析旋转轴
    v = move % 3 + 1;
}

// 列表转换
QList<CubeRotateValue> CubeRotateValue::transfer(moveList moveValueList)
{
    QList<CubeRotateValue> list;
    foreach (moveType move, moveValueList)
    {
        CubeRotateAxis a;
        int i = 0;
        quint8 v = 0;
        CubeRotateValue::Analysis(move, a, i, v);
        // 如果上一个步骤非空
        if (!list.isEmpty())
        {
            // 如果上一个步骤的旋转轴相同，可以合并
            if (list.last().cubeRotateAxis == a)
            {
                list.last().append(i, v);
                continue;
            }
        }
        // 如果之前没有步骤，或不能与上一个步骤合并，添加新的步骤
        list.append(CubeRotateValue(a, i, v));
    }
    return list;
}
QStringList CubeRotateValue::transfer(QList<CubeRotateValue> rotateValueList)
{
    QStringList list;
    for (int i = 0; i < rotateValueList.length(); i++)
    {
        list.append(rotateValueList[i].getStepList());
    }
    return list;
}

// 化简旋转步骤列表
QStringList CubeRotateValue::simplify(moveList &moveValueList)
{
    return CubeRotateValue::transfer(CubeRotateValue::transfer(moveValueList));
}

// 追加旋转值
void CubeRotateValue::append(int i, quint8 v)
{
    quint8 t = r[i] & 0x03;
    quint8 x = r[i] & 0xFC;
    r[i] = x | ((t + v) & 0x03);
}

// 获取旋转步骤列表
QStringList CubeRotateValue::getStepList()
{
    QStringList list;
    for (int i = 0; i < 3; i++)
    {
        quint8 t = r[i] & 0x03;
        if (0 != t)
        {
            QString str;
            switch (cubeRotateAxis)
            {
            case XAxis:
                str.append("LMR"[i]);
                break;
            case YAxis:
                str.append("FSB"[i]);
                break;
            case ZAxis:
                str.append("UED"[i]);
                break;
            }
            if (2 == t) str.append('2');
            else if (3 == t) str.append('\'');
            list.append(str);
        }
    }
    return list;
}

// 步骤的逆步骤
moveType inverse(moveType move)
{
    return move + 2 - 2 * (move % 3);
}
