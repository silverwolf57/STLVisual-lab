#include "MainWindow.h"
#include "VisualVector.h"
#include "VisualList.h"
#include "VisualStackQueue.h"
#include "VisualSort.h"
#include "VisualLowerBound.h"
#include "VisualReverse.h"
#include "VisualUnique.h"
#include "VisualMerge.h"
#include "VisualRemove.h"
#include "VisualRotate.h"
#include "VisualPermutation.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPixmap>

// MainWindow 构造函数：初始化所有界面组件并启动
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("STL Visual Lab - C++ 容器与算法可视化学习平台");
    resize(1080, 720);

    mainStack = new QStackedWidget(this);
    setCentralWidget(mainStack);

    // 初始化各个子界面的播放定时器
    dsPlayTimer = new QTimer(this);
    connect(dsPlayTimer, &QTimer::timeout, this, &MainWindow::dsPlayTimerTick);

    algoPlayTimer = new QTimer(this);
    connect(algoPlayTimer, &QTimer::timeout, this, &MainWindow::algoPlayTimerTick);

    // 添加主要的堆叠页面
    mainStack->addWidget(createDashboardWidget());      // 0
    mainStack->addWidget(createDataStructuresWidget());  // 1
    mainStack->addWidget(createAlgorithmsWidget());      // 2

    showDashboard();
}

MainWindow::~MainWindow() {}

// --- 页面切换逻辑 ---
// 显示主仪表盘/首页界面，同时停止所有播放器
void MainWindow::showDashboard() {
    dsPlayTimer->stop();
    algoPlayTimer->stop();
    if (btnDSPlay) btnDSPlay->setText(" 自动播放");
    if (btnAlgoPlay) btnAlgoPlay->setText(" 自动播放");
    mainStack->setCurrentIndex(0);
}

// 切换到数据结构演示界面
void MainWindow::showDataStructures() {
    mainStack->setCurrentIndex(1);
    onDSNavChanged(dsNavList->currentRow());
}

// 切换到算法演示界面
void MainWindow::showAlgorithms() {
    mainStack->setCurrentIndex(2);
    onAlgoNavChanged(algoNavList->currentRow());
}

