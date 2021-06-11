/****************************************
****************************************
***********  ���ߣ�����  **************
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
    // ��ʼ������
    runTimeType = InterateTime;
    pa = patternType(9, patternSubType(12, patternElementType(-1, -1)));
    isPortEnable = 0;
    // ����������
    axis = new Axis(QPoint(325, 325));
    // ����ħ��
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
    // ��Ӳ��谴ť��
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
            // �Ƿ�̶�����
            if (1 == CubeValue::CUBE_TOPLAYER_FIX)
            {
                if (text.contains('u', Qt::CaseInsensitive))
                {
                    addLog(codec->toUnicode("ħ������̶����޷�����") + text);
                    return;
                }
            }
            // �رհ�ť�����״̬
            if (cube->isEnableRotating) setStatus2(false);
            // ħ����ת
            cube->rotate(text);
            MainWindow::send_command(text);//����ͨ��
            addLog(QString(codec->toUnicode("ħ����ת:"))+ text);
            if (!cube->isEnableRotating) update();
        });
    }
    // �������л�
    void (QComboBox::*func)(int) = &QComboBox::currentIndexChanged;
    connect(ui->cmb_stepGroup, func, [=](int index){
        for (int i = 0; i < 18; i++)
        {
            // �������谴ť����
            btn_step[i]->setText(CubeValue::CUBE_ROTATE_TYPE[i + 18 * index]);
        }
    });
    // �鿴ħ����ת��ʷ��¼
    connect(ui->btn_checkCubeHistory, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ִ��:�鿴ħ����ת��ʷ��¼\n")) + cube->history.join(' '));
    });

    // ���ħ����ת��ʷ��¼
    connect(ui->btn_clearCubeHistory, &QPushButton::clicked, [=](){
        cube->history.clear();
        addLog(codec->toUnicode("����ִ��:���ħ����ת��ʷ��¼"));
    });

    // �����־����Ϣ
    connect(ui->btn_clearLog, &QPushButton::clicked, [=](){
        ui->edit_log->setText(QString(codec->toUnicode("")));
    });

    // ���õ�ǰħ��
    connect(ui->btn_resetCube, &QPushButton::clicked, [=](){
        cube->reset(false);
        addLog(codec->toUnicode("����ִ��:ħ������"));
        update();
    });

    // ���ҵ�ǰħ��
    connect(ui->btn_disorderCube, &QPushButton::clicked, [=](){
        if (cube->isEnableRotating)
            setStatus2(false);
            if(MainWindow::isPortEnable)
                ui->check_fixTopLayer->setCheckState(Qt::Checked);
        // ����ħ��
        QStringList list = cube->disorder(); //�����Ҳ���
        addLog(QString(codec->toUnicode("����ִ��:���ҵ�ǰħ��������%1")).arg(list.length()));
        addLog(list.join(' '));
        for (int i = 0; i < list.length(); i++)
        {
            cube->rotate(list[i]);
            MainWindow::send_command(list[i]); //��λ��ͬ��
        }
        if (!cube->isEnableRotating)
            update();
            if(MainWindow::isPortEnable)
                ui->check_fixTopLayer->setCheckState(Qt::Unchecked);
    });

    // ��⵱ǰħ���ɽ���
    connect(ui->btn_cheackCubeSoluted, &QPushButton::clicked, [=](){
        if (InterateTime == runTimeType)
        {
            addLog(QString(codec->toUnicode("����ִ��:��⵱ǰħ���ɽ���")));
            cubeSolute();
        }
        else if (PaintingTime == runTimeType)
        {
            addLog(QString(codec->toUnicode("���༭�����µ�ħ���Ŀɽ���...")));
            if (isPatternSoluteble(nullptr)) addLog(QString(codec->toUnicode("��ǰ�༭�����µ�ħ���ɽ�")));
        }
    });

    // ������ת����
    connect(ui->check_enableRotating, &QCheckBox::stateChanged, [=](){
        cube->isEnableRotating = ui->check_enableRotating->isChecked();
    });
    // �Զ���ħ��
    connect(ui->btn_DIYCube, &QPushButton::clicked, [=](){
        // ����ħ���༭����
        if (InterateTime == runTimeType)
        {
            runTimeType = PaintingTime;
            selectColorIndex = 1;
            // ��ȡ��ǰħ������ɫ״̬��
            cube->getPattern(pa);
            setStatus(false);
            addLog(codec->toUnicode("����ִ��:����ħ���Զ���"));
        }
        // �˳�ħ���༭����
        else if (PaintingTime == runTimeType)
        {
            // ���Խ���Ӧ��
            applyPatternSolution();
            runTimeType = InterateTime;
            setStatus(true);
            addLog(codec->toUnicode("����ִ��:�˳�ħ���Զ���"));
        }
        update();
    });
    // ħ���༭
    connect(this, &MainWindow::clickPattern, this, &MainWindow::setColor);
    // Ӧ����ɫ����������
    connect(ui->btn_colorsensor, &QPushButton::clicked, [=](){
        // ����ħ���༭����
        if (InterateTime == runTimeType)
        {
            runTimeType = PaintingTime;
            selectColorIndex = 1;
            // ��ȡ��ǰħ������ɫ״̬��
            cube->getPattern(pa);
            getcolorsensor();
            setcolorsensor();
            setStatus3(false);
            addLog(codec->toUnicode("����ִ��:����ħ���Զ���"));
            colorstate.clear();
        }
        // �˳�ħ���༭����
        else if (PaintingTime == runTimeType)
        {
            // ���Խ���Ӧ��
            applyPatternSolution();
            runTimeType = InterateTime;
            setStatus3(true);
            addLog(codec->toUnicode("����ִ��:�˳�ħ���Զ���"));
        }
        update();
    });

    /**********************ħ����ԭ*************************/
    connect(ui->btn_1_4, &QPushButton::clicked, [=](){
        cubeSolute();//�ص���ԭ״̬
    });
    connect(ui->btn_2_4, &QPushButton::clicked, [=](){
        cubeSolute();//�ص���ԭ״̬
    });
    connect(ui->btn_3_6, &QPushButton::clicked, [=](){
        cubeSolute();//�ص���ԭ״̬
    });
    connect(ui->btn_4_7, &QPushButton::clicked, [=](){
        cubeSolute();//�ص���ԭ״̬
    });
    connect(ui->btn_5_10, &QPushButton::clicked, [=](){
        cubeSolute();//�ص���ԭ״̬
    });
    connect(ui->btn_6_5, &QPushButton::clicked, [=](){
        cubeSolute();//�ص���ԭ״̬
    });
    connect(ui->btn_7_7, &QPushButton::clicked, [=](){
        cubeSolute();//�ص���ԭ״̬
    });

    /**********************����ʵ��*************************/
    //�Զ��嶯�����
    QStringList ssss1;//����ħ��
    ssss1<<"F2"<<"B2"<<"D'"<<"F"<<"B"<<"F"<<"R'"<<"F"<<"D2"<<"R2"<<"B"<<"R"<<"D'"<<"F"<<"L2"<<"B'"<<"L"<<"R"<<"L'"<<"D2"<<"B'"<<"D"<<"R'"<<"R"<<"D2"<<"L'"<<"F'"<<"B'"<<"R"<<"B"<<"F"<<"D"<<"R2"<<"F2";
    QStringList ssss2;//��ɫ��
    ssss2 <<"F2"<<"B"<<"D2"<<"B'"<<"F"<<"D2"<<"B2"<<"D'"<<"B'"<<"D"<<"B"<<"R2"<<"D'"<<"L'";
    QStringList ssss3;//ͬɫ��������ɫ����ʼ��
    ssss3 <<"D'"<<"B2"<<"L2"<<"D"<<"R2"<<"D2"<<"R2"<<"D2"<<"F2";
    QStringList ssss4;//ͬɫ��(�Ӹ�ԭ̬��ʼ)
    ssss4 <<"F2"<<"D2"<<"F"<<"D'"<<"L"<<"F2"<<"L"<<"D2"<<"F2"<<"B2"<<"D"<<"L"<<"F2"<<"D"<<"B2"<<"D'"<<"F2"<<"L2"<<"D'"<<"L2"<<"B2"<<"R2"<<"D2"<<"F2"<<"D2"<<"F2"<<"R2"<<"D2"<<"L2"<<"D2";
    QStringList ssss5;//�Ժõ�һ������ĸ������T���Σ���ͬɫ����ʼ��
    ssss5 <<"L'"<<"D2"<<"L"<<"F"<<"D"<<"F'"<<"D'"<<"L"<<"D2"<<"L'"<<"D2"<<"F'"<<"D2"<<"F"<<"D'"<<"B"<<"D"<<"B'"<<"D'";
    QStringList ssss6;//�������������
    ssss6 << "F2"<<"B2"<<"D'"<<"F"<<"B"<<"D"<<"R'"<<"B2"<<"D"<<"L"<<"R"<<"D"<<"L"<<"F2"<<"L2"<<"D'"<<"R2"<<"F2"<<"D'"<<"F2"<<"D"<<"F2"<<"D'"<<"L2"<<"R2"<<"B2"<<"L2"<<"D2"<<"B2"<<"D2"<<"B2"<<"D2"<<"F2";
    QStringList ssss7;//������λ����ȷ��ɫ��ת
    ssss7 <<"D'"<<"R'"<<"D'"<<"R'"<<"D'"<<"R"<<"D"<<"R"<<"D"<<"R"<<"D"<<"F2"<<"B2"<<"D'"<<"F"<<"B"<<"D"<<"R'"<<"B2"<<"D"<<"L"<<"R"<<"D"<<"L"<<"F2"<<"L2"<<"D'"<<"R2"<<"F2"<<"D'"<<"F2"<<"D"<<"F2"<<"D'"<<"L2"<<"R2"<<"B2"<<"L2"<<"D2"<<"B2"<<"D2"<<"B2"<<"D2"<<"F2";
    QStringList ssss8;//�������Ҳ�����
    ssss8 <<"R2"<<"D'"<<"R'"<<"D'"<<"R'"<<"D"<<"R"<<"D"<<"R"<<"D"<<"R"<<"F"<<"D"<<"F"<<"D"<<"F'"<<"D'"<<"F'"<<"D'"<<"F'"<<"F2"<<"B2"<<"D'"<<"F"<<"B"<<"D"<<"R'"<<"B2"<<"D"<<"L"<<"R"<<"D"<<"L"<<"F2"<<"L2"<<"D'"<<"R2"<<"F2"<<"D'"<<"F2"<<"D"<<"F2"<<"D'"<<"L2"<<"R2"<<"B2"<<"L2"<<"D2"<<"B2"<<"D2"<<"B2"<<"D2"<<"F2";
    QStringList ssss9;//���Ĳ���
    ssss9 << "F2"<<"D2"<<"L"<<"R'"<<"F"<<"B2"<<"R'"<<"D"<<"L2"<<"D'"<<"L2"<<"D"<<"L"<<"D2"<<"F2"<<"L2"<<"D'"<<"L2"<<"D'"<<"R2"<<"D"<<"F2"<<"D'"<<"L2"<<"B2"<<"D2"<<"B2"<<"L2"<<"D2"<<"B2"<<"L2"<<"B2"<<"R2"<<"D2"<<"F2"<<"D2";
    QStringList ssss10;//���Ĳ�С����
    ssss10 << "B"<<"L'"<<"B"<<"D"<<"L'"<<"R'"<<"D'"<<"F2"<<"L"<<"D"<<"R'"<<"D2"<<"B2"<<"D"<<"L2"<<"D'"<<"L2"<<"F2"<<"D'"<<"F2"<<"B2"<<"D2"<<"B2"<<"D2"<<"L2"<<"F2"<<"R2"<<"D2"<<"R2"<<"D2"<<"L2";
    QStringList ssss11;//���Ĳ�һ��
    ssss11 << "D'"<<"L'"<<"B'"<<"D'"<<"B'"<<"L'"<<"F2"<<"D"<<"R2"<<"F2"<<"D"<<"L"<<"D2"<<"R2"<<"D'"<<"R2"<<"D'"<<"B2"<<"D"<<"L2"<<"D'"<<"R2"<<"D'"<<"D2"<<"F2"<<"L2"<<"B2"<<"R2"<<"D2"<<"R2"<<"F2"<<"L2"<<"F2"<<"D2";
    QStringList ssss12;//���Ĳ�ʮ��
    ssss12 << "L'"<<"D'"<<"B2"<<"L"<<"B2"<<"L2"<<"D"<<"L"<<"B2"<<"D'"<<"B2"<<"D"<<"L2"<<"D'"<<"B2"<<"D"<<"B2"<<"L2"<<"D"<<"L2"<<"D2"<<"L2"<<"D2"<<"L2"<<"B2"<<"D2"<<"B2"<<"D2"<<"L2"<<"B2";
    QStringList ssss13;//���岽���1
    ssss13 << "R"<<"D'"<<"L'"<<"D"<<"R'"<<"D'"<<"L"<<"R2"<<"D'"<<"B2"<<"D"<<"R2"<<"D'"<<"B2"<<"D"<<"R2"<<"D"<<"B2"<<"D2"<<"R2"<<"D2"<<"B2"<<"D2"<<"B2"<<"R2";
    QStringList ssss14;//���岽���2
    ssss14 << "L"<<"F2"<<"L'"<<"D'"<<"R"<<"D'"<<"R"<<"D2"<<"F2"<<"D'"<<"R2"<<"D2"<<"F2"<<"D"<<"R2"<<"D2"<<"F2"<<"D2"<<"F2"<<"D2"<<"F2"<<"D2"<<"F2"<<"L2"<<"F2"<<"D2"<<"L2"<<"D2"<<"F2"<<"L2";
    QStringList ssss15;//���岽���3
    ssss15 << "L2"<<"D'"<<"R'"<<"D"<<"L2"<<"D'"<<"R"<<"D2"<<"F2"<<"D'"<<"L2"<<"D"<<"F2"<<"D"<<"L2"<<"D2"<<"F2"<<"D2"<<"F2"<<"D2"<<"F2"<<"L2"<<"D2";
    QStringList ssss16;//���岽���4
    ssss16 <<"F"<<"D2"<<"F'"<<"D'"<<"F"<<"D"<<"F'"<<"D'"<<"F"<<"D'"<<"F2"<<"D2"<<"F"<<"D"<<"F'"<<"D"<<"F"<<"L2"<<"D'"<<"R'"<<"D"<<"L2"<<"D'"<<"R"<<"D2"<<"F2"<<"D'"<<"L2"<<"D"<<"F2"<<"D"<<"L2"<<"D2"<<"F2"<<"D2"<<"F2"<<"D2"<<"F2"<<"L2"<<"D2";
    QStringList ssss17;//���岽���5
    ssss17 << "L"<<"R2"<<"D'"<<"R2"<<"D"<<"L"<<"D2"<<"R2"<<"D"<<"R2"<<"D'"<<"L2"<<"D"<<"R2"<<"D'"<<"F2"<<"D"<<"F2"<<"D"<<"L2"<<"D2"<<"F2"<<"R2"<<"B2"<<"L2"<<"B2"<<"D2"<<"L2";
    QStringList ssss18;//���岽���6
    ssss18 << "F2"<<"L'"<<"D'"<<"R'"<<"B2"<<"R'"<<"D"<<"L"<<"F2"<<"L2"<<"D'"<<"R2"<<"B2"<<"D"<<"R2"<<"B2"<<"R2"<<"F2"<<"L2"<<"D2"<<"B2"<<"R2"<<"D2"<<"B2"<<"D2";
    QStringList ssss19;//���岽���7
    ssss19 << "L'"<<"D'"<<"B2"<<"L"<<"B2"<<"L2"<<"D"<<"L"<<"B2"<<"D'"<<"B2"<<"D"<<"L2"<<"D'"<<"B2"<<"D"<<"B2"<<"L2"<<"D"<<"L2"<<"D2"<<"L2"<<"D2"<<"L2"<<"B2"<<"D2"<<"B2"<<"D2"<<"L2"<<"B2";
    QStringList ssss20;//���������1
    ssss20 << "F2"<<"D'"<<"F2"<<"D"<<"R2"<<"D'"<<"F2"<<"D"<<"F2"<<"R2"<<"D2"<<"R2"<<"F2"<<"D2"<<"R2"<<"F2"<<"D2"<<"R2"<<"F2";
    QStringList ssss21;//���������2
    ssss21 << "F2"<<"D'"<<"F2"<<"D2"<<"R2"<<"D"<<"R2"<<"D'"<<"R2"<<"D2"<<"F2"<<"D"<<"R2"<<"D2"<<"F2"<<"D2"<<"R2"<<"F2";
    QStringList ssss22;//���߲� ������ʱ�뽻��
    ssss22 << "F2"<<"D'"<<"L2"<<"F2"<<"L2"<<"D'"<<"L2"<<"D2"<<"L2"<<"D2"<<"F2"<<"L2"<<"F2"<<"D2"<<"F2"<<"D2";
    QStringList ssss23;//���߲�����˳ʱ�뽻��
    ssss23 << "B2"<<"D'"<<"L2"<<"B2"<<"L2"<<"D'"<<"L2"<<"B2"<<"D2"<<"L2"<<"B2"<<"L2"<<"D2"<<"L2";
    QStringList ssss24;//���߲� ���潻��
    ssss24 << "L2"<<"F2"<<"D2"<<"L2"<<"D2"<<"F2"<<"L2"<<"F2"<<"D2"<<"F2"<<"D2"<<"F2";
    QStringList ssss25;//���߲� ���ҽ���
    ssss25 << "F2"<<"L2"<<"R2"<<"F2"<<"D'"<<"F2"<<"L2"<<"R2"<<"F2"<<"D"<<"R2"<<"F2"<<"D2"<<"R2"<<"F2"<<"R2"<<"F2"<<"L2"<<"F2"<<"D2"<<"F2"<<"L2";
    QStringList ssss26;//������ ���3
    ssss26 << "D";
    //ѡ����ʾ����
    //����1������ħ��
    connect(ui->btn_1_1, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣�����ħ��")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss1.length(); i > 0; i--)
        {
            cube->rotate(ssss1[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss1[i-1]);//����ͨ��
        }
    });
    //����2�����İ׻�
    connect(ui->btn_1_3, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣���ɫ��")));
        for (int i = ssss2.length(); i > 0; i--)
        {
            cube->rotate(ssss2[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss2[i-1]);//����ͨ��
        }
    });
    //����3��ͬɫ�׻�
    connect(ui->btn_1_5, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣�ͬɫ��")));
        for (int i = ssss3.length(); i > 0; i--)
        {
            cube->rotate(ssss3[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss3[i-1]);//����ͨ��
        }
    });
    //����4��ͬɫ�׻�
    connect(ui->btn_2_1, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣�ͬɫ��")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss4.length(); i > 0; i--)
        {
            cube->rotate(ssss4[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss4[i-1]);//����ͨ��
        }
    });
    //����5���Ժõ�һ������ĸ������T���Σ���ͬɫ����ʼ��
    connect(ui->btn_2_3, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣��Ժõ�һ������ĸ������T����")));
        for (int i = ssss5.length(); i > 0; i--)
        {
            cube->rotate(ssss5[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss5[i-1]);//����ͨ��
        }
    });
    //����6���������������
    connect(ui->btn_3_1, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣��������������")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss6.length(); i > 0; i--)
        {
            cube->rotate(ssss6[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss6[i-1]);//����ͨ��
        }
    });
    //����7��������λ����ȷ��ɫ��ת
    connect(ui->btn_3_2, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣�������λ����ȷ��ɫ��ת")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss7.length(); i > 0; i--)
        {
            cube->rotate(ssss7[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss7[i-1]);//����ͨ��
        }
    });
    //����8���������Ҳ�����
    connect(ui->btn_3_3, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣��������Ҳ�����")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss8.length(); i > 0; i--)
        {
            cube->rotate(ssss8[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss8[i-1]);//����ͨ��
        }
    });
    //����9�����Ĳ�С����
    connect(ui->btn_4_1, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣����Ĳ�С����")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss9.length(); i > 0; i--)
        {
            cube->rotate(ssss9[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss9[i-1]);//����ͨ��
        }
    });
    //����10�����Ĳ�һ��
    connect(ui->btn_4_2, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣����Ĳ�һ��")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss10.length(); i > 0; i--)
        {
            cube->rotate(ssss10[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss10[i-1]);//����ͨ��
        }
    });
    //����11�����Ĳ���
    connect(ui->btn_4_3, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣����Ĳ���")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss11.length(); i > 0; i--)
        {
            cube->rotate(ssss11[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss11[i-1]);//����ͨ��
        }
    });
    //����12�����Ĳ�ʮ��
    connect(ui->btn_4_4, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣����Ĳ�ʮ��")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss12.length(); i > 0; i--)
        {
            cube->rotate(ssss12[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss12[i-1]);//����ͨ��
        }
    });
    //����13�����岽���1
    connect(ui->btn_5_1, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣����岽���1")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss13.length(); i > 0; i--)
        {
            cube->rotate(ssss13[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss13[i-1]);//����ͨ��
        }
    });
    //����14�����岽���2
    connect(ui->btn_5_2, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣����岽���2")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss14.length(); i > 0; i--)
        {
            cube->rotate(ssss14[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss14[i-1]);//����ͨ��
        }
    });
    //����15�����岽���3
    connect(ui->btn_5_3, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣����岽���3")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss15.length(); i > 0; i--)
        {
            cube->rotate(ssss15[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss15[i-1]);//����ͨ��
        }
    });
    //����16�����岽���4
    connect(ui->btn_5_4, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣����岽���4")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss16.length(); i > 0; i--)
        {
            cube->rotate(ssss16[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss16[i-1]);//����ͨ��
        }
    });
    //����17�����岽���5
    connect(ui->btn_5_5, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣����岽���5")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss17.length(); i > 0; i--)
        {
            cube->rotate(ssss17[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss17[i-1]);//����ͨ��
        }
    });
    //����18�����岽���6
    connect(ui->btn_5_6, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣����岽���6")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss18.length(); i > 0; i--)
        {
            cube->rotate(ssss18[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss18[i-1]);//����ͨ��
        }
    });
    //����19�����岽���7
    connect(ui->btn_5_7, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣����岽���7")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss19.length(); i > 0; i--)
        {
            cube->rotate(ssss19[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss19[i-1]);//����ͨ��
        }
    });
    //����20�����������1
    connect(ui->btn_6_1, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣����������1")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss20.length(); i > 0; i--)
        {
            cube->rotate(ssss20[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss20[i-1]);//����ͨ��
        }
    });
    //����21�����������2
    connect(ui->btn_6_2, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣����������2")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss21.length(); i > 0; i--)
        {
            cube->rotate(ssss21[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss21[i-1]);//����ͨ��
        }
    });
    //����22�����߲�����˳ʱ�뽻��
    connect(ui->btn_7_1, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣����߲�����˳ʱ�뽻��")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss22.length(); i > 0; i--)
        {
            cube->rotate(ssss22[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss22[i-1]);//����ͨ��
        }
    });
    //����23�����߲� ������ʱ�뽻��
    connect(ui->btn_7_2, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣����߲� ������ʱ�뽻��")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss23.length(); i > 0; i--)
        {
            cube->rotate(ssss23[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss23[i-1]);//����ͨ��
        }
    });
    //����24�����߲� ���潻��
    connect(ui->btn_7_3, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣����߲� ���潻��")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss24.length(); i > 0; i--)
        {
            cube->rotate(ssss24[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss24[i-1]);//����ͨ��
        }
    });
    //����25�����߲� ���ҽ���
    connect(ui->btn_7_4, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣����߲� ���ҽ���")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss25.length(); i > 0; i--)
        {
            cube->rotate(ssss25[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss25[i-1]);//����ͨ��
        }
    });
    //����26�������� ���3
    connect(ui->btn_6_4, &QPushButton::clicked, [=](){
        addLog(QString(codec->toUnicode("����ʵ�֣������� ���3")));
        cubeSolute();//�Ȼص���ԭ״̬
        for (int i = ssss26.length(); i > 0; i--)
        {
            cube->rotate(ssss26[i-1]);//�����һ�׶���
            MainWindow::send_command(ssss26[i-1]);//����ͨ��
        }
    });

    /*************************�����ǳ���ʵ��***************************/


    /************************����ͨ��****************************/
    serialport = new QSerialPort;
    find_port();          //���ҿ��ô���
    isPortEnable = 0;

    //�Ƿ�򿪴���
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

    /**********************�����Ǵ���ͨ��************************/

    // �̶����㲻��
    connect(ui->check_fixTopLayer, &QCheckBox::stateChanged, [=](){
        if (ui->check_fixTopLayer->isChecked()) CubeValue::CUBE_TOPLAYER_FIX = 1;
        else CubeValue::CUBE_TOPLAYER_FIX = 0;
    });

    // �¼�׷��
    ui->widget_draw->installEventFilter(this);
    this->setMouseTracking(true);
    ui->centralWidget->setMouseTracking(true);
    ui->widget_teach->setMouseTracking(true);
    ui->widget_draw->setMouseTracking(true);
    //    setWindowFlags(Qt::WindowCloseButtonHint); //���ر������ʺ�
}

