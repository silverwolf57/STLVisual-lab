#include "VisualLowerBound.h"
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
VisualLowerBound::VisualLowerBound(QWidget *parent) : VisualBase(parent) {
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

    QLabel* lblInit = new QLabel("自定义有序数组(以逗号分隔):", this);
    valueEdit = new QLineEdit(this);
    valueEdit->setPlaceholderText("以逗号分隔，如 1,2,3");
    valueEdit->setText("1,3,3,5,7,9,9,11");
    valueEdit->setFixedWidth(200);
    QPushButton* btnLoad = new QPushButton("加载", this);

    QLabel* lblTarget = new QLabel("查找目标():", this);
    targetEdit = new QLineEdit(this);
    targetEdit->setPlaceholderText("值");
    targetEdit->setFixedWidth(50);
    QPushButton* btnFind = new QPushButton("lower_bound", this);

    inputLayout->addWidget(lblInit);
    inputLayout->addWidget(valueEdit);
    inputLayout->addWidget(btnLoad);
    inputLayout->addSpacing(15);
    inputLayout->addWidget(lblTarget);
    inputLayout->addWidget(targetEdit);
    inputLayout->addWidget(btnFind);
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
    std::vector<QuizQuestion> questions = QuizDatabase::getQuestionsFor(QuizDatabase::Topic::LowerBound, UserProfile::instance().getErrorsForTopic("LowerBound"));
    SubQuizWidget* quizWidget = new SubQuizWidget(questions, "LowerBound", this);
    tabWidget->addTab(quizWidget, "巩固练习");
    // 限制只能输入数字和逗号，并加入占位符提示
    QRegularExpression regExp("^[0-9,\\-\\s，]*$");
    QRegularExpressionValidator* listValidator = new QRegularExpressionValidator(regExp, this);
    valueEdit->setValidator(listValidator);
    QIntValidator* intValidator = new QIntValidator(0, 9999, this);
    targetEdit->setValidator(intValidator);


    connect(btnLoad, &QPushButton::clicked, this, &VisualLowerBound::handleLoad);
    connect(btnFind, &QPushButton::clicked, this, &VisualLowerBound::handleFind);

    realData = {1, 3, 3, 5, 7, 9, 9, 11};
    targetVal = 3;
    QStringList initCode = {"// std::lower_bound 有序折半查找", "// 可以选择顶部菜单执行操作..."};
    animSteps.push_back({realData, 0, (int)realData.size(), -1, targetVal, "就绪。点击上方 lower_bound 开始演示二分查找。", initCode, -1});
    updateCodeBrowser();
}

// 处理用户输入并重新加载数组，包含严格的有序性与越界校验
void VisualLowerBound::handleLoad() {
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
    
    // 校验数组是否有序
    for (size_t i = 1; i < newData.size(); ++i) {
        if (newData[i] < newData[i-1]) {
            QMessageBox::warning(this, "警告", "lower_bound 要求数组必须升序排列！");
            return;
        }
    }

    realData = newData;
    animSteps.clear();
    currentStep = 0;
    QStringList initCode = {"// 数组重新加载完成，已经过升序检查"};
    animSteps.push_back({realData, 0, (int)realData.size(), -1, targetVal, "数组重新加载完成。", initCode, -1});
    updateCodeBrowser();
    canvas->update();
}

// 开始查找目标
void VisualLowerBound::handleFind() {
    QString text = targetEdit->text().trimmed();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入查找目标值！");
        return;
    }
    bool ok;
    int val = text.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "错误", "请输入有效的查找数值！");
        return;
    }
    targetVal = val;

    animSteps.clear();
    currentStep = 0;
    
    QStringList lbCode = {
        "template<typename ForwardIterator, typename T>",
        "ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& val) {",
        "    auto len = distance(first, last);",
        "    while (len > 0) {",
        "        auto half = len >> 1;",
        "        auto middle = first + half; // 找到区间中点",
        "        if (*middle < val) {",
        "            first = middle + 1; // 目标在右半侧",
        "            len = len - half - 1;",
        "        } else {",
        "            len = half; // 目标在左侧或中点",
        "        }",
        "    }",
        "    return first;",
        "}"
    };

    animSteps.push_back({realData, 0, (int)realData.size(), -1, targetVal, QString("开始 std::lower_bound 查找  %1 的第一个元素").arg(targetVal), lbCode, 2});
    generateSteps();
    
    updateCodeBrowser();
    canvas->update();
}

