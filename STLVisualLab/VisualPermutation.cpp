#include "VisualPermutation.h"
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
#include <QSplitter>
#include <QFileDialog>
#include <algorithm>

// -----------------------------------------------------------------------------
//  next_permutation 的核心伪代码（右侧代码面板始终显示这一套完整实现）
// -----------------------------------------------------------------------------
static QStringList nextPermCode() {
    return {
        "bool next_permutation(first, last) {",
        "    if (first == last) return false;",
        "    auto i = last;",
        "    if (i == first) return false;",
        "    // Step 1: 从右向左找第一个「下降点」i",
        "    //   使得 *i < *(i+1)，即找到字典序可以增大的位置",
        "    while (true) {",
        "        auto i1 = i;",
        "        if (*--i < *i1) {          // 找到下降点 i",
        "            // Step 2: 从右向左找第一个大于 *i 的元素 j",
        "            auto j = last;",
        "            while (!(*i < *--j));   // j 即为替换数位置",
        "            // Step 3: 交换 *i 和 *j",
        "            std::iter_swap(i, j);",
        "            // Step 4: 翻转 i+1 到末尾，得到最小字典序后缀",
        "            std::reverse(i1, last);",
        "            return true;",
        "        }",
        "        if (i == first) {           // 已是最大排列",
        "            std::reverse(first, last);",
        "            return false;",
        "        }",
        "    }",
        "}"
    };
}

