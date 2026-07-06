#include "VisualVector.h"
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


VisualVector::VisualVector(QWidget *parent) : VisualBase(parent), currentStep(0) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet("QTabWidget::pane { border: 1px solid #3c3c3c; background-color: #1e1e1e; }"
                             "QTabBar::tab { background-color: #252526; color: #888; padding: 8px 20px; border: 1px solid #3c3c3c; border-bottom: none; }"
                             "QTabBar::tab:selected { background-color: #1e1e1e; color: #569cd6; font-weight: bold; }");
    mainLayout->addWidget(tabWidget);

    // --- Tab 0: Animation Tab ---
    QWidget* animTab = new QWidget(this);
    QVBoxLayout* animLayout = new QVBoxLayout(animTab);

    // Control Panel
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

    // Initial Data
    QLabel* lblInit = new QLabel("初始数组(以逗号分隔):", this);
    initEdit = new QLineEdit(this);
    initEdit->setPlaceholderText("以逗号分隔，如 1,2,3");
    initEdit->setText("1,2,3");
    initEdit->setFixedWidth(100);
    QPushButton* btnLoad = new QPushButton("加载", this);

    // Operations
    QLabel* lblPush = new QLabel("尾插:", this);
    pushValueEdit = new QLineEdit(this);
    pushValueEdit->setPlaceholderText("值");
    pushValueEdit->setFixedWidth(50);
    QPushButton* btnPush = new QPushButton("push_back", this);

    QLabel* lblInsert = new QLabel("指定插入:", this);
    insertIdxEdit = new QLineEdit(this);
    insertIdxEdit->setPlaceholderText("索引");
    insertIdxEdit->setFixedWidth(50);
    insertValueEdit = new QLineEdit(this);
    insertValueEdit->setPlaceholderText("值");
    insertValueEdit->setFixedWidth(50);
    QPushButton* btnInsert = new QPushButton("insert", this);

    QLabel* lblErase = new QLabel("删除:", this);
    eraseIdxEdit = new QLineEdit(this);
    eraseIdxEdit->setPlaceholderText("索引");
    eraseIdxEdit->setFixedWidth(50);
    QPushButton* btnErase = new QPushButton("erase", this);

    inputLayout->addWidget(lblInit);
    inputLayout->addWidget(initEdit);
    inputLayout->addWidget(btnLoad);
    inputLayout->addSpacing(15);
    inputLayout->addWidget(lblPush);
    inputLayout->addWidget(pushValueEdit);
    inputLayout->addWidget(btnPush);
    inputLayout->addSpacing(15);
    inputLayout->addWidget(lblInsert);
    inputLayout->addWidget(insertIdxEdit);
    inputLayout->addWidget(insertValueEdit);
    inputLayout->addWidget(btnInsert);
    inputLayout->addSpacing(15);
    inputLayout->addWidget(lblErase);
    inputLayout->addWidget(eraseIdxEdit);
    inputLayout->addWidget(btnErase);
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

    // --- Tab 1: Info Tab ---
    QTextBrowser* infoBrowser = new QTextBrowser(this);
    infoBrowser->setStyleSheet("background-color: #1e1e1e; border: none; padding: 20px; font-size: 15px; line-height: 1.5;");
    infoBrowser->setHtml(getInfo());
    // Enable copy and mouse selection explicitly
    infoBrowser->setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    tabWidget->addTab(infoBrowser, "知识卡片");

    // --- Tab 2: Quiz Tab ---
    std::vector<QuizQuestion> questions = QuizDatabase::getQuestionsFor(QuizDatabase::Topic::Vector, UserProfile::instance().getErrorsForTopic("Vector"));
    SubQuizWidget* quizWidget = new SubQuizWidget(questions, "Vector", this);
    tabWidget->addTab(quizWidget, "巩固练习");

    // Connects
    // 限制只能输入数字和逗号，并加入占位符提示
    QRegularExpression regExp("^[0-9,\\-\\s，]*$");
    QRegularExpressionValidator* listValidator = new QRegularExpressionValidator(regExp, this);
    initEdit->setValidator(listValidator);
    QIntValidator* intValidator = new QIntValidator(0, 9999, this);
    pushValueEdit->setValidator(intValidator);
    insertIdxEdit->setValidator(intValidator);
    insertValueEdit->setValidator(intValidator);
    eraseIdxEdit->setValidator(intValidator);

    connect(btnLoad, &QPushButton::clicked, this, &VisualVector::handleLoad);
    connect(btnPush, &QPushButton::clicked, this, &VisualVector::handlePushBack);
    connect(btnInsert, &QPushButton::clicked, this, &VisualVector::handleInsert);
    connect(btnErase, &QPushButton::clicked, this, &VisualVector::handleErase);

    // 初始状态设置
    realData = {1, 2, 3};
    capacity = 4;
    QStringList initCode = {"// Vector 初始就绪状态", "// 可以选择顶部菜单执行操作..."};
    animSteps.push_back({realData, -1, -1, false, capacity, "Vector 就绪，初始状态。当前容量 Capacity=" + QString::number(capacity), initCode, -1});
    updateCodeBrowser();
}

