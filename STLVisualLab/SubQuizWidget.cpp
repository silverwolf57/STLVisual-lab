#include "SubQuizWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QMessageBox>

#include <QKeyEvent>
#include <QTextBlock>
#include <QStandardPaths>
#include <QThread>
#include "UserProfile.h"
#include "CppHighlighter.h"

SubQuizWidget::SubQuizWidget(const std::vector<QuizQuestion>& questions, const QString& topicName, QWidget *parent)
    : QWidget(parent), questions(questions), topicName(topicName), currentQ(0), score(0) {
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(15);
    
    questionLabel = new QLabel(this);
    questionLabel->setFont(QFont("Arial", 13, QFont::Bold));
    questionLabel->setWordWrap(true);
    questionLabel->setStyleSheet("color: #569cd6;");
    layout->addWidget(questionLabel);

    // 1. 选择题容器
    choiceContainer = new QWidget(this);
    QVBoxLayout* choiceLayout = new QVBoxLayout(choiceContainer);
    choiceLayout->setContentsMargins(0, 0, 0, 0);
    choiceLayout->setSpacing(10);
    
    buttonGroup = new QButtonGroup(this);
    for (int i = 0; i < 4; ++i) {
        QRadioButton *rb = new QRadioButton(this);
        rb->setFont(QFont("Arial", 12));
        rb->setStyleSheet("QRadioButton { padding: 4px; color: #d4d4d4; }"
                          "QRadioButton::indicator { width: 16px; height: 16px; }"
                          "QRadioButton:hover { background-color: #2d2d30; border-radius: 4px; }");
        radioButtons.push_back(rb);
        buttonGroup->addButton(rb, i);
        choiceLayout->addWidget(rb);
    }
    layout->addWidget(choiceContainer);

    // 2. 编程题容器
    codingContainer = new QWidget(this);
    QVBoxLayout* codingLayout = new QVBoxLayout(codingContainer);
    codingLayout->setContentsMargins(0, 0, 0, 0);
    codingLayout->setSpacing(10);

    QLabel* lblEditor = new QLabel("代码编辑器 (C++):", this);
    lblEditor->setStyleSheet("color: #4ec9b0; font-weight: bold;");
    codingLayout->addWidget(lblEditor);

    codeEditor = new QPlainTextEdit(this);
    codeEditor->setFont(QFont("Consolas", 11));
    codeEditor->setStyleSheet("QPlainTextEdit { background-color: #1e1e1e; color: #d4d4d4; border: 1px solid #3c3c3c; border-radius: 4px; padding: 5px; }");
    codeEditor->setTabStopDistance(codeEditor->fontMetrics().horizontalAdvance(' ') * 4); // 4个空格制表符
    codeEditor->installEventFilter(this); // 安装事件过滤器
    highlighter = new CppHighlighter(codeEditor->document()); // 绑定语法高亮器
    codingLayout->addWidget(codeEditor, 3); // 给编辑器更多比重

    QLabel* lblConsole = new QLabel("编译与测试控制台:", this);
    lblConsole->setStyleSheet("color: #ce9178; font-weight: bold;");
    codingLayout->addWidget(lblConsole);

    consoleOutput = new QTextBrowser(this);
    consoleOutput->setFont(QFont("Consolas", 10));
    consoleOutput->setStyleSheet("QTextBrowser { background-color: #0d0d0d; color: #a9a9a9; border: 1px solid #3c3c3c; border-radius: 4px; padding: 5px; }");
    codingLayout->addWidget(consoleOutput, 1);
    layout->addWidget(codingContainer, 1); // 允许编程题容器在有空间时自由拉伸

    feedbackLabel = new QLabel(this);
    feedbackLabel->setFont(QFont("Arial", 12));
    feedbackLabel->setWordWrap(true);
    layout->addWidget(feedbackLabel);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    prevBtn = new QPushButton("上一题", this);
    prevBtn->setStyleSheet("QPushButton { background-color: #555; color: white; border-radius: 4px; padding: 6px 14px; font-weight: bold; }"
                           "QPushButton:hover { background-color: #666; }");
    submitBtn = new QPushButton("提交答案", this);
    submitBtn->setStyleSheet("QPushButton { background-color: #0e639c; color: white; border-radius: 4px; padding: 6px 14px; font-weight: bold; }"
                             "QPushButton:hover { background-color: #1177bb; }");
    nextBtn = new QPushButton("下一题", this);
    nextBtn->setStyleSheet("QPushButton { background-color: #D77800; color: white; border-radius: 4px; padding: 6px 14px; font-weight: bold; }"
                           "QPushButton:hover { background-color: #ff9900; }");
    btnLayout->addWidget(prevBtn);
    btnLayout->addWidget(submitBtn);
    btnLayout->addWidget(nextBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    connect(prevBtn, &QPushButton::clicked, this, &SubQuizWidget::prevQuestion);
    connect(submitBtn, &QPushButton::clicked, this, &SubQuizWidget::submitAnswer);
    connect(nextBtn, &QPushButton::clicked, this, &SubQuizWidget::nextQuestion);

    showQuestion(0);
}

// 显示指定索引的题目内容
void SubQuizWidget::showQuestion(int index) {
    if (questions.empty()) {
        questionLabel->setText("暂无练习题。");
        choiceContainer->hide();
        codingContainer->hide();
        submitBtn->hide();
        nextBtn->hide();
        return;
    }

    if (index >= (int)questions.size()) {
        questionLabel->setText(QString(" 本节测试完成！你的得分: %1 / %2").arg(score).arg(questions.size()));
        choiceContainer->hide();
        codingContainer->hide();
        submitBtn->hide();
        nextBtn->hide();
        feedbackLabel->hide();
        return;
    }
    
    prevBtn->setVisible(index > 0);
    feedbackLabel->show();
    const QuizQuestion &q = questions[index];
    questionLabel->setText(QString("第 %1 题 / 共 %2 题\n\n%3").arg(index + 1).arg(questions.size()).arg(q.text));
    
    if (q.isCoding) {
        choiceContainer->hide();
        codingContainer->show();
        if (userCodingAnswers.contains(index)) {
            codeEditor->setPlainText(userCodingAnswers[index]);
        } else {
            codeEditor->setPlainText(q.codeTemplate);
        }
        consoleOutput->clear();
        consoleOutput->setHtml("<span style='color: #888;'>点击\"提交答案\"开始编译评测...</span>");
    } else {
        choiceContainer->show();
        codingContainer->hide();
        
        for (int i = 0; i < 4; ++i) {
            if (i < q.options.size()) {
                radioButtons[i]->setText(q.options[i]);
                radioButtons[i]->show();
            } else {
                radioButtons[i]->hide();
            }
        }
        
        buttonGroup->setExclusive(false);
        for(auto rb : radioButtons) rb->setChecked(false);
        if (userChoiceAnswers.contains(index)) {
            int selected = userChoiceAnswers[index];
            if (selected >= 0 && selected < (int)radioButtons.size()) {
                radioButtons[selected]->setChecked(true);
            }
        }
        buttonGroup->setExclusive(true);
    }
    
    feedbackLabel->setText("");
    submitBtn->setText(q.isCoding ? "编译并提交测试" : "提交答案");
    submitBtn->show();
    nextBtn->hide();
}

// 提交单个答案
void SubQuizWidget::submitAnswer() {
    const QuizQuestion &q = questions[currentQ];
    if (q.isCoding) {
        runCompileAndTest();
        return;
    }

    int selected = buttonGroup->checkedId();
    if (selected == -1) {
        feedbackLabel->setText("[!] 请先选择一个答案！");
        feedbackLabel->setStyleSheet("color: #ffcc00;");
        return;
    }
    userChoiceAnswers[currentQ] = selected;

    if (selected == q.correctIndex) {
        feedbackLabel->setText("[OK] <b>回答正确！</b><br><br>" + q.explanation.toHtmlEscaped());
        feedbackLabel->setStyleSheet("color: #00d778; font-size: 13px;");
        score++;
    } else {
        feedbackLabel->setText("[XX] <b>回答错误。</b><br><br>" + q.explanation.toHtmlEscaped());
        feedbackLabel->setStyleSheet("color: #ff5c5c; font-size: 13px;");
        UserProfile::instance().recordError(topicName, currentQ);
    }

    submitBtn->hide();
    nextBtn->show();
}

// 处理编程题的编译与运行测试
void SubQuizWidget::runCompileAndTest() {
    const QuizQuestion &q = questions[currentQ];
    QString userCode = codeEditor->toPlainText();
    userCodingAnswers[currentQ] = userCode;
    
    consoleOutput->clear();
    consoleOutput->append("正在准备环境与源码...");
    submitBtn->setEnabled(false);

    // 寻找编译器路径
    QString compilerPath = "";
    compilerPath = QProcessEnvironment::systemEnvironment().value("STL_GPP_COMPILER");
    if (compilerPath.isEmpty()) {
        compilerPath = QStandardPaths::findExecutable("g++");
    }
    if (compilerPath.isEmpty()) {
        QString defaultPath = "C:/Qt/Tools/mingw1310_64/bin/g++.exe";
        if (QFile::exists(defaultPath)) {
            compilerPath = defaultPath;
        }
    }

    if (compilerPath.isEmpty()) {
        consoleOutput->append("<span style='color: #ff5c5c; font-weight: bold;'>"
                              "错误: 未在系统环境变量 PATH 中找到 g++ 编译器！<br>"
                              "为了进行编程题评测，请选择以下一种方法配置环境：<br>"
                              "1. 安装 MinGW 并将 g++.exe 所在的 bin 目录添加到系统环境变量 PATH 中；<br>"
                              "2. 或者设置系统环境变量 STL_GPP_COMPILER 指向您的 g++.exe 绝对路径。</span>");
        submitBtn->setEnabled(true);
        return;
    }

    // 1. 注入流输出重载以支持 vector, list, stack, queue 打印
    QString helpers = R"raw(
#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <string>
#include <algorithm>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    os << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        os << v[i] << (i + 1 < v.size() ? "," : "");
    }
    os << "]";
    return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::list<T>& lst) {
    os << "[";
    bool first = true;
    for (const auto& x : lst) {
        if (!first) os << ",";
        os << x;
        first = false;
    }
    os << "]";
    return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, std::stack<T> s) {
    os << "[";
    std::vector<T> v;
    while (!s.empty()) {
        v.push_back(s.top());
        s.pop();
    }
    for (auto it = v.rbegin(); it != v.rend(); ++it) {
        os << *it << (it + 1 != v.rend() ? "," : "");
    }
    os << "]";
    return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, std::queue<T> q) {
    os << "[";
    bool first = true;
    while (!q.empty()) {
        if (!first) os << ",";
        os << q.front();
        q.pop();
        first = false;
    }
    os << "]";
    return os;
}
)raw";

    QString testCode = q.testCasesCode;

    // 2. 动态用正则将 C++ assert/return 替换为 std::cout 流输出并递增 failures_count
    testCode.replace("int main() {", "int main() {\n    int failures_count = 0;");

    // 匹配 if (expr != expected) return exitCode;
    QRegularExpression reg1("if\\s*\\(([^!=]+)\\s*!=\\s*([^\\)]+)\\)\\s*return\\s*(\\d+)\\s*;");
    testCode.replace(reg1, 
        "std::cout << \"TEST_CASE_INEQ| \" << R\"#(\\1)#\" << \" | \" << R\"#(\\2)#\" << \" | \" << (\\1) << std::endl; if (\\1 != \\2) { failures_count++; }");

    // 匹配 if (expression) return exitCode;
    QRegularExpression reg2("if\\s*\\(([^!=\\n]+)\\)\\s*return\\s*(\\d+)\\s*;");
    testCode.replace(reg2,
        "std::cout << \"TEST_CASE_ASSERT| \" << R\"#(\\1)#\" << \" | \" << ((\\1) ? \"failed\" : \"passed\") << std::endl; if (\\1) { failures_count++; }");

    // 替换 ALL_TESTS_PASSED 打印
    testCode.replace("std::cout << \"ALL_TESTS_PASSED\" << std::endl;", 
                     "if (failures_count == 0) { std::cout << \"ALL_TESTS_PASSED\" << std::endl; }");

    // 替换 main 函数最后的 return 0; 为 return failures_count;
    testCode.replace("return 0;", "return failures_count;");

    // 拼接代码
    QString fullCode = helpers + "\n\n" + userCode + "\n\n" + testCode;

    // 写入临时文件到 QStandardPaths::TempLocation
    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QString cppFilePath = QDir(tempDir).absoluteFilePath("temp_compile.cpp");