// -----------------------------------------------------------------------------
//  构造函数 -- 搭建 UI 并生成初始步骤
// -----------------------------------------------------------------------------
VisualPermutation::VisualPermutation(QWidget *parent)
    : VisualBase(parent), currentStep(0), permCount(1)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #3c3c3c; background-color: #1e1e1e; }"
        "QTabBar::tab { background-color: #252526; color: #888; padding: 8px 20px; border: 1px solid #3c3c3c; border-bottom: none; }"
        "QTabBar::tab:selected { background-color: #1e1e1e; color: #569cd6; font-weight: bold; }");
    mainLayout->addWidget(tabWidget);

    // -- 动画演示 Tab ----------------------------------------------------------
    QWidget* animTab = new QWidget(this);
    QVBoxLayout* animLayout = new QVBoxLayout(animTab);

    QWidget* inputPanel = new QWidget(this);
    inputPanel->setStyleSheet(
        "QWidget { background-color: #252526; border-radius: 6px; }"
        "QPushButton { background-color: #0e639c; color: white; border: none; padding: 6px 14px; border-radius: 4px; font-weight: bold; }"
        "QPushButton:hover { background-color: #1177bb; }"
        "QPushButton:pressed { background-color: #094771; }"
        "QLineEdit { background-color: #3c3c3c; color: white; border: 1px solid #555; border-radius: 3px; padding: 4px; }");
    QHBoxLayout* inputLayout = new QHBoxLayout(inputPanel);
    inputLayout->setContentsMargins(10, 5, 10, 5);

    QLabel* lblInit = new QLabel("初始排列 (1~6 个不同数字，逗号分隔):", this);
    lblInit->setStyleSheet("color: #d4d4d4; font-size: 13px;");
    valueEdit = new QLineEdit(this);
    valueEdit->setPlaceholderText("如 1,2,3");
    valueEdit->setText("1,2,3");
    valueEdit->setFixedWidth(140);
    QPushButton* btnLoad = new QPushButton("加载", this);

    QPushButton* btnNext = new QPushButton(" 下一个排列", this);
    btnNext->setStyleSheet(
        "QPushButton { background-color: #0e639c; color: white; border: none; padding: 6px 14px; border-radius: 4px; font-weight: bold; }"
        "QPushButton:hover { background-color: #1177bb; }");

    QPushButton* btnAll = new QPushButton(" 生成全部排列", this);
    btnAll->setStyleSheet(
        "QPushButton { background-color: #9b4dca; color: white; border: none; padding: 6px 14px; border-radius: 4px; font-weight: bold; }"
        "QPushButton:hover { background-color: #b563e8; }");

    lblPermCount = new QLabel("当前排列: #1", this);
    lblPermCount->setStyleSheet("color: #00d778; font-size: 13px; font-weight: bold; margin-left: 10px;");

    inputLayout->addWidget(lblInit);
    inputLayout->addWidget(valueEdit);
    inputLayout->addWidget(btnLoad);
    inputLayout->addSpacing(20);
    inputLayout->addWidget(btnNext);
    inputLayout->addWidget(btnAll);
    inputLayout->addWidget(lblPermCount);
    inputLayout->addStretch();

    animLayout->addWidget(inputPanel);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, animTab);
    canvas = new CanvasWidget(this);
    codeBrowser = new QTextBrowser(this);
    codeBrowser->setStyleSheet("QTextBrowser { background-color: #1e1e1e; border: 1px solid #3c3c3c; border-radius: 4px; }");
    splitter->addWidget(canvas);
    splitter->addWidget(codeBrowser);
    splitter->setStretchFactor(0, 15);
    splitter->setStretchFactor(1, 8);
    splitter->setSizes(QList<int>() << 580 << 310);
    animLayout->addWidget(splitter, 1);
    tabWidget->addTab(animTab, "动画演示");

    // -- 知识卡片 Tab ----------------------------------------------------------
    QTextBrowser* infoBrowser = new QTextBrowser(this);
    infoBrowser->setStyleSheet("background-color: #1e1e1e; border: none; padding: 20px; font-size: 15px;");
    infoBrowser->setHtml(getInfo());
    infoBrowser->setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    tabWidget->addTab(infoBrowser, "知识卡片");

    // -- 巩固练习 Tab ----------------------------------------------------------
    std::vector<QuizQuestion> questions = QuizDatabase::getQuestionsFor(
        QuizDatabase::Topic::Permutation,
        UserProfile::instance().getErrorsForTopic("Permutation"));
    SubQuizWidget* quizWidget = new SubQuizWidget(questions, "Permutation", this);
    tabWidget->addTab(quizWidget, "巩固练习");

    // -- Validator & Connections -----------------------------------------------
    QRegularExpression regExp("^[0-9,\\-\\s，]*$");
    valueEdit->setValidator(new QRegularExpressionValidator(regExp, this));

    connect(btnLoad, &QPushButton::clicked, this, &VisualPermutation::handleLoad);
    connect(btnNext, &QPushButton::clicked, this, &VisualPermutation::handleNextPerm);
    connect(btnAll,  &QPushButton::clicked, this, &VisualPermutation::handleRunAll);

    // -- 初始化默认排列 {1,2,3} ------------------------------------------------
    realData    = {1, 2, 3};
    originalData = realData;
    permCount   = 1;

    PermAnimStep init;
    init.data         = realData;
    init.idx1         = -1;
    init.idx2         = -1;
    init.reverseStart = -1;
    init.highlightType = 0;
    init.desc         = "初始排列 {1,2,3}。点击「下一个排列」开始逐步演示 next_permutation 算法。";
    init.codeLines    = nextPermCode();
    init.activeLine   = 0;
    animSteps.push_back(init);
    updateCodeBrowser();
}

