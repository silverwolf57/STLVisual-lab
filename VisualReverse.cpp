#include "VisualReverse.h"
#include "SubQuizWidget.h"
#include "QuizDatabase.h"
#include "UserProfile.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QPainter>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QIntValidator>
#include <QSplitter>
#include <QFileDialog>


// 构造函数：搭建界面，连接信号和槽，初始化数据
VisualReverse::VisualReverse(QWidget *parent) : VisualBase(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet("QTabWidget::pane { border: 1px solid #3c3c3c; background-color: #1e1e1e; }"
                             "QTabBar::tab { background-color: #252526; color: #888; padding: 8px 20px; border: 1px solid #3c3c3c; border-bottom: none; }"
                             "QTabBar::tab:selected { background-color: #1e1e1e; color: #569cd6; font-weight: bold; }");
    mainLayout->addWidget(tabWidget);

    QWidget* animTab = new QWidget(this);
    QVBoxLayout* animLayout = new QVBoxLayout(animTab);

    QWidget* inputPanel = new QWidget(this);
    inputPanel->setStyleSheet(
        "QWidget { background-color: #252526; border-radius: 6px; }"
        "QPushButton { background-color: #0e639c; color: white; border: none; padding: 6px 12px; border-radius: 4px; font-weight: bold; }"
        "QPushButton:hover { background-color: #1177bb; }"
        "QPushButton:pressed { background-color: #094771; }"
        "QLineEdit { background-color: #3c3c3c; color: white; border: 1px solid #555; border-radius: 3px; padding: 4px; }"
    );
    QHBoxLayout* inputLayout = new QHBoxLayout(inputPanel);
    inputLayout->setContentsMargins(10, 5, 10, 5);

    QLabel* lblInit = new QLabel("自定义数组(以逗号分隔):", this);
    valueEdit = new QLineEdit(this);
    valueEdit->setPlaceholderText("以逗号分隔，如 1,2,3");
    valueEdit->setText("10,20,30,40,50,60,70");
    valueEdit->setFixedWidth(200);
    QPushButton* btnLoad = new QPushButton("加载并翻转", this);

    inputLayout->addWidget(lblInit);
    inputLayout->addWidget(valueEdit);
    inputLayout->addWidget(btnLoad);
    inputLayout->addStretch();

    animLayout->addWidget(inputPanel);
    // 分割器：容纳画板与右侧的代码高亮面板
    QSplitter* splitter = new QSplitter(Qt::Horizontal, animTab);
    canvas = new CanvasWidget(this);
    codeBrowser = new QTextBrowser(this);
    codeBrowser->setStyleSheet("QTextBrowser { background-color: #1e1e1e; border: 1px solid #3c3c3c; border-radius: 4px; }");
    splitter->addWidget(canvas);
    splitter->addWidget(codeBrowser);
    splitter->setStretchFactor(0, 15);
    splitter->setStretchFactor(1, 7);
    splitter->setSizes(QList<int>() << 600 << 280);
    animLayout->addWidget(splitter, 1);
    tabWidget->addTab(animTab, "动画演示");

    QTextBrowser* infoBrowser = new QTextBrowser(this);
    infoBrowser->setStyleSheet("background-color: #1e1e1e; border: none; padding: 20px; font-size: 15px; line-height: 1.5;");
    infoBrowser->setHtml(getInfo());
    infoBrowser->setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    tabWidget->addTab(infoBrowser, "知识卡片");

    // --- Tab 2: Quiz Tab ---
    std::vector<QuizQuestion> questions = QuizDatabase::getQuestionsFor(QuizDatabase::Topic::Reverse, UserProfile::instance().getErrorsForTopic("Reverse"));
    SubQuizWidget* quizWidget = new SubQuizWidget(questions, "Reverse", this);
    tabWidget->addTab(quizWidget, "巩固练习");
    // 限制只能输入数字和逗号，并加入占位符提示
    QRegularExpression regExp("^[0-9,\\-\\s，]*$");
    QRegularExpressionValidator* listValidator = new QRegularExpressionValidator(regExp, this);
    valueEdit->setValidator(listValidator);


    connect(btnLoad, &QPushButton::clicked, this, &VisualReverse::handleLoad);

    realData = {10, 20, 30, 40, 50, 60, 70};
    QStringList initCode = {"// std::reverse 区间翻转", "// 可以选择顶部菜单执行操作..."};
    animSteps.push_back({realData, -1, -1, "就绪。点击按钮开始翻转。", initCode, -1});
    updateCodeBrowser();
}