#ifdef Q_OS_WIN
    QString exeFilePath = QDir(tempDir).absoluteFilePath("temp_compile.exe");
#else
    QString exeFilePath = QDir(tempDir).absoluteFilePath("temp_compile");
#endif

    // 在后台线程执行编译和测试，避免阻塞 UI
    QThread* workerThread = QThread::create([this, cppFilePath, exeFilePath, compilerPath, fullCode, q, topicName = this->topicName, currentQ = this->currentQ]() {
        // 写入临时文件
        QFile file(cppFilePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMetaObject::invokeMethod(this, [this]() {
                consoleOutput->append("<span style='color: #ff5c5c;'>错误: 无法写入测试源码文件！</span>");
                submitBtn->setEnabled(true);
            });
            return;
        }
        QTextStream out(&file);
        out << fullCode;
        file.close();

        QMetaObject::invokeMethod(this, [this]() {
            consoleOutput->append("编译线程已启动，正在调用 g++ 编译器进行后台编译评测...");
        });

        // 编译外部进程
        QProcess compileProcess;
        QStringList arguments;
        arguments << "-O2" << "-Werror=return-type" << cppFilePath << "-o" << exeFilePath;
        compileProcess.start(compilerPath, arguments);
        
        if (!compileProcess.waitForFinished(8000)) { // 8秒编译超时
            compileProcess.kill();
            QMetaObject::invokeMethod(this, [this, topicName, currentQ]() {
                consoleOutput->append("<span style='color: #ff5c5c;'>编译超时！后台编译进程已被强行终止。</span>");
                UserProfile::instance().recordError(topicName, currentQ);
                submitBtn->setEnabled(true);
            });
            return;
        }

        if (compileProcess.exitCode() != 0) {
            QString errors = QString::fromUtf8(compileProcess.readAllStandardError());
            QMetaObject::invokeMethod(this, [this, errors, topicName, currentQ]() {
                consoleOutput->append("<span style='color: #ff5c5c; font-weight: bold;'>编译失败！错误信息：</span>");
                QString errorsHtml = errors.toHtmlEscaped();
                errorsHtml.replace("\n", "<br>");
                consoleOutput->append(QString("<span style='color: #ff5c5c; font-family: Consolas;'>%1</span>").arg(errorsHtml));
                UserProfile::instance().recordError(topicName, currentQ);
                submitBtn->setEnabled(true);
            });
            return;
        }

        QMetaObject::invokeMethod(this, [this]() {
            consoleOutput->append("<span style='color: #00d778;'>编译成功！正在后台运行安全隔离评测...</span>");
        });

        // 运行评测外部进程
        QProcess runProcess;
        runProcess.start(exeFilePath);
        
        if (!runProcess.waitForFinished(2000)) { // 2秒运行硬超时，防死循环
            runProcess.kill();
            QMetaObject::invokeMethod(this, [this, topicName, currentQ]() {
                consoleOutput->append("<span style='color: #ffcc00; font-weight: bold;'>运行超时！可能存在无限循环或逻辑死锁。评测已被强行中止。</span>");
                UserProfile::instance().recordError(topicName, currentQ);
                submitBtn->setEnabled(true);
            });
            return;
        }

        int exitCode = runProcess.exitCode();
        QByteArray rawOutput = runProcess.readAllStandardOutput();
        QByteArray rawErrOutput = runProcess.readAllStandardError();

        // 输出大小截断保护，防内存暴涨
        if (rawOutput.size() > 65536) {
            rawOutput = rawOutput.left(65536) + "\n[评测系统提示: 输出量过大，已被截断]";
        }
        if (rawErrOutput.size() > 65536) {
            rawErrOutput = rawErrOutput.left(65536) + "\n[评测系统提示: 错误输出过大，已被截断]";
        }

        QString output = QString::fromUtf8(rawOutput).trimmed();
        QString errOutput = QString::fromUtf8(rawErrOutput).trimmed();

        QMetaObject::invokeMethod(this, [this, exitCode, output, errOutput, q, topicName, currentQ, cppFilePath, exeFilePath]() {
            // 解析评测结果
            QStringList lines = output.split('\n');
            QString formattedOutput = "";
            int testCaseCount = 0;
            
            for (const QString& line : lines) {
                QString trimmedLine = line.trimmed();
                if (trimmedLine.startsWith("TEST_CASE_INEQ|")) {
                    testCaseCount++;
                    QStringList parts = trimmedLine.split('|');
                    if (parts.size() >= 4) {
                        QString expr = parts[1].trimmed();
                        QString expected = parts[2].trimmed();
                        QString actual = parts[3].trimmed();
                        bool isCorrect = (expected == actual);
                        QString colorStr = isCorrect ? "#00d778" : "#ff5c5c";
                        formattedOutput += QString("<span style='color: #fff;'>测试点 %1:</span> 输入: <code>%2</code><br>"
                                                   "&nbsp;&nbsp;&nbsp;&nbsp;期望输出: <code style='color: #00d778;'>%3</code> | "
                                                   "您的实际输出: <code style='color: %4;'>%5</code><br>")
                                                   .arg(testCaseCount).arg(expr.toHtmlEscaped()).arg(expected.toHtmlEscaped()).arg(colorStr).arg(actual.toHtmlEscaped());
                    }
                } else if (trimmedLine.startsWith("TEST_CASE_ASSERT|")) {
                    testCaseCount++;
                    QStringList parts = trimmedLine.split('|');
                    if (parts.size() >= 3) {
                        QString expr = parts[1].trimmed();
                        QString status = parts[2].trimmed();
                        bool isCorrect = (status == "passed");
                        QString colorStr = isCorrect ? "#00d778" : "#ff5c5c";
                        formattedOutput += QString("<span style='color: #fff;'>测试点 %1 (断言):</span> <code>%2</code> | "
                                                   "结果: <code style='color: %3;'>%4</code><br>")
                                                   .arg(testCaseCount).arg(expr.toHtmlEscaped()).arg(colorStr).arg(isCorrect ? "通过" : "失败");
                    }
                } else if (trimmedLine.contains("ALL_TESTS_PASSED")) {
                    // 保留标志
                } else if (!trimmedLine.isEmpty()) {
                    formattedOutput += trimmedLine.toHtmlEscaped() + "<br>";
                }
            }

            if (exitCode == 0) {
                consoleOutput->append("<span style='color: #00d778; font-weight: bold;'>[OK] 所有测试点通过！恭喜完成。</span>");
                if (!formattedOutput.isEmpty()) {
                    consoleOutput->append("<span style='color: #888;'><b>测试点运行报告：</b></span><br>" + formattedOutput);
                }
                QString escapedExpl = q.explanation.toHtmlEscaped();
                feedbackLabel->setText(QString("[OK] 编程测试通过！可以继续下一题。<br><br><b>参考答案：</b><br>"
                                               "<pre style='font-family: Consolas, monospace; background-color: #1e1e1e; "
                                               "color: #d4d4d4; padding: 10px; border-radius: 4px; border: 1px solid #3c3c3c;'>%1</pre>")
                                               .arg(escapedExpl));
                feedbackLabel->setStyleSheet("color: #00d778; font-size: 13px;");
                score++;
                submitBtn->hide();
                nextBtn->show();
            } else {
                consoleOutput->append("<span style='color: #ff5c5c; font-weight: bold;'>[XX] 测试未通过！</span>");
                if (!formattedOutput.isEmpty()) {
                    consoleOutput->append("<span style='color: #888;'><b>测试点运行报告（包含错误项）：</b></span><br>" + formattedOutput);
                }
                if (!errOutput.isEmpty()) {
                    QString errHtml = errOutput.toHtmlEscaped();
                    errHtml.replace("\n", "<br>");
                    consoleOutput->append(QString("<span style='color: #ff5c5c;'>标准错误输出: %1</span>").arg(errHtml));
                }
                UserProfile::instance().recordError(topicName, currentQ);
            }

            // 清理临时文件
            QFile::remove(cppFilePath);
            QFile::remove(exeFilePath);
            submitBtn->setEnabled(true);
        });
    });

    connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);
    workerThread->start();
}

