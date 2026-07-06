#include "VisualMerge.h"
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
VisualMerge::VisualMerge(QWidget *parent) : VisualBase(parent) {
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

    QLabel* lblInit1 = new QLabel("有序数组1(以逗号分隔):", this);
    valueEdit1 = new QLineEdit(this);
    valueEdit1->setPlaceholderText("以逗号分隔，如 1,2,3");
    valueEdit1->setText("1,3,5,7");
    valueEdit1->setFixedWidth(120);

    QLabel* lblInit2 = new QLabel("有序数组2(以逗号分隔):", this);
    valueEdit2 = new QLineEdit(this);
    valueEdit2->setPlaceholderText("以逗号分隔，如 1,2,3");
    valueEdit2->setText("2,4,6,8,10");
    valueEdit2->setFixedWidth(120);

    QPushButton* btnLoad = new QPushButton("加载并合并", this);

    inputLayout->addWidget(lblInit1);
    inputLayout->addWidget(valueEdit1);
    inputLayout->addSpacing(10);
    inputLayout->addWidget(lblInit2);
    inputLayout->addWidget(valueEdit2);
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
    std::vector<QuizQuestion> questions = QuizDatabase::getQuestionsFor(QuizDatabase::Topic::Merge, UserProfile::instance().getErrorsForTopic("Merge"));
    SubQuizWidget* quizWidget = new SubQuizWidget(questions, "Merge", this);
    tabWidget->addTab(quizWidget, "巩固练习");
    // 限制只能输入数字和逗号，并加入占位符提示
    QRegularExpression regExp("^[0-9,\\-\\s，]*$");
    QRegularExpressionValidator* listValidator = new QRegularExpressionValidator(regExp, this);
    valueEdit1->setValidator(listValidator);
    valueEdit2->setValidator(listValidator);


    connect(btnLoad, &QPushButton::clicked, this, &VisualMerge::handleLoad);

    arr1 = {1, 3, 5, 7};
    arr2 = {2, 4, 6, 8, 10};
    resultArr.clear();
    QStringList initCode = {"// std::merge 双数组有序归并", "// 可以选择顶部菜单执行操作..."};
    animSteps.push_back({arr1, arr2, resultArr, 0, 0, "就绪。点击合并。", initCode, -1});
    updateCodeBrowser();
}

// 处理用户输入并重新加载数组，包含格式与有序性校验
void VisualMerge::handleLoad() {
    auto parse = [this](QLineEdit* edit, std::vector<int>& out) -> bool {
        out.clear();
        QString text = edit->text().trimmed();
        if (text.isEmpty()) return true;
        QStringList parts = text.split(',');
        for (auto p : parts) {
            QString trimmed = p.trimmed();
            if (trimmed.isEmpty()) continue;
            bool ok;
            int val = trimmed.toInt(&ok);
            if (!ok) {
                QMessageBox::warning(this, "格式错误", QString("无法解析元素: \"%1\"，请输入纯数字！").arg(trimmed));
                return false;
            }
            out.push_back(val);
        }
        return true;
    };

    std::vector<int> t1, t2;
    if (!parse(valueEdit1, t1) || !parse(valueEdit2, t2)) {
        return;
    }

    if (t1.empty() && t2.empty()) {
        QMessageBox::warning(this, "错误", "两个数组不能同时为空！");
        return;
    }

    if (t1.size() > 8 || t2.size() > 8) {
        QMessageBox::warning(this, "限制", "每个数组最多支持 8 个元素！");
        return;
    }

    auto checkSorted = [](const std::vector<int>& v) {
        for (size_t i = 1; i < v.size(); ++i) {
            if (v[i] < v[i-1]) return false;
        }
        return true;
    };

    if (!checkSorted(t1) || !checkSorted(t2)) {
        QMessageBox::warning(this, "错误", "两个输入数组都必须是有序（升序）的！");
        return;
    }

    arr1 = t1;
    arr2 = t2;
    resultArr.clear();
    animSteps.clear();
    currentStep = 0;
    
    QStringList initCode = {"// 数组加载完毕，已通过升序校验"};
    animSteps.push_back({arr1, arr2, resultArr, 0, 0, "加载成功。开始归并。", initCode, -1});
    generateSteps();
    
    updateCodeBrowser();
    canvas->update();
}