// 处理用户输入并初始化数组，包含严格的错误校验与提示
void VisualVector::handleLoad() {
    QString text = initEdit->text().trimmed();
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
            QMessageBox::warning(this, "格式错误", QString("无法解析元素: \"%1\"，请输入逗号分隔的纯数字！").arg(trimmed));
            return;
        }
        newData.push_back(val);
    }
    if (newData.empty()) {
        QMessageBox::warning(this, "错误", "请输入有效的数字列表！");
        return;
    }
    if (newData.size() > 10) {
        QMessageBox::warning(this, "限制", "最多支持 10 个元素！");
        return;
    }
    realData = newData;
    capacity = realData.size() > 0 ? realData.size() : 1;
    animSteps.clear();
    currentStep = 0;
    QStringList initCode = {"// 成功加载初始数据"};
    animSteps.push_back({realData, -1, -1, false, capacity, "重置 Vector 数据成功。", initCode, -1});
    updateCodeBrowser();
    canvas->update();
}

// 模拟 std::vector::push_back 操作的步骤录制
void VisualVector::handlePushBack() {
    QString text = pushValueEdit->text().trimmed();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入要追加的数字值！");
        return;
    }
    bool ok;
    int val = text.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "错误", "请输入有效的数字！");
        return;
    }
    if (realData.size() >= 10) {
        QMessageBox::warning(this, "限制", "最多支持 10 个元素！");
        return;
    }
    
    animSteps.clear();
    currentStep = 0;

    QStringList pushBackCode = {
        "void push_back(const T& val) {",
        "    if (size == capacity) {",
        "        allocate_double_capacity(); // 触发 2 倍扩容",
        "    }",
        "    data[size++] = val; // 在尾端写入新元素",
        "}"
    };
    
    if ((int)realData.size() == capacity) {
        int oldCap = capacity;
        capacity = oldCap == 0 ? 1 : oldCap * 2;
        animSteps.push_back({realData, -1, -1, true, capacity, QString("内存不足 (Size=%1, Cap=%2)！触发 2 倍扩容。新容量: %3").arg(realData.size()).arg(oldCap).arg(capacity), pushBackCode, 2});
    } else {
        animSteps.push_back({realData, -1, -1, false, capacity, "检测到空间充足，无需触发扩容。", pushBackCode, 1});
    }
    
    realData.push_back(val);
    animSteps.push_back({realData, (int)realData.size() - 1, -1, false, capacity, QString("在尾端追加元素: %1").arg(val), pushBackCode, 4});
    
    updateCodeBrowser();
    canvas->update();
}

// 模拟 std::vector::insert 操作的步骤录制与平移动画
void VisualVector::handleInsert() {
    QString idxText = insertIdxEdit->text().trimmed();
    QString valText = insertValueEdit->text().trimmed();
    if (idxText.isEmpty() || valText.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入插入索引和数字值！");
        return;
    }
    bool ok1, ok2;
    int idx = idxText.toInt(&ok1);
    int val = valText.toInt(&ok2);
    if (!ok1 || !ok2) {
        QMessageBox::warning(this, "错误", "请输入有效的数字！");
        return;
    }
    if (idx < 0 || idx > (int)realData.size()) {
        QMessageBox::warning(this, "错误", QString("插入索引越界！当前有效插入范围 [0, %1]").arg(realData.size()));
        return;
    }
    if (realData.size() >= 10) {
        QMessageBox::warning(this, "限制", "最多支持 10 个元素！");
        return;
    }

    animSteps.clear();
    currentStep = 0;

    QStringList insertCode = {
        "iterator insert(iterator pos, const T& val) {",
        "    if (size == capacity) {",
        "        allocate_double_capacity(); // 触发扩容",
        "    }",
        "    // 将插入点后的元素依次向后平移",
        "    for (int i = size; i > idx; --i) {",
        "        data[i] = data[i - 1];",
        "    }",
        "    data[idx] = val; // 在空出的位置放入新元素",
        "    return pos;",
        "}"
    };

    if ((int)realData.size() == capacity) {
        capacity = capacity == 0 ? 1 : capacity * 2;
        animSteps.push_back({realData, -1, -1, true, capacity, QString("容量已满，触发扩容。新容量: %1").arg(capacity), insertCode, 2});
    } else {
        animSteps.push_back({realData, -1, -1, false, capacity, "空间充足，准备执行插入...", insertCode, 1});
    }

    realData.push_back(0); // 占位符
    animSteps.push_back({realData, -1, -1, false, capacity, "在尾端追加占位，准备后移元素...", insertCode, 5});

    for (int i = realData.size() - 1; i > idx; --i) {
        realData[i] = realData[i - 1];
        animSteps.push_back({realData, i, i - 1, false, capacity, QString("元素 %1 向后平移").arg(realData[i]), insertCode, 6});
    }

    realData[idx] = val;
    animSteps.push_back({realData, idx, -1, false, capacity, QString("在空出的位置 [%1] 放入新元素 %2").arg(idx).arg(val), insertCode, 8});
    
    updateCodeBrowser();
    canvas->update();
}