// --- 主控制台/仪表盘界面 ---
// 创建初始的"首页/概览"部件界面
QWidget* MainWindow::createDashboardWidget() {
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(50, 60, 50, 60);
    layout->setSpacing(30);

    // 标题区域
    QLabel* titleLabel = new QLabel("STL Visual Lab", this);
    titleLabel->setFont(QFont("Segoe UI", 32, QFont::Bold));
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #569cd6; margin-bottom: 5px;");

    QLabel* subLabel = new QLabel("交互式 C++ 标准模板库 (STL) 容器与算法动态演练平台", this);
    subLabel->setFont(QFont("Segoe UI", 14));
    subLabel->setAlignment(Qt::AlignCenter);
    subLabel->setStyleSheet("color: #888888; margin-bottom: 30px;");

    layout->addWidget(titleLabel);
    layout->addWidget(subLabel);

    // 两个分类卡片的水平布局
    QHBoxLayout* cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(40);

    // 卡片 1：数据结构
    QFrame* dsCard = new QFrame(this);
    dsCard->setObjectName("dashboardCard");
    dsCard->setStyleSheet(
        "QFrame#dashboardCard { background-color: #252526; border: 2px solid #3c3c3c; border-radius: 12px; padding: 25px; }"
        "QFrame#dashboardCard:hover { border-color: #0e639c; background-color: #2d2d30; }"
    );
    QVBoxLayout* dsCardLayout = new QVBoxLayout(dsCard);
    dsCardLayout->setSpacing(15);

    QLabel* dsIcon = new QLabel(this);
    QPixmap dsPixmap(":/images/data_structures.png");
    if (!dsPixmap.isNull()) {
        dsIcon->setPixmap(dsPixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    dsIcon->setAlignment(Qt::AlignCenter);

    QLabel* dsTitle = new QLabel("数据结构 (Data Structures)", this);
    dsTitle->setFont(QFont("Segoe UI", 18, QFont::Bold));
    dsTitle->setAlignment(Qt::AlignCenter);
    dsTitle->setStyleSheet("color: #d4d4d4;");

    QLabel* dsDesc = new QLabel("探究连续内存与链式内存结构。\n包含 std::vector 的动态倍增扩容演练、std::list 双向指针断开重连动画、栈(LIFO)与队列(FIFO)的工作流水线展示。", this);
    dsDesc->setFont(QFont("Segoe UI", 11));
    dsDesc->setWordWrap(true);
    dsDesc->setAlignment(Qt::AlignCenter);
    dsDesc->setStyleSheet("color: #aaaaaa; line-height: 1.4;");

    QPushButton* dsBtn = new QPushButton("进入学习", this);
    dsBtn->setStyleSheet("QPushButton { background-color: #0e639c; color: white; padding: 10px; border-radius: 6px; font-weight: bold; font-size: 13px; }"
                         "QPushButton:hover { background-color: #1177bb; }");
    connect(dsBtn, &QPushButton::clicked, this, &MainWindow::showDataStructures);

    dsCardLayout->addWidget(dsIcon);
    dsCardLayout->addWidget(dsTitle);
    dsCardLayout->addWidget(dsDesc);
    dsCardLayout->addStretch();
    dsCardLayout->addWidget(dsBtn);

    // 卡片 2：STL算法
    QFrame* algoCard = new QFrame(this);
    algoCard->setObjectName("dashboardCard");
    algoCard->setStyleSheet(
        "QFrame#dashboardCard { background-color: #252526; border: 2px solid #3c3c3c; border-radius: 12px; padding: 25px; }"
        "QFrame#dashboardCard:hover { border-color: #ffaa00; background-color: #2d2d30; }"
    );
    QVBoxLayout* algoCardLayout = new QVBoxLayout(algoCard);
    algoCardLayout->setSpacing(15);

    QLabel* algoIcon = new QLabel(this);
    QPixmap algoPixmap(":/images/algorithms.png");
    if (!algoPixmap.isNull()) {
        algoIcon->setPixmap(algoPixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    algoIcon->setAlignment(Qt::AlignCenter);

    QLabel* algoTitle = new QLabel("算法 (Algorithms)", this);
    algoTitle->setFont(QFont("Segoe UI", 18, QFont::Bold));
    algoTitle->setAlignment(Qt::AlignCenter);
    algoTitle->setStyleSheet("color: #d4d4d4;");

    QLabel* algoDesc = new QLabel("掌握迭代器操作与区间处理。\n包含 std::sort 内省排序原理解析、std::lower_bound 有序二分查找、std::reverse 原地反转、std::unique 读写双指针去重、std::merge 有序归并算法流程。", this);
    algoDesc->setFont(QFont("Segoe UI", 11));
    algoDesc->setWordWrap(true);
    algoDesc->setAlignment(Qt::AlignCenter);
    algoDesc->setStyleSheet("color: #aaaaaa; line-height: 1.4;");

    QPushButton* algoBtn = new QPushButton("进入学习", this);
    algoBtn->setStyleSheet("QPushButton { background-color: #ffaa00; color: #1e1e1e; padding: 10px; border-radius: 6px; font-weight: bold; font-size: 13px; }"
                           "QPushButton:hover { background-color: #ffc457; }");
    connect(algoBtn, &QPushButton::clicked, this, &MainWindow::showAlgorithms);

    algoCardLayout->addWidget(algoIcon);
    algoCardLayout->addWidget(algoTitle);
    algoCardLayout->addWidget(algoDesc);
    algoCardLayout->addStretch();
    algoCardLayout->addWidget(algoBtn);

    cardsLayout->addWidget(dsCard);
    cardsLayout->addWidget(algoCard);
    layout->addLayout(cardsLayout);
    layout->addStretch();

    return page;
}

// --- 数据结构学习平台界面 ---
// 创建"数据结构"相关的可视化组合界面
QWidget* MainWindow::createDataStructuresWidget() {
    QWidget* dsPage = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(dsPage);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧导航侧边栏
    QWidget* sidebar = new QWidget(this);
    sidebar->setStyleSheet("background-color: #252526; border-right: 1px solid #3c3c3c;");
    sidebar->setFixedWidth(220);
    QVBoxLayout* sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(10, 15, 10, 15);

    QPushButton* btnBack = new QPushButton(" 返回主菜单", this);
    btnBack->setStyleSheet("QPushButton { background-color: #3e3e42; color: #d4d4d4; padding: 8px; border-radius: 4px; font-weight: bold; }"
                           "QPushButton:hover { background-color: #505054; }");
    connect(btnBack, &QPushButton::clicked, this, &MainWindow::showDashboard);
    sidebarLayout->addWidget(btnBack);

    QLabel* sectionTitle = new QLabel("数据结构分类", this);
    sectionTitle->setFont(QFont("Segoe UI", 12, QFont::Bold));
    sectionTitle->setStyleSheet("color: #888888; margin-top: 15px; margin-bottom: 5px; padding-left: 5px;");
    sidebarLayout->addWidget(sectionTitle);

    dsNavList = new QListWidget(this);
    dsNavList->setStyleSheet("QListWidget { background-color: transparent; border: none; outline: none; }"
                            "QListWidget::item { color: #d4d4d4; padding: 10px; border-radius: 4px; margin-bottom: 2px; }"
                            "QListWidget::item:hover { background-color: #2d2d30; }"
                            "QListWidget::item:selected { background-color: #0e639c; color: white; font-weight: bold; }");
    dsNavList->addItem("std::vector 动态数组");
    dsNavList->addItem("std::list 双向链表");
    dsNavList->addItem("std::stack & queue");
    dsNavList->setCurrentRow(0);
    connect(dsNavList, &QListWidget::currentRowChanged, this, &MainWindow::onDSNavChanged);
    sidebarLayout->addWidget(dsNavList);
    sidebarLayout->addStretch();

    // 右侧可视化区域与底部控制面板
    QWidget* rightArea = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightArea);
    rightLayout->setContentsMargins(15, 15, 15, 15);
    rightLayout->setSpacing(15);

    dsSubStack = new QStackedWidget(this);
    VisualVector* vv = new VisualVector(this);
    VisualList* vl = new VisualList(this);
    VisualStackQueue* vsq = new VisualStackQueue(this);

    connect(vv, &VisualBase::requestPlay, this, &MainWindow::autoStartDSPlay);
    connect(vl, &VisualBase::requestPlay, this, &MainWindow::autoStartDSPlay);
    connect(vsq, &VisualBase::requestPlay, this, &MainWindow::autoStartDSPlay);

    dsSubStack->addWidget(vv);      // 0
    dsSubStack->addWidget(vl);        // 1
    dsSubStack->addWidget(vsq);  // 2
    rightLayout->addWidget(dsSubStack, 1);

    // 动画播放控制面板
    QWidget* controlPanel = new QWidget(this);
    controlPanel->setStyleSheet("background-color: #252526; border-radius: 6px;");
    QHBoxLayout* controlLayout = new QHBoxLayout(controlPanel);
    controlLayout->setContentsMargins(15, 8, 15, 8);

    btnDSPlay = new QPushButton(" 自动播放", this);
    btnDSPlay->setStyleSheet("QPushButton { background-color: #00d778; color: #1e1e1e; padding: 7px 15px; border-radius: 4px; font-weight: bold; }"
                             "QPushButton:hover { background-color: #2bf095; }");
    btnDSStepBack = new QPushButton(" 上一步", this);
    btnDSStepBack->setStyleSheet("QPushButton { background-color: #0e639c; color: white; padding: 7px 15px; border-radius: 4px; font-weight: bold; }"
                                 "QPushButton:hover { background-color: #1177bb; }");
    QPushButton* btnStep = new QPushButton(" 单步执行", this);
    btnStep->setStyleSheet("QPushButton { background-color: #0e639c; color: white; padding: 7px 15px; border-radius: 4px; font-weight: bold; }"
                           "QPushButton:hover { background-color: #1177bb; }");
    QPushButton* btnReset = new QPushButton(" 重置动画", this);
    btnReset->setStyleSheet("QPushButton { background-color: #3e3e42; color: #d4d4d4; padding: 7px 15px; border-radius: 4px; font-weight: bold; }"
                            "QPushButton:hover { background-color: #505054; }");
    QPushButton* btnDSExport = new QPushButton(" 导出截图", this);
    btnDSExport->setStyleSheet("QPushButton { background-color: #3e3e42; color: #d4d4d4; padding: 7px 15px; border-radius: 4px; font-weight: bold; }"
                               "QPushButton:hover { background-color: #505054; }");

    controlLayout->addWidget(btnDSPlay);
    controlLayout->addWidget(btnDSStepBack);
    controlLayout->addWidget(btnStep);
    controlLayout->addWidget(btnReset);
    controlLayout->addWidget(btnDSExport);

    lblDSSpeed = new QLabel("播放速度: 1.0x", this);
    lblDSSpeed->setStyleSheet("color: #d4d4d4; margin-left: 10px;");
    dsSpeedSlider = new QSlider(Qt::Horizontal, this);
    dsSpeedSlider->setRange(0, 4);
    dsSpeedSlider->setValue(2);
    dsSpeedSlider->setFixedWidth(100);
    dsSpeedSlider->setStyleSheet("QSlider::groove:horizontal { height: 4px; background: #3e3e42; }"
                                 "QSlider::handle:horizontal { width: 12px; background: #00d778; margin: -4px 0; border-radius: 6px; }");
    controlLayout->addWidget(lblDSSpeed);
    controlLayout->addWidget(dsSpeedSlider);
    controlLayout->addStretch();

    connect(btnDSPlay, &QPushButton::clicked, this, &MainWindow::dsPlayToggle);
    connect(btnDSStepBack, &QPushButton::clicked, this, &MainWindow::dsStepBackward);
    connect(btnStep, &QPushButton::clicked, this, &MainWindow::dsStep);
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::dsReset);
    connect(btnDSExport, &QPushButton::clicked, this, &MainWindow::dsExport);
    connect(dsSpeedSlider, &QSlider::valueChanged, this, &MainWindow::updateDSSpeed);

    rightLayout->addWidget(controlPanel);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(rightArea, 1);

    return dsPage;
}

// --- 算法学习平台界面 ---
// 创建"STL算法"相关的可视化组合界面
QWidget* MainWindow::createAlgorithmsWidget() {
    QWidget* algoPage = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(algoPage);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧导航侧边栏
    QWidget* sidebar = new QWidget(this);
    sidebar->setStyleSheet("background-color: #252526; border-right: 1px solid #3c3c3c;");
    sidebar->setFixedWidth(220);
    QVBoxLayout* sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(10, 15, 10, 15);

    QPushButton* btnBack = new QPushButton(" 返回主菜单", this);
    btnBack->setStyleSheet("QPushButton { background-color: #3e3e42; color: #d4d4d4; padding: 8px; border-radius: 4px; font-weight: bold; }"
                           "QPushButton:hover { background-color: #505054; }");
    connect(btnBack, &QPushButton::clicked, this, &MainWindow::showDashboard);
    sidebarLayout->addWidget(btnBack);

    QLabel* sectionTitle = new QLabel("算法分类", this);
    sectionTitle->setFont(QFont("Segoe UI", 12, QFont::Bold));
    sectionTitle->setStyleSheet("color: #888888; margin-top: 15px; margin-bottom: 5px; padding-left: 5px;");
    sidebarLayout->addWidget(sectionTitle);

    algoNavList = new QListWidget(this);
    algoNavList->setStyleSheet("QListWidget { background-color: transparent; border: none; outline: none; }"
                              "QListWidget::item { color: #d4d4d4; padding: 10px; border-radius: 4px; margin-bottom: 2px; }"
                              "QListWidget::item:hover { background-color: #2d2d30; }"
                              "QListWidget::item:selected { background-color: #ffaa00; color: #1e1e1e; font-weight: bold; }");
    algoNavList->addItem("std::sort 排序");
    algoNavList->addItem("std::lower_bound 二分下界");
    algoNavList->addItem("std::reverse 反转");
    algoNavList->addItem("std::unique 去重");
    algoNavList->addItem("std::merge 归并");
    algoNavList->addItem("std::remove 移除");
    algoNavList->addItem("std::rotate 旋转");
    algoNavList->addItem("std::next_permutation 下一个排列");
    algoNavList->setCurrentRow(0);
    connect(algoNavList, &QListWidget::currentRowChanged, this, &MainWindow::onAlgoNavChanged);
    sidebarLayout->addWidget(algoNavList);
    sidebarLayout->addStretch();

    // 右侧可视化区域与底部控制面板
    QWidget* rightArea = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightArea);
    rightLayout->setContentsMargins(15, 15, 15, 15);
    rightLayout->setSpacing(15);

    algoSubStack = new QStackedWidget(this);
    algoSubStack->addWidget(new VisualSort(this));        // 0
    algoSubStack->addWidget(new VisualLowerBound(this));  // 1
    algoSubStack->addWidget(new VisualReverse(this));     // 2
    algoSubStack->addWidget(new VisualUnique(this));      // 3
    algoSubStack->addWidget(new VisualMerge(this));       // 4
    algoSubStack->addWidget(new VisualRemove(this));      // 5
    algoSubStack->addWidget(new VisualRotate(this));      // 6
    algoSubStack->addWidget(new VisualPermutation(this)); // 7
    // 将所有算法子组件连接到请求播放信号
    for (int i = 0; i < algoSubStack->count(); ++i) {
        VisualBase* algoWidget = qobject_cast<VisualBase*>(algoSubStack->widget(i));
        if (algoWidget) {
            connect(algoWidget, &VisualBase::requestPlay, this, &MainWindow::autoStartAlgoPlay);
        }
    }
    rightLayout->addWidget(algoSubStack, 1);

    // 动画播放控制面板
    QWidget* controlPanel = new QWidget(this);
    controlPanel->setStyleSheet("background-color: #252526; border-radius: 6px;");
    QHBoxLayout* controlLayout = new QHBoxLayout(controlPanel);
    controlLayout->setContentsMargins(15, 8, 15, 8);

    btnAlgoPlay = new QPushButton(" 自动播放", this);
    btnAlgoPlay->setStyleSheet("QPushButton { background-color: #ffaa00; color: #1e1e1e; padding: 7px 15px; border-radius: 4px; font-weight: bold; }"
                               "QPushButton:hover { background-color: #ffc457; }");
    btnAlgoStepBack = new QPushButton(" 上一步", this);
    btnAlgoStepBack->setStyleSheet("QPushButton { background-color: #0e639c; color: white; padding: 7px 15px; border-radius: 4px; font-weight: bold; }"
                                   "QPushButton:hover { background-color: #1177bb; }");
    QPushButton* btnStep = new QPushButton(" 单步执行", this);
    btnStep->setStyleSheet("QPushButton { background-color: #0e639c; color: white; padding: 7px 15px; border-radius: 4px; font-weight: bold; }"
                           "QPushButton:hover { background-color: #1177bb; }");
    QPushButton* btnReset = new QPushButton(" 重置动画", this);
    btnReset->setStyleSheet("QPushButton { background-color: #3e3e42; color: #d4d4d4; padding: 7px 15px; border-radius: 4px; font-weight: bold; }"
                            "QPushButton:hover { background-color: #505054; }");
    QPushButton* btnAlgoExport = new QPushButton(" 导出截图", this);
    btnAlgoExport->setStyleSheet("QPushButton { background-color: #3e3e42; color: #d4d4d4; padding: 7px 15px; border-radius: 4px; font-weight: bold; }"
                                 "QPushButton:hover { background-color: #505054; }");

    controlLayout->addWidget(btnAlgoPlay);
    controlLayout->addWidget(btnAlgoStepBack);
    controlLayout->addWidget(btnStep);
    controlLayout->addWidget(btnReset);
    controlLayout->addWidget(btnAlgoExport);

    lblAlgoSpeed = new QLabel("播放速度: 1.0x", this);
    lblAlgoSpeed->setStyleSheet("color: #d4d4d4; margin-left: 10px;");
    algoSpeedSlider = new QSlider(Qt::Horizontal, this);
    algoSpeedSlider->setRange(0, 4);
    algoSpeedSlider->setValue(2);
    algoSpeedSlider->setFixedWidth(100);
    algoSpeedSlider->setStyleSheet("QSlider::groove:horizontal { height: 4px; background: #3e3e42; }"
                                   "QSlider::handle:horizontal { width: 12px; background: #ffaa00; margin: -4px 0; border-radius: 6px; }");
    controlLayout->addWidget(lblAlgoSpeed);
    controlLayout->addWidget(algoSpeedSlider);
    controlLayout->addStretch();

    connect(btnAlgoPlay, &QPushButton::clicked, this, &MainWindow::algoPlayToggle);
    connect(btnAlgoStepBack, &QPushButton::clicked, this, &MainWindow::algoStepBackward);
    connect(btnStep, &QPushButton::clicked, this, &MainWindow::algoStep);
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::algoReset);
    connect(btnAlgoExport, &QPushButton::clicked, this, &MainWindow::algoExport);
    connect(algoSpeedSlider, &QSlider::valueChanged, this, &MainWindow::updateAlgoSpeed);
    rightLayout->addWidget(controlPanel);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(rightArea, 1);

    return algoPage;
}

