#include "VisualList.h"
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

VisualList::VisualList(QWidget *parent) : VisualBase(parent), currentStep(0) {
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

    QLabel* lblInit = new QLabel("初始链表(以逗号分隔):", this);
    initEdit = new QLineEdit(this);
    initEdit->setPlaceholderText("以逗号分隔，如 1,2,3");
    initEdit->setText("1,2,3");
    initEdit->setFixedWidth(100);
    QPushButton* btnLoad = new QPushButton("加载", this);

    QLabel* lblPush = new QLabel("追加:", this);
    pushValueEdit = new QLineEdit(this);
    pushValueEdit->setPlaceholderText("值");
    pushValueEdit->setFixedWidth(50);
    QPushButton* btnPush = new QPushButton("push_back", this);

    QLabel* lblInsert = new QLabel("插入:", this);
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
    infoBrowser->setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    tabWidget->addTab(infoBrowser, "知识卡片");

    // --- Tab 2: Quiz Tab ---
    std::vector<QuizQuestion> questions = QuizDatabase::getQuestionsFor(QuizDatabase::Topic::List, UserProfile::instance().getErrorsForTopic("List"));
    SubQuizWidget* quizWidget = new SubQuizWidget(questions, "List", this);
    tabWidget->addTab(quizWidget, "巩固练习");
    // 限制只能输入数字和逗号，并加入占位符提示
    QRegularExpression regExp("^[0-9,\\-\\s，]*$");
    QRegularExpressionValidator* listValidator = new QRegularExpressionValidator(regExp, this);
    initEdit->setValidator(listValidator);
    QIntValidator* intValidator = new QIntValidator(0, 9999, this);
    pushValueEdit->setValidator(intValidator);
    insertIdxEdit->setValidator(intValidator);
    insertValueEdit->setValidator(intValidator);
    eraseIdxEdit->setValidator(intValidator);


    connect(btnLoad, &QPushButton::clicked, this, &VisualList::handleLoad);
    connect(btnPush, &QPushButton::clicked, this, &VisualList::handlePushBack);
    connect(btnInsert, &QPushButton::clicked, this, &VisualList::handleInsert);
    connect(btnErase, &QPushButton::clicked, this, &VisualList::handleErase);

    realData = {1, 2, 3};
    QStringList initCode = {"// std::list 初始就绪状态", "// 可以选择顶部菜单执行操作..."};
    animSteps.push_back({realData, -1, -1, "双向链表已就绪", initCode, -1});
    updateCodeBrowser();
}

// 处理用户输入并初始化链表，包含错误拦截校验
void VisualList::handleLoad() {
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
            QMessageBox::warning(this, "格式错误", QString("无法解析元素: \"%1\"，请输入纯数字列表！").arg(trimmed));
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
    QStringList initCode = {"// 成功加载初始数据"};
    animSteps.push_back({realData, -1, -1, "重新加载链表", initCode, -1});
    updateCodeBrowser();
    canvas->update();
}

// 模拟 std::list::push_back 并展示双向链接步骤
void VisualList::handlePushBack() {
    QString text = pushValueEdit->text().trimmed();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入追加的值！");
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

    QStringList listPushBackCode = {
        "void push_back(const T& val) {",
        "    // 1. 创建独立新节点",
        "    Node* new_node = new Node(val);",
        "    Node* old_tail = sentinel->prev; // 获取当前尾节点",
        "    // 2. 将新节点链接到末尾",
        "    old_tail->next = new_node;",
        "    new_node->prev = old_tail;",
        "    new_node->next = sentinel;",
        "    sentinel->prev = new_node;",
        "}"
    };

    // Step 1: Create node
    animSteps.push_back({realData, -1, -1, QString("第一步：分配内存创建新节点并赋初始值 %1").arg(val), listPushBackCode, 2});

    // Step 2: Insert data
    std::vector<int> nextData = realData;
    nextData.push_back(val);

    // Step 3: Link tail pointers
    int lastIdx = nextData.size() - 1;
    animSteps.push_back({nextData, lastIdx, lastIdx > 0 ? lastIdx - 1 : -1, "第二步：修改原尾节点的后继指向新节点", listPushBackCode, 5});
    animSteps.push_back({nextData, lastIdx, lastIdx > 0 ? lastIdx - 1 : -1, "第三步：修改新节点的前驱指向原尾节点", listPushBackCode, 6});

    realData = nextData;
    animSteps.push_back({realData, (int)realData.size() - 1, -1, QString("第四步：新节点 (%1) 成功追加，尾插操作完成。时间复杂度为 O(1)").arg(val), listPushBackCode, 8});
    
    updateCodeBrowser();
    canvas->update();
}

