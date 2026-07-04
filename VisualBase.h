#ifndef VISUALBASE_H
#define VISUALBASE_H

#include <QWidget>
#include <QString>
#include <QRegularExpression>
// 可视化组件的抽象基类，所有具体的 STL 演示组件都需要继承此类
class VisualBase : public QWidget {
    Q_OBJECT
public:
    explicit VisualBase(QWidget *parent = nullptr) : QWidget(parent) {}
    // 执行动画的下一步
    virtual void stepForward() = 0;
    // 执行动画的退一步操作
    virtual void stepBackward() = 0;
    // 重置状态
    virtual void reset() = 0;
    // 判断当前动画是否已经执行到最后一步
    virtual bool isAtEnd() const = 0;
    // 获取相关的知识卡片 HTML 内容
    virtual QString getInfo() const = 0;
    // 将当前画布状态导出为图片
    virtual void exportImage() = 0;

    // 渲染带有高亮的伪代码/源码 HTML
    static inline QString renderCodeWithHighlight(const QStringList& lines, int highlightLineIdx) {
        QString html = "<html><head><style>"
                       "body { background-color: #1e1e1e; color: #d4d4d4; font-family: 'Consolas', 'Courier New', monospace; font-size: 12px; line-height: 1.4; margin: 0; padding: 8px; }"
                       ".line { white-space: pre-wrap; padding: 2px 5px; }"
                       ".highlight { background-color: #3e3e40; color: #ffaa00; border-left: 3px solid #ffaa00; font-weight: bold; }"
                       ".keyword { color: #569cd6; }"
                       ".type { color: #4ec9b0; }"
                       ".comment { color: #6a9955; }"
                       ".number { color: #b5cea8; }"
                       "</style></head><body>";

        for (int i = 0; i < lines.size(); ++i) {
            QString line = lines[i];
            line.replace("&", "&amp;");
            line.replace("<", "&lt;");
            line.replace(">", "&gt;");

            // 语法高亮
            QStringList keywords = {"void", "template", "typename", "struct", "class", "public", "protected", "private", "if", "else", "while", "for", "return", "const", "auto", "true", "false", "inline"};
            QString keywordPattern = "\\b(" + keywords.join("|") + ")\\b";
            line.replace(QRegularExpression(keywordPattern), "<span class=\"keyword\">\\1</span>");

            QStringList types = {"size_type", "pointer", "iterator", "reference", "value_type", "int", "float", "double", "char", "bool", "std", "vector", "list", "deque", "stack", "queue", "_Tp", "T"};
            QString typePattern = "\\b(" + types.join("|") + ")\\b";
            line.replace(QRegularExpression(typePattern), "<span class=\"type\">\\1</span>");

            line.replace(QRegularExpression("\\b(\\d+)\\b"), "<span class=\"number\">\\1</span>");
            line.replace(QRegularExpression("(//.*)"), "<span class=\"comment\">\\1</span>");

            if (i == highlightLineIdx) {
                html += QString("<div class=\"line highlight\">%1</div>").arg(line);
            } else {
                html += QString("<div class=\"line\">%1</div>").arg(line);
            }
        }
        html += "</body></html>";
        return html;
    }

    // 将 C++ 源码高亮渲染成 HTML
    static inline QString highlightCode(const QString& code) {
        QString res = code;
        res.replace("&", "&amp;");
        res.replace("<", "&lt;");
        res.replace(">", "&gt;");

        QStringList keywords = {"void", "template", "typename", "struct", "class", "public", "protected", "private", "if", "else", "while", "return", "const", "auto", "true", "false", "inline"};
        QString keywordPattern = "\\b(" + keywords.join("|") + ")\\b";
        res.replace(QRegularExpression(keywordPattern), "<span style=\"color:#569cd6;\">\\1</span>");

        QStringList types = {"size_type", "pointer", "iterator", "reference", "value_type", "int", "float", "double", "char", "bool", "std", "vector", "list", "deque", "stack", "queue", "_Tp", "_Sequence", "_RandomAccessIterator", "_BidirectionalIterator", "_ForwardIterator", "_InputIterator1", "_InputIterator2", "_OutputIterator"};
        QString typePattern = "\\b(" + types.join("|") + ")\\b";
        res.replace(QRegularExpression(typePattern), "<span style=\"color:#4ec9b0;\">\\1</span>");

        res.replace(QRegularExpression("\\b(\\d+)\\b"), "<span style=\"color:#b5cea8;\">\\1</span>");

        res.replace(QRegularExpression("(//.*)"), "<span style=\"color:#6a9955;\">\\1</span>");

        return res;
    }

signals:
    void requestPlay();
};

#endif // VISUALBASE_H