// --- 数据结构子堆叠窗口逻辑 ---
// 响应数据结构侧边栏的切换：重置对应动画并停止播放
void MainWindow::onDSNavChanged(int row) {
    dsPlayTimer->stop();
    btnDSPlay->setText(" 自动播放");
    if (row >= 0 && row < dsSubStack->count()) {
        dsSubStack->setCurrentIndex(row);
        VisualBase* active = dynamic_cast<VisualBase*>(dsSubStack->currentWidget());
        if (active) active->reset();
    }
}

// 静态助手函数：将滑块的挡位值转换为播放定时器的毫秒间隔
static int getIntervalFromSpeedVal(int val) {
    switch (val) {
        case 0: return 4000; // 0.25倍速
        case 1: return 2000; // 0.5倍速
        case 2: return 1000; // 1.0倍速
        case 3: return 500;  // 2.0倍速
        case 4: return 250;  // 4.0倍速
        default: return 1000;
    }
}

// 静态助手函数：获取对应速度的文本显示
static QString getSpeedTextFromVal(int val) {
    switch (val) {
        case 0: return "0.25x";
        case 1: return "0.5x";
        case 2: return "1.0x";
        case 3: return "2.0x";
        case 4: return "4.0x";
        default: return "1.0x";
    }
}

// 切换数据结构动画的 播放/暂停 状态
void MainWindow::dsPlayToggle() {
    VisualBase* active = dynamic_cast<VisualBase*>(dsSubStack->currentWidget());
    if (!active) return;

    if (dsPlayTimer->isActive()) {
        dsPlayTimer->stop();
        btnDSPlay->setText(" 自动播放");
    } else {
        if (active->isAtEnd()) {
            active->reset();
        }
        dsPlayTimer->start(getIntervalFromSpeedVal(dsSpeedSlider->value()));
        btnDSPlay->setText(" 暂停播放");
    }
}