// 模拟 std::list::insert 并展示 O(N) 遍历寻址 + O(1) 插入
void VisualList::handleInsert() {
    QString idxText = insertIdxEdit->text().trimmed();
    QString valText = insertValueEdit->text().trimmed();
    if (idxText.isEmpty() || valText.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入要插入的索引和数字值！");
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
        QMessageBox::warning(this, "错误", QString("插入位置 %1 越界！有效范围为 [0, %2]").arg(idx).arg(realData.size()));
        return;
    }
    if (realData.size() >= 8) {
        QMessageBox::warning(this, "限制", "最多支持 8 个元素！");
        return;
    }

    animSteps.clear();
    currentStep = 0;

    QStringList listInsertCode = {
        "iterator insert(iterator pos, const T& val) {",
        "    // 1. 创建独立的新节点",
        "    Node* new_node = new Node(val);",
        "    // 2. O(N) 寻址：沿当前链表查找位置 pos",
        "    Node* curr = head;",
        "    for (int i = 0; i < pos_idx; ++i) curr = curr->next;",
        "    // 3. O(1) 指针断开与重新拼合",
        "    new_node->next = curr;      // 新节点后继指向当前节点",
        "    new_node->prev = curr->prev; // 新节点前驱指向前一节点",
        "    curr->prev->next = new_node; // 前一节点后继指向新节点",
        "    curr->prev = new_node;       // 当前节点前驱指向新节点",
        "    return iterator(new_node);",
        "}"
    };

    // Step 1: 创建新节点 (activeLine = 2)
    animSteps.push_back({realData, -1, -1, QString("第一步：分配内存创建新节点并赋初始值 %1").arg(val), listInsertCode, 2});

    // Step 2: 寻址 (activeLine = 5)
    if (idx == 0) {
        animSteps.push_back({realData, 0, -1, "第二步：寻址完成，当前插入位置为头节点 (index 0)", listInsertCode, 5});
    } else {
        for (int i = 0; i < idx; ++i) {
            animSteps.push_back({realData, i, -1, QString("第二步：O(N) 寻址中，当前访问节点索引 %1 ...").arg(i), listInsertCode, 5});
        }
        animSteps.push_back({realData, idx - 1, idx == (int)realData.size() ? -1 : idx, QString("第二步：寻址完成，定位到插入点前驱节点索引 %1").arg(idx - 1), listInsertCode, 5});
    }

    // 执行数据插入
    std::vector<int> nextData = realData;
    nextData.insert(nextData.begin() + idx, val);

    // Step 3: 指针重新拼合步骤 (activeLine = 7, 8, 9, 10)
    animSteps.push_back({nextData, idx, idx + 1 < (int)nextData.size() ? idx + 1 : -1, "第三步：新节点后继 next 指向当前节点", listInsertCode, 7});
    animSteps.push_back({nextData, idx, idx > 0 ? idx - 1 : -1, "第四步：新节点前驱 prev 指向原前驱节点", listInsertCode, 8});
    animSteps.push_back({nextData, idx, idx > 0 ? idx - 1 : -1, "第五步：修改前驱节点的后继 next 指向新节点", listInsertCode, 9});
    animSteps.push_back({nextData, idx, idx + 1 < (int)nextData.size() ? idx + 1 : -1, "第六步：修改当前节点的前驱 prev 指向新节点", listInsertCode, 10});

    // 最终插入完成
    realData = nextData;
    animSteps.push_back({realData, idx, -1, QString("插入操作全部完成！新元素已成功插入在第 %1 个位置。").arg(idx), listInsertCode, 11});
    
    updateCodeBrowser();
    canvas->update();
}

