#ifndef VISUALSORT_H
#define VISUALSORT_H

#include "VisualBase.h"
#include <vector>
#include <QString>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QTextBrowser>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QTimer>

struct SortAnimStep {
    std::vector<int> data;
    std::vector<QString> stringData; // 新增：支持字符串排序数据
    int idx1;
    int idx2;
    QString desc;
    QStringList codeLines; // 伪代码行
    int activeLine;        // 高亮行索引

    // 默认构造函数
    SortAnimStep() : idx1(-1), idx2(-1), activeLine(-1) {}

    // 构造函数1：用于整型数据
    SortAnimStep(const std::vector<int>& d, int i1, int i2, const QString& ds, const QStringList& cl, int al)
        : data(d), idx1(i1), idx2(i2), desc(ds), codeLines(cl), activeLine(al) {}

    // 构造函数2：用于字符串数据
    SortAnimStep(const std::vector<int>& d, const std::vector<QString>& sd, int i1, int i2, const QString& ds, const QStringList& cl, int al)
        : data(d), stringData(sd), idx1(i1), idx2(i2), desc(ds), codeLines(cl), activeLine(al) {}
};

// 派生自 VisualBase，用于可视化特定 STL 算法或数据结构的类
class VisualSort : public VisualBase {
    Q_OBJECT
public:
    // 构造函数，初始化 UI 界面布局
    explicit VisualSort(QWidget *parent = nullptr);
    // 执行动画的一步进操作
    void stepForward() override;
    // 执行动画的退一步操作
    void stepBackward() override;
    // 重置可视化状态
    void reset() override;
    // 判断当前动画是否已经执行到最后一步
    bool isAtEnd() const override;
    // 获取用于侧边栏显示的知识卡片 HTML
    QString getInfo() const override;
    // 将当前画布状态导出为图片
    void exportImage() override;

private slots:
    // 槽函数：处理用户点击加载或开始演示的操作
    void handleLoad();

private:
    // 自定义的内部画板部件类，专门用来承载绘制
    class CanvasWidget : public QWidget {
        VisualSort* p;
    public:
        CanvasWidget(VisualSort* parent) : QWidget(parent), p(parent) {}
    protected:
        // 重写绘制事件，调用外部 of paintCanvas 绘制内容
        void paintEvent(QPaintEvent* event) override;
    };

    // 在指定的画板区域进行自定义绘制
    void paintCanvas(QWidget* target);
    void generateSortSteps(int sortType);
    void generateStringSortSteps(int sortType);

    QTabWidget* tabWidget;
    CanvasWidget* canvas;
    QTextBrowser* codeBrowser; // 伪代码显示面板

    QComboBox* dataTypeCombo;  // 数据类型选择框 (整数/字符串)
    QComboBox* sortTypeCombo;
    QLineEdit* valueEdit;

    std::vector<int> realData;
    std::vector<QString> realStringData; // 字符串实际数据
    std::vector<SortAnimStep> animSteps;
    int currentStep;
    void updateCodeBrowser(); // 更新高亮源码显示
};

#endif // VISUALSORT_H
