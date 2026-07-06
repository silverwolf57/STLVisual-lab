#ifndef VISUALREVERSE_H
#define VISUALREVERSE_H

#include "VisualBase.h"
#include <vector>
#include <QString>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QTextBrowser>

struct RevAnimStep {
    std::vector<int> data;
    int idx1;
    int idx2;
    QString desc;
    QStringList codeLines; // 伪代码行
    int activeLine;        // 高亮行索引
};

// 派生自 VisualBase，用于可视化特定 STL 算法或数据结构的类
class VisualReverse : public VisualBase {
    Q_OBJECT
public:
    // 构造函数，初始化 UI 界面布局
    explicit VisualReverse(QWidget *parent = nullptr);
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
        VisualReverse* p;
    public:
        CanvasWidget(VisualReverse* parent) : QWidget(parent), p(parent) {}
    protected:
        // 重写绘制事件，调用外部 of paintCanvas 绘制内容
        void paintEvent(QPaintEvent* event) override;
    };

    // 在指定的画板区域进行自定义绘制
    void paintCanvas(QWidget* target);
    void generateSteps();

    QTabWidget* tabWidget;
    CanvasWidget* canvas;
    QTextBrowser* codeBrowser; // 伪代码显示面板

    QLineEdit* valueEdit;

    std::vector<int> realData;
    std::vector<RevAnimStep> animSteps;
    int currentStep;
    void updateCodeBrowser(); // 更新高亮源码显示
};

#endif // VISUALREVERSE_H