// 模拟 std::vector::erase 操作的步骤录制与前移覆盖动画
void VisualVector::handleErase() {
    QString idxText = eraseIdxEdit->text().trimmed();
    if (idxText.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入要删除的索引！");
        return;
    }
    bool ok;
    int idx = idxText.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "错误", "请输入有效的索引！");
        return;
    }
    if (idx < 0 || idx >= (int)realData.size()) {
        QMessageBox::warning(this, "错误", QString("删除索引越界！有效范围为 [0, %1]").arg((int)realData.size() - 1));
        return;
    }

    animSteps.clear();
    currentStep = 0;

    QStringList eraseCode = {
        "iterator erase(iterator pos) {",
        "    // 将被删位置后的元素向前覆盖",
        "    for (int i = idx; i < size - 1; ++i) {",
        "        data[i] = data[i + 1];",
        "    }",
        "    --size; // 更新 size，但物理容量保持不变",
        "    return pos;",
        "}"
    };

    animSteps.push_back({realData, idx, -1, false, capacity, QString("准备删除位置 [%1] 的元素 %2，为了保持内存连续性，其后元素将向前平移。").arg(idx).arg(realData[idx]), eraseCode, 2});

    for (int i = idx; i < (int)realData.size() - 1; ++i) {
        realData[i] = realData[i + 1];
        animSteps.push_back({realData, i, i + 1, false, capacity, QString("将后面元素向前覆盖覆盖 [%1] <- [%2]").arg(i).arg(i + 1), eraseCode, 3});
    }

    realData.pop_back();
    animSteps.push_back({realData, -1, -1, false, capacity, "删除完成，移去尾端空余，物理容量 Capacity 保持不变。", eraseCode, 5});
    
    updateCodeBrowser();
    canvas->update();
}

// 推进动画的当前步骤并刷新代码高亮
void VisualVector::stepForward() {
    if (currentStep < (int)animSteps.size() - 1) {
        currentStep++;
        updateCodeBrowser();
        canvas->update();
    }
}

// 动画后退一步并刷新代码高亮
void VisualVector::stepBackward() {
    if (currentStep > 0) {
        currentStep--;
        updateCodeBrowser();
        canvas->update();
    }
}

// 恢复初始状态
void VisualVector::reset() {
    currentStep = 0;
    updateCodeBrowser();
    canvas->update();
}

// 检查是否到达最后一步
bool VisualVector::isAtEnd() const {
    return animSteps.empty() || currentStep >= (int)animSteps.size() - 1;
}

// 更新伪代码文本框的显示
void VisualVector::updateCodeBrowser() {
    if (currentStep >= 0 && currentStep < (int)animSteps.size()) {
        const auto& step = animSteps[currentStep];
        codeBrowser->setHtml(VisualBase::renderCodeWithHighlight(step.codeLines, step.activeLine));
    }
}

// 导出当前画布截图
void VisualVector::exportImage() {
    QPixmap pixmap = canvas->grab();
    QString fileName = QFileDialog::getSaveFileName(this, "导出截图", "vector_snapshot.png", "PNG 图像 (*.png);;JPEG 图像 (*.jpg)");
    if (!fileName.isEmpty()) {
        pixmap.save(fileName);
    }
}