// -----------------------------------------------------------------------------
//  handleLoad -- 加载新排列
// -----------------------------------------------------------------------------
void VisualPermutation::handleLoad() {
    QString text = valueEdit->text().trimmed();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "错误", "输入不能为空！");
        return;
    }
    QStringList parts = text.split(',');
    std::vector<int> newData;
    for (auto& p : parts) {
        QString t = p.trimmed();
        if (t.isEmpty()) continue;
        bool ok;
        int v = t.toInt(&ok);
        if (!ok) { QMessageBox::warning(this, "格式错误", QString("无法解析: \"%1\"").arg(t)); return; }
        newData.push_back(v);
    }
    if (newData.size() < 2 || newData.size() > 6) {
        QMessageBox::warning(this, "范围错误", "请输入 2~6 个数字（过多则动画太密集）！");
        return;
    }
    // 排序到最小字典序，作为"第一个排列"
    std::sort(newData.begin(), newData.end());

    realData     = newData;
    originalData = newData;
    permCount    = 1;
    animSteps.clear();
    currentStep = 0;
    lblPermCount->setText("当前排列: #1");

    PermAnimStep init;
    init.data          = realData;
    init.idx1          = -1;
    init.idx2          = -1;
    init.reverseStart  = -1;
    init.highlightType = 0;
    init.desc          = QString("加载完毕，已重排为最小字典序排列。共 %1! = %2 种排列。")
                             .arg(realData.size())
                             .arg([&]{ int f=1; for(int i=2;i<=(int)realData.size();++i) f*=i; return f; }());
    init.codeLines    = nextPermCode();
    init.activeLine   = 0;
    animSteps.push_back(init);

    updateCodeBrowser();
    canvas->update();
}

