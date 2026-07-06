#ifndef VISUALSTACKQUEUE_H
#define VISUALSTACKQUEUE_H

#include "VisualBase.h"
#include <vector>
#include <QString>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QTextBrowser>
#include <deque>

struct SQAnimStep {
    std::deque<int> data;
    int highlightIdx;
    QString desc;
    QStringList codeLines; // 伪代码行
    int activeLine;        // 高亮行索引
};

class VisualStackQueue : public VisualBase {
    Q_OBJECT
public:
    explicit VisualStackQueue(QWidget *parent = nullptr);
    void stepForward() override;
    void stepBackward() override;
    void reset() override;
    bool isAtEnd() const override;
    QString getInfo() const override;
    void exportImage() override;

private slots:
    void handlePush();
    void handlePopStack();
    void handlePopQueue();
    void handleLoad();

private:
    class CanvasWidget : public QWidget {
        VisualStackQueue* p;
    public:
        CanvasWidget(VisualStackQueue* parent) : QWidget(parent), p(parent) {}
    protected:
        void paintEvent(QPaintEvent* event) override;
    };

    void paintCanvas(QWidget* target);

    QTabWidget* tabWidget;
    CanvasWidget* canvas;
    QTextBrowser* codeBrowser; // 伪代码显示面板

    QLineEdit* initEdit;
    QLineEdit* pushValueEdit;

    std::deque<int> realData;

    std::vector<SQAnimStep> animSteps;
    int currentStep;
    bool isStackMode = true; // 记录当前是栈模式还是队列模式，用于选择展示的代码
    void updateCodeBrowser(); // 更新高亮源码显示
};

#endif // VISUALSTACKQUEUE_H