// 返回包含该数据结构/算法原理和示例代码的 HTML 字符串
QString VisualVector::getInfo() const {
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
<h2>std::vector 动态连续数组</h2>
<p><b>std::vector</b> 是 C++ 标准模板库 (STL) 中最为核心的顺序容器。它在底层维护了一段<b>连续的物理内存</b>空间，并且能够根据元素的增加动态地改变自身的容量（Capacity）。</p>

<h3>1. 迭代器 (Iterators) 与寻址</h3>
<p>因为 vector 的内存是连续的，它的迭代器本质上就是<b>裸指针 (Raw Pointers)</b> 或其简单封装。它提供的是 <b>Random Access Iterator (随机访问迭代器)</b>。</p>
<ul>
    <li>支持指针算术运算：<code>it + n</code>, <code>it - n</code>。</li>
    <li>支持直接比大小：<code>it1 &lt; it2</code>。</li>
    <li>可以在 <b>O(1)</b> 时间内通过 <code>v[i]</code> 访问任意元素。</li>
</ul>

<h3>2. 动态扩容机制 (Dynamic Reallocation)</h3>
<p>当通过 <code>push_back</code> 插入新元素，而当前 <code>size == capacity</code> 时，vector 会触发扩容。常见的扩容因子是 <b>2 倍 (GCC)</b> 或 <b>1.5 倍 (MSVC)</b>。扩容代价极高，需要重新分配内存，并将旧元素拷贝（或移动）到新内存中。</p>

<h3>3. C++ 底层源码提炼 </h3>
<p>在 <code>push_back</code> 时，若空间不足，则调用 <code>_M_realloc_insert</code>：</p>
<pre>%1</pre>

<h3>4. 性能特征分析</h3>
<table>
  <tr><th>操作</th><th>时间复杂度</th><th>说明</th></tr>
  <tr><td>下标访问 <code>[i]</code></td><td><b>O(1)</b></td><td>内存连续，直接基址偏移计算。</td></tr>
  <tr><td>尾部插入 <code>push_back</code></td><td><b>摊还 O(1)</b></td><td>偶尔触发 O(N) 扩容，平摊下来为 O(1)。</td></tr>
  <tr><td>中间插入/删除</td><td><b>O(N)</b></td><td>由于需保持连续，必须整体平移其后所有的元素。</td></tr>
</table>
)").arg(VisualBase::highlightCode(R"(void _M_realloc_insert(iterator __position, const _Tp& __x) {
    const size_type __old_size = size();
    // 容量翻倍逻辑：若为0则申请1，否则申请两倍
    const size_type __len = __old_size != 0 ? 2 * __old_size : 1;
    
    // 1. 申请一块大小为 __len 的新内存
    pointer __new_start = this->_M_allocate(__len);
    pointer __new_finish = __new_start;
    
    // 2. 将旧内存中的数据拷贝/移动到新内存
    __new_finish = std::__uninitialized_move_if_noexcept_a(
        this->_M_impl._M_start, __position, __new_start, _M_get_Tp_allocator());
        
    // 3. 在新位置构造新元素
    _Alloc_traits::construct(this->_M_impl, __new_finish, __x);
    ++__new_finish;
    
    // 4. 释放旧内存
    _M_deallocate(this->_M_impl._M_start, __old_size);
    
    // 5. 更新内部指针
    this->_M_impl._M_start = __new_start;
    this->_M_impl._M_finish = __new_finish;
    this->_M_impl._M_end_of_storage = __new_start + __len;
})"));
}

// 将画板的重绘委托给外层类的 paintCanvas 方法
void VisualVector::CanvasWidget::paintEvent(QPaintEvent*) {
    p->paintCanvas(this);
}

// 绘制核心逻辑：根据当前步骤(currentStep)画出所有元素
void VisualVector::paintCanvas(QWidget* target) {
    QPainter painter(target);
    painter.setRenderHint(QPainter::Antialiasing);

    if (animSteps.empty()) return;
    const VectorStep &step = animSteps[currentStep];

    // Info
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12));
    painter.drawText(30, 35, "操作进度描述:");
    painter.setPen(QColor("#ffaa00"));
    painter.drawText(30, 58, step.desc);

    int boxW = 60, boxH = 60;
    int spacing = 12;
    int startX = 50;
    int startY = 120;

    // Draw capacity boxes
    for (int i = 0; i < step.capacity; ++i) {
        int x = startX + i * (boxW + spacing);
        QRect rect(x, startY, boxW, boxH);

        if (i < (int)step.data.size()) {
            if (i == step.highlightIdx || i == step.secondaryIdx) {
                painter.setBrush(step.isReallocating ? QColor("#e51400") : QColor("#ffaa00"));
            } else {
                painter.setBrush(QColor("#0e639c")); // normal blue
            }
            painter.setPen(Qt::white);
            painter.drawRect(rect);
            painter.setFont(QFont("Arial", 14, QFont::Bold));
            painter.drawText(rect, Qt::AlignCenter, QString::number(step.data[i]));
        } else {
            painter.setBrush(QColor("#252526")); // empty slot
            painter.setPen(QPen(QColor("#555555"), 2, Qt::DashLine));
            painter.drawRect(rect);
        }

        painter.setFont(QFont("Arial", 9));
        painter.setPen(QColor("#888888"));
        painter.drawText(QRect(x, startY + boxH + 5, boxW, 20), Qt::AlignCenter, QString("[%1]").arg(i));
    }
}