// -----------------------------------------------------------------------------
//  generateNextPermSteps -- 核心：把 next_permutation 的每一步展开成动画帧
// -----------------------------------------------------------------------------
void VisualPermutation::generateNextPermSteps(std::vector<int> arr) {
    QStringList code = nextPermCode();

    int n = (int)arr.size();

    // -- Step 1: 从右向左扫描，找到第一个「下降点」i -------------------------
    int i = n - 2;
    while (i >= 0 && arr[i] >= arr[i + 1]) {
        // 扫描过程动画：逐步高亮扫过的位置
        PermAnimStep scan;
        scan.data          = arr;
        scan.idx1          = i;
        scan.idx2          = i + 1;
        scan.reverseStart  = -1;
        scan.highlightType = 2;
        scan.desc          = QString("Step 1 扫描：arr[%1]=%2  arr[%3]=%4，继续向左...")
                                 .arg(i).arg(arr[i]).arg(i+1).arg(arr[i+1]);
        scan.codeLines    = code;
        scan.activeLine   = 7;
        animSteps.push_back(scan);
        i--;
    }

    if (i < 0) {
        // 已经是最大排列，翻转回最小
        PermAnimStep maxStep;
        maxStep.data          = arr;
        maxStep.idx1          = -1;
        maxStep.idx2          = -1;
        maxStep.reverseStart  = 0;
        maxStep.highlightType = 3;
        maxStep.desc          = "已是最大排列！整体翻转回最小字典序，返回 false。";
        maxStep.codeLines    = code;
        maxStep.activeLine   = 19;
        animSteps.push_back(maxStep);

        std::reverse(arr.begin(), arr.end());
        PermAnimStep done;
        done.data          = arr;
        done.idx1          = -1;
        done.idx2          = -1;
        done.reverseStart  = -1;
        done.highlightType = 0;
        done.desc          = "已回到最小排列。所有排列已遍历完毕。";
        done.codeLines    = code;
        done.activeLine   = 22;
        animSteps.push_back(done);
        realData = arr;
        return;
    }

    // 找到下降点 i -- 标记高亮
    {
        PermAnimStep found;
        found.data          = arr;
        found.idx1          = i;
        found.idx2          = i + 1;
        found.reverseStart  = -1;
        found.highlightType = 2;
        found.desc          = QString("[OK] Step 1 完成：找到下降点 i=%1（arr[%1]=%2 < arr[%3]=%4）")
                                  .arg(i).arg(arr[i]).arg(i+1).arg(arr[i+1]);
        found.codeLines    = code;
        found.activeLine   = 8;
        animSteps.push_back(found);
    }

    // -- Step 2: 从右向左找第一个大于 arr[i] 的 j -----------------------------
    int j = n - 1;
    while (arr[j] <= arr[i]) {
        PermAnimStep scanJ;
        scanJ.data          = arr;
        scanJ.idx1          = i;
        scanJ.idx2          = j;
        scanJ.reverseStart  = -1;
        scanJ.highlightType = 2;
        scanJ.desc          = QString("Step 2 扫描：arr[%1]=%2  arr[%3]=%4，j 继续左移...")
                                  .arg(j).arg(arr[j]).arg(i).arg(arr[i]);
        scanJ.codeLines    = code;
        scanJ.activeLine   = 11;
        animSteps.push_back(scanJ);
        j--;
    }

    {
        PermAnimStep foundJ;
        foundJ.data          = arr;
        foundJ.idx1          = i;
        foundJ.idx2          = j;
        foundJ.reverseStart  = -1;
        foundJ.highlightType = 2;
        foundJ.desc          = QString("[OK] Step 2 完成：找到替换数 j=%1（arr[%1]=%2 > arr[%3]=%4）")
                                   .arg(j).arg(arr[j]).arg(i).arg(arr[i]);
        foundJ.codeLines    = code;
        foundJ.activeLine   = 11;
        animSteps.push_back(foundJ);
    }

    // -- Step 3: 交换 arr[i] 和 arr[j] --------------------------------------
    {
        PermAnimStep beforeSwap;
        beforeSwap.data          = arr;
        beforeSwap.idx1          = i;
        beforeSwap.idx2          = j;
        beforeSwap.reverseStart  = -1;
        beforeSwap.highlightType = 2;
        beforeSwap.desc          = QString("Step 3：交换 arr[%1]=%2  arr[%3]=%4")
                                       .arg(i).arg(arr[i]).arg(j).arg(arr[j]);
        beforeSwap.codeLines    = code;
        beforeSwap.activeLine   = 13;
        animSteps.push_back(beforeSwap);
    }

    std::swap(arr[i], arr[j]);

    {
        PermAnimStep afterSwap;
        afterSwap.data          = arr;
        afterSwap.idx1          = i;
        afterSwap.idx2          = j;
        afterSwap.reverseStart  = -1;
        afterSwap.highlightType = 2;
        afterSwap.desc          = QString("[OK] Step 3 完成：交换后 arr[%1]=%2，arr[%3]=%4")
                                      .arg(i).arg(arr[i]).arg(j).arg(arr[j]);
        afterSwap.codeLines    = code;
        afterSwap.activeLine   = 13;
        animSteps.push_back(afterSwap);
    }

    // -- Step 4: 翻转 i+1 到末尾 ---------------------------------------------
    {
        PermAnimStep beforeRev;
        beforeRev.data          = arr;
        beforeRev.idx1          = i;
        beforeRev.idx2          = -1;
        beforeRev.reverseStart  = i + 1;
        beforeRev.highlightType = 3;
        beforeRev.desc          = QString("Step 4：翻转后缀区间 [%1, %2]，使其变为最小字典序")
                                      .arg(i+1).arg(n-1);
        beforeRev.codeLines    = code;
        beforeRev.activeLine   = 15;
        animSteps.push_back(beforeRev);
    }

    std::reverse(arr.begin() + i + 1, arr.end());

    {
        PermAnimStep afterRev;
        afterRev.data          = arr;
        afterRev.idx1          = i;
        afterRev.idx2          = -1;
        afterRev.reverseStart  = i + 1;
        afterRev.highlightType = 3;
        afterRev.desc          = QString("[OK] Step 4 完成：翻转后缀。新排列已生成 [OK]");
        afterRev.codeLines    = code;
        afterRev.activeLine   = 16;
        animSteps.push_back(afterRev);
    }

    // -- 最终状态：展示新排列 -------------------------------------------------
    realData = arr;
    permCount++;
    PermAnimStep result;
    result.data          = arr;
    result.idx1          = -1;
    result.idx2          = -1;
    result.reverseStart  = -1;
    result.highlightType = 0;
    result.desc          = QString("第 %1 个排列生成完毕   继续点击「下一个排列」查看下一步。").arg(permCount);
    result.codeLines    = nextPermCode();
    result.activeLine   = 0;
    animSteps.push_back(result);

    lblPermCount->setText(QString("当前排列: #%1").arg(permCount));
}