// 归并步骤的具体算法生成，包含伪代码高亮绑定
void VisualMerge::generateSteps() {
    int i = 0, j = 0;
    std::vector<int> res;

    QStringList mergeCode = {
        "template<typename InputIt1, typename InputIt2, typename OutputIt>",
        "OutputIt merge(InputIt1 f1, InputIt1 l1, InputIt2 f2, InputIt2 l2, OutputIt res) {",
        "    while (f1 != l1 && f2 != l2) {",
        "        if (*f2 < *f1) { // 比较双数组的当前元素",
        "            *res++ = *f2++; // 归并 Array2 较小值",
        "        } else {",
        "            *res++ = *f1++; // 归并 Array1 较小/等值",
        "        }",
        "    }",
        "    res = copy(f1, l1, res); // 复制 Array1 剩余",
        "    res = copy(f2, l2, res); // 复制 Array2 剩余",
        "    return res;",
        "}"
    };

    while (i < (int)arr1.size() && j < (int)arr2.size()) {
        animSteps.push_back({arr1, arr2, res, i, j, QString("比较 Array1[%1]=%2 和 Array2[%3]=%4").arg(i).arg(arr1[i]).arg(j).arg(arr2[j]), mergeCode, 3});
        
        if (arr1[i] <= arr2[j]) {
            res.push_back(arr1[i]);
            animSteps.push_back({arr1, arr2, res, i, j, QString("%1 较小或等于，将 %1 放入结果数组，指针 1 前进").arg(arr1[i]), mergeCode, 6});
            i++;
        } else {
            res.push_back(arr2[j]);
            animSteps.push_back({arr1, arr2, res, i, j, QString("%1 较小，将 %1 放入结果数组，指针 2 前进").arg(arr2[j]), mergeCode, 4});
            j++;
        }
    }

    while (i < (int)arr1.size()) {
        res.push_back(arr1[i]);
        animSteps.push_back({arr1, arr2, res, i, j, QString("Array2 已空，将 Array1 剩余元素 %1 直接追加").arg(arr1[i]), mergeCode, 9});
        i++;
    }

    while (j < (int)arr2.size()) {
        res.push_back(arr2[j]);
        animSteps.push_back({arr1, arr2, res, i, j, QString("Array1 已空，将 Array2 剩余元素 %1 直接追加").arg(arr2[j]), mergeCode, 10});
        j++;
    }

    QStringList finishCode = {"// 合并完成！得到一个有序大数组"};
    animSteps.push_back({arr1, arr2, res, -1, -1, "合并完成。得到一个完整的有序数组。", finishCode, 0});
    resultArr = res;
}

// 推进动画的当前步骤并刷新代码高亮
void VisualMerge::stepForward() {
    if (currentStep < (int)animSteps.size() - 1) {
        currentStep++;
        updateCodeBrowser();
        canvas->update();
    }
}

// 动画后退一步并刷新代码高亮
void VisualMerge::stepBackward() {
    if (currentStep > 0) {
        currentStep--;
        updateCodeBrowser();
        canvas->update();
    }
}

// 恢复初始状态
void VisualMerge::reset() {
    currentStep = 0;
    updateCodeBrowser();
    canvas->update();
}

// 判断当前动画是否已经执行到最后一步
bool VisualMerge::isAtEnd() const {
    return animSteps.empty() || currentStep >= (int)animSteps.size() - 1;
}

// 更新伪代码显示面板
void VisualMerge::updateCodeBrowser() {
    if (currentStep >= 0 && currentStep < (int)animSteps.size()) {
        const auto& step = animSteps[currentStep];
        codeBrowser->setHtml(VisualBase::renderCodeWithHighlight(step.codeLines, step.activeLine));
    }
}

// 导出当前画布截图
void VisualMerge::exportImage() {
    QPixmap pixmap = canvas->grab();
    QString fileName = QFileDialog::getSaveFileName(this, "导出截图", "merge_snapshot.png", "PNG 图像 (*.png);;JPEG 图像 (*.jpg)");
    if (!fileName.isEmpty()) {
        pixmap.save(fileName);
    }
}