/***********************************************������������*****************************************************/

/*void MainWindow::getcolorsensor()
{
    // Ӧ�ô�������ɫ����
    connect(serialport,SIGNAL(readyRead()), this, SLOT(readyReadSlot()));
}*/

//���Ҵ���
void MainWindow::find_port()
{
    //���ҿ��õĴ���
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);   //���ô���
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->comboBox_port->addItem(serial.portName());        //��ʾ����name
            serial.close();
        }
    }
}

//��ʱ����
void MainWindow::sleep( int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//�򿪴���
void MainWindow::open_port()
{
    update();
    sleep(100);      //��ʱ100ms
    find_port();     //���²���com
    //��ʼ������
    serialport->setPortName(ui->comboBox_port->currentText());        //���ô�����
    if(serialport->open(QIODevice::ReadWrite))              //�򿪴��ڳɹ�
    {
        serialport->setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections);//���ò����ʺͶ�д����,ħ����������λ���Ĳ�����Ϊ115200
        serialport->setDataBits(QSerialPort::Data8);		//����λΪ8λ
        serialport->setFlowControl(QSerialPort::NoFlowControl);//��������
        serialport->setParity(QSerialPort::NoParity);	//��У��λ
        serialport->setStopBits(QSerialPort::OneStop); //һλֹͣλ
        qDebug()<<codec->toUnicode("�ɹ��򿪴���");
        //�򿪴��ں�ر�U������İ�ť�������ڴ��ڴ�ʱ����U����bug
        ui->btn_step1->setEnabled(false);
        ui->btn_step7->setEnabled(false);
        ui->btn_step13->setEnabled(false);
        //serialport->write("01");
    }
    else    //��ʧ����ʾ
    {
        sleep(100);
        QMessageBox::information(this,tr("Error"),tr("Failed to open"),QMessageBox::Ok);
    }

}