// -----------------------------------------------------------------------------
//  handleNextPerm -- 用户点击「下一个排列」
// -----------------------------------------------------------------------------
void VisualPermutation::handleNextPerm() {
    animSteps.clear();
    currentStep = 0;

    // 从当前 realData 状态出发生成新的一轮动画帧
    std::vector<int> start = realData;
    PermAnimStep init;
    init.data          = start;
    init.idx1          = -1;
    init.idx2          = -1;
    init.reverseStart  = -1;
    init.highlightType = 0;
    init.desc          = QString("当前为第 %1 个排列，开始求 next_permutation...").arg(permCount);
    init.codeLines    = nextPermCode();
    init.activeLine   = 0;
    animSteps.push_back(init);

    generateNextPermSteps(start);
    updateCodeBrowser();
    canvas->update();

    // 自动触发播放
    emit requestPlay();
}

// -----------------------------------------------------------------------------
//  handleRunAll -- 快速生成所有排列（仅做排列计数，不逐帧展示）
// -----------------------------------------------------------------------------
void VisualPermutation::handleRunAll() {
    if (originalData.empty()) return;

    // 重置到最小排列
    std::vector<int> arr = originalData;
    std::sort(arr.begin(), arr.end());

    animSteps.clear();
    currentStep = 0;
    permCount   = 0;
    realData    = arr;

    QStringList allPerms;
    do {
        permCount++;
        QString row;
        for (int v : arr) row += QString::number(v) + " ";
        allPerms << QString("#%1: { %2}").arg(permCount).arg(row);

        PermAnimStep s;
        s.data          = arr;
        s.idx1          = -1;
        s.idx2          = -1;
        s.reverseStart  = -1;
        s.highlightType = 0;
        s.desc          = QString("排列 #%1：{ %2}").arg(permCount).arg(row);
        s.codeLines     = nextPermCode();
        s.activeLine    = 0;
        animSteps.push_back(s);
    } while (std::next_permutation(arr.begin(), arr.end()));

    realData = originalData; // 回到最初排列

    // 最后一帧显示统计结果
    PermAnimStep done;
    done.data          = originalData;
    done.idx1          = -1;
    done.idx2          = -1;
    done.reverseStart  = -1;
    done.highlightType = 0;
    done.desc          = QString("全部 %1 个排列已生成完毕！单步或播放查看每一个排列。").arg(permCount);
    done.codeLines     = nextPermCode();
    done.activeLine    = 0;
    animSteps.push_back(done);

    lblPermCount->setText(QString("共 %1 个排列").arg(permCount));
    updateCodeBrowser();
    canvas->update();
    emit requestPlay();
}

// -----------------------------------------------------------------------------
//  动画控制
// -----------------------------------------------------------------------------
void VisualPermutation::stepForward() {
    if (currentStep < (int)animSteps.size() - 1) {
        currentStep++;
        updateCodeBrowser();
        canvas->update();
    }
}

void VisualPermutation::stepBackward() {
    if (currentStep > 0) {
        currentStep--;
        updateCodeBrowser();
        canvas->update();
    }
}

void VisualPermutation::reset() {
    currentStep = 0;
    updateCodeBrowser();
    canvas->update();
}

bool VisualPermutation::isAtEnd() const {
    return animSteps.empty() || currentStep >= (int)animSteps.size() - 1;
}

void VisualPermutation::updateCodeBrowser() {
    if (currentStep >= 0 && currentStep < (int)animSteps.size()) {
        const auto& step = animSteps[currentStep];
        codeBrowser->setHtml(VisualBase::renderCodeWithHighlight(step.codeLines, step.activeLine));
    }
}

void VisualPermutation::exportImage() {
    QPixmap pixmap = canvas->grab();
    QString fileName = QFileDialog::getSaveFileName(this, "导出截图", "permutation_snapshot.png",
                                                    "PNG 图像 (*.png);;JPEG 图像 (*.jpg)");
    if (!fileName.isEmpty()) pixmap.save(fileName);
}

// -----------------------------------------------------------------------------
//  paintCanvas -- 核心绘制：卡片式元素 + 多种高亮模式
// -----------------------------------------------------------------------------
void VisualPermutation::CanvasWidget::paintEvent(QPaintEvent*) {
    p->paintCanvas(this);
}

