#ifndef VISUALPERMUTATION_H
#define VISUALPERMUTATION_H

#include "VisualBase.h"
#include <vector>
#include <QString>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QTextBrowser>
#include <QLabel>

// 全排列动画步骤结构体
struct PermAnimStep {
    std::vector<int> data;
    int idx1;         // 高亮索引（转折点 i 或交换左）
    int idx2;         // 高亮索引（替换数 j 或交换右）
    int reverseStart; // 翻转区间的起始索引
    QString desc;
    QStringList codeLines;
    int activeLine;
    // 区间类型：0=无, 1=单高亮, 2=双高亮, 3=翻转区间
    int highlightType;
};

class VisualPermutation : public VisualBase {
    Q_OBJECT
public:
    explicit VisualPermutation(QWidget *parent = nullptr);
    void stepForward() override;
    void stepBackward() override;
    void reset() override;
    bool isAtEnd() const override;
    QString getInfo() const override;
    void exportImage() override;

private slots:
    void handleLoad();
    void handleNextPerm();
    void handleRunAll();

private:
    class CanvasWidget : public QWidget {
        VisualPermutation* p;
    public:
        CanvasWidget(VisualPermutation* parent) : QWidget(parent), p(parent) {}
    protected:
        void paintEvent(QPaintEvent*) override;
    };

    void paintCanvas(QWidget* target);
    void generateNextPermSteps(std::vector<int> arr);
    void updateCodeBrowser();

    QTabWidget* tabWidget;
    CanvasWidget* canvas;
    QTextBrowser* codeBrowser;
    QLabel* lblPermCount;

    QLineEdit* valueEdit;

    std::vector<int> realData;       // 当前最新的排列状态
    std::vector<int> originalData;   // 最初加载的排列（用于 reset）
    std::vector<PermAnimStep> animSteps;
    int currentStep;
    int permCount;                   // 已生成第几个排列
};

#endif // VISUALPERMUTATION_H
