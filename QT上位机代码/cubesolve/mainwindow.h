#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "video.h"
#include "videoplayer.h"
#include "axis.h"
#include "cube.h"
#include "mypushbutton.h"
#include "cubevalue.h"
#include "log.h"
#include <QThread>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTime>

namespace Ui {
class MainWindow;
}

// 文本类型 { 无效信息, 文本信息, 命令信息 }
enum ContentType { InvalidContent, TextContent, CommandContent };
// 运行类型 { 交互式, 绘图式 }
enum RunTimeType { InterateTime, PaintingTime };

class MainWindow : public QMainWindow
{
    Q_OBJECT
public slots:
    // 修改颜色
    void setColor(int C, int L);
    //串口通信命令转换函数
    QByteArray* convert_command(const QString& command);
    void find_port();       //查找可用串口
    void sleep(int msec);      //延时函数
    void open_port();  //打开串口
    void close_port(); //关闭串口
    void send_command(const QString & rotateStr); //发送命令到下位机
    void getcolorsensor();//接收STM8颜色参数
    void setcolorsensor();//应用颜色传感器参数
//    void delay_ms(unsigned int ms);
    void colorchange();
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //关闭窗口提示
    void closeEvent(QCloseEvent *event);
    // 日志
    Log logCreator;
    // 坐标系
    Axis * axis;
    // 步骤按钮组
    MyPushButton * btn_step[18];
    // 魔方
    Cube * cube;
    // 当前运行类型
    RunTimeType runTimeType;
    // 当前拾取颜色
    int selectColorIndex;
    // 颜色状态表
    patternType pa;
    //串口是否打开
    bool isPortEnable;
//    //串口相关
//    QSerialPort *serialport;
    // 辨别文本类型
    ContentType analizeContent(QString content, QString & result);
    // 添加日志
    void addLog(const QString & str);
    // 设置控件状态
    void setStatus(bool status);
    void setStatus2(bool status);
    void setStatus3(bool status);
    // 魔方求解
    void cubeSolute();
    // 绘图
    bool eventFilter(QObject *watched, QEvent *event);
    // 绘制颜色状态表
    void drawPattern(QPainter * painter, int size);
    // 鼠标事件
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    void on_btn_1_2_clicked(bool checked);
    void on_btn_2_2_clicked(bool checked);
    void on_btn_3_4_clicked(bool checked);
    void on_btn_4_5_clicked(bool checked);
    void on_btn_5_8_clicked(bool checked);
    void on_btn_6_3_clicked(bool checked);
    void on_btn_7_5_clicked(bool checked);
    void on_btn_step1_clicked(bool checked);
    void on_btn_step1_clicked();

private:
    Ui::MainWindow *ui;
    //创建视频窗口的指针
    videoplayer *view;
    videoplayer *view1;
    videoplayer *view2;
    videoplayer *view3;
    videoplayer *view4;
    videoplayer *view5;
    videoplayer *view6;
    // 检测颜色状态表方向值
    void checkDeltaInPattern(int L, int C);
    // 检测颜色状态表完整性
    bool isPatternCompleted();
    // 检测颜色状态表是否可解
    bool isPatternSoluteble(pas * blockMsg);
    // 应用颜色状态表
    void applyPatternSolution();
    QSerialPort *serialport;//串口相关

signals:
    void clickPattern(int C, int L);
};
#endif // MAINWINDOW_H