// 处理用户输入并重新加载数组，包含格式校验
void VisualReverse::handleLoad() {
    QString text = valueEdit->text().trimmed();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "错误", "输入不能为空！");
        return;
    }
    QStringList parts = text.split(',');
    std::vector<int> newData;
    for (auto p : parts) {
        QString trimmed = p.trimmed();
        if (trimmed.isEmpty()) continue;
        bool ok;
        int val = trimmed.toInt(&ok);
        if (!ok) {
            QMessageBox::warning(this, "格式错误", QString("无法解析元素: \"%1\"，请输入纯数字！").arg(trimmed));
            return;
        }
        newData.push_back(val);
    }
    if (newData.empty() || newData.size() > 15) {
        QMessageBox::warning(this, "错误", "请输入 1~15 个数字！");
        return;
    }
    
    realData = newData;
    animSteps.clear();
    currentStep = 0;
    
    QStringList initCode = {"// 数组加载完毕，已就绪"};
    animSteps.push_back({realData, -1, -1, "加载完毕，开始 std::reverse 动画", initCode, -1});
    generateSteps();
    
    updateCodeBrowser();
    canvas->update();
}

// 翻转步骤的具体算法生成，并绑定伪代码高亮
void VisualReverse::generateSteps() {
    std::vector<int> arr = realData;
    int first = 0;
    int last = arr.size() - 1;

    QStringList revCode = {
        "template<typename BidirectionalIterator>",
        "void reverse(BidirectionalIterator first, BidirectionalIterator last) {",
        "    while (true) {",
        "        if (first == last || first == --last)",
        "            return;",
        "        std::iter_swap(first, last); // 交换首尾元素",
        "        ++first; // 首指针后移",
        "    }",
        "}"
    };

    while (first < last) {
        animSteps.push_back({arr, first, last, QString("准备交换 [%1]=%2 和 [%3]=%4").arg(first).arg(arr[first]).arg(last).arg(arr[last]), revCode, 3});
        std::swap(arr[first], arr[last]);
        animSteps.push_back({arr, first, last, QString("交换完成"), revCode, 5});
        first++;
        last--;
    }
    
    QStringList finishCode = {"// 翻转完成！"};
    animSteps.push_back({arr, -1, -1, "std::reverse 翻转完成", finishCode, 0});
    realData = arr;
}

// 推进动画的当前步骤并刷新代码高亮
void VisualReverse::stepForward() {
    if (currentStep < (int)animSteps.size() - 1) {
        currentStep++;
        updateCodeBrowser();
        canvas->update();
    }
}

// 动画后退一步并刷新代码高亮
void VisualReverse::stepBackward() {
    if (currentStep > 0) {
        currentStep--;
        updateCodeBrowser();
        canvas->update();
    }
}

// 恢复初始状态
void VisualReverse::reset() {
    currentStep = 0;
    updateCodeBrowser();
    canvas->update();
}

// 判断当前动画是否已经执行到最后一步
bool VisualReverse::isAtEnd() const {
    return animSteps.empty() || currentStep >= (int)animSteps.size() - 1;
}

// 更新伪代码显示面板
void VisualReverse::updateCodeBrowser() {
    if (currentStep >= 0 && currentStep < (int)animSteps.size()) {
        const auto& step = animSteps[currentStep];
        codeBrowser->setHtml(VisualBase::renderCodeWithHighlight(step.codeLines, step.activeLine));
    }
}

// 导出当前画布截图
void VisualReverse::exportImage() {
    QPixmap pixmap = canvas->grab();
    QString fileName = QFileDialog::getSaveFileName(this, "导出截图", "reverse_snapshot.png", "PNG 图像 (*.png);;JPEG 图像 (*.jpg)");
    if (!fileName.isEmpty()) {
        pixmap.save(fileName);
    }
}