// 二分查找步骤的具体算法生成
void VisualLowerBound::generateSteps() {
    int first = 0;
    int len = realData.size();

    QStringList lbCode = {
        "template<typename ForwardIterator, typename T>",
        "ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& val) {",
        "    auto len = distance(first, last);",
        "    while (len > 0) {",
        "        auto half = len >> 1;",
        "        auto middle = first + half; // 找到区间中点",
        "        if (*middle < val) { // 若中值小于目标值",
        "            first = middle + 1; // 目标在右侧，跳过中点",
        "            len = len - half - 1;",
        "        } else { // 若中值大于或等于目标值",
        "            len = half; // 目标在中点或左侧，收紧右边界",
        "        }",
        "    }",
        "    return first;",
        "}"
    };

    while (len > 0) {
        int half = len >> 1;
        int mid = first + half;

        animSteps.push_back({realData, first, first + len, mid, targetVal, QString("Len=%1, Half=%2。检查 mid [%3] = %4").arg(len).arg(half).arg(mid).arg(realData[mid]), lbCode, 5});

        if (realData[mid] < targetVal) {
            first = mid + 1;
            len = len - half - 1;
            animSteps.push_back({realData, first, first + len, -1, targetVal, QString("值 %1 < 目标值 %2，目标必定在右半部分，更新 first").arg(realData[mid]).arg(targetVal), lbCode, 7});
        } else {
            len = half;
            animSteps.push_back({realData, first, first + len, -1, targetVal, QString("值 %1  目标值 %2，目标可能在此处或其左侧，收缩 len").arg(realData[mid]).arg(targetVal), lbCode, 10});
        }
    }
    
    animSteps.push_back({realData, first, first, first, targetVal, QString("循环结束，Len=0。最终结果: 索引 %1 (值: %2)").arg(first).arg(first < (int)realData.size() ? QString::number(realData[first]) : "超出边界"), lbCode, 13});
}

// 推进动画的当前步骤并刷新代码高亮
void VisualLowerBound::stepForward() {
    if (currentStep < (int)animSteps.size() - 1) {
        currentStep++;
        updateCodeBrowser();
        canvas->update();
    }
}

// 动画后退一步并刷新代码高亮
void VisualLowerBound::stepBackward() {
    if (currentStep > 0) {
        currentStep--;
        updateCodeBrowser();
        canvas->update();
    }
}

// 恢复初始状态
void VisualLowerBound::reset() {
    currentStep = 0;
    updateCodeBrowser();
    canvas->update();
}

// 判断当前动画是否已经执行到最后一步
bool VisualLowerBound::isAtEnd() const {
    return animSteps.empty() || currentStep >= (int)animSteps.size() - 1;
}

// 更新伪代码显示面板
void VisualLowerBound::updateCodeBrowser() {
    if (currentStep >= 0 && currentStep < (int)animSteps.size()) {
        const auto& step = animSteps[currentStep];
        codeBrowser->setHtml(VisualBase::renderCodeWithHighlight(step.codeLines, step.activeLine));
    }
}

// 导出当前画布截图
void VisualLowerBound::exportImage() {
    QPixmap pixmap = canvas->grab();
    QString fileName = QFileDialog::getSaveFileName(this, "导出截图", "lowerbound_snapshot.png", "PNG 图像 (*.png);;JPEG 图像 (*.jpg)");
    if (!fileName.isEmpty()) {
        pixmap.save(fileName);
    }
}

