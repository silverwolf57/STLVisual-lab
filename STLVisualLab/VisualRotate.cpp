 #include "VisualRotate.h"
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
 #include <algorithm>
 
 // 构造函数：搭建界面，连接信号和槽，初始化数据
 VisualRotate::VisualRotate(QWidget *parent) : VisualBase(parent), currentStep(0) {
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
     valueEdit->setPlaceholderText("逗号分隔，如 1,2,3,4,5");
     valueEdit->setText("1,2,3,4,5,6,7");
     valueEdit->setFixedWidth(200);
 
     QLabel* lblPivot = new QLabel("旋转中点:", this);
     pivotEdit = new QLineEdit(this);
     pivotEdit->setPlaceholderText("中点值");
     pivotEdit->setText("3");
     pivotEdit->setFixedWidth(50);
 
     QPushButton* btnLoad = new QPushButton("加载并旋转", this);
 
     inputLayout->addWidget(lblInit);
     inputLayout->addWidget(valueEdit);
     inputLayout->addSpacing(10);
     inputLayout->addWidget(lblPivot);
     inputLayout->addWidget(pivotEdit);
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
     std::vector<QuizQuestion> questions = QuizDatabase::getQuestionsFor(QuizDatabase::Topic::Rotate, UserProfile::instance().getErrorsForTopic("Rotate"));
     SubQuizWidget* quizWidget = new SubQuizWidget(questions, "Rotate", this);
     tabWidget->addTab(quizWidget, "巩固练习");
 
     // 限制输入为数字和逗号
     QRegularExpression regExp("^[0-9,\\-\\s]*$");
     valueEdit->setValidator(new QRegularExpressionValidator(regExp, this));
     QIntValidator* intValidator = new QIntValidator(-999, 9999, this);
     pivotEdit->setValidator(intValidator);
 
     connect(btnLoad, &QPushButton::clicked, this, &VisualRotate::handleLoad);
 
     // 初始状态
     realData = {1, 2, 3, 4, 5, 6, 7};
     pivotValue = 3;
     QStringList initCode = {"// std::rotate 三次反转旋转算法", "// 可以点击按钮查看动画过程"};
     animSteps.push_back({realData, pivotValue, -1, -1, 0, "就绪。点击加载并旋转开始演示。", initCode, -1});
     updateCodeBrowser();
 }
 
 // 处理用户输入并重新加载数组，包含格式和边界校验
 void VisualRotate::handleLoad() {
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
 
     // 获取旋转中点
     QString pivotText = pivotEdit->text().trimmed();
     if (pivotText.isEmpty()) {
         QMessageBox::warning(this, "提示", "请输入旋转中点值！");
         return;
     }
     bool ok;
     int pivot = pivotText.toInt(&ok);
     if (!ok) {
         QMessageBox::warning(this, "格式错误", "请输入有效的整数！");
         return;
     }
     if (pivot <= 0 || pivot >= (int)newData.size()) {
         QMessageBox::warning(this, "错误",
             QString("旋转中点需要在 1 和 %1 之间！").arg(newData.size() - 1));
         return;
     }
 
     realData = newData;
     pivotValue = pivot;
     animSteps.clear();
     currentStep = 0;
 
     QStringList initCode = {"// 数组重新加载完成"};
     animSteps.push_back({realData, pivotValue, -1, -1, 0, QString("加载完成，中点=%1。开始生成旋转步骤。").arg(pivot), initCode, -1});
     generateSteps();
 
     updateCodeBrowser();
     canvas->update();
 }
 
 // 生成三次反转旋转的动画步骤
 void VisualRotate::generateSteps() {
     std::vector<int> arr = realData;
     int n = arr.size();
     int mid = pivotValue;
 
     // std::rotate 的伪代码（三次反转法）
     QStringList rotateCode = {
         "void rotate(iterator first, iterator mid, iterator last) {",
         "    // 三次反转法：O(N) 时间，O(1) 空间",
         "    // Step 1: 反转前段 [first, mid)",
         "    reverse(first, mid);",
         "    // Step 2: 反转后段 [mid, last)",
         "    reverse(mid, last);",
         "    // Step 3: 反转整体 [first, last)",
         "    reverse(first, last);",
         "}"
     };
 
     // 初始状态
     animSteps.push_back({arr, mid, -1, -1, 0,
         QString("初始数组 [0..%1]，以位置 %2 为旋转中点").arg(n-1).arg(mid),
         rotateCode, 0});
 
     // Step 1: 反转前段 [0, mid)
     std::reverse(arr.begin(), arr.begin() + mid);
     animSteps.push_back({arr, mid, 0, mid - 1, 1,
         QString("Step 1: 反转前段 [0, %1)，区间元素已翻转").arg(mid),
         rotateCode, 3});
 
     // Step 2: 反转后段 [mid, n)
     std::reverse(arr.begin() + mid, arr.end());
     animSteps.push_back({arr, mid, mid, n - 1, 2,
         QString("Step 2: 反转后段 [%1, %2)，区间元素已翻转").arg(mid).arg(n),
         rotateCode, 5});
 
     // Step 3: 反转整体 [0, n)
     std::reverse(arr.begin(), arr.end());
     animSteps.push_back({arr, mid, 0, n - 1, 3,
         "Step 3: 反转整体，旋转完成！",
         rotateCode, 7});
 
     // 完成状态
     animSteps.push_back({arr, mid, -1, -1, 4,
         "std::rotate 旋转完成！三次反转法，O(N) 时间，O(1) 空间。",
         rotateCode, 8});
 
     realData = arr;
 }
 
 void VisualRotate::stepForward() {
     if (currentStep < (int)animSteps.size() - 1) {
         currentStep++;
         updateCodeBrowser();
         canvas->update();
     }
 }
 
 void VisualRotate::stepBackward() {
     if (currentStep > 0) {
         currentStep--;
         updateCodeBrowser();
         canvas->update();
     }
 }
 
 void VisualRotate::reset() {
     currentStep = 0;
     updateCodeBrowser();
     canvas->update();
 }
 
 bool VisualRotate::isAtEnd() const {
     return animSteps.empty() || currentStep >= (int)animSteps.size() - 1;
 }
 
 void VisualRotate::updateCodeBrowser() {
     if (currentStep >= 0 && currentStep < (int)animSteps.size()) {
         const auto& step = animSteps[currentStep];
         codeBrowser->setHtml(VisualBase::renderCodeWithHighlight(step.codeLines, step.activeLine));
     }
 }
 
 void VisualRotate::exportImage() {
     QPixmap pixmap = canvas->grab();
     QString fileName = QFileDialog::getSaveFileName(this, "导出截图", "rotate_snapshot.png", "PNG 图像 (*.png);;JPEG 图像 (*.jpg)");
     if (!fileName.isEmpty()) {
         pixmap.save(fileName);
     }
 }
 
 // 返回包含算法原理和示例代码的 HTML 字符串
 QString VisualRotate::getInfo() const {
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
 <h2>std::rotate 区间旋转</h2>
 <p><b>std::rotate</b> 将区间 [first, last) 中的元素以中点 mid 为轴旋转，使得 [mid, last) 中的元素移动到区间前段，[first, mid) 中的元素紧随其后。</p>
 
 <h3>1. 三次反转法</h3>
 <p>std::rotate 的典型实现使用三次反转：</p>
 <pre>
 reverse(first, mid);        // Step 1: 反转前段
 reverse(mid, last);         // Step 2: 反转后段
 reverse(first, last);       // Step 3: 反转整体
 </pre>
 
 <h3>2. 算法特征</h3>
 <ul>
     <li>时间复杂度：<b>O(N)</b>，精确执行 N 次交换。</li>
     <li>空间复杂度：<b>O(1)</b>，原地修改。</li>
     <li>迭代器要求：<b>Forward Iterator</b> 及以上。</li>
     <li>对于 C++11 及以上，如果迭代器是 RandomAccess，会使用更高效的旋转实现。</li>
 </ul>
 
 <h3>3. C++ 底层源码提炼</h3>
 <pre>%1</pre>
 )").arg(VisualBase::highlightCode(R"(template<typename _ForwardIterator>
 _ForwardIterator rotate(_ForwardIterator __first,
                           _ForwardIterator __middle,
                           _ForwardIterator __last) {
     if (__first == __middle || __last == __middle)
         return __first;
 
     // 对于 ForwardIterator 使用三次反转
     _ForwardIterator __next = __first;
     while (__next != __middle) {
         std::iter_swap(__first++, __next++);
         if (__first == __middle) __middle = __next;
     }
     return __first;
 }
 
 // 对于 RandomAccessIterator 的重载使用三次 reverse（效率更高）
 template<>
 void __rotate(_RandomAccessIterator __first,
               _RandomAccessIterator __middle,
               _RandomAccessIterator __last) {
     std::reverse(__first, __middle);
     std::reverse(__middle, __last);
     std::reverse(__first, __last);
 })"));
 }
 
 void VisualRotate::CanvasWidget::paintEvent(QPaintEvent*) {
     p->paintCanvas(this);
 }
 
 // 核心绘制逻辑：根据当前步骤画出所有元素
 void VisualRotate::paintCanvas(QWidget* target) {
     QPainter painter(target);
     painter.setRenderHint(QPainter::Antialiasing);
 
     if (animSteps.empty()) return;
     const RotateAnimStep &step = animSteps[currentStep];
 
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
 
         // 根据阶段着色
         if (step.phase == 1 && (int)i >= step.highlightStart && (int)i <= step.highlightEnd) {
             painter.setBrush(QColor("#4ec9b0"));  // 前段（青色）
         } else if (step.phase == 2 && (int)i >= step.highlightStart && (int)i <= step.highlightEnd) {
             painter.setBrush(QColor("#ce9178"));  // 后段（橙色）
         } else if (step.phase == 3) {
             painter.setBrush(QColor("#9b4dca"));  // 整体（紫色）
         } else {
             painter.setBrush(QColor("#0e639c"));  // 普通蓝色
         }
 
         painter.setPen(Qt::white);
         painter.drawRect(rect);
         painter.setFont(QFont("Arial", 14, QFont::Bold));
         painter.drawText(rect, Qt::AlignCenter, QString::number(step.data[i]));
 
         // 标记中点
         if ((int)i == step.pivot) {
             painter.setPen(QColor("#ffcc00"));
             painter.setFont(QFont("Arial", 10, QFont::Bold));
             painter.drawText(QRect(x, startY + boxH + 10, boxW, 20), Qt::AlignCenter, "MID");
         }
     }
 
     // 绘制阶段标签
     QString phaseLabel;
     if (step.phase == 0) phaseLabel = "阶段: 初始数组";
     else if (step.phase == 1) phaseLabel = "阶段 1/3: 反转前段";
     else if (step.phase == 2) phaseLabel = "阶段 2/3: 反转后段";
     else if (step.phase == 3) phaseLabel = "阶段 3/3: 反转整体";
     else if (step.phase == 4) phaseLabel = "旋转完成";
 
     if (!phaseLabel.isEmpty()) {
         painter.setPen(QColor("#888888"));
         painter.setFont(QFont("Arial", 11, QFont::Bold));
         painter.drawText(30, 110, phaseLabel);
     }
 }
