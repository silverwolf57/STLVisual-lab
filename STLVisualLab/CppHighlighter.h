#ifndef CPPHIGHLIGHTER_H
#define CPPHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QTextDocument>

class CppHighlighter : public QSyntaxHighlighter {
public:
    explicit CppHighlighter(QTextDocument *parent = nullptr) : QSyntaxHighlighter(parent) {
        HighlightingRule rule;

        // 关键字高亮格式 (浅蓝色/青色 #569cd6)
        keywordFormat.setForeground(QColor("#569cd6"));
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns = {
            "\\bchar\\b", "\\bclass\\b", "\\bconst\\b", "\\bdouble\\b", "\\benum\\b",
            "\\bexplicit\\b", "\\bfloat\\b", "\\bfor\\b", "\\bif\\b", "\\binline\\b",
            "\\bint\\b", "\\blong\\b", "\\boperator\\b", "\\bprivate\\b", "\\bprotected\\b",
            "\\bpublic\\b", "\\bshort\\b", "\\bsignals\\b", "\\bsigned\\b", "\\bslots\\b",
            "\\bstatic\\b", "\\bstruct\\b", "\\btemplate\\b", "\\btypedef\\b", "\\btypename\\b",
            "\\bunion\\b", "\\bunsigned\\b", "\\bvirtual\\b", "\\bvoid\\b", "\\bvolatile\\b",
            "\\bbool\\b", "\\btrue\\b", "\\bfalse\\b", "\\breturn\\b", "\\bwhile\\b", "\\belse\\b",
            "\\bauto\\b", "\\bstd\\b", "\\bvector\\b", "\\bstring\\b"
        };
        for (const QString &pattern : keywordPatterns) {
            rule.pattern = QRegularExpression(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }

        // 头文件指令高亮格式 (紫红色 #bd7af5)
        preprocessorFormat.setForeground(QColor("#bd7af5"));
        rule.pattern = QRegularExpression(R"(\b#[a-zA-Z]+\b)");
        rule.format = preprocessorFormat;
        highlightingRules.append(rule);

        // 字符串高亮格式 (淡橙色 #ce9178)
        stringFormat.setForeground(QColor("#ce9178"));
        rule.pattern = QRegularExpression(R"("[^\n"]*")");
        rule.format = stringFormat;
        highlightingRules.append(rule);
        rule.pattern = QRegularExpression(R"('[^\n']*')");
        rule.format = stringFormat;
        highlightingRules.append(rule);

        // 单行注释高亮格式 (绿色 #6a9955)
        singleLineCommentFormat.setForeground(QColor("#6a9955"));
        rule.pattern = QRegularExpression(R"(//[^\n]*)");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        // 多行注释高亮格式 (绿色 #6a9955)
        multiLineCommentFormat.setForeground(QColor("#6a9955"));
        commentStartExpression = QRegularExpression(R"(/\*)");
        commentEndExpression = QRegularExpression(R"(\*/)");
    }

protected:
    void highlightBlock(const QString &text) override {
        // 应用单行高亮规则
        for (const HighlightingRule &rule : highlightingRules) {
            QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
            while (matchIterator.hasNext()) {
                QRegularExpressionMatch match = matchIterator.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }

        // 处理多行注释 (/* 到 */)
        setCurrentBlockState(0);
        int startIndex = 0;
        if (previousBlockState() != 1) {
            QRegularExpressionMatch match = commentStartExpression.match(text);
            if (match.hasMatch()) {
                startIndex = match.capturedStart();
            } else {
                startIndex = -1;
            }
        }

        while (startIndex >= 0) {
            QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
            int endIndex = -1;
            int commentLength = 0;
            if (match.hasMatch()) {
                endIndex = match.capturedStart();
                commentLength = endIndex - startIndex + match.capturedLength();
            } else {
                setCurrentBlockState(1);
                commentLength = text.length() - startIndex;
            }
            setFormat(startIndex, commentLength, multiLineCommentFormat);
            if (currentBlockState() == 1) {
                break;
            }
            QRegularExpressionMatch nextStartMatch = commentStartExpression.match(text, startIndex + commentLength);
            if (nextStartMatch.hasMatch()) {
                startIndex = nextStartMatch.capturedStart();
            } else {
                startIndex = -1;
            }
        }
    }

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QList<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat preprocessorFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat stringFormat;
};

#endif // CPPHIGHLIGHTER_H