// 控制数据结构动画前进一步
void MainWindow::dsStep() {
    dsPlayTimer->stop();
    btnDSPlay->setText(" 自动播放");
    VisualBase* active = dynamic_cast<VisualBase*>(dsSubStack->currentWidget());
    if (active) {
        active->stepForward();
        if (active->isAtEnd()) {
            btnDSPlay->setText(" 重新播放");
        }
    }
}

// 控制数据结构动画退一步
void MainWindow::dsStepBackward() {
    dsPlayTimer->stop();
    btnDSPlay->setText(" 自动播放");
    VisualBase* active = dynamic_cast<VisualBase*>(dsSubStack->currentWidget());
    if (active) {
        active->stepBackward();
    }
}

// 重置当前数据结构的动画状态
void MainWindow::dsReset() {
    dsPlayTimer->stop();
    btnDSPlay->setText(" 自动播放");
    VisualBase* active = dynamic_cast<VisualBase*>(dsSubStack->currentWidget());
    if (active) active->reset();
}

// 数据结构播放定时器触发：自动执行下一步，若到达终点则停止
void MainWindow::dsPlayTimerTick() {
    VisualBase* active = dynamic_cast<VisualBase*>(dsSubStack->currentWidget());
    if (active) {
        active->stepForward();
        if (active->isAtEnd()) {
            dsPlayTimer->stop();
            btnDSPlay->setText(" 重新播放");
        }
    }
}

