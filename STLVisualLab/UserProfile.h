#ifndef USERPROFILE_H
#define USERPROFILE_H

#include <QString>
#include <QSet>
#include <QMap>

// 用户个人配置文件，用于记录错题和历史答题统计
class UserProfile {
public:
    static UserProfile& instance();

    // 记录一道题目的错误次数
    void recordError(const QString& topic, int questionId);
    
    // 获取用户在特定主题下的错题 ID 列表
    QList<int> getErrorsForTopic(const QString& topic) const;

    // 清除错题记录
    void clearErrors();

private:
    UserProfile();
    void load();
    void save();

    QString filePath;
    // 键: 主题名 (如 "Vector"), 值: 错题 Question ID 及其错误次数的映射
    QMap<QString, QMap<int, int>> errorStats;
};

#endif // USERPROFILE_H
