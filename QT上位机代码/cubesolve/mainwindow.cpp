/****************************************
****************************************
***********  作者：许创鸿  **************
**************************************
************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_videoplayer.h"
#include "video.h"
#include "videoplayer.h"
#include "solution.h"
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QFileDialog>
#include <QMouseEvent>
#include <QDebug>
#include "cube.h"
#include <QMessageBox>
#include <QSerialPort>
#include <QTextCodec>
#include <QSerialPortInfo>

QTextCodec *codec = QTextCodec::codecForName("GB18030");
QByteArray colorstate;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 初始化变量
    runTimeType = InterateTime;
    pa = patternType(9, patternSubType(12, patternElementType(-1, -1)));
    isPortEnable = 0;
    // 创建坐标轴
    axis = new Axis(QPoint(325, 325));
    // 创建魔方
    cube = new Cube(axis, this);
    connect(cube, &Cube::updatePattern, this, [=](){
        cube->getPattern(pa);
    });
    connect(cube, &Cube::endRotating, this, [=](){
        setStatus2(true);
        foreach (CubeBlock * block, cube->blockList)
        {
            block->isRotating = false;
        }
    });
    // 添加步骤按钮组
    btn_step[0] = ui->btn_step1;
    btn_step[1] = ui->btn_step2;
    btn_step[2] = ui->btn_step3;
    btn_step[3] = ui->btn_step4;
    btn_step[4] = ui->btn_step5;
    btn_step[5] = ui->btn_step6;
    btn_step[6] = ui->btn_step7;
    btn_step[7] = ui->btn_step8;
    btn_step[8] = ui->btn_step9;
    btn_step[9] = ui->btn_step10;
    btn_step[10] = ui->btn_step11;
    btn_step[11] = ui->btn_step12;
    btn_step[12] = ui->btn_step13;
    btn_step[13] = ui->btn_step14;
    btn_step[14] = ui->btn_step15;
    btn_step[15] = ui->btn_step16;
    btn_step[16] = ui->btn_step17;
    btn_step[17] = ui->btn_step18;
    for (int i = 0; i < 18; i++)
    {
        connect(btn_step[i], &MyPushButton::myClicked, [=](const QString & text){
            // 是否固定顶层
            if (1 == CubeValue::CUBE_TOPLAYER_FIX)
            {
                if (text.contains('u', Qt::CaseInsensitive))
                {
                    addLog(codec->toUnicode("魔方顶层固定，无法进行") + text);
                    return;
                }
            }
            // 关闭按钮组可用状态
            if (cube->isEnableRotating) setStatus2(false);
            // 魔方旋转
            cube->rotate(text);
            MainWindow::send_command(text);//串口通信
            addLog(QString(codec->toUnicode("魔方旋转:"))+ text);
            if (!cube->isEnableRotating) update();
        });
    }
    // 步骤组切换
    void (QComboBox::*func)(int) = &QComboBox::currentIndexChanged;
    connect(ui->cmb_stepGroup, func, [=](int index){
        for (int i = 0; i < 18; i++)
        {
            // 更换步骤按钮名称
            btn_step[i]->setText(CubeValue::CUBE_ROTATE_TYPE[i + 18 * index]);
        }
    });
    // 查看魔方旋转历史记录
    connect(ui->btn_checkCubeHistory, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("命令执行:查看魔方旋转历史记录\n")) + cube->history.join(' '));
    });

    // 清空魔方旋转历史记录
    connect(ui->btn_clearCubeHistory, &QPushButton::clicked, [=](){
        cube->history.clear();
        addLog(codec->toUnicode("命令执行:清空魔方旋转历史记录"));
    });

    // 清空日志栏信息
    connect(ui->btn_clearLog, &QPushButton::clicked, [=](){
        ui->edit_log->setText(QString(codec->toUnicode("")));
    });

    // 重置当前魔方
    connect(ui->btn_resetCube, &QPushButton::clicked, [=](){
        cube->reset(false);
        addLog(codec->toUnicode("命令执行:魔方重置"));
        update();
    });

    // 打乱当前魔方
    connect(ui->btn_disorderCube, &QPushButton::clicked, [=](){
        if (cube->isEnableRotating)
            setStatus2(false);
            if(MainWindow::isPortEnable)
                ui->check_fixTopLayer->setCheckState(Qt::Checked);
        // 打乱魔方
        QStringList list = cube->disorder(); //求解打乱步骤
        addLog(QString(codec->toUnicode("命令执行:打乱当前魔方，步长%1")).arg(list.length()));
        addLog(list.join(' '));
        for (int i = 0; i < list.length(); i++)
        {
            cube->rotate(list[i]);
            MainWindow::send_command(list[i]); //下位机同步
        }
        if (!cube->isEnableRotating)
            update();
            if(MainWindow::isPortEnable)
                ui->check_fixTopLayer->setCheckState(Qt::Unchecked);
    });

    // 检测当前魔方可解性
    connect(ui->btn_cheackCubeSoluted, &QPushButton::clicked, [=](){
        if (InterateTime == runTimeType)
        {
            addLog(QString(codec->toUnicode("命令执行:检测当前魔方可解性")));
            cubeSolute();
        }
        else if (PaintingTime == runTimeType)
        {
            addLog(QString(codec->toUnicode("检测编辑界面下的魔方的可解性...")));
            if (isPatternSoluteble(nullptr)) addLog(QString(codec->toUnicode("当前编辑界面下的魔方可解")));
        }
    });

    // 呈现旋转动画
    connect(ui->check_enableRotating, &QCheckBox::stateChanged, [=](){
        cube->isEnableRotating = ui->check_enableRotating->isChecked();
    });
    // 自定义魔方
    connect(ui->btn_DIYCube, &QPushButton::clicked, [=](){
        // 进入魔方编辑界面
        if (InterateTime == runTimeType)
        {
            runTimeType = PaintingTime;
            selectColorIndex = 1;
            // 获取当前魔方的颜色状态表
            cube->getPattern(pa);
            setStatus(false);
            addLog(codec->toUnicode("命令执行:进入魔方自定义"));
        }
        // 退出魔方编辑界面
        else if (PaintingTime == runTimeType)
        {
            // 尝试进行应用
            applyPatternSolution();
            runTimeType = InterateTime;
            setStatus(true);
            addLog(codec->toUnicode("命令执行:退出魔方自定义"));
        }
        update();
    });
    // 魔方编辑
    connect(this, &MainWindow::clickPattern, this, &MainWindow::setColor);
    // 应用颜色传感器参数
    connect(ui->btn_colorsensor, &QPushButton::clicked, [=](){
        // 进入魔方编辑界面
        if (InterateTime == runTimeType)
        {
            runTimeType = PaintingTime;
            selectColorIndex = 1;
            // 获取当前魔方的颜色状态表
            cube->getPattern(pa);
            getcolorsensor();
            setcolorsensor();
            setStatus3(false);
            addLog(codec->toUnicode("命令执行:进入魔方自定义"));
            colorstate.clear();
        }
        // 退出魔方编辑界面
        else if (PaintingTime == runTimeType)
        {
            // 尝试进行应用
            applyPatternSolution();
            runTimeType = InterateTime;
            setStatus3(true);
            addLog(codec->toUnicode("命令执行:退出魔方自定义"));
        }
        update();
    });

    /**********************魔方还原*************************/
    connect(ui->btn_1_4, &QPushButton::clicked, [=](){
        cubeSolute();//回到复原状态
    });
    connect(ui->btn_2_4, &QPushButton::clicked, [=](){
        cubeSolute();//回到复原状态
    });
    connect(ui->btn_3_6, &QPushButton::clicked, [=](){
        cubeSolute();//回到复原状态
    });
    connect(ui->btn_4_7, &QPushButton::clicked, [=](){
        cubeSolute();//回到复原状态
    });
    connect(ui->btn_5_10, &QPushButton::clicked, [=](){
        cubeSolute();//回到复原状态
    });
    connect(ui->btn_6_5, &QPushButton::clicked, [=](){
        cubeSolute();//回到复原状态
    });
    connect(ui->btn_7_7, &QPushButton::clicked, [=](){
        cubeSolute();//回到复原状态
    });

    /**********************场景实现*************************/
    //自定义动作组库
    QStringList ssss1;//打乱魔方
    ssss1<<"F2"<<"B2"<<"D'"<<"F"<<"B"<<"F"<<"R'"<<"F"<<"D2"<<"R2"<<"B"<<"R"<<"D'"<<"F"<<"L2"<<"B'"<<"L"<<"R"<<"L'"<<"D2"<<"B'"<<"D"<<"R'"<<"R"<<"D2"<<"L'"<<"F'"<<"B'"<<"R"<<"B"<<"F"<<"D"<<"R2"<<"F2";
    QStringList ssss2;//异色花
    ssss2 <<"F2"<<"B"<<"D2"<<"B'"<<"F"<<"D2"<<"B2"<<"D'"<<"B'"<<"D"<<"B"<<"R2"<<"D'"<<"L'";
    QStringList ssss3;//同色花（从异色花开始）
    ssss3 <<"D'"<<"B2"<<"L2"<<"D"<<"R2"<<"D2"<<"R2"<<"D2"<<"F2";
    QStringList ssss4;//同色花(从复原态开始)
    ssss4 <<"F2"<<"D2"<<"F"<<"D'"<<"L"<<"F2"<<"L"<<"D2"<<"F2"<<"B2"<<"D"<<"L"<<"F2"<<"D"<<"B2"<<"D'"<<"F2"<<"L2"<<"D'"<<"L2"<<"B2"<<"R2"<<"D2"<<"F2"<<"D2"<<"F2"<<"R2"<<"D2"<<"L2"<<"D2";
    QStringList ssss5;//对好第一面加上四个侧面的T字形（从同色花开始）
    ssss5 <<"L'"<<"D2"<<"L"<<"F"<<"D"<<"F'"<<"D'"<<"L"<<"D2"<<"L'"<<"D2"<<"F'"<<"D2"<<"F"<<"D'"<<"B"<<"D"<<"B'"<<"D'";
    QStringList ssss6;//第三步左侧连线
    ssss6 << "F2"<<"B2"<<"D'"<<"F"<<"B"<<"D"<<"R'"<<"B2"<<"D"<<"L"<<"R"<<"D"<<"L"<<"F2"<<"L2"<<"D'"<<"R2"<<"F2"<<"D'"<<"F2"<<"D"<<"F2"<<"D'"<<"L2"<<"R2"<<"B2"<<"L2"<<"D2"<<"B2"<<"D2"<<"B2"<<"D2"<<"F2";
    QStringList ssss7;//第三步位置正确颜色反转
    ssss7 <<"D'"<<"R'"<<"D'"<<"R'"<<"D'"<<"R"<<"D"<<"R"<<"D"<<"R"<<"D"<<"F2"<<"B2"<<"D'"<<"F"<<"B"<<"D"<<"R'"<<"B2"<<"D"<<"L"<<"R"<<"D"<<"L"<<"F2"<<"L2"<<"D'"<<"R2"<<"F2"<<"D'"<<"F2"<<"D"<<"F2"<<"D'"<<"L2"<<"R2"<<"B2"<<"L2"<<"D2"<<"B2"<<"D2"<<"B2"<<"D2"<<"F2";
    QStringList ssss8;//第三步右侧连线
    ssss8 <<"R2"<<"D'"<<"R'"<<"D'"<<"R'"<<"D"<<"R"<<"D"<<"R"<<"D"<<"R"<<"F"<<"D"<<"F"<<"D"<<"F'"<<"D'"<<"F'"<<"D'"<<"F'"<<"F2"<<"B2"<<"D'"<<"F"<<"B"<<"D"<<"R'"<<"B2"<<"D"<<"L"<<"R"<<"D"<<"L"<<"F2"<<"L2"<<"D'"<<"R2"<<"F2"<<"D'"<<"F2"<<"D"<<"F2"<<"D'"<<"L2"<<"R2"<<"B2"<<"L2"<<"D2"<<"B2"<<"D2"<<"B2"<<"D2"<<"F2";
    QStringList ssss9;//第四步点
    ssss9 << "F2"<<"D2"<<"L"<<"R'"<<"F"<<"B2"<<"R'"<<"D"<<"L2"<<"D'"<<"L2"<<"D"<<"L"<<"D2"<<"F2"<<"L2"<<"D'"<<"L2"<<"D'"<<"R2"<<"D"<<"F2"<<"D'"<<"L2"<<"B2"<<"D2"<<"B2"<<"L2"<<"D2"<<"B2"<<"L2"<<"B2"<<"R2"<<"D2"<<"F2"<<"D2";
    QStringList ssss10;//第四步小拐弯
    ssss10 << "B"<<"L'"<<"B"<<"D"<<"L'"<<"R'"<<"D'"<<"F2"<<"L"<<"D"<<"R'"<<"D2"<<"B2"<<"D"<<"L2"<<"D'"<<"L2"<<"F2"<<"D'"<<"F2"<<"B2"<<"D2"<<"B2"<<"D2"<<"L2"<<"F2"<<"R2"<<"D2"<<"R2"<<"D2"<<"L2";
    QStringList ssss11;//第四步一字
    ssss11 << "D'"<<"L'"<<"B'"<<"D'"<<"B'"<<"L'"<<"F2"<<"D"<<"R2"<<"F2"<<"D"<<"L"<<"D2"<<"R2"<<"D'"<<"R2"<<"D'"<<"B2"<<"D"<<"L2"<<"D'"<<"R2"<<"D'"<<"D2"<<"F2"<<"L2"<<"B2"<<"R2"<<"D2"<<"R2"<<"F2"<<"L2"<<"F2"<<"D2";
    QStringList ssss12;//第四步十字
    ssss12 << "L'"<<"D'"<<"B2"<<"L"<<"B2"<<"L2"<<"D"<<"L"<<"B2"<<"D'"<<"B2"<<"D"<<"L2"<<"D'"<<"B2"<<"D"<<"B2"<<"L2"<<"D"<<"L2"<<"D2"<<"L2"<<"D2"<<"L2"<<"B2"<<"D2"<<"B2"<<"D2"<<"L2"<<"B2";
    QStringList ssss13;//第五步情况1
    ssss13 << "R"<<"D'"<<"L'"<<"D"<<"R'"<<"D'"<<"L"<<"R2"<<"D'"<<"B2"<<"D"<<"R2"<<"D'"<<"B2"<<"D"<<"R2"<<"D"<<"B2"<<"D2"<<"R2"<<"D2"<<"B2"<<"D2"<<"B2"<<"R2";
    QStringList ssss14;//第五步情况2
    ssss14 << "L"<<"F2"<<"L'"<<"D'"<<"R"<<"D'"<<"R"<<"D2"<<"F2"<<"D'"<<"R2"<<"D2"<<"F2"<<"D"<<"R2"<<"D2"<<"F2"<<"D2"<<"F2"<<"D2"<<"F2"<<"D2"<<"F2"<<"L2"<<"F2"<<"D2"<<"L2"<<"D2"<<"F2"<<"L2";
    QStringList ssss15;//第五步情况3
    ssss15 << "L2"<<"D'"<<"R'"<<"D"<<"L2"<<"D'"<<"R"<<"D2"<<"F2"<<"D'"<<"L2"<<"D"<<"F2"<<"D"<<"L2"<<"D2"<<"F2"<<"D2"<<"F2"<<"D2"<<"F2"<<"L2"<<"D2";
    QStringList ssss16;//第五步情况4
    ssss16 <<"F"<<"D2"<<"F'"<<"D'"<<"F"<<"D"<<"F'"<<"D'"<<"F"<<"D'"<<"F2"<<"D2"<<"F"<<"D"<<"F'"<<"D"<<"F"<<"L2"<<"D'"<<"R'"<<"D"<<"L2"<<"D'"<<"R"<<"D2"<<"F2"<<"D'"<<"L2"<<"D"<<"F2"<<"D"<<"L2"<<"D2"<<"F2"<<"D2"<<"F2"<<"D2"<<"F2"<<"L2"<<"D2";
    QStringList ssss17;//第五步情况5
    ssss17 << "L"<<"R2"<<"D'"<<"R2"<<"D"<<"L"<<"D2"<<"R2"<<"D"<<"R2"<<"D'"<<"L2"<<"D"<<"R2"<<"D'"<<"F2"<<"D"<<"F2"<<"D"<<"L2"<<"D2"<<"F2"<<"R2"<<"B2"<<"L2"<<"B2"<<"D2"<<"L2";
    QStringList ssss18;//第五步情况6
    ssss18 << "F2"<<"L'"<<"D'"<<"R'"<<"B2"<<"R'"<<"D"<<"L"<<"F2"<<"L2"<<"D'"<<"R2"<<"B2"<<"D"<<"R2"<<"B2"<<"R2"<<"F2"<<"L2"<<"D2"<<"B2"<<"R2"<<"D2"<<"B2"<<"D2";
    QStringList ssss19;//第五步情况7
    ssss19 << "L'"<<"D'"<<"B2"<<"L"<<"B2"<<"L2"<<"D"<<"L"<<"B2"<<"D'"<<"B2"<<"D"<<"L2"<<"D'"<<"B2"<<"D"<<"B2"<<"L2"<<"D"<<"L2"<<"D2"<<"L2"<<"D2"<<"L2"<<"B2"<<"D2"<<"B2"<<"D2"<<"L2"<<"B2";
    QStringList ssss20;//第六步情况1
    ssss20 << "F2"<<"D'"<<"F2"<<"D"<<"R2"<<"D'"<<"F2"<<"D"<<"F2"<<"R2"<<"D2"<<"R2"<<"F2"<<"D2"<<"R2"<<"F2"<<"D2"<<"R2"<<"F2";
    QStringList ssss21;//第六步情况2
    ssss21 << "F2"<<"D'"<<"F2"<<"D2"<<"R2"<<"D"<<"R2"<<"D'"<<"R2"<<"D2"<<"F2"<<"D"<<"R2"<<"D2"<<"F2"<<"D2"<<"R2"<<"F2";
    QStringList ssss22;//第七步 三棱逆时针交换
    ssss22 << "F2"<<"D'"<<"L2"<<"F2"<<"L2"<<"D'"<<"L2"<<"D2"<<"L2"<<"D2"<<"F2"<<"L2"<<"F2"<<"D2"<<"F2"<<"D2";
    QStringList ssss23;//第七步三棱顺时针交换
    ssss23 << "B2"<<"D'"<<"L2"<<"B2"<<"L2"<<"D'"<<"L2"<<"B2"<<"D2"<<"L2"<<"B2"<<"L2"<<"D2"<<"L2";
    QStringList ssss24;//第七步 对面交换
    ssss24 << "L2"<<"F2"<<"D2"<<"L2"<<"D2"<<"F2"<<"L2"<<"F2"<<"D2"<<"F2"<<"D2"<<"F2";
    QStringList ssss25;//第七步 左右交换
    ssss25 << "F2"<<"L2"<<"R2"<<"F2"<<"D'"<<"F2"<<"L2"<<"R2"<<"F2"<<"D"<<"R2"<<"F2"<<"D2"<<"R2"<<"F2"<<"R2"<<"F2"<<"L2"<<"F2"<<"D2"<<"F2"<<"L2";
    QStringList ssss26;//第六步 情况3
    ssss26 << "D";
    //选择显示场景
    //场景1：打乱魔方
    connect(ui->btn_1_1, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：打乱魔方")));
        cubeSolute();//先回到复原状态
        for (int i = ssss1.length(); i > 0; i--)
        {
            cube->rotate(ssss1[i-1]);//最后做一套动作
            MainWindow::send_command(ssss1[i-1]);//串口通信
        }
    });
    //场景2：黄心白花
    connect(ui->btn_1_3, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：异色花")));
        for (int i = ssss2.length(); i > 0; i--)
        {
            cube->rotate(ssss2[i-1]);//最后做一套动作
            MainWindow::send_command(ssss2[i-1]);//串口通信
        }
    });
    //场景3：同色白花
    connect(ui->btn_1_5, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：同色花")));
        for (int i = ssss3.length(); i > 0; i--)
        {
            cube->rotate(ssss3[i-1]);//最后做一套动作
            MainWindow::send_command(ssss3[i-1]);//串口通信
        }
    });
    //场景4：同色白花
    connect(ui->btn_2_1, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：同色花")));
        cubeSolute();//先回到复原状态
        for (int i = ssss4.length(); i > 0; i--)
        {
            cube->rotate(ssss4[i-1]);//最后做一套动作
            MainWindow::send_command(ssss4[i-1]);//串口通信
        }
    });
    //场景5：对好第一面加上四个侧面的T字形（从同色花开始）
    connect(ui->btn_2_3, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：对好第一面加上四个侧面的T字形")));
        for (int i = ssss5.length(); i > 0; i--)
        {
            cube->rotate(ssss5[i-1]);//最后做一套动作
            MainWindow::send_command(ssss5[i-1]);//串口通信
        }
    });
    //场景6：第三步左侧连线
    connect(ui->btn_3_1, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第三步左侧连线")));
        cubeSolute();//先回到复原状态
        for (int i = ssss6.length(); i > 0; i--)
        {
            cube->rotate(ssss6[i-1]);//最后做一套动作
            MainWindow::send_command(ssss6[i-1]);//串口通信
        }
    });
    //场景7：第三步位置正确颜色反转
    connect(ui->btn_3_2, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第三步位置正确颜色反转")));
        cubeSolute();//先回到复原状态
        for (int i = ssss7.length(); i > 0; i--)
        {
            cube->rotate(ssss7[i-1]);//最后做一套动作
            MainWindow::send_command(ssss7[i-1]);//串口通信
        }
    });
    //场景8：第三步右侧连线
    connect(ui->btn_3_3, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第三步右侧连线")));
        cubeSolute();//先回到复原状态
        for (int i = ssss8.length(); i > 0; i--)
        {
            cube->rotate(ssss8[i-1]);//最后做一套动作
            MainWindow::send_command(ssss8[i-1]);//串口通信
        }
    });
    //场景9：第四步小拐弯
    connect(ui->btn_4_1, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第四步小拐弯")));
        cubeSolute();//先回到复原状态
        for (int i = ssss9.length(); i > 0; i--)
        {
            cube->rotate(ssss9[i-1]);//最后做一套动作
            MainWindow::send_command(ssss9[i-1]);//串口通信
        }
    });
    //场景10：第四步一字
    connect(ui->btn_4_2, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第四步一字")));
        cubeSolute();//先回到复原状态
        for (int i = ssss10.length(); i > 0; i--)
        {
            cube->rotate(ssss10[i-1]);//最后做一套动作
            MainWindow::send_command(ssss10[i-1]);//串口通信
        }
    });
    //场景11：第四步点
    connect(ui->btn_4_3, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第四步点")));
        cubeSolute();//先回到复原状态
        for (int i = ssss11.length(); i > 0; i--)
        {
            cube->rotate(ssss11[i-1]);//最后做一套动作
            MainWindow::send_command(ssss11[i-1]);//串口通信
        }
    });
    //场景12：第四步十字
    connect(ui->btn_4_4, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第四步十字")));
        cubeSolute();//先回到复原状态
        for (int i = ssss12.length(); i > 0; i--)
        {
            cube->rotate(ssss12[i-1]);//最后做一套动作
            MainWindow::send_command(ssss12[i-1]);//串口通信
        }
    });
    //场景13：第五步情况1
    connect(ui->btn_5_1, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第五步情况1")));
        cubeSolute();//先回到复原状态
        for (int i = ssss13.length(); i > 0; i--)
        {
            cube->rotate(ssss13[i-1]);//最后做一套动作
            MainWindow::send_command(ssss13[i-1]);//串口通信
        }
    });
    //场景14：第五步情况2
    connect(ui->btn_5_2, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第五步情况2")));
        cubeSolute();//先回到复原状态
        for (int i = ssss14.length(); i > 0; i--)
        {
            cube->rotate(ssss14[i-1]);//最后做一套动作
            MainWindow::send_command(ssss14[i-1]);//串口通信
        }
    });
    //场景15：第五步情况3
    connect(ui->btn_5_3, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第五步情况3")));
        cubeSolute();//先回到复原状态
        for (int i = ssss15.length(); i > 0; i--)
        {
            cube->rotate(ssss15[i-1]);//最后做一套动作
            MainWindow::send_command(ssss15[i-1]);//串口通信
        }
    });
    //场景16：第五步情况4
    connect(ui->btn_5_4, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第五步情况4")));
        cubeSolute();//先回到复原状态
        for (int i = ssss16.length(); i > 0; i--)
        {
            cube->rotate(ssss16[i-1]);//最后做一套动作
            MainWindow::send_command(ssss16[i-1]);//串口通信
        }
    });
    //场景17：第五步情况5
    connect(ui->btn_5_5, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第五步情况5")));
        cubeSolute();//先回到复原状态
        for (int i = ssss17.length(); i > 0; i--)
        {
            cube->rotate(ssss17[i-1]);//最后做一套动作
            MainWindow::send_command(ssss17[i-1]);//串口通信
        }
    });
    //场景18：第五步情况6
    connect(ui->btn_5_6, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第五步情况6")));
        cubeSolute();//先回到复原状态
        for (int i = ssss18.length(); i > 0; i--)
        {
            cube->rotate(ssss18[i-1]);//最后做一套动作
            MainWindow::send_command(ssss18[i-1]);//串口通信
        }
    });
    //场景19：第五步情况7
    connect(ui->btn_5_7, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第五步情况7")));
        cubeSolute();//先回到复原状态
        for (int i = ssss19.length(); i > 0; i--)
        {
            cube->rotate(ssss19[i-1]);//最后做一套动作
            MainWindow::send_command(ssss19[i-1]);//串口通信
        }
    });
    //场景20：第六步情况1
    connect(ui->btn_6_1, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第六步情况1")));
        cubeSolute();//先回到复原状态
        for (int i = ssss20.length(); i > 0; i--)
        {
            cube->rotate(ssss20[i-1]);//最后做一套动作
            MainWindow::send_command(ssss20[i-1]);//串口通信
        }
    });
    //场景21：第六步情况2
    connect(ui->btn_6_2, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第六步情况2")));
        cubeSolute();//先回到复原状态
        for (int i = ssss21.length(); i > 0; i--)
        {
            cube->rotate(ssss21[i-1]);//最后做一套动作
            MainWindow::send_command(ssss21[i-1]);//串口通信
        }
    });
    //场景22：第七步三棱顺时针交换
    connect(ui->btn_7_1, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第七步三棱顺时针交换")));
        cubeSolute();//先回到复原状态
        for (int i = ssss22.length(); i > 0; i--)
        {
            cube->rotate(ssss22[i-1]);//最后做一套动作
            MainWindow::send_command(ssss22[i-1]);//串口通信
        }
    });
    //场景23：第七步 三棱逆时针交换
    connect(ui->btn_7_2, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第七步 三棱逆时针交换")));
        cubeSolute();//先回到复原状态
        for (int i = ssss23.length(); i > 0; i--)
        {
            cube->rotate(ssss23[i-1]);//最后做一套动作
            MainWindow::send_command(ssss23[i-1]);//串口通信
        }
    });
    //场景24：第七步 对面交换
    connect(ui->btn_7_3, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第七步 对面交换")));
        cubeSolute();//先回到复原状态
        for (int i = ssss24.length(); i > 0; i--)
        {
            cube->rotate(ssss24[i-1]);//最后做一套动作
            MainWindow::send_command(ssss24[i-1]);//串口通信
        }
    });
    //场景25：第七步 左右交换
    connect(ui->btn_7_4, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第七步 左右交换")));
        cubeSolute();//先回到复原状态
        for (int i = ssss25.length(); i > 0; i--)
        {
            cube->rotate(ssss25[i-1]);//最后做一套动作
            MainWindow::send_command(ssss25[i-1]);//串口通信
        }
    });
    //场景26：第六步 情况3
    connect(ui->btn_6_4, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("场景实现：第六步 情况3")));
        cubeSolute();//先回到复原状态
        for (int i = ssss26.length(); i > 0; i--)
        {
            cube->rotate(ssss26[i-1]);//最后做一套动作
            MainWindow::send_command(ssss26[i-1]);//串口通信
        }
    });

    /*************************以上是场景实现***************************/


    /************************串口通信****************************/
    serialport = new QSerialPort;
    find_port();          //查找可用串口
    isPortEnable = 0;

    //是否打开串口
    connect(ui->check_port, &QCheckBox::stateChanged, [=](){
        bool port_state = ui->check_port->isChecked();
        if(port_state == 1)
        {
            isPortEnable = 1;
            open_port();
        }
        else if(port_state == 0)
        {
            isPortEnable = 0;
            close_port();
        }
        qDebug()<<"isPortEnable:"<<isPortEnable;
        update();
    });

    /**********************以上是串口通信************************/

    // 固定顶层不动
    connect(ui->check_fixTopLayer, &QCheckBox::stateChanged, [=](){
        if (ui->check_fixTopLayer->isChecked()) CubeValue::CUBE_TOPLAYER_FIX = 1;
        else CubeValue::CUBE_TOPLAYER_FIX = 0;
    });

    // 事件追踪
    ui->widget_draw->installEventFilter(this);
    this->setMouseTracking(true);
    ui->centralWidget->setMouseTracking(true);
    ui->widget_teach->setMouseTracking(true);
    ui->widget_draw->setMouseTracking(true);
    //    setWindowFlags(Qt::WindowCloseButtonHint); //隐藏标题栏问号
}