// 模拟 std::list::erase 并展示 O(N) 遍历寻址 + O(1) 指针断开释放
void VisualList::handleErase() {
    QString idxText = eraseIdxEdit->text().trimmed();
    if (idxText.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入删除索引！");
        return;
    }
    bool ok;
    int idx = idxText.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "错误", "请输入有效的索引数字！");
        return;
    }
    if (idx < 0 || idx >= (int)realData.size()) {
        QMessageBox::warning(this, "错误", QString("删除位置 %1 越界！有效范围为 [0, %2]").arg(idx).arg((int)realData.size() - 1));
        return;
    }

    animSteps.clear();
    currentStep = 0;

    QStringList listEraseCode = {
        "iterator erase(iterator pos) {",
        "    // 1. O(N) 寻址：沿当前链表查找待删位置 pos",
        "    Node* curr = head;",
        "    for (int i = 0; i < pos_idx; ++i) curr = curr->next;",
        "    // 2. O(1) 断开前后链接并释放内存",
        "    curr->prev->next = curr->next; // 前驱节点的后继指向后继节点",
        "    curr->next->prev = curr->prev; // 后继节点的前驱指向前驱节点",
        "    delete curr;                 // 释放待删节点内存",
        "    return next_pos;",
        "}"
    };

    // Step 1: 寻址 (activeLine = 3)
    if (idx == 0) {
        animSteps.push_back({realData, 0, -1, "第一步：寻址完成，当前待删除目标为头节点 (index 0)", listEraseCode, 3});
    } else {
        for (int i = 0; i < idx; ++i) {
            animSteps.push_back({realData, i, -1, QString("第一步：O(N) 寻址中，当前访问节点索引 %1 ...").arg(i), listEraseCode, 3});
        }
        animSteps.push_back({realData, idx, -1, QString("第一步：寻址完成，定位到待删目标节点索引 %1").arg(idx), listEraseCode, 3});
    }

    // Step 2: 重新连线 (activeLine = 5)
    animSteps.push_back({realData, idx, idx > 0 ? idx - 1 : -1, "第二步：修改前驱节点的后继指向当前节点的后继", listEraseCode, 5});
    animSteps.push_back({realData, idx, idx + 1 < (int)realData.size() ? idx + 1 : -1, "第三步：修改后继节点的前驱指向当前节点的前驱", listEraseCode, 6});

    // 执行删除
    std::vector<int> nextData = realData;
    nextData.erase(nextData.begin() + idx);

    // activeLine = 7 (delete curr)
    animSteps.push_back({nextData, -1, -1, "第四步：执行 delete 释放当前节点的内存空间", listEraseCode, 7});

    // 最终完成
    realData = nextData;
    animSteps.push_back({realData, -1, -1, QString("删除操作完成，元素成功移除！"), listEraseCode, 8});
    
    updateCodeBrowser();
    canvas->update();
}

// 推进动画的当前步骤并刷新代码高亮
void VisualList::stepForward() {
    if (currentStep < (int)animSteps.size() - 1) {
        currentStep++;
        updateCodeBrowser();
        canvas->update();
    }
}

// 动画后退一步并刷新代码高亮
void VisualList::stepBackward() {
    if (currentStep > 0) {
        currentStep--;
        updateCodeBrowser();
        canvas->update();
    }
}

// 恢复初始状态
void VisualList::reset() {
    currentStep = 0;
    updateCodeBrowser();
    canvas->update();
}

// 判断当前动画是否执行到末尾
bool VisualList::isAtEnd() const {
    return animSteps.empty() || currentStep >= (int)animSteps.size() - 1;
}