// --- 算法子堆叠窗口逻辑 ---
// 响应算法侧边栏的切换：重置对应动画并停止播放
void MainWindow::onAlgoNavChanged(int row) {
    algoPlayTimer->stop();
    btnAlgoPlay->setText(" 自动播放");
    if (row >= 0 && row < algoSubStack->count()) {
        algoSubStack->setCurrentIndex(row);
        VisualBase* active = dynamic_cast<VisualBase*>(algoSubStack->currentWidget());
        if (active) active->reset();
    }
}

// 切换算法动画的 播放/暂停 状态
void MainWindow::algoPlayToggle() {
    VisualBase* active = dynamic_cast<VisualBase*>(algoSubStack->currentWidget());
    if (!active) return;

    if (algoPlayTimer->isActive()) {
        algoPlayTimer->stop();
        btnAlgoPlay->setText(" 自动播放");
    } else {
        if (active->isAtEnd()) {
            active->reset();
        }
        algoPlayTimer->start(getIntervalFromSpeedVal(algoSpeedSlider->value()));
        btnAlgoPlay->setText(" 暂停播放");
    }
}

// 控制算法动画前进一步
void MainWindow::algoStep() {
    algoPlayTimer->stop();
    btnAlgoPlay->setText(" 自动播放");
    VisualBase* active = dynamic_cast<VisualBase*>(algoSubStack->currentWidget());
    if (active) {
        active->stepForward();
        if (active->isAtEnd()) {
            btnAlgoPlay->setText(" 重新播放");
        }
    }
}