/***********************************************以上是主函数*****************************************************/

/*void MainWindow::getcolorsensor()
{
    // 应用传感器颜色参数
    connect(serialport,SIGNAL(readyRead()), this, SLOT(readyReadSlot()));
}*/

//查找串口
void MainWindow::find_port()
{
    //查找可用的串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);   //设置串口
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->comboBox_port->addItem(serial.portName());        //显示串口name
            serial.close();
        }
    }
}

//延时函数
void MainWindow::sleep( int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//打开串口
void MainWindow::open_port()
{
    update();
    sleep(100);      //延时100ms
    find_port();     //重新查找com
    //初始化串口
    serialport->setPortName(ui->comboBox_port->currentText());        //设置串口名
    if(serialport->open(QIODevice::ReadWrite))              //打开串口成功
    {
        serialport->setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections);//设置波特率和读写方向,魔方机器人下位机的波特率为115200
        serialport->setDataBits(QSerialPort::Data8);		//数据位为8位
        serialport->setFlowControl(QSerialPort::NoFlowControl);//无流控制
        serialport->setParity(QSerialPort::NoParity);	//无校验位
        serialport->setStopBits(QSerialPort::OneStop); //一位停止位
        qDebug()<<codec->toUnicode("成功打开串口");
        //打开串口后关闭U动作组的按钮，否则在串口打开时按了U会有bug
        ui->btn_step1->setEnabled(false);
        ui->btn_step7->setEnabled(false);
        ui->btn_step13->setEnabled(false);
        //serialport->write("01");
    }
    else    //打开失败提示
    {
        sleep(100);
        QMessageBox::information(this,tr("Error"),tr("Failed to open"),QMessageBox::Ok);
    }

}

