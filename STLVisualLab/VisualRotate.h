 #ifndef VISUALROTATE_H
 #define VISUALROTATE_H
 
 #include "VisualBase.h"
 #include <vector>
 #include <QString>
 #include <QLineEdit>
 #include <QPushButton>
 #include <QTabWidget>
 #include <QTextBrowser>
 
 // 旋转算法动画步骤结构体
 struct RotateAnimStep {
     std::vector<int> data;      // 当前数组数据
     int pivot;                  // 旋转中点
     int highlightStart;         // 高亮区间起点
     int highlightEnd;           // 高亮区间终点
     int phase;                  // 旋转阶段: 0=初始 1=反转前段 2=反转后段 3=反转整体 4=完成
     QString desc;               // 步骤描述文本
     QStringList codeLines;      // 当前步骤对应的伪代码
     int activeLine;             // 高亮行索引
 };
 
 // std::rotate 可视化类，继承自 VisualBase
 class VisualRotate : public VisualBase {
     Q_OBJECT
 public:
     explicit VisualRotate(QWidget *parent = nullptr);
     void stepForward() override;
     void stepBackward() override;
     void reset() override;
     bool isAtEnd() const override;
     QString getInfo() const override;
     void exportImage() override;
 
 private slots:
     void handleLoad();
 
 private:
     class CanvasWidget : public QWidget {
         VisualRotate* p;
     public:
         CanvasWidget(VisualRotate* parent) : QWidget(parent), p(parent) {}
     protected:
         void paintEvent(QPaintEvent* event) override;
     };
 
     void paintCanvas(QWidget* target);
     void generateSteps();
 
     QTabWidget* tabWidget;
     CanvasWidget* canvas;
     QTextBrowser* codeBrowser;
 
     QLineEdit* valueEdit;       // 初始数组输入框
     QLineEdit* pivotEdit;       // 旋转中点输入框
 
     std::vector<int> realData;
     int pivotValue;             // 旋转中点
 
     std::vector<RotateAnimStep> animSteps;
     int currentStep;
     void updateCodeBrowser();
 };
 
 #endif // VISUALROTATE_H
