#include "VisualSort.h"
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
#include <functional>
#include <QSplitter>
#include <QFileDialog>

// 构造函数：搭建界面，连接信号和槽，初始化数据
VisualSort::VisualSort(QWidget *parent) : VisualBase(parent) {
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

    dataTypeCombo = new QComboBox(this);
    dataTypeCombo->addItem("整数类型 (数值高度条)");
    dataTypeCombo->addItem("字符串型 (文本卡片)");
    dataTypeCombo->setStyleSheet("background-color: #3c3c3c; color: white; border: 1px solid #555; padding: 4px; border-radius: 3px; font-weight: bold;");

    QLabel* lblInit = new QLabel("自定义无序数据 (以逗号分隔):", this);
    valueEdit = new QLineEdit(this);
    valueEdit->setPlaceholderText("以逗号分隔，如 1,2,3");
    valueEdit->setText("42,15,8,73,50,22,99,1,34");
    valueEdit->setFixedWidth(200);
    QPushButton* btnLoad = new QPushButton("加载并排序", this);

    sortTypeCombo = new QComboBox(this);
    sortTypeCombo->addItem("快速排序 (默认情况)");
    sortTypeCombo->addItem("堆排序 (递归过深时切换)");
    sortTypeCombo->addItem("插入排序 (元素较少时切换)");
    sortTypeCombo->setStyleSheet("background-color: #3c3c3c; color: white; border: 1px solid #555; padding: 4px; border-radius: 3px; font-weight: bold;");

    inputLayout->addWidget(dataTypeCombo);
    inputLayout->addWidget(sortTypeCombo);
    inputLayout->addWidget(lblInit);
    inputLayout->addWidget(valueEdit);
    inputLayout->addWidget(btnLoad);
    inputLayout->addStretch();

    animLayout->addWidget(inputPanel);
    
    // 分割器：容纳画板与右侧的代码显示面板
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
    std::vector<QuizQuestion> questions = QuizDatabase::getQuestionsFor(QuizDatabase::Topic::Sort, UserProfile::instance().getErrorsForTopic("Sort"));
    SubQuizWidget* quizWidget = new SubQuizWidget(questions, "Sort", this);
    tabWidget->addTab(quizWidget, "巩固练习");

    // 数据类型切换
    connect(dataTypeCombo, &QComboBox::currentIndexChanged, this, [this](int idx){
        if (idx == 0) {
            valueEdit->setPlaceholderText("以逗号分隔，如 1,2,3");
            valueEdit->setText("42,15,8,73,50,22,99,1,34");
            sortTypeCombo->setEnabled(true);
        } else {
            valueEdit->setPlaceholderText("以逗号分隔，如 apple,banana,grape");
            valueEdit->setText("cherry,banana,apple,date,elderberry");
            sortTypeCombo->setEnabled(false); // 字符串演示限定冒泡
        }
    });

    connect(btnLoad, &QPushButton::clicked, this, &VisualSort::handleLoad);

    realData = {42, 15, 8, 73, 50, 22, 99, 1, 34};
    currentStep = 0;
    generateSortSteps(0);
}

void VisualSort::handleLoad() {
    if (dataTypeCombo->currentIndex() == 0) {
        QString text = valueEdit->text().trimmed();
        if (text.isEmpty()) {
            QMessageBox::warning(this, "错误", "输入不能为空！");
            return;
        }

        text.replace(" ", "");
        text.replace("，", ",");
        
        QRegularExpression reg("^-?[0-9]+(,-?[0-9]+)*$");
        if (!reg.match(text).hasMatch()) {
            QMessageBox::warning(this, "格式错误", "请输入以逗号分隔的整数数组，例如：4,1,3,9");
            return;
        }

        QStringList parts = text.split(',');
        std::vector<int> data;
        for (const QString& part : parts) {
            bool ok;
            int val = part.toInt(&ok);
            if (ok) data.push_back(val);
        }

        if (data.size() < 2) {
            QMessageBox::warning(this, "数据量过小", "请至少输入2个以上的数字以进行排序演示。");
            return;
        }
        if (data.size() > 20) {
            QMessageBox::warning(this, "数据量过大", "为确保可视化画板显示效果，请输入20个以内的整数。");
            return;
        }

        realData = data;
        int sortType = sortTypeCombo->currentIndex();
        generateSortSteps(sortType);
        updateCodeBrowser();
        canvas->update();
    } else {
        // 字符串排序处理
        QString text = valueEdit->text().trimmed();
        if (text.isEmpty()) {
            QMessageBox::warning(this, "错误", "输入不能为空！");
            return;
        }

        text.replace("，", ",");
        QStringList parts = text.split(',', Qt::SkipEmptyParts);
        std::vector<QString> data;
        for (const QString& part : parts) {
            QString cleanPart = part.trimmed();
            if (!cleanPart.isEmpty()) data.push_back(cleanPart);
        }

        if (data.size() < 2) {
            QMessageBox::warning(this, "数据量过小", "请至少输入2个以上的单词。");
            return;
        }
        if (data.size() > 10) {
            QMessageBox::warning(this, "数据量过大", "请限制在 10 个以内的单词。");
            return;
        }

        realStringData = data;
        int sortType = sortTypeCombo->currentIndex();
        generateStringSortSteps(sortType);
        updateCodeBrowser();
        canvas->update();
    }
}

