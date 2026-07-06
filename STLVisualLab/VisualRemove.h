 #ifndef VISUALREMOVE_H
 #define VISUALREMOVE_H
 
 #include "VisualBase.h"
 #include <vector>
 #include <QString>
 #include <QLineEdit>
 #include <QPushButton>
 #include <QTabWidget>
 #include <QTextBrowser>
 
 // 移除算法动画步骤结构体
 struct RemoveAnimStep {
     std::vector<int> data;      // 当前数组数据
     int destIdx;                // 写指针（慢指针）位置
     int currIdx;                // 读指针（快指针）位置
     int removeVal;              // 要移除的目标值
     QString desc;               // 步骤描述文本
     QStringList codeLines;      // 当前步骤对应的伪代码
     int activeLine;             // 高亮行索引
 };
 
 // std::remove 可视化类，继承自 VisualBase
 class VisualRemove : public VisualBase {
     Q_OBJECT
 public:
     explicit VisualRemove(QWidget *parent = nullptr);
     void stepForward() override;
     void stepBackward() override;
     void reset() override;
     bool isAtEnd() const override;
     QString getInfo() const override;
     void exportImage() override;
 
 private slots:
     void handleLoad();
 
 private:
     // 内部画布组件类
     class CanvasWidget : public QWidget {
         VisualRemove* p;
     public:
         CanvasWidget(VisualRemove* parent) : QWidget(parent), p(parent) {}
     protected:
         void paintEvent(QPaintEvent* event) override;
     };
 
     void paintCanvas(QWidget* target);
     void generateSteps();
 
     QTabWidget* tabWidget;
     CanvasWidget* canvas;
     QTextBrowser* codeBrowser;
 
     QLineEdit* valueEdit;          // 初始数组输入框
     QLineEdit* removeValueEdit;    // 移除值输入框
 
     std::vector<int> realData;
     int removeValue;               // 要移除的值
 
     std::vector<RemoveAnimStep> animSteps;
     int currentStep;
     void updateCodeBrowser();
 };
 
 #endif // VISUALREMOVE_H