void VisualPermutation::paintCanvas(QWidget* target) {
    QPainter painter(target);
    painter.setRenderHint(QPainter::Antialiasing);

    if (animSteps.empty()) return;
    const PermAnimStep& step = animSteps[currentStep];

    int W = target->width();
    int H = target->height();

    // -- 渐变背景 ------------------------------------------------------------
    QLinearGradient bg(0, 0, 0, H);
    bg.setColorAt(0, QColor("#1a1a2e"));
    bg.setColorAt(1, QColor("#16213e"));
    painter.fillRect(target->rect(), bg);

    // -- 描述文字 ------------------------------------------------------------
    painter.setFont(QFont("Segoe UI", 10));
    painter.setPen(QColor("#888888"));
    painter.drawText(24, 30, "操作进度:");
    painter.setFont(QFont("Segoe UI", 12, QFont::Bold));
    painter.setPen(QColor("#00d778"));
    painter.drawText(24, 56, step.desc);

    // -- 排列卡片 ------------------------------------------------------------
    int n       = (int)step.data.size();
    int cardW   = 64;
    int cardH   = 64;
    int gap     = 18;
    int totalW  = n * cardW + (n - 1) * gap;
    int startX  = (W - totalW) / 2;
    int cardY   = H / 2 - cardH / 2 - 20;

    for (int idx = 0; idx < n; ++idx) {
        int x = startX + idx * (cardW + gap);
        QRect cardRect(x, cardY, cardW, cardH);

        // 确定卡片颜色模式
        QColor fillColor("#0e639c");
        QColor borderColor("#2080c0");
        QColor textColor(Qt::white);

        bool isReverseZone = (step.highlightType == 3) && (step.reverseStart >= 0) && (idx >= step.reverseStart);

        if (isReverseZone) {
            // 翻转区间：紫色调
            fillColor   = QColor("#4b2080");
            borderColor = QColor("#9b4dca");
        } else if (idx == step.idx1 && step.highlightType >= 1) {
            // 转折点 i：红橙色
            fillColor   = QColor("#7a2a00");
            borderColor = QColor("#ff6b35");
        } else if (idx == step.idx2 && step.highlightType >= 2) {
            // 替换数 j：金黄色
            fillColor   = QColor("#5a4200");
            borderColor = QColor("#ffaa00");
        }

        // 卡片阴影效果
        painter.setBrush(QColor(0, 0, 0, 60));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(cardRect.adjusted(3, 3, 3, 3), 8, 8);

        // 卡片主体
        painter.setBrush(fillColor);
        painter.setPen(QPen(borderColor, 2));
        painter.drawRoundedRect(cardRect, 8, 8);

        // 数字
        painter.setFont(QFont("Arial", 20, QFont::Bold));
        painter.setPen(textColor);
        painter.drawText(cardRect, Qt::AlignCenter, QString::number(step.data[idx]));

        // 索引标注
        painter.setFont(QFont("Consolas", 8));
        painter.setPen(QColor("#666666"));
        painter.drawText(QRect(x, cardY + cardH + 4, cardW, 16), Qt::AlignCenter,
                         QString("[%1]").arg(idx));

        // 特殊标注
        if (idx == step.idx1 && step.highlightType >= 1) {
            painter.setFont(QFont("Segoe UI", 9, QFont::Bold));
            painter.setPen(QColor("#ff6b35"));
            painter.drawText(QRect(x - 10, cardY - 22, cardW + 20, 18), Qt::AlignCenter, " i (下降点)");
        }
        if (idx == step.idx2 && step.highlightType >= 2) {
            painter.setFont(QFont("Segoe UI", 9, QFont::Bold));
            painter.setPen(QColor("#ffaa00"));
            painter.drawText(QRect(x - 10, cardY + cardH + 20, cardW + 20, 18), Qt::AlignCenter, " j (替换数)");
        }
        if (isReverseZone && idx == step.reverseStart) {
            painter.setFont(QFont("Segoe UI", 9, QFont::Bold));
            painter.setPen(QColor("#9b4dca"));
            painter.drawText(QRect(x - 10, cardY - 22, cardW * (n - step.reverseStart) + gap * (n - step.reverseStart - 1) + 20, 18),
                             Qt::AlignCenter, "--- 翻转后缀区间 ---");
        }
    }

    // -- 步骤进度指示条 -------------------------------------------------------
    if (!animSteps.empty()) {
        int barW   = W - 48;
        int barH   = 5;
        int barY   = H - 24;
        double pct = (double)currentStep / (double)(std::max(1, (int)animSteps.size() - 1));

        painter.setBrush(QColor("#2d2d44"));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(24, barY, barW, barH, 3, 3);

        painter.setBrush(QColor("#9b4dca"));
        painter.drawRoundedRect(24, barY, (int)(barW * pct), barH, 3, 3);

        painter.setFont(QFont("Segoe UI", 9));
        painter.setPen(QColor("#666666"));
        painter.drawText(24, barY - 6,
                         QString("步骤 %1 / %2").arg(currentStep + 1).arg(animSteps.size()));
    }
}