// 返回包含该数据结构/算法原理和示例代码的 HTML 字符串
QString VisualLowerBound::getInfo() const {
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
<h2>std::lower_bound 二分查找</h2>
<p><b>std::lower_bound</b> 用于在<b>已排序</b>的区间中，使用二分查找法寻找第一个<b>大于或等于 ()</b> 给定值的元素。</p>

<h3>1. 算法特征</h3>
<ul>
    <li>返回的是一个迭代器，指向满足条件的第一处位置。</li>
    <li>如果区间中所有的元素都小于目标值，它会返回区间的尾后迭代器 (<code>end()</code>)。</li>
    <li>对于随机访问迭代器 (如 <code>std::vector</code>)，时间复杂度为 <b>O(log N)</b> 次比较和移动。</li>
    <li>对于非随机访问迭代器 (如 <code>std::list</code>)，比较次数依然是 O(log N)，但迭代器移动的次数会退化为 <b>O(N)</b>。</li>
</ul>

<h3>2. C++ 底层源码提炼 </h3>
<p>GCC 的 <code>std::lower_bound</code> 实现并没有使用经典的 <code>left</code> 和 <code>right</code> 指针，而是使用了 <code>__first</code> 和 <code>__len</code> 的巧妙设计。这种设计使得代码更加紧凑，且完全避免了 <code>left + right</code> 带来的整型溢出风险：</p>
<pre>%1</pre>
)").arg(VisualBase::highlightCode(R"(template<typename _ForwardIterator, typename _Tp>
_ForwardIterator __lower_bound(_ForwardIterator __first, _ForwardIterator __last,
                               const _Tp& __val) {
    // std::distance 计算两个迭代器之间的距离，对于 vector 来说就是直接相减
    auto __len = std::distance(__first, __last);

    while (__len > 0) {
        // 右移 1 位，相当于除以 2，速度更快
        auto __half = __len >> 1;
        
        // __middle 指向区间中间的位置
        _ForwardIterator __middle = __first;
        std::advance(__middle, __half);

        // 如果中间值小于目标值，说明目标一定在右半边
        if (*__middle < __val) {
            __first = __middle;
            ++__first; // 跳过 __middle 本身
            __len = __len - __half - 1;
        }
        else {
            // 如果中间值大于或等于目标值，目标可能就是 __middle，或者在左半边
            __len = __half;
        }
    }
    // 最终 __first 会收敛到我们想要的位置
    return __first;
})"));
}

// 将画板的重绘委托给外层类的 paintCanvas 方法
void VisualLowerBound::CanvasWidget::paintEvent(QPaintEvent*) {
    p->paintCanvas(this);
}

// 绘制核心逻辑：根据当前步骤(currentStep)画出所有元素
void VisualLowerBound::paintCanvas(QWidget* target) {
    QPainter painter(target);
    painter.setRenderHint(QPainter::Antialiasing);

    if (animSteps.empty()) return;
    const LBAnimStep &step = animSteps[currentStep];

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12));
    painter.drawText(30, 35, "操作进度描述:");
    painter.setPen(QColor("#ffaa00"));
    painter.drawText(30, 58, step.desc);

    int boxW = 50, boxH = 50;
    int spacing = 15;
    int startX = 50;
    int startY = 150;

    for (int i = 0; i <= (int)step.data.size(); ++i) {
        int x = startX + i * (boxW + spacing);
        QRect rect(x, startY, boxW, boxH);

        if (i < (int)step.data.size()) {
            if (i >= step.left && i < step.right) {
                painter.setBrush(QColor("#00d778")); // In range
            } else {
                painter.setBrush(QColor("#333333")); // Out of range
            }

            if (i == step.midIdx) {
                painter.setBrush(QColor("#e51400")); // Mid point
                painter.setPen(QColor("#ffcc00"));
                painter.setFont(QFont("Arial", 10, QFont::Bold));
                painter.drawText(QRect(x, startY - 25, boxW, 20), Qt::AlignCenter, "MID");
            }

            painter.setPen(Qt::white);
            painter.drawRect(rect);
            painter.setFont(QFont("Arial", 14, QFont::Bold));
            painter.drawText(rect, Qt::AlignCenter, QString::number(step.data[i]));
        } else {
            // "end" iterator visualization
            if (step.left == i && step.right == i) {
                painter.setBrush(QColor("#00d778"));
                painter.setPen(Qt::white);
                painter.drawRect(rect);
                painter.setFont(QFont("Arial", 10, QFont::Bold));
                painter.drawText(rect, Qt::AlignCenter, "END");
            }
        }

        if (step.left == step.right && i == step.left) {
            painter.setPen(QColor("#ffaa00"));
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(QRect(x, startY + boxH + 10, boxW, 20), Qt::AlignCenter, "RESULT");
        }
    }
}