//�رմ���
void MainWindow::close_port()
{
    serialport->clear();        //��ջ�����
    qDebug()<<codec->toUnicode("�رմ���");
    serialport->close();        //�رմ���

    //�رմ��ں�ָ�U������İ�ť
    ui->btn_step1->setEnabled(true);
    ui->btn_step7->setEnabled(true);
    ui->btn_step13->setEnabled(true);
}

//���������λ��
void MainWindow::send_command(const QString & rotateStr)
{
    if(MainWindow::isPortEnable)
    {
        QByteArray* temp_array;
        temp_array = MainWindow::convert_command(rotateStr);
        serialport->write(*temp_array);
        MainWindow::sleep(1200);//��ʱ�������ٶ�
        delete temp_array;
        qDebug()<<codec->toUnicode("����͵���λ��");
    }
}

//������ɫ������������
void MainWindow::getcolorsensor()
{
    int receBytes = 0;
    sleep(30);
    colorstate = serialport->readAll();
    if (!colorstate.isEmpty())
    {
        qDebug()<<codec->toUnicode("������ɫ������������:");
        qDebug()<<colorstate;
        receBytes += colorstate.size();
        QString redata = QString("colorstate size received:%1").arg(QString::number(receBytes));
        addLog(QString(redata));
    }
    else
    {
        qDebug()<<codec->toUnicode("��ɫ�����������ݷ���");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete axis;
    delete cube;
}

// �����־
void MainWindow::addLog(const QString & str)
{
    QString log = ui->edit_log->toPlainText();
    // �Զ�����
    if (false == log.endsWith('\n') && false == log.isEmpty())
        log += '\n';
    log += str;
    ui->edit_log->setText(log);
    // �Զ��ƶ�����־�ײ�
    ui->edit_log->moveCursor(QTextCursor::End);
    // �����־�ļ�
    logCreator.saveLog(str + '\n');
}

// ���ÿؼ�״̬
void MainWindow::setStatus(bool status)
{
    // ��֤����ת���������У��û��޷�����ĳЩ���������
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
    if (status) ui->btn_DIYCube->setText(codec->toUnicode("�Զ���ħ����ɫ..."));
    else ui->btn_DIYCube->setText(codec->toUnicode("Ӧ��ħ���༭"));
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
    if (status) ui->btn_colorsensor->setText(codec->toUnicode("��ȡ��ɫ����������..."));
    else ui->btn_colorsensor->setText(codec->toUnicode("Ӧ����ɫ����������"));
}

// ħ�����
void MainWindow::cubeSolute()
{
    if(MainWindow::isPortEnable)
        ui->check_fixTopLayer->setCheckState(Qt::Checked);
    // �ж�ħ���Ƿ��ڸ�ԭ״̬
    if (cube->isInResetStatus()) addLog(QString(codec->toUnicode("�ɽ�")));
    else
    {
        if (cube->isSoluteble())
        {
            addLog(QString(codec->toUnicode("�ɽ⣬��ѯ�ⷨ��...")));
            // ��ȡħ����״̬
            stateType s = cube->state();
            for(int i = 0; i < s.length(); i++)
            {
                const bspd& ss = s.at(i);
                qDebug()<<"ss.n"<<ss.n;
            }
            if (s.isEmpty())
            {
                addLog(QString(codec->toUnicode("���󣺵�ǰħ��״̬����")));
                return;
            }
            // ������⣬��ʱ
            CubeSolution solution;
            int startTime = QTime::currentTime().msecsSinceStartOfDay();
            moveList answer = solution.getSolution(s);
            qDebug()<<codec->toUnicode("answer")<<answer;
            int endTime = QTime::currentTime().msecsSinceStartOfDay();
            if (answer.isEmpty())
            {
                switch (CubeValue::ERRO_CODE) {
                case OverTime:
                    QMessageBox::information(nullptr, codec->toUnicode("��ʾ"), codec->toUnicode("��ⳬʱ����ʱ��ֵΪ20s"));
                    addLog(QString(codec->toUnicode("��ⳬʱ%1ms").arg(endTime - startTime)));
                    break;
                case UnSolveble:
                    QMessageBox::information(nullptr, codec->toUnicode("��ʾ"),
                                             QString(codec->toUnicode("�޷���⣬��ʱ%1ms")).arg(endTime - startTime));
                    addLog(QString(codec->toUnicode("�޷���⵱ǰħ��������ʱ��%1ms")).arg(endTime - startTime));
                case Normal:
                default:
                    break;
                }
            }
            else
            {
                // ���м򵥵Ĳ��軯�򡢺ϲ�
                QStringList ssss = CubeRotateValue::simplify(answer);
                qDebug()<<codec->toUnicode("ssss")<<ssss;
                addLog(QString(codec->toUnicode("�ⷨΪ:����%1����ʱ%2ms")).arg(ssss.length()).arg(endTime - startTime));
                addLog(ssss.join(' '));
                addLog(QString(codec->toUnicode("��һ�׶�:����%1")).arg(solution.phaseSteps[0]));
                addLog(QString(codec->toUnicode("�ڶ��׶�:����%1")).arg(solution.phaseSteps[1]));
                addLog(QString(codec->toUnicode("�����׶�:����%1")).arg(solution.phaseSteps[2]));
                addLog(QString(codec->toUnicode("���Ľ׶�:����%1")).arg(solution.phaseSteps[3]));
                // ���и�ԭ
                if (cube->isEnableRotating) setStatus2(false);
                for (int i = 0; i < ssss.length(); i++)
                {
                    cube->rotate(ssss[i]);
                    MainWindow::send_command(ssss[i]);//����ͨ��
                }
                if (!cube->isEnableRotating) update();
            }
        }
        else addLog(QString(codec->toUnicode("���ɽ�")));
    }
    if(MainWindow::isPortEnable)
        ui->check_fixTopLayer->setCheckState(Qt::Unchecked);
}

// �����ɫ״̬����ֵ
void MainWindow::checkDeltaInPattern(int L, int C)
{
    // �������ĸ�����
    int stateIndex = CubeValue::N2STATE[L][C];
    // �÷�����м�����
    int eleCount = (stateIndex < 12) ? 2 : 3;
    // �÷��������������
    QVector<int> ele(eleCount, 0);
    // �÷����һ���������
    ele[0] = CubeValue::STATE2N[stateIndex];
    // �÷��������
    QString ds;
    for (int i = 0; i < eleCount; i++)
    {
        int N = ele[i];
        int index = pa[N / 12][N % 12].second;
        ds += CubeValue::CUBE_FACE[index - 1];
        if (eleCount - 1 != i) ele[i + 1] = CubeValue::N2NEXT[N];
    }
    // �÷���ķ���ֵ
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
    if (3 == d) d = -1; // ��Ч����ֵ
    // Ӧ�ø÷���ֵ
    for (int i = 0; i < eleCount; i++)
    {
        int N = ele[i];
        int index = pa[N / 12][N % 12].second;
        pa[N / 12][N % 12] = patternElementType(d, index);
    }
}

// �����ɫ״̬��������
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

// �����ɫ״̬���Ƿ�ɽ�
bool MainWindow::isPatternSoluteble(pas * blockMsg)
{
    // ���з����Ƿ���ɫ�ɹ�
    if (!isPatternCompleted())
    {
        addLog(QString(codec->toUnicode("���󣺴���ɫ����ִ������ɫ��")));
        return false;
    }

    // �Ƿ����20���������Ϣ
    bool iBblockMsgInit[20] = {false};
    // 12�����ı��+8���ǿ�ı��
    int series[20] = {0};
    // ���/�ǿ�������������
    int s_edge = 0, s_corner = 0;
    // ���/�ǿ鷽��ֵ�ܺ�
    int d_edge = 0, d_corner = 0;
    for (int stateIndex = 0; stateIndex < 20; stateIndex++)
    {
        // �Ǻų���(λ������)
        int eleCount = (stateIndex < 12) ? 2:3;
        int N = CubeValue::STATE2N[stateIndex];
        int L = N / 12, C = N % 12;
        // ����ɽ���
        if (2 == eleCount) d_edge += pa[L][C].first;
        else d_corner += pa[L][C].first;
        // ��������
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
        // ������ɫ
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
        // �������np
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
        // �洢��Ϣ
        if (iBblockMsgInit[np])
        {
            addLog(QString(codec->toUnicode("���󣺴����ظ�ɫ�棡�ظ�������Ϊ��%1")).arg(np));
            return false;
        }
        iBblockMsgInit[np] = true;
        series[stateIndex] = np;
        if (nullptr != blockMsg)
        {
            blockMsg[np].p = stateIndex; // ���np�ķ�����λ��stateIndex��
            blockMsg[np].x = x; // ���np�ķ�����λ��[x,y,z]��
            blockMsg[np].y = y;
            blockMsg[np].z = z;
            for (int i = 0; i < 6; i++) blockMsg[np].cd[i] = cld[i]; // ���np�ķ�����ɫΪcld[]
        }
    }

    // �����Ϣ������
    for (int i = 0; i < 20; i++)
    {
        if (!iBblockMsgInit[i])
        {
            addLog(QString(codec->toUnicode("���󣺴���ȱʧɫ�棡ȱʧ������Ϊ��%1")).arg(i));
            return false;
        }
    }

    // �������������������
    for (int i = 0; i < 12; i++) for (int j = i + 1; j < 12; j++) s_edge ^= series[i] > series[j];
    for (int i = 12; i < 20; i++) for (int j = i + 1; j < 20; j++) s_corner ^= series[i] > series[j];
    // ����Ƿ�ɽ⣺��鷽��ֵ�ܺ���ż�����ǿ鷽��ֵ�ܺ���3�ı����������������������ǿ�������������һ��
    if (0 != d_edge % 2 || 0 != d_corner % 3 || s_edge != s_corner)
    {
        addLog(QString(codec->toUnicode("���󣺵�ǰħ�����ɽ�")));
        return false;
    }

    return true;
}

// Ӧ����ɫ״̬��
void MainWindow::applyPatternSolution()
{
    // ��ɫ״̬���Ƿ�ɽ�
    pas blockMsg[20];
    if (!isPatternSoluteble(blockMsg)) return;

    // Ӧ����Щ������Ϣ
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

// ��ͼ
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    // ���˿ؼ���ͼ�¼�
    if (watched == ui->widget_draw && event->type() == QEvent::Paint)
    {
        // ��������
        if (InterateTime == runTimeType)
        {
            QPainter painter(ui->widget_draw);
            // ����������
            axis->draw(&painter);
            // ����ħ��
            cube->draw(&painter);
        }
        // ��ͼ�༭
        else
        {
            int g = CubeValue::CUBE_COLOR_GROUP;
            QPainter painter(ui->widget_draw);
            painter.drawText(10, 20,codec->toUnicode("��ǰʰȡ��ɫ��"));
            painter.drawText(175, 265,codec->toUnicode("�����"));
            painter.drawText(475, 265, codec->toUnicode("�Ҳ���"));
            painter.drawText(625, 265,codec->toUnicode( "����"));
            painter.drawText(325, 115, codec->toUnicode("�϶���"));
            painter.drawText(225, 505, codec->toUnicode("�µ���"));
            painter.setBrush(CubeValue::CUBE_COLOR[g][selectColorIndex]);
            painter.drawRect(10, 30, 50, 50);
            drawPattern(&painter, 50);
        }
        return true;
    }
    return QMainWindow::eventFilter(watched, event);
}

// ������ɫ״̬��
void MainWindow::drawPattern(QPainter *painter, int size)
{
    int g = CubeValue::CUBE_COLOR_GROUP;
    int w = painter->device()->width()-1;
    int h = painter->device()->height()-1;
    int ox = w - 12 * size;
    int oy = h - 9 * size;
    // ������ɫ״̬��
    for (int line = 0; line < 9; line++)
    {
        for (int column = 0; column < 12; column++)
        {
            patternElementType m = pa[line][column];
            if (-1 != m.second)
            {
                // ������ɫ
                painter->setBrush(CubeValue::CUBE_COLOR[g][m.second]);
                painter->drawRect(ox + size * column, oy - 50 + size * line, size, size);
                // ����״̬������ֵ
                painter->drawText(ox + size * column, oy - 50 + size * line, size, size,
                                  Qt::AlignCenter, QString::number(m.first));
            }
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    // ��ħ���༭�����£���������¼�����
    if (PaintingTime == runTimeType)
    {
        QPoint point = ui->widget_draw->mapFromGlobal(event->globalPos());
        int w = ui->widget_draw->width();
        int h = ui->widget_draw->height();
        int x = point.x();
        int y = point.y() + 50;
        // λ��ui->widget_draw����
        if (0 <= x && x <= w && 0 <= y && y <= h)
        {
            int C = (w - x) / 50;
            int L = (h - y) / 50;
            // λ����Ч����
            if ((0 <= C && C <= 11 && 3 <= L && L <= 5) || (6 <= C && C <= 8 && 0 <= L && L <= 8))
            {
                // λ�����Ŀ飺ʰȡ���(ʮ�ӹ��)
                int n = 107 - L * 12 - C;
                if (16 == n || 49 == n || 52 == n || 55 == n || 58 == n || 88 == n) setCursor(Qt::CrossCursor);
                // λ�ڿɱ༭ɫ�飺��ɫ���(������)
                else setCursor(Qt::PointingHandCursor);
            }
            // ��������Ĭ�Ϲ��
            else setCursor(Qt::ArrowCursor);
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // ��ħ���༭�����£���������¼�����
    if (PaintingTime == runTimeType && event->button() == Qt::LeftButton)
    {
        QPoint point = ui->widget_draw->mapFromGlobal(event->globalPos());
        int w = ui->widget_draw->width();
        int h = ui->widget_draw->height();
        int x = point.x();
        int y = point.y() + 50;
        int C = (w - x) / 50;
        int L = (h - y) / 50;
        // ����Ӧ����
        if ((0 <= C && C <= 11 && 3 <= L && L <= 5) || (6 <= C && C <= 8 && 0 <= L && L <= 8))
        {
            emit clickPattern(8 - L, 11 - C);
        }
    }
}

//�޸���ɫ
void MainWindow::setColor(int L, int C)
{
    // ʰȡ���Ŀ���ɫ
    int n = L * 12 + C;
    if (16 == n || 49 == n || 52 == n || 55 == n || 58 == n || 88 == n)
    {
        selectColorIndex = pa[L][C].second;
        update();
    }
    // �ı�����ɫ����ɫ
    else
    {
        pa[L][C] = patternElementType(pa[L][C].first, selectColorIndex);
        checkDeltaInPattern(L, C);
        update();
    }
}

//��ɫ��������ɫת��
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
        qDebug()<<codec->toUnicode("��ɫת�����");
    }
}

//Ӧ����ɫ����������
void MainWindow::setcolorsensor()
{
    int L, C;
    // ����ɫ
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
        // �ı���ɫ״̬����ɫ
        for(L=0;L<9;L++)
            for(C=0;C<12;C++)
            {
                checkDeltaInPattern(L, C);
                update();
            }
        qDebug()<<codec->toUnicode("������ɫ�����������ɹ�");
    }
    else
    {
        qDebug()<<codec->toUnicode("������ɫ����������ʧ��");
    }
}

/*********************************����*************************************/
//����ͨ������ת������
QByteArray* MainWindow::convert_command(const QString& command){
    QByteArray* array = new QByteArray();
    array->resize(6);
    (*array)[0] = 0x66;
    (*array)[1] = 0x31;
    (*array)[2] = 0x02;
    (*array)[5] = 0x63;
    if(command == "B")
    {
        (*array)[3] = 0x42;/*ֱ����ASCII�������ĸB*/
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
        (*array)[3] = 0x44;/*ֱ����ASCII�������ĸD*/
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
        (*array)[3] = 0x46;/*ֱ����ASCII�������ĸF*/
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
        (*array)[3] = 0x4c;/*ֱ����ASCII�������ĸL*/
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
        (*array)[3] = 0x52;/*ֱ����ASCII�������ĸR*/
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
    view = new videoplayer(this) ;//����Ƶ����ָ��ʵ����
    view->show();
}

void MainWindow::on_btn_2_2_clicked(bool checked)
{
    view1 = new videoplayer(this) ;//����Ƶ����ָ��ʵ����
    view1->show();
}

void MainWindow::on_btn_3_4_clicked(bool checked)
{
    view2 = new videoplayer(this) ;//����Ƶ����ָ��ʵ����
    view2->show();
}

void MainWindow::on_btn_4_5_clicked(bool checked)
{
    view3 = new videoplayer(this) ;//����Ƶ����ָ��ʵ����
    view3->show();
}

void MainWindow::on_btn_5_8_clicked(bool checked)
{
    view4 = new videoplayer(this) ;//����Ƶ����ָ��ʵ����
    view4->show();
}

void MainWindow::on_btn_6_3_clicked(bool checked)
{
    view5 = new videoplayer(this) ;//����Ƶ����ָ��ʵ����
    view5->show();
}

void MainWindow::on_btn_7_5_clicked(bool checked)
{
    view6 = new videoplayer(this) ;//����Ƶ����ָ��ʵ����
    view6->show();
}

void MainWindow::on_btn_step1_clicked(){}

void MainWindow::on_btn_step1_clicked(bool checked){}

//�رմ�����ʾ
void MainWindow::closeEvent(QCloseEvent *event)
{
    int b=QMessageBox::question(this,QString(codec->toUnicode("��ʾ")),QString(codec->toUnicode("�Ƿ�رճ���")),
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

// //�������¼�ѭ��-����������ʱ��
//void MainWindow::delay_ms(unsigned int ms)
//{
//    QEventLoop loop;//����һ���µ��¼�ѭ��
//    QTimer::singleShot(ms, &loop, SLOT(quit()));//�������ζ�ʱ�����ۺ���Ϊ�¼�ѭ�����˳�����
//    loop.exec();//�¼�ѭ����ʼִ�У�����Ῠ�����ֱ����ʱʱ�䵽����ѭ�����˳�
//}
