 #include "VisualRemove.h"
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
 VisualRemove::VisualRemove(QWidget *parent) : VisualBase(parent), currentStep(0) {
     QVBoxLayout* mainLayout = new QVBoxLayout(this);
     mainLayout->setContentsMargins(0, 0, 0, 0);
 
     tabWidget = new QTabWidget(this);
     tabWidget->setStyleSheet("QTabWidget::pane { border: 1px solid #3c3c3c; background-color: #1e1e1e; }"
                              "QTabBar::tab { background-color: #252526; color: #888; padding: 8px 20px; border: 1px solid #3c3c3c; border-bottom: none; }"
                              "QTabBar::tab:selected { background-color: #1e1e1e; color: #569cd6; font-weight: bold; }");
     mainLayout->addWidget(tabWidget);
 
     // --- Tab 0: 动画演示 ---
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
 
     QLabel* lblInit = new QLabel("初始数组(逗号分隔):", this);
     valueEdit = new QLineEdit(this);
     valueEdit->setPlaceholderText("逗号分隔，如 1,2,3");
     valueEdit->setText("2,1,3,3,2,5,1,2");
     valueEdit->setFixedWidth(200);
 
     QLabel* lblRemove = new QLabel("移除值:", this);
     removeValueEdit = new QLineEdit(this);
     removeValueEdit->setPlaceholderText("值");
     removeValueEdit->setText("2");
     removeValueEdit->setFixedWidth(50);
 
     QPushButton* btnLoad = new QPushButton("加载并移除", this);
 
     inputLayout->addWidget(lblInit);
     inputLayout->addWidget(valueEdit);
     inputLayout->addSpacing(10);
     inputLayout->addWidget(lblRemove);
     inputLayout->addWidget(removeValueEdit);
     inputLayout->addWidget(btnLoad);
     inputLayout->addStretch();
 
     animLayout->addWidget(inputPanel);
 
     // 分割器：容纳画板与右侧的高亮代码面板
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
 
     // --- Tab 1: 知识卡片 ---
     QTextBrowser* infoBrowser = new QTextBrowser(this);
     infoBrowser->setStyleSheet("background-color: #1e1e1e; border: none; padding: 20px; font-size: 15px; line-height: 1.5;");
     infoBrowser->setHtml(getInfo());
     infoBrowser->setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
     tabWidget->addTab(infoBrowser, "知识卡片");
 
     // --- Tab 2: 巩固练习 ---
     std::vector<QuizQuestion> questions = QuizDatabase::getQuestionsFor(QuizDatabase::Topic::Remove, UserProfile::instance().getErrorsForTopic("Remove"));
     SubQuizWidget* quizWidget = new SubQuizWidget(questions, "Remove", this);
     tabWidget->addTab(quizWidget, "巩固练习");
 
     // 限制输入为数字和逗号
     QRegularExpression regExp("^[0-9,\\-\\s]*$");
     valueEdit->setValidator(new QRegularExpressionValidator(regExp, this));
     QIntValidator* intValidator = new QIntValidator(-999, 9999, this);
     removeValueEdit->setValidator(intValidator);
 
     connect(btnLoad, &QPushButton::clicked, this, &VisualRemove::handleLoad);
 
     // 初始状态
     realData = {2, 1, 3, 3, 2, 5, 1, 2};
     removeValue = 2;
     QStringList initCode = {"// std::remove 快慢指针移除元素", "// 可以点击按钮查看动画过程"};
     animSteps.push_back({realData, -1, -1, removeValue, "就绪。点击加载并移除开始演示。", initCode, -1});
     updateCodeBrowser();
 }
 
 // 处理用户输入并重新加载数组，包含格式校验
 void VisualRemove::handleLoad() {
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
 
     // 获取要移除的值
     QString removeText = removeValueEdit->text().trimmed();
     if (removeText.isEmpty()) {
         QMessageBox::warning(this, "提示", "请输入要移除的目标值！");
         return;
     }
     bool ok;
     int val = removeText.toInt(&ok);
     if (!ok) {
         QMessageBox::warning(this, "格式错误", "请输入有效的目标值！");
         return;
     }
 
     realData = newData;
     removeValue = val;
     animSteps.clear();
     currentStep = 0;
 
     QStringList initCode = {"// 数组重新加载完成"};
     animSteps.push_back({realData, -1, -1, removeValue, QString("加载完成，准备移除值 %1").arg(removeValue), initCode, -1});
     generateSteps();
 
     updateCodeBrowser();
     canvas->update();
 }
 
 // 生成移除算法的动画步骤
 void VisualRemove::generateSteps() {
     std::vector<int> arr = realData;
     if (arr.empty()) return;
 
     // std::remove 的伪代码
     QStringList removeCode = {
         "template<typename ForwardIterator, typename T>",
         "ForwardIterator remove(ForwardIterator first, ForwardIterator last, const T& val) {",
         "    ForwardIterator dest = first;  // 写指针(慢指针)",
         "    while (first != last) {        // 读指针(快指针)扫描",
         "        if (!(*first == val)) {    // 如果当前元素不是目标值",
         "            *dest = *first;        // 将值保留到写指针位置",
         "            ++dest;                // 写指针前进",
         "        }",
         "        ++first;                   // 读指针始终前进",
         "    }",
         "    return dest;                   // 返回新的逻辑尾部",
         "}"
     };
 
     int dest = 0;  // 写指针
     for (size_t i = 0; i < arr.size(); ++i) {
         // 显示当前比较步骤
         if (arr[i] == removeValue) {
             animSteps.push_back({arr, dest, (int)i, removeValue,
                 QString("快指针 READ[%1]=%2 == 目标值 %3，跳过此元素（仅读指针前进）")
                     .arg(i).arg(arr[i]).arg(removeValue),
                 removeCode, 4});
         } else {
             animSteps.push_back({arr, dest, (int)i, removeValue,
                 QString("发现非目标元素: READ[%1]=%2 != %3，写入到 DEST[%4]")
                     .arg(i).arg(arr[i]).arg(removeValue).arg(dest),
                 removeCode, 5});
             if (dest != (int)i) {
                 animSteps.push_back({arr, dest, (int)i, removeValue,
                     QString("写入完成: DEST[%1] = %2，写指针前进").arg(dest).arg(arr[i]),
                     removeCode, 6});
             }
             arr[dest] = arr[i];
             dest++;
         }
     }
 
     int newSize = dest;
     QStringList finishCode = {"// 移除完成，返回新的逻辑尾部"};
     animSteps.push_back({arr, newSize, -1, removeValue,
         QString("移除完成。新的有效长度为 %1，剩余可配合 erase 清理。").arg(newSize),
         finishCode, 0});
     realData = arr;
 }
 
 void VisualRemove::stepForward() {
     if (currentStep < (int)animSteps.size() - 1) {
         currentStep++;
         updateCodeBrowser();
         canvas->update();
     }
 }
 
 void VisualRemove::stepBackward() {
     if (currentStep > 0) {
         currentStep--;
         updateCodeBrowser();
         canvas->update();
     }
 }
 
 void VisualRemove::reset() {
     currentStep = 0;
     updateCodeBrowser();
     canvas->update();
 }
 
 bool VisualRemove::isAtEnd() const {
     return animSteps.empty() || currentStep >= (int)animSteps.size() - 1;
 }
 
 void VisualRemove::updateCodeBrowser() {
     if (currentStep >= 0 && currentStep < (int)animSteps.size()) {
         const auto& step = animSteps[currentStep];
         codeBrowser->setHtml(VisualBase::renderCodeWithHighlight(step.codeLines, step.activeLine));
     }
 }
 
 void VisualRemove::exportImage() {
     QPixmap pixmap = canvas->grab();
     QString fileName = QFileDialog::getSaveFileName(this, "导出截图", "remove_snapshot.png", "PNG 图像 (*.png);;JPEG 图像 (*.jpg)");
     if (!fileName.isEmpty()) {
         pixmap.save(fileName);
     }
 }
 
 // 返回包含算法原理和示例代码的 HTML 字符串
 QString VisualRemove::getInfo() const {
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
 <h2>std::remove 移除特定值的元素</h2>
 <p><b>std::remove</b> 并不实际删除容器中的元素，而是将"要保留"的元素移动到区间前段，返回新的逻辑尾部迭代器。这是 Erase-Remove 惯用法的前半部分。</p>
 
 <h3>1. Erase-Remove 惯用法</h3>
 <p>因为 STL 的"算法与容器解耦"原则，算法本身没有能力调用容器的 erase。正确的做法是：</p>
 <pre><code>v.erase(std::remove(v.begin(), v.end(), val), v.end());</code></pre>
 
 <h3>2. 算法特征</h3>
 <ul>
     <li>时间复杂度：<b>O(N)</b>，一次线性扫描完成。</li>
     <li>空间复杂度：<b>O(1)</b>，采用双指针技术原地修改。</li>
     <li>迭代器要求：<b>Forward Iterator</b> 及以上。</li>
 </ul>
 
 <h3>3. C++ 底层源码提炼</h3>
 <pre>%1</pre>
 )").arg(VisualBase::highlightCode(R"(template<typename _ForwardIterator, typename _Tp>
 _ForwardIterator remove(_ForwardIterator __first, _ForwardIterator __last,
                         const _Tp& __value) {
     // __dest 是写指针，__first 是读指针
     _ForwardIterator __dest = __first;
     while (__first != __last) {
         if (!(*__first == __value)) {
             *__dest = std::move(*__first);
             ++__dest;
         }
         ++__first;
     }
     return __dest;  // 返回新的逻辑尾部
 })"));
 }
 
 void VisualRemove::CanvasWidget::paintEvent(QPaintEvent*) {
     p->paintCanvas(this);
 }
 
 // 核心绘制逻辑：根据当前步骤画出所有元素
 void VisualRemove::paintCanvas(QWidget* target) {
     QPainter painter(target);
     painter.setRenderHint(QPainter::Antialiasing);
 
     if (animSteps.empty()) return;
     const RemoveAnimStep &step = animSteps[currentStep];
 
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
 
         // 根据指针状态着色
         bool isTrash = (step.destIdx != -1 && (int)i >= step.destIdx && step.currIdx == -1);
         if (isTrash) {
             painter.setBrush(QColor("#333333"));  // 废弃数据
         } else if ((int)i == step.destIdx) {
             painter.setBrush(QColor("#00d778"));  // 写指针（DEST）
         } else if ((int)i == step.currIdx) {
             painter.setBrush(QColor("#e51400"));  // 读指针（READ）
         } else {
             painter.setBrush(QColor("#0e639c"));  // 普通蓝色
         }
 
         painter.setPen(Qt::white);
         painter.drawRect(rect);
         painter.setFont(QFont("Arial", 14, QFont::Bold));
         painter.drawText(rect, Qt::AlignCenter, QString::number(step.data[i]));
 
         // 标记标签
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
         if (isTrash) {
             painter.setPen(QColor("#888888"));
             painter.setFont(QFont("Arial", 9));
             painter.drawText(QRect(x, startY + boxH + 5, boxW, 20), Qt::AlignCenter, "废弃");
         }
     }
 }