// -----------------------------------------------------------------------------
//  知识卡片 HTML
// -----------------------------------------------------------------------------
QString VisualPermutation::getInfo() const {
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
<h2>std::next_permutation 全排列</h2>
<p><b>std::next_permutation</b> 能在 <b>O(N)</b> 时间内将一个序列就地变换为其字典序的下一个全排列，并返回 <code>true</code>。若当前已是最大排列，则翻转回最小排列并返回 <code>false</code>。</p>

<h3>1. 四步核心算法</h3>
<table>
  <tr><th>步骤</th><th>操作</th><th>作用</th></tr>
  <tr><td>Step 1</td><td>从右向左找到第一个「下降点」i，使 arr[i] &lt; arr[i+1]</td><td>确定字典序可增大的最后一位</td></tr>
  <tr><td>Step 2</td><td>从右向左找到第一个大于 arr[i] 的元素 j</td><td>找到替换 arr[i] 的最小合法值</td></tr>
  <tr><td>Step 3</td><td>交换 arr[i] 和 arr[j]</td><td>使该位变大（但变大幅度最小）</td></tr>
  <tr><td>Step 4</td><td>翻转 i+1 到末尾的后缀</td><td>使后缀变为最小字典序（确保整体最小增量）</td></tr>
</table>

<h3>2. 复杂度与性质</h3>
<ul>
    <li>时间复杂度：<b>O(N)</b>，平均每次操作仅需约 O(1) 次交换</li>
    <li>空间复杂度：<b>O(1)</b>，原地操作</li>
    <li>N 个不同元素共有 <b>N! 个排列</b>（如 3!=6，4!=24，5!=120，6!=720）</li>
    <li>配合 <code>std::sort</code> 从最小排列开始，可以枚举所有全排列</li>
</ul>

<h3>3. 经典使用模式</h3>
<pre>%1</pre>

<h3>4. 常见面试题</h3>
<ul>
    <li>LeetCode 31 - Next Permutation（直接就是本算法）</li>
    <li>LeetCode 46 - Permutations（全排列 DFS vs next_permutation 两种思路）</li>
    <li>LeetCode 47 - Permutations II（含重复元素的全排列）</li>
</ul>
)").arg(VisualBase::highlightCode(R"(#include <algorithm>
#include <vector>
#include <iostream>

// 枚举 {1,2,3} 的所有全排列
int main() {
    std::vector<int> v = {1, 2, 3};
    std::sort(v.begin(), v.end()); // 必须先排序到最小排列
    do {
        for (int x : v) std::cout << x << " ";
        std::cout << "\n";
    } while (std::next_permutation(v.begin(), v.end()));
    // 输出：1 2 3 / 1 3 2 / 2 1 3 / 2 3 1 / 3 1 2 / 3 2 1
})"));
}