// 更新伪代码文本框的显示
void VisualList::updateCodeBrowser() {
    if (currentStep >= 0 && currentStep < (int)animSteps.size()) {
        const auto& step = animSteps[currentStep];
        codeBrowser->setHtml(VisualBase::renderCodeWithHighlight(step.codeLines, step.activeLine));
    }
}

// 导出当前画布截图
void VisualList::exportImage() {
    QPixmap pixmap = canvas->grab();
    QString fileName = QFileDialog::getSaveFileName(this, "导出截图", "list_snapshot.png", "PNG 图像 (*.png);;JPEG 图像 (*.jpg)");
    if (!fileName.isEmpty()) {
        pixmap.save(fileName);
    }
}

// 返回包含该数据结构/算法原理和示例代码的 HTML 字符串
QString VisualList::getInfo() const {
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
<h2>std::list 双向循环链表</h2>
<p><b>std::list</b> 是一种物理内存不连续的容器，其本质是由 <code>_List_node</code> 组成的双向循环链表。每次插入/删除只需更改局部的指针指向，极大地提升了修改效率，但牺牲了随机访问能力。</p>

<h3>1. 迭代器 (Iterators)</h3>
<p>list 提供的是 <b>Bidirectional Iterator (双向迭代器)</b>：</p>
<ul>
    <li>支持 <code>++</code> (向后遍历) 和 <code>--</code> (向前遍历)。</li>
    <li><b>不支持</b> <code>it + n</code>，要走到第 N 个元素只能执行 N 次 <code>++it</code>。</li>
    <li>插入和删除<b>绝对不会导致原有的迭代器失效</b>（除了被删除的那个）。</li>
</ul>

<h3>2. C++ 底层源码提炼 </h3>
<p>链表节点的核心结构 <code>_List_node_base</code> 及 <code>insert</code> 的底层实现：</p>
<pre>%1</pre>

<h3>3. 性能特征分析</h3>
<table>
  <tr><th>操作</th><th>时间复杂度</th><th>说明</th></tr>
  <tr><td>下标访问 <code>[i]</code></td><td><b>O(N)</b></td><td>不支持下标，必须依靠 <code>std::advance</code> 线性遍历。</td></tr>
  <tr><td>头尾插入 <code>push_back</code></td><td><b>O(1)</b></td><td>直接操作指针。</td></tr>
  <tr><td>中间插入/删除</td><td><b>O(1)</b> (前提是已经持有迭代器)</td><td>只需 4 次指针赋值，极快。</td></tr>
</table>
)").arg(VisualBase::highlightCode(R"(struct _List_node_base {
    _List_node_base* _M_next;
    _List_node_base* _M_prev;
};

// list::insert 最终调用的底层 hook 函数
void _List_node_base::_M_hook(_List_node_base* const __position) {
    // 经典的双向链表 4 步指针断开重连逻辑
    this->_M_next = __position;             // (1) 新节点 next 指向 position
    this->_M_prev = __position->_M_prev;    // (2) 新节点 prev 指向 position 前驱
    __position->_M_prev->_M_next = this;    // (3) 前驱的 next 指向新节点
    __position->_M_prev = this;             // (4) position 的 prev 指向新节点
})"));
}

// 将画板的重绘委托给外层类的 paintCanvas 方法
void VisualList::CanvasWidget::paintEvent(QPaintEvent*) {
    p->paintCanvas(this);
}

