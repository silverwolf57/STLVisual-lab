#include "VisualStackQueue.h"
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


VisualStackQueue::VisualStackQueue(QWidget *parent) : VisualBase(parent), currentStep(0) {
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

    QLabel* lblInit = new QLabel("初始队列(以逗号分隔):", this);
    initEdit = new QLineEdit(this);
    initEdit->setPlaceholderText("以逗号分隔，如 1,2,3");
    initEdit->setText("1,2,3");
    initEdit->setFixedWidth(100);
    QPushButton* btnLoad = new QPushButton("加载", this);

    QLabel* lblPush = new QLabel("入队/入栈:", this);
    pushValueEdit = new QLineEdit(this);
    pushValueEdit->setPlaceholderText("值");
    pushValueEdit->setFixedWidth(50);
    QPushButton* btnPush = new QPushButton("push", this);

    QPushButton* btnPopStack = new QPushButton("Stack Pop (后进先出)", this);
    QPushButton* btnPopQueue = new QPushButton("Queue Pop (先进先出)", this);

    inputLayout->addWidget(lblInit);
    inputLayout->addWidget(initEdit);
    inputLayout->addWidget(btnLoad);
    inputLayout->addSpacing(15);
    inputLayout->addWidget(lblPush);
    inputLayout->addWidget(pushValueEdit);
    inputLayout->addWidget(btnPush);
    inputLayout->addSpacing(15);
    inputLayout->addWidget(btnPopStack);
    inputLayout->addWidget(btnPopQueue);
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
    std::vector<QuizQuestion> questions = QuizDatabase::getQuestionsFor(QuizDatabase::Topic::StackQueue, UserProfile::instance().getErrorsForTopic("StackQueue"));
    SubQuizWidget* quizWidget = new SubQuizWidget(questions, "StackQueue", this);
    tabWidget->addTab(quizWidget, "巩固练习");
    // 限制只能输入数字和逗号，并加入占位符提示
    QRegularExpression regExp("^[0-9,\\-\\s，]*$");
    QRegularExpressionValidator* listValidator = new QRegularExpressionValidator(regExp, this);
    initEdit->setValidator(listValidator);
    QIntValidator* intValidator = new QIntValidator(0, 9999, this);
    pushValueEdit->setValidator(intValidator);


    connect(btnLoad, &QPushButton::clicked, this, &VisualStackQueue::handleLoad);
    connect(btnPush, &QPushButton::clicked, this, &VisualStackQueue::handlePush);
    connect(btnPopStack, &QPushButton::clicked, this, &VisualStackQueue::handlePopStack);
    connect(btnPopQueue, &QPushButton::clicked, this, &VisualStackQueue::handlePopQueue);

    realData = {1, 2, 3};
    isStackMode = true;
    QStringList initCode = {
        "template <typename T, typename Container = std::deque<T>>",
        "class stack {",
        "protected:",
        "    Container c; // 底层容器 (默认为 deque)",
        "public:",
        "    bool empty() const { return c.empty(); }",
        "    size_t size() const { return c.size(); }",
        "    T& top() { return c.back(); }",
        "    void push(const T& val) {",
        "        c.push_back(val);",
        "    }",
        "    void pop() {",
        "        c.pop_back();",
        "    }",
        "};"
    };
    animSteps.push_back({realData, -1, "底层 Deque 初始化就绪，当前以 stack 适配器进行封装", initCode, -1});
    updateCodeBrowser();
}

