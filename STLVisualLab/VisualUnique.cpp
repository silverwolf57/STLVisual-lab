#include "VisualUnique.h"
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
VisualUnique::VisualUnique(QWidget *parent) : VisualBase(parent) {
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
    valueEdit->setText("1,1,2,2,2,3,4,4,5");
    valueEdit->setFixedWidth(200);
    QPushButton* btnLoad = new QPushButton("加载并去重", this);

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
    std::vector<QuizQuestion> questions = QuizDatabase::getQuestionsFor(QuizDatabase::Topic::Unique, UserProfile::instance().getErrorsForTopic("Unique"));
    SubQuizWidget* quizWidget = new SubQuizWidget(questions, "Unique", this);
    tabWidget->addTab(quizWidget, "巩固练习");
    // 限制只能输入数字和逗号，并加入占位符提示
    QRegularExpression regExp("^[0-9,\\-\\s，]*$");
    QRegularExpressionValidator* listValidator = new QRegularExpressionValidator(regExp, this);
    valueEdit->setValidator(listValidator);


    connect(btnLoad, &QPushButton::clicked, this, &VisualUnique::handleLoad);

    realData = {1, 1, 2, 2, 2, 3, 4, 4, 5};
    QStringList initCode = {"// std::unique 双指针相邻去重", "// 可以选择顶部菜单执行操作..."};
    animSteps.push_back({realData, -1, -1, -1, "就绪。点击按钮开始 std::unique 去重。", initCode, -1});
    updateCodeBrowser();
}

// 处理用户输入并重新加载数组，包含格式校验
void VisualUnique::handleLoad() {
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
    animSteps.push_back({realData, -1, -1, -1, "加载完毕，开始 std::unique 动画", initCode, -1});
    generateSteps();
    
    updateCodeBrowser();
    canvas->update();
}

// 相邻去重步骤具体算法生成，包含伪代码高亮行绑定
void VisualUnique::generateSteps() {
    std::vector<int> arr = realData;
    if (arr.empty()) return;

    QStringList uniqueCode = {
        "template<typename ForwardIterator>",
        "ForwardIterator unique(ForwardIterator first, ForwardIterator last) {",
        "    if (first == last) return last;",
        "    ForwardIterator dest = first; // 慢指针",
        "    while (++first != last) { // 快指针扫描",
        "        if (!(*dest == *first)) { // 发现新元素",
        "            *(++dest) = *first; // 复制覆盖并前移慢指针",
        "        }",
        "    }",
        "    return ++dest; // 返回新边界",
        "}"
    };

    int dest = 0; // 慢指针 (结果)
    for (size_t i = 1; i < arr.size(); ++i) {
        animSteps.push_back({arr, dest, (int)i, -1, QString("快慢指针比较: dest[%1]=%2 与 first[%3]=%4").arg(dest).arg(arr[dest]).arg(i).arg(arr[i]), uniqueCode, 5});
        
        if (arr[dest] != arr[i]) {
            dest++;
            if (dest != (int)i) {
                animSteps.push_back({arr, dest, (int)i, -1, QString("发现不重复元素，快指针覆盖写入慢指针指向 [%1] <- %2").arg(dest).arg(arr[i]), uniqueCode, 6});
                arr[dest] = arr[i];
            } else {
                animSteps.push_back({arr, dest, (int)i, -1, QString("不重复元素，且指针位置重合，dest 直接前进"), uniqueCode, 3});
            }
        } else {
            animSteps.push_back({arr, dest, (int)i, -1, QString("元素值相同，跳过此相邻重复值"), uniqueCode, 4});
        }
    }
    
    int newSize = dest + 1;
    QStringList finishCode = {"// 去重完毕！返回逻辑尾后迭代器"};
    animSteps.push_back({arr, -1, -1, newSize, QString("去重完成。返回新的逻辑尾迭代器，新长度为 %1。后续可用 erase 删除垃圾数据。").arg(newSize), finishCode, 0});
    realData = arr;
}

// 推进动画的当前步骤并刷新代码高亮
void VisualUnique::stepForward() {
    if (currentStep < (int)animSteps.size() - 1) {
        currentStep++;
        updateCodeBrowser();
        canvas->update();
    }
}

// 动画后退一步并刷新代码高亮
void VisualUnique::stepBackward() {
    if (currentStep > 0) {
        currentStep--;
        updateCodeBrowser();
        canvas->update();
    }
}

// 恢复初始状态
void VisualUnique::reset() {
    currentStep = 0;
    updateCodeBrowser();
    canvas->update();
}

