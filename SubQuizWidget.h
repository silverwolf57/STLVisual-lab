#ifndef SUBQUIZWIDGET_H
#define SUBQUIZWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QButtonGroup>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QMap>
#include <vector>
#include <QStringList>
#include "QuizDatabase.h"

// 测验小部件类，用于显示各个知识点的选择题与编程挑战题
class SubQuizWidget : public QWidget {
    Q_OBJECT
public:
    // 构造函数，接受题目列表、主题名称并初始化界面
    explicit SubQuizWidget(const std::vector<QuizQuestion>& q, const QString& topicName, QWidget *parent = nullptr);
private slots:
    // 处理用户提交答案的槽函数
    void submitAnswer();
    // 切换到下一题的槽函数
    void nextQuestion();
    // 切换到上一题的槽函数
    void prevQuestion();

private:
    void showQuestion(int index);
    // 处理编程题的编译与运行测试
    void runCompileAndTest();

    // 所有的测验题目
    std::vector<QuizQuestion> questions;
    QString topicName; // 主题名称
    // 单选按钮组的列表，用于选择题互斥选择
    std::vector<QButtonGroup*> buttonGroups;
    // 答题结果和解析的显示标签
    std::vector<QLabel*> resultLabels;
    int currentQ;
    int score;

    QLabel *questionLabel;
    
    // 选择题容器与组件
    QWidget *choiceContainer;
    QButtonGroup *buttonGroup;
    std::vector<QRadioButton*> radioButtons;
    
    // 编程题容器与组件
    QWidget *codingContainer;
    QPlainTextEdit *codeEditor;
    QTextBrowser *consoleOutput;

    QPushButton *prevBtn;
    QPushButton *submitBtn;
    QPushButton *nextBtn;
    QLabel *feedbackLabel;

    // 记录用户的答题历史状态，以实现返回上一题时的状态恢复
    QMap<int, int> userChoiceAnswers;
    QMap<int, QString> userCodingAnswers;
    class CppHighlighter *highlighter; // 语法高亮器

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
};

#endif // SUBQUIZWIDGET_H