// 处理用户输入并初始化底层容器，包含格式和越界校验
void VisualStackQueue::handleLoad() {
    QString text = initEdit->text().trimmed();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "错误", "输入不能为空！");
        return;
    }
    QStringList parts = text.split(',');
    std::deque<int> newData;
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
    if (newData.empty() || newData.size() > 8) {
        QMessageBox::warning(this, "错误", "请提供 1~8 个数字");
        return;
    }
    realData = newData;
    animSteps.clear();
    currentStep = 0;
    
    QStringList initCode;
    if (isStackMode) {
        initCode = {
            "template <typename T, typename Container = std::deque<T>>",
            "class stack {",
            "protected:",
            "    Container c; // 底层容器 (默认为 deque)",
            "public:",
            "    bool empty() const { return c.empty(); }",
            "    size_t size() const { return c.size(); }",
            "    T& top() { return c.back(); }",
            "    void push(const T& val) {",
            "        c.push_back(val);",
            "    }",
            "    void pop() {",
            "        c.pop_back();",
            "    }",
            "};"
        };
    } else {
        initCode = {
            "template <typename T, typename Container = std::deque<T>>",
            "class queue {",
            "protected:",
            "    Container c; // 底层容器 (默认为 deque)",
            "public:",
            "    bool empty() const { return c.empty(); }",
            "    size_t size() const { return c.size(); }",
            "    T& front() { return c.front(); }",
            "    T& back() { return c.back(); }",
            "    void push(const T& val) {",
            "        c.push_back(val);",
            "    }",
            "    void pop() {",
            "        c.pop_front();",
            "    }",
            "};"
        };
    }
    
    animSteps.push_back({realData, -1, "重新加载底层容器", initCode, -1});
    updateCodeBrowser();
    canvas->update();
}

// 模拟压入操作，在 Stack 和 Queue 中均在底层执行 push_back
void VisualStackQueue::handlePush() {
    QString text = pushValueEdit->text().trimmed();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入要入队/入栈的数值！");
        return;
    }
    bool ok;
    int val = text.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "错误", "请输入有效的数字！");
        return;
    }
    if (realData.size() >= 8) {
        QMessageBox::warning(this, "限制", "最多支持 8 个元素！");
        return;
    }

    animSteps.clear();
    currentStep = 0;

    QStringList pushCode;
    int pushLine = 9;
    if (isStackMode) {
        pushCode = {
            "template <typename T, typename Container = std::deque<T>>",
            "class stack {",
            "protected:",
            "    Container c; // 底层容器 (默认为 deque)",
            "public:",
            "    bool empty() const { return c.empty(); }",
            "    size_t size() const { return c.size(); }",
            "    T& top() { return c.back(); }",
            "    void push(const T& val) {",
            "        c.push_back(val); // 压入尾部",
            "    }",
            "    void pop() {",
            "        c.pop_back();",
            "    }",
            "};"
        };
    } else {
        pushCode = {
            "template <typename T, typename Container = std::deque<T>>",
            "class queue {",
            "protected:",
            "    Container c; // 底层容器 (默认为 deque)",
            "public:",
            "    bool empty() const { return c.empty(); }",
            "    size_t size() const { return c.size(); }",
            "    T& front() { return c.front(); }",
            "    T& back() { return c.back(); }",
            "    void push(const T& val) {",
            "        c.push_back(val); // 压入尾部",
            "    }",
            "    void pop() {",
            "        c.pop_front();",
            "    }",
            "};"
        };
        pushLine = 10;
    }
    
    animSteps.push_back({realData, -1, "准备将元素压入底层容器", pushCode, pushLine - 1});
    realData.push_back(val);
    animSteps.push_back({realData, (int)realData.size() - 1, QString("成功在尾端写入新元素 %1").arg(val), pushCode, pushLine});
    
    updateCodeBrowser();
    canvas->update();
}