void VisualSort::generateSortSteps(int sortType) {
    if (dataTypeCombo->currentIndex() == 1) {
        generateStringSortSteps(sortType);
        return;
    }

    std::vector<int> arr = realData;
    int n = arr.size();
    animSteps.clear();
    currentStep = 0;

    if (sortType == 0) {
        // 快速排序 (Quick Sort)
        QStringList qsortCode = {
            "void quickSort(int low, int high) {",
            "    if (low < high) {",
            "        int pivot = arr[high]; // 选取主元",
            "        int i = partition(arr, low, high);",
            "        quickSort(low, i - 1); // 递归左半部",
            "        quickSort(i + 1, high); // 递归右半部",
            "    }",
            "}"
        };

        animSteps.push_back({arr, -1, -1, "快速排序初始化...", qsortCode, 0});

        std::function<void(int, int)> quickSort = [&](int low, int high) {
            if (low < high) {
                // Partition logic
                int pivot = arr[high];
                int i = low - 1;
                animSteps.push_back({arr, -1, high, QString("选取主元 arr[%1] = %2").arg(high).arg(pivot), qsortCode, 2});

                for (int j = low; j < high; ++j) {
                    animSteps.push_back({arr, j, high, QString("比较 arr[%1] 与主元").arg(j), qsortCode, 3});
                    if (arr[j] <= pivot) {
                        i++;
                        std::swap(arr[i], arr[j]);
                        animSteps.push_back({arr, i, j, QString("交换 arr[%1] 与 arr[%2]").arg(i).arg(j), qsortCode, 3});
                    }
                }
                std::swap(arr[i + 1], arr[high]);
                animSteps.push_back({arr, i + 1, high, QString("将主元交换到正确位置"), qsortCode, 3});
                int pi = i + 1;

                quickSort(low, pi - 1);
                quickSort(pi + 1, high);
            }
        };

        quickSort(0, n - 1);
        animSteps.push_back({arr, -1, -1, "快速排序完成！", qsortCode, 7});

    } else if (sortType == 1) {
        // 堆排序 (Heap Sort)
        QStringList heapCode = {
            "void heapSort() {",
            "    // 1. 建最大堆",
            "    for (int i = n/2 - 1; i >= 0; i--)",
            "        heapify(n, i);",
            "    // 2. 依次取出堆顶",
            "    for (int i = n-1; i > 0; i--) {",
            "        swap(arr[0], arr[i]);",
            "        heapify(i, 0);",
            "    }",
            "}"
        };

        animSteps.push_back({arr, -1, -1, "堆排序初始化...", heapCode, 0});

        std::function<void(int, int)> heapify = [&](int size, int i) {
            int largest = i;
            int l = 2 * i + 1;
            int r = 2 * i + 2;

            if (l < size && arr[l] > arr[largest]) largest = l;
            if (r < size && arr[r] > arr[largest]) largest = r;

            if (largest != i) {
                std::swap(arr[i], arr[largest]);
                animSteps.push_back({arr, i, largest, QString("堆调整: 交换节点 %1 与较大子节点 %2").arg(i).arg(largest), heapCode, 6});
                heapify(size, largest);
            }
        };

        for (int i = n / 2 - 1; i >= 0; i--) {
            heapify(n, i);
        }
        animSteps.push_back({arr, -1, -1, "构建最大堆完成，开始进行堆排序...", heapCode, 3});

        for (int i = n - 1; i > 0; i--) {
            std::swap(arr[0], arr[i]);
            animSteps.push_back({arr, 0, i, QString("交换堆顶元素与当前末尾元素"), heapCode, 6});
            heapify(i, 0);
        }
        animSteps.push_back({arr, -1, -1, "堆排序完成！", heapCode, 9});

    } else {
        // 插入排序 (Insertion Sort)
        QStringList insertCode = {
            "void insertionSort() {",
            "    for (int i = 1; i < n; ++i) {",
            "        int key = arr[i];",
            "        int j = i - 1;",
            "        while (j >= 0 && arr[j] > key) {",
            "            arr[j + 1] = arr[j];",
            "            j--;",
            "        }",
            "        arr[j + 1] = key;",
            "    }",
            "}"
        };

        animSteps.push_back({arr, -1, -1, "插入排序初始化...", insertCode, 0});

        for (int i = 1; i < n; ++i) {
            int key = arr[i];
            int j = i - 1;
            animSteps.push_back({arr, i, -1, QString("当前待插入关键字: %1").arg(key), insertCode, 2});

            while (j >= 0) {
                animSteps.push_back({arr, j, j + 1, QString("比较 arr[%1] 与 待插入值 %2").arg(j).arg(key), insertCode, 4});
                if (arr[j] > key) {
                    arr[j + 1] = arr[j];
                    animSteps.push_back({arr, j, j + 1, QString("元素右移"), insertCode, 5});
                    j--;
                } else {
                    break;
                }
            }
            arr[j + 1] = key;
            animSteps.push_back({arr, j + 1, -1, QString("插入到位置 %1").arg(j + 1), insertCode, 8});
        }
        animSteps.push_back({arr, -1, -1, "插入排序完成！", insertCode, 10});
    }
}