// 绘制核心逻辑：根据当前步骤(currentStep)画出所有元素
void VisualList::paintCanvas(QWidget* target) {
    QPainter painter(target);
    painter.setRenderHint(QPainter::Antialiasing);

    if (animSteps.empty()) return;
    const ListAnimStep &step = animSteps[currentStep];

    // 1. Draw status/desc text
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 11));
    painter.drawText(30, 35, "操作进度描述:");
    
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.setPen(QColor("#00d778"));
    painter.drawText(30, 58, step.desc);

    int nodeW = 90;
    int nodeH = 40;
    int spacing = 50;
    int startX = 40;
    int startY = 120;

    for (size_t i = 0; i < step.data.size(); ++i) {
        int x = startX + i * (nodeW + spacing);
        QRect nodeRect(x, startY, nodeW, nodeH);

        // Determine node background color based on highlight status
        QColor borderColor = QColor("#0e639c");
        QColor fillBgColor = QColor("#1e1e1e");
        if (i == (size_t)step.highlightIdx) {
            borderColor = QColor("#00d778"); // Active node (emerald green)
            fillBgColor = QColor("#1b4d3e");
        } else if (i == (size_t)step.secondaryIdx) {
            borderColor = QColor("#ffaa00"); // Secondary highlight (gold)
            fillBgColor = QColor("#4d3a1b");
        }

        // Draw node container body
        painter.setBrush(fillBgColor);
        painter.setPen(QPen(borderColor, 2));
        painter.drawRoundedRect(nodeRect, 4, 4);

        // Draw dividing lines for Prev, Data, Next fields
        painter.setPen(QPen(borderColor, 1, Qt::SolidLine));
        painter.drawLine(x + 20, startY, x + 20, startY + nodeH);
        painter.drawLine(x + 70, startY, x + 70, startY + nodeH);

        // Draw fields labels / values
        painter.setFont(QFont("Consolas", 9));
        
        // Prev pointer field
        if (i == 0) {
            // Null pointer diagonal slash
            painter.setPen(QColor("#ff5c5c"));
            painter.drawLine(x + 2, startY + nodeH - 2, x + 18, startY + 2);
            painter.drawText(x + 3, startY + 24, "X");
        } else {
            // Prev pointer dot
            painter.setBrush(QColor("#888888"));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(x + 7, startY + nodeH / 2 - 3, 6, 6);
        }

        // Next pointer field
        if (i == step.data.size() - 1) {
            // Null pointer diagonal slash
            painter.setPen(QColor("#ff5c5c"));
            painter.drawLine(x + 72, startY + nodeH - 2, x + 88, startY + 2);
            painter.drawText(x + 73, startY + 24, "X");
        } else {
            // Next pointer dot
            painter.setBrush(QColor("#888888"));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(x + 77, startY + nodeH / 2 - 3, 6, 6);
        }

        // Data field value
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.setPen(Qt::white);
        QRect dataRect(x + 20, startY, 50, nodeH);
        painter.drawText(dataRect, Qt::AlignCenter, QString::number(step.data[i]));

        // Draw double arrows to the next node
        if (i < step.data.size() - 1) {
            int nextX = x + nodeW + spacing;

            painter.setPen(QPen(QColor("#888888"), 2));
            painter.setBrush(QColor("#888888"));

            // 1. Next pointer arrow (Forward pointer from next-field of i to prev-field of i+1)
            int fStartX = x + 80;
            int fStartY = startY + nodeH / 2 - 4;
            int fEndX = nextX + 10;
            int fEndY = startY + nodeH / 2 - 4;
            painter.drawLine(fStartX, fStartY, fEndX, fEndY);
            // Draw forward arrow head
            QPolygon fArrow;
            fArrow << QPoint(fEndX, fEndY) 
                   << QPoint(fEndX - 6, fEndY - 4) 
                   << QPoint(fEndX - 6, fEndY + 4);
            painter.drawPolygon(fArrow);

            // 2. Prev pointer arrow (Backward pointer from prev-field of i+1 to next-field of i)
            int bStartX = nextX + 10;
            int bStartY = startY + nodeH / 2 + 4;
            int bEndX = x + 80;
            int bEndY = startY + nodeH / 2 + 4;
            painter.drawLine(bStartX, bStartY, bEndX, bEndY);
            // Draw backward arrow head
            QPolygon bArrow;
            bArrow << QPoint(bEndX, bEndY) 
                   << QPoint(bEndX + 6, bEndY - 4) 
                   << QPoint(bEndX + 6, bEndY + 4);
            painter.drawPolygon(bArrow);
        }
    }
}