// 判断当前动画是否已经执行到最后一步
bool VisualUnique::isAtEnd() const {
    return animSteps.empty() || currentStep >= (int)animSteps.size() - 1;
}

// 更新伪代码显示面板
void VisualUnique::updateCodeBrowser() {
    if (currentStep >= 0 && currentStep < (int)animSteps.size()) {
        const auto& step = animSteps[currentStep];
        codeBrowser->setHtml(VisualBase::renderCodeWithHighlight(step.codeLines, step.activeLine));
    }
}

// 导出当前画布截图
void VisualUnique::exportImage() {
    QPixmap pixmap = canvas->grab();
    QString fileName = QFileDialog::getSaveFileName(this, "导出截图", "unique_snapshot.png", "PNG 图像 (*.png);;JPEG 图像 (*.jpg)");
    if (!fileName.isEmpty()) {
        pixmap.save(fileName);
    }
}

// 返回包含该数据结构/算法原理和示例代码的 HTML 字符串
QString VisualUnique::getInfo() const {
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
<h2>std::unique 相邻去重</h2>
<p><b>std::unique</b> 是一个非常容易被误解的算法。很多初学者以为调用它就能完美去重并缩小容器。实际上，由于 STL 的"算法与容器解耦"原则，算法本身是没有能力调用 <code>erase</code> 改变容器大小的。</p>

<h3>1. Erase-Remove 惯用法</h3>
<p>因为 <code>std::unique</code> 只是将不重复的元素移到了区间前端，把被覆盖的"垃圾数据"留在了尾部，所以正确的做法是使用返回的迭代器进行擦除：</p>
<pre><code>v.erase(std::unique(v.begin(), v.end()), v.end());</code></pre>
<p>而且由于它只比较相邻元素，所以通常需要先 <code>std::sort</code>。</p>

<h3>2. 算法特征</h3>
<ul>
    <li>时间复杂度：<b>O(N)</b>，只需要一次线性扫描。</li>
    <li>空间复杂度：<b>O(1)</b>，采用双指针技巧就地修改。</li>
    <li>迭代器要求：<b>Forward Iterator</b> 及以上。</li>
</ul>

<h3>3. C++ 底层源码提炼 </h3>
<p>经典的快慢双指针（Read 和 Write 指针）实现：</p>
<pre>%1</pre>
)").arg(VisualBase::highlightCode(R"(template<typename _ForwardIterator>
_ForwardIterator unique(_ForwardIterator __first, _ForwardIterator __last) {
    if (__first == __last) return __last;
    
    // __dest 相当于 Write 指针，__first 相当于 Read 指针
    _ForwardIterator __dest = __first;
    ++__first;
    
    while (__first != __last) {
        // 如果当前读取的值和上一个写入的值不相等
        if (!(*__dest == *__first)) {
            // Write 指针前进一步
            ++__dest;
            // 如果 Read 和 Write 错开了（说明发生过重复跳过）
            // 将 Read 的值赋给 Write 位置
            if (__dest != __first) {
                *__dest = std::move(*__first);
            }
        }
        ++__first;
    }
    // 返回新的逻辑尾部（也就是原数组的有效长度）
    return ++__dest;
})"));
}

// 将画板的重绘委托给外层类的 paintCanvas 方法
void VisualUnique::CanvasWidget::paintEvent(QPaintEvent*) {
    p->paintCanvas(this);
}

// 绘制核心逻辑：根据当前步骤(currentStep)画出所有元素
void VisualUnique::paintCanvas(QWidget* target) {
    QPainter painter(target);
    painter.setRenderHint(QPainter::Antialiasing);

    if (animSteps.empty()) return;
    const UniqueAnimStep &step = animSteps[currentStep];

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

        if (step.newSize != -1 && (int)i >= step.newSize) {
            painter.setBrush(QColor("#333333")); // Trashed data
        } else if ((int)i == step.destIdx) {
            painter.setBrush(QColor("#00d778")); // Write head
        } else if ((int)i == step.currIdx) {
            painter.setBrush(QColor("#e51400")); // Read head
        } else {
            painter.setBrush(QColor("#0e639c")); // normal blue
        }

        painter.setPen(Qt::white);
        painter.drawRect(rect);
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(rect, Qt::AlignCenter, QString::number(step.data[i]));

        if ((int)i == step.destIdx) {
            painter.setPen(QColor("#00d778"));
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(QRect(x, startY - 25, boxW, 20), Qt::AlignCenter, "DEST");
        }
        if ((int)i == step.currIdx) {
            painter.setPen(QColor("#e51400"));
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(QRect(x, startY + boxH + 5, boxW, 20), Qt::AlignCenter, "READ");
        }
    }
}