// 控制算法动画退一步
void MainWindow::algoStepBackward() {
    algoPlayTimer->stop();
    btnAlgoPlay->setText(" 自动播放");
    VisualBase* active = dynamic_cast<VisualBase*>(algoSubStack->currentWidget());
    if (active) {
        active->stepBackward();
    }
}

// 重置当前算法的动画状态
void MainWindow::algoReset() {
    algoPlayTimer->stop();
    btnAlgoPlay->setText(" 自动播放");
    VisualBase* active = dynamic_cast<VisualBase*>(algoSubStack->currentWidget());
    if (active) active->reset();
}

// 算法播放定时器触发：自动执行下一步，若到达终点则停止
void MainWindow::algoPlayTimerTick() {
    VisualBase* active = dynamic_cast<VisualBase*>(algoSubStack->currentWidget());
    if (active) {
        active->stepForward();
        if (active->isAtEnd()) {
            algoPlayTimer->stop();
            btnAlgoPlay->setText(" 重新播放");
        }
    }
}

// 当内部组件请求播放时，自动启动数据结构播放器
void MainWindow::autoStartDSPlay() {
    if (!dsPlayTimer->isActive()) {
        dsPlayTimer->start(getIntervalFromSpeedVal(dsSpeedSlider->value()));
        btnDSPlay->setText(" 暂停播放");
    }
}