// 返回包含该数据结构/算法原理和示例代码的 HTML 字符串
QString VisualMerge::getInfo() const {
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
<h2>std::merge 归并操作</h2>
<p><b>std::merge</b> 将两个<b>已经排序</b>的区间合并为一个新的有序区间。它是经典的"外排"算法（归并排序的核心步骤）。</p>

<h3>1. 算法特征</h3>
<ul>
    <li>时间复杂度：<b>O(N1 + N2)</b>。严格执行最多 <code>N1 + N2 - 1</code> 次比较。</li>
    <li>稳定性：<b>稳定 (Stable)</b>。如果两个元素相等，来自于第一个区间的元素总是优先被放入目标区间。</li>
    <li>迭代器要求：输入至少为 <b>Input Iterator</b>，输出为 <b>Output Iterator</b>。它可以非常泛型地工作，甚至合并两个从文件读取的数据流！</li>
</ul>

<h3>2. 内存重叠问题</h3>
<p><code>std::merge</code> 要求两个输入区间不能重叠。如果数据就在同一个数组的相邻两段并且你想合并它们，应该使用 <b>std::inplace_merge</b>，它在底层会尝试申请缓冲内存进行归并，如果内存不足则采用更复杂的 O(N log N) 旋转合并法。</p>

<h3>3. C++ 底层源码提炼 </h3>
<p>代码逻辑非常简单明了，就是谁小拷贝谁，若相等则优先拷贝前者（保证稳定性）：</p>
<pre>%1</pre>
)").arg(VisualBase::highlightCode(R"(template<typename _InputIterator1, typename _InputIterator2,
         typename _OutputIterator>
_OutputIterator merge(_InputIterator1 __first1, _InputIterator1 __last1,
                      _InputIterator2 __first2, _InputIterator2 __last2,
                      _OutputIterator __result) {
    // 只要两个区间都没空
    while (__first1 != __last1 && __first2 != __last2) {
        // 如果第二个元素严格小于第一个元素 (注意这里用的是 <，不是 <= )
        if (*__first2 < *__first1) {
            *__result = *__first2;  // 放入结果
            ++__first2;             // 指针2前进
        }
        else {
            // 如果 first1 <= first2，优先放入 first1 从而保持稳定性
            *__result = *__first1;
            ++__first1;
        }
        ++__result;
    }
    // 将剩下的元素全部拷贝到目标（这两个 copy 只有一个会真正执行，因为其中一个区间已经空了）
    __result = std::copy(__first1, __last1, __result);
    __result = std::copy(__first2, __last2, __result);
    return __result;
})"));
}

// 将画板的重绘委托给外层类的 paintCanvas 方法
void VisualMerge::CanvasWidget::paintEvent(QPaintEvent*) {
    p->paintCanvas(this);
}

// 绘制核心逻辑：根据当前步骤(currentStep)画出所有元素
void VisualMerge::paintCanvas(QWidget* target) {
    QPainter painter(target);
    painter.setRenderHint(QPainter::Antialiasing);

    if (animSteps.empty()) return;
    const MergeAnimStep &step = animSteps[currentStep];

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12));
    painter.drawText(30, 35, "操作进度描述:");
    painter.setPen(QColor("#ffaa00"));
    painter.drawText(30, 58, step.desc);

    int boxW = 40, boxH = 40;
    int spacing = 15;
    int startX = 50;

    auto drawArray = [&](const std::vector<int>& arr, int y, const QString& title, int pointerIdx, QColor pointerColor) {
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 12));
        painter.drawText(10, y + 25, title);

        for (size_t i = 0; i < arr.size(); ++i) {
            int x = startX + 70 + i * (boxW + spacing);
            QRect rect(x, y, boxW, boxH);

            if ((int)i == pointerIdx) {
                painter.setBrush(pointerColor);
            } else {
                painter.setBrush(QColor("#0e639c"));
            }

            painter.setPen(Qt::white);
            painter.drawRect(rect);
            painter.setFont(QFont("Arial", 12, QFont::Bold));
            painter.drawText(rect, Qt::AlignCenter, QString::number(arr[i]));
        }
    };

    drawArray(step.arr1, 100, "Arr 1:", step.idx1, QColor("#e51400"));
    drawArray(step.arr2, 180, "Arr 2:", step.idx2, QColor("#d81b60"));

    // Result array
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12));
    painter.drawText(10, 300 + 25, "Result:");

    for (size_t i = 0; i < step.result.size(); ++i) {
        int x = startX + 70 + i * (boxW + spacing);
        QRect rect(x, 300, boxW, boxH);

        painter.setBrush(QColor("#00d778")); // Result color
        painter.setPen(Qt::white);
        painter.drawRect(rect);
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.drawText(rect, Qt::AlignCenter, QString::number(step.result[i]));
    }
}