// 模拟 Stack 弹出操作（后进先出 LIFO），调用底层的 pop_back
void VisualStackQueue::handlePopStack() {
    if (realData.empty()) {
        QMessageBox::warning(this, "错误", "栈已空，无法执行 Pop 操作！");
        return;
    }
    isStackMode = true;
    animSteps.clear();
    currentStep = 0;

    QStringList popStackCode = {
        "template <typename T, typename Container = std::deque<T>>",
        "class stack {",
        "protected:",
        "    Container c; // 底层容器 (默认为 deque)",
        "public:",
        "    bool empty() const { return c.empty(); }",
        "    size_t size() const { return c.size(); }",
        "    T& top() { return c.back(); }",
        "    void push(const T& val) {",
        "        c.push_back(val);",
        "    }",
        "    void pop() {",
        "        c.pop_back(); // 弹出尾部",
        "    }",
        "};"
    };

    int idx = realData.size() - 1;
    animSteps.push_back({realData, idx, "Stack 模式：锁定栈顶元素（最后一个写入的元素）", popStackCode, 11});
    realData.pop_back();
    animSteps.push_back({realData, -1, "Stack 弹出完毕，底层容器执行 pop_back", popStackCode, 12});
    
    updateCodeBrowser();
    canvas->update();
}

// 模拟 Queue 弹出操作（先进先出 FIFO），调用底层的 pop_front
void VisualStackQueue::handlePopQueue() {
    if (realData.empty()) {
        QMessageBox::warning(this, "错误", "队列已空，无法执行 Pop 操作！");
        return;
    }
    isStackMode = false;
    animSteps.clear();
    currentStep = 0;

    QStringList popQueueCode = {
        "template <typename T, typename Container = std::deque<T>>",
        "class queue {",
        "protected:",
        "    Container c; // 底层容器 (默认为 deque)",
        "public:",
        "    bool empty() const { return c.empty(); }",
        "    size_t size() const { return c.size(); }",
        "    T& front() { return c.front(); }",
        "    T& back() { return c.back(); }",
        "    void push(const T& val) {",
        "        c.push_back(val);",
        "    }",
        "    void pop() {",
        "        c.pop_front(); // 弹出头部",
        "    }",
        "};"
    };

    animSteps.push_back({realData, 0, "Queue 模式：锁定队首元素（最早写入的元素）", popQueueCode, 12});
    realData.pop_front();
    animSteps.push_back({realData, -1, "Queue 弹出完毕，底层容器执行 pop_front", popQueueCode, 13});
    
    updateCodeBrowser();
    canvas->update();
}

// 推进动画的当前步骤并刷新代码高亮
void VisualStackQueue::stepForward() {
    if (currentStep < (int)animSteps.size() - 1) {
        currentStep++;
        updateCodeBrowser();
        canvas->update();
    }
}

// 动画后退一步并刷新代码高亮
void VisualStackQueue::stepBackward() {
    if (currentStep > 0) {
        currentStep--;
        updateCodeBrowser();
        canvas->update();
    }
}

// 恢复初始状态
void VisualStackQueue::reset() {
    currentStep = 0;
    updateCodeBrowser();
    canvas->update();
}

// 判断当前动画是否执行到末尾
bool VisualStackQueue::isAtEnd() const {
    return animSteps.empty() || currentStep >= (int)animSteps.size() - 1;
}

// 更新伪代码文本框的显示
void VisualStackQueue::updateCodeBrowser() {
    if (currentStep >= 0 && currentStep < (int)animSteps.size()) {
        const auto& step = animSteps[currentStep];
        codeBrowser->setHtml(VisualBase::renderCodeWithHighlight(step.codeLines, step.activeLine));
    }
}

// 导出当前画布截图
void VisualStackQueue::exportImage() {
    QPixmap pixmap = canvas->grab();
    QString fileName = QFileDialog::getSaveFileName(this, "导出截图", "stackqueue_snapshot.png", "PNG 图像 (*.png);;JPEG 图像 (*.jpg)");
    if (!fileName.isEmpty()) {
        pixmap.save(fileName);
    }
}

