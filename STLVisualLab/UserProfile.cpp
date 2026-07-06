#include "UserProfile.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>

UserProfile::UserProfile() {
    // 存放在当前工作目录下的 build/user_profile.json
    QDir().mkpath("build");
    filePath = "build/user_profile.json";
    load();
}

UserProfile& UserProfile::instance() {
    static UserProfile inst;
    return inst;
}

void UserProfile::recordError(const QString& topic, int questionId) {
    errorStats[topic][questionId]++;
    save();
}

QList<int> UserProfile::getErrorsForTopic(const QString& topic) const {
    if (!errorStats.contains(topic)) {
        return QList<int>();
    }
    // 返回该主题下错误次数大于 0 的所有题目 ID
    return errorStats[topic].keys();
}

void UserProfile::clearErrors() {
    errorStats.clear();
    save();
}

void UserProfile::load() {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        return;
    }
    
    QJsonObject root = doc.object();
    for (auto it = root.begin(); it != root.end(); ++it) {
        QString topic = it.key();
        QJsonObject topicObj = it.value().toObject();
        QMap<int, int> topicErrors;
        for (auto qIt = topicObj.begin(); qIt != topicObj.end(); ++qIt) {
            topicErrors[qIt.key().toInt()] = qIt.value().toInt();
        }
        errorStats[topic] = topicErrors;
    }
}

void UserProfile::save() {
    QJsonObject root;
    for (auto it = errorStats.begin(); it != errorStats.end(); ++it) {
        QString topic = it.key();
        QJsonObject topicObj;
        for (auto qIt = it.value().begin(); qIt != it.value().end(); ++qIt) {
            topicObj[QString::number(qIt.key())] = qIt.value();
        }
        root[topic] = topicObj;
    }
    
    QJsonDocument doc(root);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(doc.toJson());
        file.close();
    }
}
