#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QPushButton>
#include <QTimer>
#include <QSlider>
#include <QLabel>
#include "VisualBase.h"

// 主窗口类，包含侧边导航栏和右侧的多个演示界面堆叠
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 导航槽函数
    void showDashboard();
    void showDataStructures();
    void showAlgorithms();
    
    // 数据结构子堆栈控制
    void onDSNavChanged(int row);
    void dsPlayToggle();
    void dsStep();
    void dsReset();
    void dsPlayTimerTick();

    // 算法子堆栈控制
    void onAlgoNavChanged(int row);
    void algoPlayToggle();
    void algoStep();
    void algoReset();
    void algoPlayTimerTick();
    void autoStartDSPlay();
    void autoStartAlgoPlay();
    void updateDSSpeed(int value);
    void updateAlgoSpeed(int value);

private:
    // 初始化 UI 界面布局
    void initUI();
    // 设置侧边栏和主界面的样式表
    void setupStyles();
    // 建立侧边栏点击事件与堆叠窗口切换的信号连接
    void connectSignals();

    QWidget* createDashboardWidget();
    QWidget* createDataStructuresWidget();
    QWidget* createAlgorithmsWidget();

    QStackedWidget* mainStack; // 主堆栈组件：索引 0 为仪表盘，1 为数据结构平台，2 为算法平台

    // 数据结构页面相关组件
    QStackedWidget* dsSubStack;
    // 侧边栏的列表部件，用于选择不同的 STL 算法演示
    QListWidget* sidebar;
    // 堆叠窗口部件，用于容纳各种可视化界面并进行切换
    QStackedWidget* stackedWidget;

    // 定时器，用于自动播放动画
    QTimer* playTimer;
    // 当前是否正在自动播放
    bool isPlaying;

    // 播放控制按钮：播放/暂停
    QPushButton* btnPlay;
    // 播放控制按钮：单步执行
    QPushButton* btnStep;
    // 播放控制按钮：重置状态
    QPushButton* btnReset;
    // 播放控制按钮：退出程序
    QPushButton* btnExit;
    
    QListWidget* dsNavList;
    QPushButton* btnDSPlay;
    QPushButton* btnDSStepBack; // 数据结构"上一步"按钮
    QTimer* dsPlayTimer;
    QSlider* dsSpeedSlider;
    QLabel* lblDSSpeed; // 数据结构播放速度标签

    // 算法页面相关组件
    QStackedWidget* algoSubStack;
    QListWidget* algoNavList;
    QPushButton* btnAlgoPlay;
    QPushButton* btnAlgoStepBack; // 算法"上一步"按钮
    QTimer* algoPlayTimer;
    QSlider* algoSpeedSlider;
    QLabel* lblAlgoSpeed; // 算法播放速度标签

private slots:
    void dsStepBackward(); // 数据结构退一步槽函数
    void algoStepBackward(); // 算法退一步槽函数
    void dsExport(); // 数据结构导出截图槽函数
    void algoExport(); // 算法导出截图槽函数
};

#endif 