// 当内部组件请求播放时，自动启动算法播放器
void MainWindow::autoStartAlgoPlay() {
    if (!algoPlayTimer->isActive()) {
        algoPlayTimer->start(getIntervalFromSpeedVal(algoSpeedSlider->value()));
        btnAlgoPlay->setText(" 暂停播放");
    }
}

// 更新数据结构演示动画的播放速度并刷新标签
void MainWindow::updateDSSpeed(int value) {
    lblDSSpeed->setText("播放速度: " + getSpeedTextFromVal(value));
    if (dsPlayTimer->isActive()) {
        dsPlayTimer->start(getIntervalFromSpeedVal(value));
    }
}

// 更新算法演示动画的播放速度并刷新标签
void MainWindow::updateAlgoSpeed(int value) {
    lblAlgoSpeed->setText("播放速度: " + getSpeedTextFromVal(value));
    if (algoPlayTimer->isActive()) {
        algoPlayTimer->start(getIntervalFromSpeedVal(value));
    }
}

// 导出数据结构组件截图
void MainWindow::dsExport() {
    VisualBase* active = dynamic_cast<VisualBase*>(dsSubStack->currentWidget());
    if (active) active->exportImage();
}

// 导出算法组件截图
void MainWindow::algoExport() {
    VisualBase* active = dynamic_cast<VisualBase*>(algoSubStack->currentWidget());
    if (active) active->exportImage();
}


