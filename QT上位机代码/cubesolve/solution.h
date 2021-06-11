#ifndef SOLUTION_H
#define SOLUTION_H

#include "cubevalue.h"

// 魔方复原解决方案
class CubeSolution
{
public:
    CubeSolution();

public:
    // 可行步骤表
    static int viableMove[2][4];
    // 魔方编码表
    static int substitutionTable[6][8];

public:
    // 复原状态
    stateType defaultState;
    // 各阶段步长
    int phaseSteps[4];

public:
    // 旋转
    void rotate(stateType & state, int move);
    // 获取ID值
    idType id(stateType & state, int phase);
    // 获取解决方案
    moveList getSolution(stateType & state);

};

#endif // SOLUTION_H