// 返回包含该数据结构/算法原理和示例代码的 HTML 字符串
QString VisualReverse::getInfo() const {
    return QString(R"(
<style>
    h2 { color: #569cd6; margin-bottom: 5px; border-bottom: 1px solid #3c3c3c; padding-bottom: 5px; font-size: 20px;}
    h3 { color: #4ec9b0; margin-top: 15px; font-size: 16px;}
    p, li { color: #cccccc; font-size: 14px; line-height: 1.6; }
    b { color: #d4d4d4; }
    code { background-color: #2d2d30; color: #ce9178; padding: 2px 4px; border-radius: 3px; font-family: Consolas, monospace; }
    pre { background-color: #0d0d0d; color: #d4d4d4; padding: 10px; border-radius: 5px; border: 1px solid #333; font-family: Consolas, monospace; line-height: 1.4; overflow: auto; }
    table { border-collapse: collapse; width: 100%; margin-top: 10px; }
    th { background-color: #252526; color: #d4d4d4; border: 1px solid #3c3c3c; padding: 8px; text-align: left; }
    td { border: 1px solid #3c3c3c; padding: 8px; color: #cccccc; }
</style>
<h2>std::reverse 区间翻转</h2>
<p><b>std::reverse</b> 是 STL 提供的一个非常直观的算法，用于将给定区间内的元素首尾翻转。</p>

<h3>1. 算法特征</h3>
<ul>
    <li>时间复杂度：<b>O(N)</b>。它准确地执行 <code>N/2</code> 次交换操作。</li>
    <li>空间复杂度：<b>O(1)</b>，原地修改（In-place）。</li>
    <li>要求迭代器类型：至少是 <b>Bidirectional Iterator（双向迭代器）</b>。由于需要同时从前往后（<code>++</code>）和从后往前（<code>--</code>）收缩，单向的 Forward Iterator（比如 <code>std::forward_list</code>）是无法使用该算法的。</li>
</ul>

<h3>2. C++ 底层源码提炼 </h3>
<p>底层实现非常优雅，一个经典的双指针向中间收缩的模板代码：</p>
<pre>%1</pre>
)").arg(VisualBase::highlightCode(R"(template<typename _BidirectionalIterator>
void reverse(_BidirectionalIterator __first, _BidirectionalIterator __last) {
    // 只要区间还未遇到，且区间大小大于1
    while (true) {
        if (__first == __last || __first == --__last)
            return;
        else {
            // 交换两个指针指向的元素
            std::iter_swap(__first, __last);
            ++__first; // 头指针向后移
        }
    }
})"));
}

// 将画板的重绘委托给外层类的 paintCanvas 方法
void VisualReverse::CanvasWidget::paintEvent(QPaintEvent*) {
    p->paintCanvas(this);
}

// 绘制核心逻辑：根据当前步骤(currentStep)画出所有元素
void VisualReverse::paintCanvas(QWidget* target) {
    QPainter painter(target);
    painter.setRenderHint(QPainter::Antialiasing);

    if (animSteps.empty()) return;
    const RevAnimStep &step = animSteps[currentStep];

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12));
    painter.drawText(30, 35, "操作进度描述:");
    painter.setPen(QColor("#ffaa00"));
    painter.drawText(30, 58, step.desc);

    int boxW = 50, boxH = 50;
    int spacing = 15;
    int startX = 50;
    int startY = 150;

    for (size_t i = 0; i < step.data.size(); ++i) {
        int x = startX + i * (boxW + spacing);
        QRect rect(x, startY, boxW, boxH);

        if ((int)i == step.idx1 || (int)i == step.idx2) {
            painter.setBrush(QColor("#00d778")); // highlight
        } else {
            painter.setBrush(QColor("#0e639c")); // normal blue
        }

        painter.setPen(Qt::white);
        painter.drawRect(rect);
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(rect, Qt::AlignCenter, QString::number(step.data[i]));

        if ((int)i == step.idx1) {
            painter.setPen(QColor("#ffcc00"));
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(QRect(x, startY + boxH + 10, boxW, 20), Qt::AlignCenter, "FIRST");
        }
        if ((int)i == step.idx2) {
            painter.setPen(QColor("#ffcc00"));
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(QRect(x, startY + boxH + 10, boxW, 20), Qt::AlignCenter, "LAST");
        }
    }
}