// 返回包含该数据结构/算法原理和示例代码的 HTML 字符串
QString VisualStackQueue::getInfo() const {
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
<h2>std::stack / std::queue 容器适配器</h2>
<p>在 STL 的架构中，Stack（栈）和 Queue（队列）被称为<b>容器适配器 (Container Adapters)</b>。这是一种非常优雅的设计模式----它们本身并不直接管理内存，而是"寄生"在别的容器之上（默认是 <code>std::deque</code>）。</p>

<h3>1. 为什么没有迭代器 (Iterators)？</h3>
<p>容器适配器是严格受限的数据结构。栈只允许 LIFO (后进先出)，队列只允许 FIFO (先进先出)。如果提供迭代器，用户就能轻易修改中间的元素，这就破坏了数据结构的设计哲学。</p>

<h3>2. 为什么默认底层容器是 std::deque？</h3>
<ul>
    <li>对比 <code>std::vector</code>：vector 的头部插入和删除是 O(N) 的，无法作为 Queue 的底层。另外 vector 扩容时需要搬运所有元素。</li>
    <li>对比 <code>std::list</code>：list 的两端操作虽然是 O(1)，但节点在物理内存上极度离散，导致 CPU Cache 命中率极低。</li>
    <li><code>std::deque</code> 是分段连续的数组，两端增删均为 O(1)，且内存局部性远好于 list，因此成为了默认的最佳选择。</li>
</ul>

<h3>3. C++ 底层源码提炼 </h3>
<p>观察 <code>std::stack</code> 的源码，你会发现它的内部其实就只是持有了一个 <code>_Sequence c;</code>，并且所有的操作都变成了 <b>内联函数 (inline)</b>，直接转发给底层的 <code>push_back</code> 和 <code>pop_back</code>，没有任何性能损耗：</p>
<pre>%1</pre>
)").arg(VisualBase::highlightCode(R"(template<typename _Tp, typename _Sequence = std::deque<_Tp> >
class stack {
protected:
    // 底层默认是 std::deque
    _Sequence c;

public:
    // 取栈顶元素：底层容器的 back()
    reference top() { return c.back(); }

    // 压栈：直接调用底层容器的 push_back
    void push(const value_type& __x) { c.push_back(__x); }

    // 弹栈：直接调用底层容器的 pop_back
    void pop() { c.pop_back(); }
};)"));
}

// 将画板的重绘委托给外层类的 paintCanvas 方法
void VisualStackQueue::CanvasWidget::paintEvent(QPaintEvent*) {
    p->paintCanvas(this);
}

// 绘制核心逻辑：根据当前步骤(currentStep)画出所有元素
void VisualStackQueue::paintCanvas(QWidget* target) {
    QPainter painter(target);
    painter.setRenderHint(QPainter::Antialiasing);

    if (animSteps.empty()) return;
    const SQAnimStep &step = animSteps[currentStep];

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12));
    painter.drawText(30, 35, "操作进度描述:");
    painter.setPen(QColor("#ffaa00"));
    painter.drawText(30, 58, step.desc);

    int boxW = 60, boxH = 60;
    int spacing = 12;
    int startX = 50;
    int startY = 120;

    for (size_t i = 0; i < step.data.size(); ++i) {
        int x = startX + i * (boxW + spacing);
        QRect rect(x, startY, boxW, boxH);

        if ((int)i == step.highlightIdx) {
            painter.setBrush(QColor("#00d778")); // Action highlight
        } else {
            painter.setBrush(QColor("#0e639c")); // normal blue
        }

        painter.setPen(Qt::white);
        painter.drawRect(rect);
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(rect, Qt::AlignCenter, QString::number(step.data[i]));

        // Lables
        QString lbl = "";
        if (i == 0) lbl = "Front (Queue)";
        if (i == step.data.size() - 1) lbl += (lbl.isEmpty() ? "" : "\n") + QString("Back (Top)");

        painter.setFont(QFont("Arial", 9));
        painter.setPen(QColor("#ffcc00"));
        painter.drawText(QRect(x, startY - 35, boxW, 30), Qt::AlignCenter, lbl);
    }
}