// 切换到下一题的逻辑处理
void SubQuizWidget::nextQuestion() {
    currentQ++;
    showQuestion(currentQ);
}

// 切换到上一题的逻辑处理
void SubQuizWidget::prevQuestion() {
    if (currentQ > 0) {
        currentQ--;
        showQuestion(currentQ);
    }
}

// 事件过滤器：提供 Tab 转空格、回车自动缩进、括号自动补全等 IDE 代码体验
bool SubQuizWidget::eventFilter(QObject* watched, QEvent* event) {
    if (watched == codeEditor && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        
        // 1. Tab 键自动转为 4 个空格
        if (keyEvent->key() == Qt::Key_Tab) {
            codeEditor->insertPlainText("    ");
            return true;
        }
        
        // 2. 回车自动缩进
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            QTextCursor cursor = codeEditor->textCursor();
            QTextBlock currentBlock = cursor.block();
            QString currentLineText = currentBlock.text();
            
            // 获取当前行的前导空格以继承缩进
            QString indent = "";
            for (int i = 0; i < currentLineText.length(); ++i) {
                if (currentLineText[i].isSpace()) {
                    indent += currentLineText[i];
                } else {
                    break;
                }
            }
            
            // 如果是以左大括号 '{' 结尾，自动增加 4 个空格的额外缩进
            QString trimmedText = currentLineText.trimmed();
            if (trimmedText.endsWith('{')) {
                indent += "    ";
            }
            
            cursor.insertText("\n" + indent);
            codeEditor->setTextCursor(cursor);
            return true;
        }
        
        // 3. 括号/引号自动闭合并回退光标 1 格
        QChar pairChar;
        if (keyEvent->key() == Qt::Key_BraceLeft) pairChar = '}';
        else if (keyEvent->key() == Qt::Key_ParenLeft) pairChar = ')';
        else if (keyEvent->key() == Qt::Key_BracketLeft) pairChar = ']';
        else if (keyEvent->key() == Qt::Key_QuoteDbl) pairChar = '"';
        else if (keyEvent->key() == Qt::Key_Apostrophe) pairChar = '\'';
        
        if (!pairChar.isNull()) {
            QTextCursor cursor = codeEditor->textCursor();
            QString currentInput = keyEvent->text();
            if (!currentInput.isEmpty()) {
                cursor.insertText(currentInput + pairChar);
                cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
                codeEditor->setTextCursor(cursor);
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}