//关闭串口
void MainWindow::close_port()
{
    serialport->clear();        //清空缓存区
    qDebug()<<codec->toUnicode("关闭串口");
    serialport->close();        //关闭串口

    //关闭串口后恢复U动作组的按钮
    ui->btn_step1->setEnabled(true);
    ui->btn_step7->setEnabled(true);
    ui->btn_step13->setEnabled(true);
}

//发送命令到下位机
void MainWindow::send_command(const QString & rotateStr)
{
    if(MainWindow::isPortEnable)
    {
        QByteArray* temp_array;
        temp_array = MainWindow::convert_command(rotateStr);
        serialport->write(*temp_array);
        MainWindow::sleep(1200);//延时，调节速度
        delete temp_array;
        qDebug()<<codec->toUnicode("命令发送到下位机");
    }
}

//接收颜色传感器的数据
void MainWindow::getcolorsensor()
{
    int receBytes = 0;
    sleep(30);
    colorstate = serialport->readAll();
    if (!colorstate.isEmpty())
    {
        qDebug()<<codec->toUnicode("接收颜色传感器的数据:");
        qDebug()<<colorstate;
        receBytes += colorstate.size();
        QString redata = QString("colorstate size received:%1").arg(QString::number(receBytes));
        addLog(QString(redata));
    }
    else
    {
        qDebug()<<codec->toUnicode("颜色传感器无数据发送");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete axis;
    delete cube;
}

// 添加日志
void MainWindow::addLog(const QString & str)
{
    QString log = ui->edit_log->toPlainText();
    // 自动换行
    if (false == log.endsWith('\n') && false == log.isEmpty())
        log += '\n';
    log += str;
    ui->edit_log->setText(log);
    // 自动移动至日志底部
    ui->edit_log->moveCursor(QTextCursor::End);
    // 输出日志文件
    logCreator.saveLog(str + '\n');
}

// 设置控件状态
void MainWindow::setStatus(bool status)
{
    // 保证在旋转动画过程中，用户无法进行某些功能与操作
    ui->widget_steps->setEnabled(status);
    ui->btn_checkCubeHistory->setEnabled(status);
    ui->btn_clearCubeHistory->setEnabled(status);
    ui->btn_resetCube->setEnabled(status);
    ui->btn_disorderCube->setEnabled(status);
    ui->check_enableRotating->setEnabled(status);
    ui->check_fixTopLayer->setEnabled(status);
    ui->btn_colorsensor->setEnabled(status);
    ui->btn_cheackCubeSoluted->setEnabled(status);
    ui->btn_clearLog->setEnabled(status);
    if (status) ui->btn_DIYCube->setText(codec->toUnicode("自定义魔方颜色..."));
    else ui->btn_DIYCube->setText(codec->toUnicode("应用魔方编辑"));
}

void MainWindow::setStatus2(bool status)
{
    ui->widget_steps->setEnabled(status);
    ui->btn_checkCubeHistory->setEnabled(status);
    ui->btn_clearCubeHistory->setEnabled(status);
    ui->btn_resetCube->setEnabled(status);
    ui->check_enableRotating->setEnabled(status);
    ui->check_fixTopLayer->setEnabled(status);
    ui->btn_DIYCube->setEnabled(status);
    ui->btn_cheackCubeSoluted->setEnabled(status);
    ui->btn_clearLog->setEnabled(status);
    ui->widget_function->setEnabled(status);
}

void MainWindow::setStatus3(bool status)
{
    ui->widget_steps->setEnabled(status);
    ui->btn_checkCubeHistory->setEnabled(status);
    ui->btn_clearCubeHistory->setEnabled(status);
    ui->btn_resetCube->setEnabled(status);
    ui->btn_disorderCube->setEnabled(status);
    ui->check_enableRotating->setEnabled(status);
    ui->check_fixTopLayer->setEnabled(status);
    ui->btn_DIYCube->setEnabled(status);
    ui->btn_cheackCubeSoluted->setEnabled(status);
    ui->btn_clearLog->setEnabled(status);
    if (status) ui->btn_colorsensor->setText(codec->toUnicode("获取颜色传感器参数..."));
    else ui->btn_colorsensor->setText(codec->toUnicode("应用颜色传感器参数"));
}

// 魔方求解
void MainWindow::cubeSolute()
{
    if(MainWindow::isPortEnable)
        ui->check_fixTopLayer->setCheckState(Qt::Checked);
    // 判断魔方是否处于复原状态
    if (cube->isInResetStatus()) addLog(QString(codec->toUnicode("可解")));
    else
    {
        if (cube->isSoluteble())
        {
            addLog(QString(codec->toUnicode("可解，查询解法中...")));
            // 获取魔方的状态
            stateType s = cube->state();
            for(int i = 0; i < s.length(); i++)
            {
                const bspd& ss = s.at(i);
                qDebug()<<"ss.n"<<ss.n;
            }
            if (s.isEmpty())
            {
                addLog(QString(codec->toUnicode("错误：当前魔方状态有误！")));
                return;
            }
            // 进行求解，计时
            CubeSolution solution;
            int startTime = QTime::currentTime().msecsSinceStartOfDay();
            moveList answer = solution.getSolution(s);
            qDebug()<<codec->toUnicode("answer")<<answer;
            int endTime = QTime::currentTime().msecsSinceStartOfDay();
            if (answer.isEmpty())
            {
                switch (CubeValue::ERRO_CODE) {
                case OverTime:
                    QMessageBox::information(nullptr, codec->toUnicode("提示"), codec->toUnicode("求解超时！超时阈值为20s"));
                    addLog(QString(codec->toUnicode("求解超时%1ms").arg(endTime - startTime)));
                    break;
                case UnSolveble:
                    QMessageBox::information(nullptr, codec->toUnicode("提示"),
                                             QString(codec->toUnicode("无法求解，花时%1ms")).arg(endTime - startTime));
                    addLog(QString(codec->toUnicode("无法求解当前魔方，花费时间%1ms")).arg(endTime - startTime));
                case Normal:
                default:
                    break;
                }
            }
            else
            {
                // 进行简单的步骤化简、合并
                QStringList ssss = CubeRotateValue::simplify(answer);
                qDebug()<<codec->toUnicode("ssss")<<ssss;
                addLog(QString(codec->toUnicode("解法为:步长%1，用时%2ms")).arg(ssss.length()).arg(endTime - startTime));
                addLog(ssss.join(' '));
                addLog(QString(codec->toUnicode("第一阶段:步长%1")).arg(solution.phaseSteps[0]));
                addLog(QString(codec->toUnicode("第二阶段:步长%1")).arg(solution.phaseSteps[1]));
                addLog(QString(codec->toUnicode("第三阶段:步长%1")).arg(solution.phaseSteps[2]));
                addLog(QString(codec->toUnicode("第四阶段:步长%1")).arg(solution.phaseSteps[3]));
                // 进行复原
                if (cube->isEnableRotating) setStatus2(false);
                for (int i = 0; i < ssss.length(); i++)
                {
                    cube->rotate(ssss[i]);
                    MainWindow::send_command(ssss[i]);//串口通信
                }
                if (!cube->isEnableRotating) update();
            }
        }
        else addLog(QString(codec->toUnicode("不可解")));
    }
    if(MainWindow::isPortEnable)
        ui->check_fixTopLayer->setCheckState(Qt::Unchecked);
}

// 检测颜色状态表方向值
void MainWindow::checkDeltaInPattern(int L, int C)
{
    // 该面是哪个方块
    int stateIndex = CubeValue::N2STATE[L][C];
    // 该方块具有几个面
    int eleCount = (stateIndex < 12) ? 2 : 3;
    // 该方块所有面的索引
    QVector<int> ele(eleCount, 0);
    // 该方块第一个面的索引
    ele[0] = CubeValue::STATE2N[stateIndex];
    // 该方块的描述
    QString ds;
    for (int i = 0; i < eleCount; i++)
    {
        int N = ele[i];
        int index = pa[N / 12][N % 12].second;
        ds += CubeValue::CUBE_FACE[index - 1];
        if (eleCount - 1 != i) ele[i + 1] = CubeValue::N2NEXT[N];
    }
    // 该方块的方向值
    int d = 0;
    while (3 != d)
    {
        if (-1 != CubeValue::CUBE_GOAL.indexOf(ds)) break;
        else
        {
            ds = ds.right(ds.length() - 1) + ds.left(1);
            d++;
        }
    }
    if (3 == d) d = -1; // 无效方向值
    // 应用该方向值
    for (int i = 0; i < eleCount; i++)
    {
        int N = ele[i];
        int index = pa[N / 12][N % 12].second;
        pa[N / 12][N % 12] = patternElementType(d, index);
    }
}

// 检测颜色状态表完整性
bool MainWindow::isPatternCompleted()
{
    for (int line = 0; line < 9; line++)
    {
        for (int column = 0; column < 12; column++)
        {
            if ((0 <= line && line <= 8 && 3 <= column && column <= 5) ||
                    (3 <= line && line <= 5 && 0 <= column && column <= 11))
            {
                if (-1 == pa[line][column].first)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

// 检测颜色状态表是否可解
bool MainWindow::isPatternSoluteble(pas * blockMsg)
{
    // 所有方块是否着色成功
    if (!isPatternCompleted())
    {
        addLog(QString(codec->toUnicode("错误：存在色面出现错误的着色！")));
        return false;
    }

    // 是否存在20个方块的信息
    bool iBblockMsgInit[20] = {false};
    // 12个棱块的编号+8个角块的编号
    int series[20] = {0};
    // 棱块/角块编号序列逆序性
    int s_edge = 0, s_corner = 0;
    // 棱块/角块方向值总和
    int d_edge = 0, d_corner = 0;
    for (int stateIndex = 0; stateIndex < 20; stateIndex++)
    {
        // 记号长度(位置类型)
        int eleCount = (stateIndex < 12) ? 2:3;
        int N = CubeValue::STATE2N[stateIndex];
        int L = N / 12, C = N % 12;
        // 计算可解性
        if (2 == eleCount) d_edge += pa[L][C].first;
        else d_corner += pa[L][C].first;
        // 解析坐标
        int x = 0, y = 0, z = 0;
        if (5 <= C && C <= 9) x = 1;
        else if (4 == C || 10 == C) x = 0;
        else x = -1;
        if (2 <= C && C <= 6 && 2 <= L && L <= 6) y = -1;
        else if (1 == C || 7 == C || 1 == L || 7 == L) y = 0;
        else y = 1;
        if (L <= 3) z = 1;
        else if (4 == L) z = 0;
        else z = -1;
        // 解析颜色
        int cld[6] = {0};
        for (int i = 0; i < eleCount; i++)
        {
            int index = pa[L][C].second;
            if (L <= 2) cld[0] = index;
            else if (6 <= L) cld[1] = index;
            else if (3 <= C && C <= 5) cld[2] = index;
            else if (9 <= C) cld[3] = index;
            else if (C <= 2) cld[4] = index;
            else cld[5] = index;
            N = CubeValue::N2NEXT[N];
            L = N / 12, C = N % 12;
        }
        // 解析编号np
        int cx = 0, cy = 0, cz = 0, np = 0;
        for (int i = 0; i < 6; i++)
        {
            if (cld[i] == 1) cz = 1;
            else if (cld[i] == 2) cz = -1;
            else if (cld[i] == 3) cy = -1;
            else if (cld[i] == 4) cy = 1;
            else if (cld[i] == 5) cx = -1;
            else if (cld[i] == 6) cx = 1;
        }
        int v = 3 * cx - cy + 9 * cz + 14;
        int t = qAbs(cx) + qAbs(cy) + qAbs(cz);
        np = CubeValue::CUBE_POSNUM[v - 1] + (t == 3) * 12;
        // 存储信息
        if (iBblockMsgInit[np])
        {
            addLog(QString(codec->toUnicode("错误：存在重复色面！重复方块编号为：%1")).arg(np));
            return false;
        }
        iBblockMsgInit[np] = true;
        series[stateIndex] = np;
        if (nullptr != blockMsg)
        {
            blockMsg[np].p = stateIndex; // 编号np的方块在位置stateIndex上
            blockMsg[np].x = x; // 编号np的方块在位置[x,y,z]上
            blockMsg[np].y = y;
            blockMsg[np].z = z;
            for (int i = 0; i < 6; i++) blockMsg[np].cd[i] = cld[i]; // 编号np的方块颜色为cld[]
        }
    }

    // 检测信息完整性
    for (int i = 0; i < 20; i++)
    {
        if (!iBblockMsgInit[i])
        {
            addLog(QString(codec->toUnicode("错误：存在缺失色面！缺失方块编号为：%1")).arg(i));
            return false;
        }
    }

    // 计算棱块编号序列逆序性
    for (int i = 0; i < 12; i++) for (int j = i + 1; j < 12; j++) s_edge ^= series[i] > series[j];
    for (int i = 12; i < 20; i++) for (int j = i + 1; j < 20; j++) s_corner ^= series[i] > series[j];
    // 检测是否可解：棱块方向值总和是偶数，角块方向值总和是3的倍数，棱块编号序列逆序性与角块编号序列逆序性一致
    if (0 != d_edge % 2 || 0 != d_corner % 3 || s_edge != s_corner)
    {
        addLog(QString(codec->toUnicode("错误：当前魔方不可解")));
        return false;
    }

    return true;
}

// 应用颜色状态表
void MainWindow::applyPatternSolution()
{
    // 颜色状态表是否可解
    pas blockMsg[20];
    if (!isPatternSoluteble(blockMsg)) return;

    // 应用这些方块信息
    foreach (CubeBlock * block, cube->blockList)
    {
        if (CentralBlock != block->positionType)
        {
            int p = block->number;
            if (0 <= p && p < 20)
            {
                block->attachPAS(blockMsg[p]);
            }
        }
    }
}

// 绘图
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    // 过滤控件绘图事件
    if (watched == ui->widget_draw && event->type() == QEvent::Paint)
    {
        // 交互界面
        if (InterateTime == runTimeType)
        {
            QPainter painter(ui->widget_draw);
            // 绘制坐标轴
            axis->draw(&painter);
            // 绘制魔方
            cube->draw(&painter);
        }
        // 绘图编辑
        else
        {
            int g = CubeValue::CUBE_COLOR_GROUP;
            QPainter painter(ui->widget_draw);
            painter.drawText(10, 20,codec->toUnicode("当前拾取颜色："));
            painter.drawText(175, 265,codec->toUnicode("左侧面"));
            painter.drawText(475, 265, codec->toUnicode("右侧面"));
            painter.drawText(625, 265,codec->toUnicode( "后背面"));
            painter.drawText(325, 115, codec->toUnicode("上顶面"));
            painter.drawText(225, 505, codec->toUnicode("下底面"));
            painter.setBrush(CubeValue::CUBE_COLOR[g][selectColorIndex]);
            painter.drawRect(10, 30, 50, 50);
            drawPattern(&painter, 50);
        }
        return true;
    }
    return QMainWindow::eventFilter(watched, event);
}

// 绘制颜色状态表
void MainWindow::drawPattern(QPainter *painter, int size)
{
    int g = CubeValue::CUBE_COLOR_GROUP;
    int w = painter->device()->width()-1;
    int h = painter->device()->height()-1;
    int ox = w - 12 * size;
    int oy = h - 9 * size;
    // 绘制颜色状态表
    for (int line = 0; line < 9; line++)
    {
        for (int column = 0; column < 12; column++)
        {
            patternElementType m = pa[line][column];
            if (-1 != m.second)
            {
                // 绘制颜色
                painter->setBrush(CubeValue::CUBE_COLOR[g][m.second]);
                painter->drawRect(ox + size * column, oy - 50 + size * line, size, size);
                // 绘制状态：方向值
                painter->drawText(ox + size * column, oy - 50 + size * line, size, size,
                                  Qt::AlignCenter, QString::number(m.first));
            }
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    // 在魔方编辑界面下，进行鼠标事件跟踪
    if (PaintingTime == runTimeType)
    {
        QPoint point = ui->widget_draw->mapFromGlobal(event->globalPos());
        int w = ui->widget_draw->width();
        int h = ui->widget_draw->height();
        int x = point.x();
        int y = point.y() + 50;
        // 位于ui->widget_draw区域
        if (0 <= x && x <= w && 0 <= y && y <= h)
        {
            int C = (w - x) / 50;
            int L = (h - y) / 50;
            // 位于有效区域
            if ((0 <= C && C <= 11 && 3 <= L && L <= 5) || (6 <= C && C <= 8 && 0 <= L && L <= 8))
            {
                // 位于中心块：拾取光标(十子光标)
                int n = 107 - L * 12 - C;
                if (16 == n || 49 == n || 52 == n || 55 == n || 58 == n || 88 == n) setCursor(Qt::CrossCursor);
                // 位于可编辑色块：着色光标(点击光标)
                else setCursor(Qt::PointingHandCursor);
            }
            // 其他区域：默认光标
            else setCursor(Qt::ArrowCursor);
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // 在魔方编辑界面下，进行鼠标事件跟踪
    if (PaintingTime == runTimeType && event->button() == Qt::LeftButton)
    {
        QPoint point = ui->widget_draw->mapFromGlobal(event->globalPos());
        int w = ui->widget_draw->width();
        int h = ui->widget_draw->height();
        int x = point.x();
        int y = point.y() + 50;
        int C = (w - x) / 50;
        int L = (h - y) / 50;
        // 可响应区域
        if ((0 <= C && C <= 11 && 3 <= L && L <= 5) || (6 <= C && C <= 8 && 0 <= L && L <= 8))
        {
            emit clickPattern(8 - L, 11 - C);
        }
    }
}

//修改颜色
void MainWindow::setColor(int L, int C)
{
    // 拾取中心块颜色
    int n = L * 12 + C;
    if (16 == n || 49 == n || 52 == n || 55 == n || 58 == n || 88 == n)
    {
        selectColorIndex = pa[L][C].second;
        update();
    }
    // 改变其他色块颜色
    else
    {
        pa[L][C] = patternElementType(pa[L][C].first, selectColorIndex);
        checkDeltaInPattern(L, C);
        update();
    }
}

//颜色传感器颜色转换
void MainWindow::colorchange()
{
    int i = 0;
    if(!colorstate.isEmpty())
    {
        for(i=0;i<56;i++)
        {
            if('D' == colorstate[i])
            {
                colorstate[i] = pa[7][4].second;
                update();
            }
            else if('U' == colorstate[i])
            {
                colorstate[i] = pa[1][4].second;
                update();
            }
            else if('B' == colorstate[i])
            {
                colorstate[i] = pa[4][10].second;
                update();
            }
            else if('F' == colorstate[i])
            {
                colorstate[i] = pa[4][4].second;
                update();
            }
            else if('R' == colorstate[i])
            {
                colorstate[i] = pa[4][7].second;
                update();
            }
            else if('L' == colorstate[i])
            {
                colorstate[i] = pa[4][1].second;
                update();
            }
        }
        qDebug()<<codec->toUnicode("颜色转换完成");
    }
}

//应用颜色传感器参数
void MainWindow::setcolorsensor()
{
    int L, C;
    // 块颜色
    if(!colorstate.isEmpty())
    {
        colorchange();
        pa[6][3] = patternElementType(pa[6][3].first, colorstate[2]);
        pa[6][4] = patternElementType(pa[6][4].first, colorstate[3]);
        pa[6][5] = patternElementType(pa[6][5].first, colorstate[4]);
        pa[7][3] = patternElementType(pa[7][3].first, colorstate[5]);
        pa[7][4] = patternElementType(pa[7][4].first, colorstate[6]);
        pa[7][5] = patternElementType(pa[7][5].first, colorstate[7]);
        pa[8][3] = patternElementType(pa[8][3].first, colorstate[8]);
        pa[8][4] = patternElementType(pa[8][4].first, colorstate[9]);
        pa[8][5] = patternElementType(pa[8][5].first, colorstate[10]);
        pa[3][9] = patternElementType(pa[3][9].first, colorstate[11]);
        pa[3][10] = patternElementType(pa[3][10].first, colorstate[12]);
        pa[3][11] = patternElementType(pa[3][11].first, colorstate[13]);
        pa[4][9] = patternElementType(pa[4][9].first, colorstate[14]);
        pa[4][10] = patternElementType(pa[4][10].first, colorstate[15]);
        pa[4][11] = patternElementType(pa[4][11].first, colorstate[16]);
        pa[5][9] = patternElementType(pa[5][9].first, colorstate[17]);
        pa[5][10] = patternElementType(pa[5][10].first, colorstate[18]);
        pa[5][11] = patternElementType(pa[5][11].first, colorstate[19]);
        pa[0][3] = patternElementType(pa[0][3].first, colorstate[20]);
        pa[0][4] = patternElementType(pa[0][4].first, colorstate[21]);
        pa[0][5] = patternElementType(pa[0][5].first, colorstate[22]);
        pa[1][3] = patternElementType(pa[1][3].first, colorstate[23]);
        pa[1][4] = patternElementType(pa[1][4].first, colorstate[24]);
        pa[1][5] = patternElementType(pa[1][5].first, colorstate[25]);
        pa[2][3] = patternElementType(pa[2][3].first, colorstate[26]);
        pa[2][4] = patternElementType(pa[2][4].first, colorstate[27]);
        pa[2][5] = patternElementType(pa[2][5].first, colorstate[28]);
        pa[3][3] = patternElementType(pa[3][3].first, colorstate[29]);
        pa[3][4] = patternElementType(pa[3][4].first, colorstate[30]);
        pa[3][5] = patternElementType(pa[3][5].first, colorstate[31]);
        pa[4][3] = patternElementType(pa[4][3].first, colorstate[32]);
        pa[4][4] = patternElementType(pa[4][4].first, colorstate[33]);
        pa[4][5] = patternElementType(pa[4][5].first, colorstate[34]);
        pa[5][3] = patternElementType(pa[5][3].first, colorstate[35]);
        pa[5][4] = patternElementType(pa[5][4].first, colorstate[36]);
        pa[5][5] = patternElementType(pa[5][5].first, colorstate[37]);
        pa[3][6] = patternElementType(pa[3][6].first, colorstate[38]);
        pa[3][7] = patternElementType(pa[3][7].first, colorstate[39]);
        pa[3][8] = patternElementType(pa[3][8].first, colorstate[40]);
        pa[4][6] = patternElementType(pa[4][6].first, colorstate[41]);
        pa[4][7] = patternElementType(pa[4][7].first, colorstate[42]);
        pa[4][8] = patternElementType(pa[4][8].first, colorstate[43]);
        pa[5][6] = patternElementType(pa[5][6].first, colorstate[44]);
        pa[5][7] = patternElementType(pa[5][7].first, colorstate[45]);
        pa[5][8] = patternElementType(pa[5][8].first, colorstate[46]);
        pa[3][0] = patternElementType(pa[3][0].first, colorstate[47]);
        pa[3][1] = patternElementType(pa[3][1].first, colorstate[48]);
        pa[3][2] = patternElementType(pa[3][2].first, colorstate[49]);
        pa[4][0] = patternElementType(pa[4][0].first, colorstate[50]);
        pa[4][1] = patternElementType(pa[4][1].first, colorstate[51]);
        pa[4][2] = patternElementType(pa[4][2].first, colorstate[52]);
        pa[5][0] = patternElementType(pa[5][0].first, colorstate[53]);
        pa[5][1] = patternElementType(pa[5][1].first, colorstate[54]);
        pa[5][2] = patternElementType(pa[5][2].first, colorstate[55]);
        // 改变颜色状态表颜色
        for(L=0;L<9;L++)
            for(C=0;C<12;C++)
            {
                checkDeltaInPattern(L, C);
                update();
            }
        qDebug()<<codec->toUnicode("接收颜色传感器参数成功");
    }
    else
    {
        qDebug()<<codec->toUnicode("接收颜色传感器参数失败");
    }
}

/*********************************调试*************************************/
//串口通信命令转换函数
QByteArray* MainWindow::convert_command(const QString& command){
    QByteArray* array = new QByteArray();
    array->resize(6);
    (*array)[0] = 0x66;
    (*array)[1] = 0x31;
    (*array)[2] = 0x02;
    (*array)[5] = 0x63;
    if(command == "B")
    {
        (*array)[3] = 0x42;/*直接用ASCII码输出字母B*/
        (*array)[4] = 0x01;
        return array;
    }
    else if(command == "B2")
    {
        (*array)[3] = 0x42;
        (*array)[4] = 0x02;
        return array;
    }
    else if(command == "B'")
    {
        (*array)[3] = 0x42;
        (*array)[4] = 0x03;
        return array;
    }
    else if(command == "D")
    {
        (*array)[3] = 0x44;/*直接用ASCII码输出字母D*/
        (*array)[4] = 0x01;
        return array;
    }
    else if(command == "D2")
    {
        (*array)[3] = 0x44;
        (*array)[4] = 0x02;
        return array;
    }
    else if(command == "D'")
    {
        (*array)[3] = 0x44;
        (*array)[4] = 0x03;
        return array;
    }
    else if(command == "F")
    {
        (*array)[3] = 0x46;/*直接用ASCII码输出字母F*/
        (*array)[4] = 0x01;
        return array;
    }
    else if(command == "F2")
    {
        (*array)[3] = 0x46;
        (*array)[4] = 0x02;
        return array;
    }
    else if(command == "F'")
    {
        (*array)[3] = 0x46;
        (*array)[4] = 0x03;
        return array;
    }
    else if(command == "L")
    {
        (*array)[3] = 0x4c;/*直接用ASCII码输出字母L*/
        (*array)[4] = 0x01;
        return array;
    }
    else if(command == "L2")
    {
        (*array)[3] = 0x4c;
        (*array)[4] = 0x02;
        return array;
    }
    else if(command == "L'")
    {
        (*array)[3] = 0x4c;
        (*array)[4] = 0x03;
        return array;
    }
    else if(command == "R")
    {
        (*array)[3] = 0x52;/*直接用ASCII码输出字母R*/
        (*array)[4] = 0x01;
        return array;
    }
    else if(command == "R2")
    {
        (*array)[3] = 0x52;
        (*array)[4] = 0x02;
        return array;
    }
    else if(command == "R'")
    {
        (*array)[3] = 0x52;
        (*array)[4] = 0x03;
        return array;
    }
    return nullptr;
}

void MainWindow::on_btn_1_2_clicked(bool checked)
{
    view = new videoplayer(this) ;//将视频窗口指针实例化
    view->show();
}

void MainWindow::on_btn_2_2_clicked(bool checked)
{
    view1 = new videoplayer(this) ;//将视频窗口指针实例化
    view1->show();
}

void MainWindow::on_btn_3_4_clicked(bool checked)
{
    view2 = new videoplayer(this) ;//将视频窗口指针实例化
    view2->show();
}

void MainWindow::on_btn_4_5_clicked(bool checked)
{
    view3 = new videoplayer(this) ;//将视频窗口指针实例化
    view3->show();
}

void MainWindow::on_btn_5_8_clicked(bool checked)
{
    view4 = new videoplayer(this) ;//将视频窗口指针实例化
    view4->show();
}

void MainWindow::on_btn_6_3_clicked(bool checked)
{
    view5 = new videoplayer(this) ;//将视频窗口指针实例化
    view5->show();
}

void MainWindow::on_btn_7_5_clicked(bool checked)
{
    view6 = new videoplayer(this) ;//将视频窗口指针实例化
    view6->show();
}

void MainWindow::on_btn_step1_clicked(){}

void MainWindow::on_btn_step1_clicked(bool checked){}

//关闭窗口提示
void MainWindow::closeEvent(QCloseEvent *event)
{
    int b=QMessageBox::question(this,QString(codec->toUnicode("提示")),QString(codec->toUnicode("是否关闭程序")),
                                QMessageBox::Yes |QMessageBox::No);
    if(b==QMessageBox::No)
    {
        event->ignore();
        return;
    }
    else
    {
        logCreator.endLog();
        event->accept();
    }
}

// //创建子事件循环-不阻塞（延时）
//void MainWindow::delay_ms(unsigned int ms)
//{
//    QEventLoop loop;//定义一个新的事件循环
//    QTimer::singleShot(ms, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
//    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
//}