void VisualSort::stepForward() {
    if (currentStep < (int)animSteps.size() - 1) {
        currentStep++;
        updateCodeBrowser();
        canvas->update();
    }
}

void VisualSort::stepBackward() {
    if (currentStep > 0) {
        currentStep--;
        updateCodeBrowser();
        canvas->update();
    }
}

void VisualSort::reset() {
    currentStep = 0;
    updateCodeBrowser();
    canvas->update();
}

bool VisualSort::isAtEnd() const {
    return animSteps.empty() || currentStep >= (int)animSteps.size() - 1;
}

void VisualSort::updateCodeBrowser() {
    if (currentStep >= 0 && currentStep < (int)animSteps.size()) {
        const auto& step = animSteps[currentStep];
        codeBrowser->setHtml(VisualBase::renderCodeWithHighlight(step.codeLines, step.activeLine));
    }
}

void VisualSort::exportImage() {
    QPixmap pixmap = canvas->grab();
    QString fileName = QFileDialog::getSaveFileName(this, "导出截图", "sort_snapshot.png", "PNG 图像 (*.png);;JPEG 图像 (*.jpg)");
    if (!fileName.isEmpty()) {
        pixmap.save(fileName);
    }
}

QString VisualSort::getInfo() const {
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
<h2>std::sort 内省式排序 (Introsort)</h2>
<p><b>std::sort</b> 是 STL 提供的最高效的通用排序算法。许多人误以为它是纯粹的快速排序 (Quick Sort)，但其实它使用的是<b>内省式排序 (Introspective Sort)</b>。</p>

<h3>1. 算法组成</h3>
<p>内省式排序是一种混合排序算法，它结合了以下三种排序：</p>
<ul>
    <li><b>快速排序 (Quick Sort)</b>：常态下使用，因为常数因子极小，速度最快。</li>
    <li><b>堆排序 (Heap Sort)</b>：当快排的递归深度过深（例如遇到了恶意的逆序数组）时，会自动切换为堆排序，从而保证最坏时间复杂度仍然是 <b>O(N log N)</b>。</li>
    <li><b>插入排序 (Insertion Sort)</b>：当递归到子序列长度小于某一阈值（通常为 16）时，停止递归。最后对整个几乎有序的数组执行一次插入排序，这利用了插入排序对"基本有序"数组的高效性。</li>
</ul>

<h3>2. 为什么需要 Random Access Iterator？</h3>
<p><code>std::sort</code> 明确要求传入的迭代器必须是 <b>Random Access Iterator</b> (例如 <code>std::vector</code>, <code>std::deque</code> 的迭代器)。因为它底层需要使用 <code>it + n</code>, <code>it - n</code> 以及三点取中法。像 <code>std::list</code> 就无法使用 <code>std::sort</code>，而是拥有自己专门的 <code>list::sort()</code>。</p>

<h3>3. C++ 底层源码提炼 </h3>
<p>在 GCC 的实现中，核心循环就是下面这段代码。它控制了快排向堆排的退化，以及递归深度的衰减：</p>
<pre>%1</pre>
)").arg(VisualBase::highlightCode(R"(template<typename _RandomAccessIterator, typename _Size, typename _Compare>
void __introsort_loop(_RandomAccessIterator __first,
                      _RandomAccessIterator __last,
                      _Size __depth_limit, _Compare __comp) {
    while (__last - __first > int(_S_threshold)) {
        // 如果递归深度消耗殆尽（达到了 2*log2(N)），说明快排退化了
        if (__depth_limit == 0) {
            // 果断切换为堆排序！
            std::__partial_sort(__first, __last, __last, __comp);
            return;
        }
        --__depth_limit;

        // 快排的核心：三点取中法选主元，然后执行 partition 划分
        _RandomAccessIterator __cut =
            std::__unguarded_partition_pivot(__first, __last, __comp);

        // 对右半部分继续进行 introsort 递归
        std::__introsort_loop(__cut, __last, __depth_limit, __comp);
        
        // 这一步是 GCC 的小优化：不递归左半部，而是通过 while 循环转为迭代，
        // 将 __last 设为 __cut 继续下一轮，从而节省栈空间
        __last = __cut;
    }
})"));
}

