#ifndef QUIZDATABASE_H
#define QUIZDATABASE_H

#include <QString>
#include <QStringList>
#include <vector>

// 测验题目结构体
struct QuizQuestion {
    QString text;               // 题干内容 / 编程题目说明
    QStringList options;        // 选项列表 (对选择题有效)
    int correctIndex;           // 正确选项的索引 (选择题有效)
    QString explanation;        // 答案解析 / 编程参考解答
    bool isCoding;              // 是否为编程题
    QString codeTemplate;       // 编程题的函数初始模板
    QString testCasesCode;      // 测试套件代码 (含 main 函数及 assert 校验)
};

// 测验题数据库类，用于管理所有模块的选择题与编程题
class QuizDatabase {
public:
    enum class Topic {
        Vector,
        List,
        StackQueue,
        Sort,
        LowerBound,
        Reverse,
        Unique,
        Merge,
        Permutation,
        Remove,
        Rotate
    };

    // 获取特定知识点模块的 15 道题列表 (10道选择 + 5道编程)，可选择传入用户错题记录进行智能推荐
    static std::vector<QuizQuestion> getQuestionsFor(Topic topic, const QList<int>& frequentErrors = QList<int>());
};

#endif // QUIZDATABASE_H
