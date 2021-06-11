#include "solution.h"
#include <QDebug>

// 可行步骤表
int CubeSolution::viableMove[2][4] =
{{
    0b111111111111111111, /* 18个1，表示18种move动作都可以执行 */
    0b111111010010111111, /* 111.111.010.010.111.111 只执行U+,D+,F2,B2,L+,R+ */
    0b010010010010111111, /* 010.010.010.010.111.111 只执行U+,D+,F2,B2,L2,R2 */
    0b010010010010010010  /* 010.010.010.010.010.010 只执行U2,D2,F2,B2,L2,R2 */
},{
     0b111111111111111000, /* 同上，取消了所有的U+动作 */
     0b111111010010111000,
     0b010010010010111000,
     0b010010010010010000
}};

// 魔方置换表
int CubeSolution::substitutionTable[6][8]=
{
    { 0,  1,  2,  3,  12,  13,  14,  15 },   // U
    { 7,  6,  5,  4,  16,  17,  18,  19 },   // D
    { 0,  9,  4,  8,  12,  15,  17,  16 },   // F
    { 2, 10,  6, 11,  14,  13,  19,  18 },   // B
    { 11, 7,  9,  3,  15,  14,  18,  17 },   // L
    { 1,  8,  5, 10,  13,  12,  16,  19 }    // R
};

// 解决方案构建
CubeSolution::CubeSolution()
{
    defaultState = CubeValue::CUBE_GOALSTATE;
    for (int i = 0; i < 4; i++) phaseSteps[i] = 0;
}

// 旋转
void CubeSolution::rotate(stateType &state, int move)
{
    int turns = move % 3 + 1;
    int face = move / 3;
    while (turns--)
    {
        stateType oldState = state;
        for (int i = 0; i < 8; i++)
        {
            int next = substitutionTable[face][i];
            int old = substitutionTable[face][(i & 3) == 3 ? i - 3 : i + 1];
            int delta = (i < 4) ? (face == 2 || face == 3) : ((face < 2) ? 0 : 2 - (i & 1));
            state[next].s.pos = oldState[old].s.pos;
            state[next].s.det = (oldState[old].s.det + delta) % (2 + i / 4);
        }
    }
}

// 获取ID值
idType CubeSolution::id(stateType &state, int phase)
{
    // 第一步：使12个棱块的朝向正确
    if (phase == 1)
    {
        int result = 0;
        for (int i = 0; i < 12; i++)
        {
            result |= state[i].s.det << (i << 1);

        }
        qDebug()<<"result:"<<result;
        return {result};
    }
    // 第二步：使8个角块的朝向正确，以及中间层棱块位于中间层上
    if (phase == 2)
    {
        int result = 0;
        for (int i = 0; i < 8; i++) result |= state[i + 12].s.det << (i << 1);
        for (int e = 0; e < 12; e++) result |= (state[e].s.pos / 8) << (e + 16);
        return {result};
    }
    // 第三步：使8个上下层棱块位于对应的前后层，以及8个角块的位于对应的角上，以及角块逆序性
    if (phase == 3)
    {
        idType result(3);
        for (int e = 0; e < 12; e++) result[0] |= ((state[e].s.pos > 7) ? 2 : (state[e].s.pos & 1)) << (2 * e);
        for (int c = 0; c < 8; c++) result[1] |= ((state[c + 12].s.pos - 12) & 5) << (3 * c);
        for (int i = 12; i < 20; i++) for (int j = i + 1; j < 20; j++) result[2] ^= state[i].s.pos > state[j].s.pos;
        return result;
    }
    // 第四步：使所有方块的位置信息与朝向正确
    idType result(5);
    for (int i = 0; i < 5; i++) for (int k = 0; k < 4; k++) result[i] |= state[(i << 2) + k].n << (k << 3);
    return result;
}

// 获取解决方案
moveList CubeSolution::getSolution(stateType &state)
{
    if (state == defaultState) return moveList();
    int f = CubeValue::CUBE_TOPLAYER_FIX;
    int phase = 0;
    moveList answer;
    stateType currentState = state;

    int overTimeStart = QTime::currentTime().msecsSinceStartOfDay();
    // 进行4步降群搜索
    while (++phase <= 4)
    {
        idType currentId = id(currentState, phase), goalId = id(defaultState, phase);
        qDebug()<<"goalId start";
        for(int i = 0; i < goalId.length(); i++)
        {
            qDebug()<<"id bit : "<<i<<" "<<goalId.at(i);
        }
        qDebug()<<"goalId end";
        // 跳过当前phase
        qDebug()<<"currentId start";
        for(int i = 0; i < currentId.length(); i++)
        {
            qDebug()<<"id bit : "<<i<<" "<<currentId.at(i);
        }
        qDebug()<<"currentId end";
        // 跳过当前phase
        if (currentId == goalId) continue;

        // 记录当前phase下的所有状态
        QQueue<stateType> q;
        q.enqueue(currentState);
        q.enqueue(defaultState);

        // 记录当前phase下的状态转移<新状态Id，旧状态Id>
        QMap<idType, idType> predecessor;
        // 记录当前phase下的状态转移<新状态Id，步骤>
        QMap<idType, moveType> lastMove;
        // 记录当前phase下的状态转移方向<新状态Id，转移方向>
        QMap<idType, int> directions;
        directions[currentId] = 1;
        directions[goalId] = 2;

        // 开始当前phase下的寻找
        bool run = true;
        while (run)
        {
            int overTimeEnd = QTime::currentTime().msecsSinceStartOfDay();
            if (20000 < overTimeEnd - overTimeStart)
            {
                CubeValue::ERRO_CODE = OverTime;
                return moveList();
            }
            // 从状态表中取出一个状态，给出Id与转移方向
            if (q.isEmpty())
            {
                CubeValue::ERRO_CODE = UnSolveble;
                return moveList();
            }
            stateType oldState = q.dequeue();
            idType oldId = id(oldState, phase);
            int & oldDir = directions[oldId];

            // 尝试所有步骤
            for (moveType move = 0; move < 18; move++)
            {
                // phase下步骤降群
                if (viableMove[f][phase - 1] & (1 << move))
                {
                    // 生成新状态，给出Id，查找是否存在
                    stateType newState = oldState;
                    rotate(newState, move);
                    idType newId = id(newState, phase);
                    int& newDir = directions[newId];

                    // 两个转移方向上获得交点，给出phase下的解决方案
                    if (0 != newDir && newDir != oldDir)
                    {
                        // 给定查找方向
                        if (oldDir == 2)
                        {
                            qSwap(newId, oldId);
                            move = inverse(move);
                        }

                        // 生成解决方案
                        moveList algorithm;
                        algorithm.append(move);
                        while (oldId != currentId)
                        {
                            algorithm.push_front(lastMove[oldId]);
                            oldId = predecessor[oldId];
                        }
                        while (newId != goalId)
                        {
                            algorithm.push_back(inverse(lastMove[newId]));
                            newId = predecessor[newId];
                        }
                        phaseSteps[phase - 1] = algorithm.size();

                        // 追加解决方案
                        for (int i = 0; i < algorithm.size(); i++)
                        {
                            answer.push_back(algorithm[i]);
                            rotate(currentState, algorithm[i]);
                        }

                        // 下一phase
                        run = false;
                        break;
                    }
                    // 新状态不存在，记录
                    if (0 == newDir)
                    {
                        q.enqueue(newState);
                        newDir = oldDir;
                        lastMove[newId] = move;
                        predecessor[newId] = oldId;
                    }
                }
            }
        }
    }
    return answer;
}