// 将画板的重绘委托给外层类的 paintCanvas 方法
void VisualSort::CanvasWidget::paintEvent(QPaintEvent*) {
    p->paintCanvas(this);
}

// 绘制核心逻辑：根据当前步骤(currentStep)画出所有元素
void VisualSort::paintCanvas(QWidget* target) {
    QPainter painter(target);
    painter.setRenderHint(QPainter::Antialiasing);

    if (animSteps.empty()) return;
    const SortAnimStep &step = animSteps[currentStep];

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12));
    painter.drawText(30, 35, "操作进度描述:");
    painter.setPen(QColor("#ffaa00"));
    painter.drawText(30, 58, step.desc);

    if (dataTypeCombo->currentIndex() == 0) {
        // 整数绘制逻辑
        int maxVal = 1;
        for (int v : step.data) {
            if (v > maxVal) maxVal = v;
        }

        int boxW = 40;
        int spacing = 15;
        int startX = 50;
        int bottomY = 300;

        for (size_t i = 0; i < step.data.size(); ++i) {
            int h = (step.data[i] * 200) / maxVal;
            if (h < 20) h = 20;

            int x = startX + i * (boxW + spacing);
            int y = bottomY - h;
            QRect rect(x, y, boxW, h);

            if ((int)i == step.idx1 || (int)i == step.idx2) {
                painter.setBrush(QColor("#00d778")); // highlight comparison/swap
            } else {
                painter.setBrush(QColor("#0e639c")); // normal blue
            }

            painter.setPen(Qt::white);
            painter.drawRect(rect);
            painter.setFont(QFont("Arial", 12, QFont::Bold));
            painter.drawText(QRect(x, y - 25, boxW, 20), Qt::AlignCenter, QString::number(step.data[i]));
        }
    } else {
        // 字符串绘制逻辑 (绘制为小卡片模型)
        int boxW = 85;
        int boxH = 45;
        int spacing = 15;
        int startX = 30;
        int startY = 150;

        for (size_t i = 0; i < step.stringData.size(); ++i) {
            int x = startX + i * (boxW + spacing);
            int y = startY;
            QRect rect(x, y, boxW, boxH);

            if ((int)i == step.idx1 || (int)i == step.idx2) {
                painter.setBrush(QColor("#00d778")); // 绿色高亮
            } else {
                painter.setBrush(QColor("#D77800")); // 橙黄色卡片区分
            }

            painter.setPen(Qt::white);
            painter.drawRoundedRect(rect, 6, 6);
            painter.setFont(QFont("Arial", 11, QFont::Bold));
            painter.drawText(rect, Qt::AlignCenter, step.stringData[i]);
            
            // 绘制索引标记
            painter.setFont(QFont("Arial", 10));
            painter.setPen(Qt::gray);
            painter.drawText(QRect(x, y + boxH + 5, boxW, 20), Qt::AlignCenter, QString("[%1]").arg(i));
            painter.setPen(Qt::white);
        }
    }
}

void VisualSort::generateStringSortSteps(int sortType) {
    Q_UNUSED(sortType); // 字符串演示这里默认展示直观的冒泡排序
    std::vector<QString> arr = realStringData;
    int n = arr.size();
    animSteps.clear();
    currentStep = 0;

    QStringList codeLines = {
        "void bubbleSort(std::vector<string>& arr) {",
        "    int n = arr.size();",
        "    for (int i = 0; i < n - 1; ++i) {",
        "        for (int j = 0; j < n - 1 - i; ++j) {",
        "            if (arr[j] > arr[j+1]) {",
        "                swap(arr[j], arr[j+1]);",
        "            }",
        "        }",
        "    }",
        "}"
    };

    animSteps.push_back({std::vector<int>(), arr, -1, -1, "字符串冒泡排序初始化...", codeLines, 0});

    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - 1 - i; ++j) {
            animSteps.push_back({std::vector<int>(), arr, j, j + 1, QString("比较 \"%1\" 与 \"%2\"").arg(arr[j]).arg(arr[j+1]), codeLines, 4});
            if (arr[j] > arr[j+1]) {
                QString temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
                animSteps.push_back({std::vector<int>(), arr, j, j + 1, QString("交换 \"%1\" 与 \"%2\"").arg(arr[j]).arg(arr[j+1]), codeLines, 5});
            }
        }
    }
    animSteps.push_back({std::vector<int>(), arr, -1, -1, "字符串冒泡排序完成！", codeLines, 9});
}


