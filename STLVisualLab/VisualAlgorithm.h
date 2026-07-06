#ifndef VISUALALGORITHM_H
#define VISUALALGORITHM_H

#include "VisualBase.h"

// 派生自 VisualBase，用于可视化特定 STL 算法或数据结构的类
class VisualAlgorithm : public VisualBase {
    Q_OBJECT
public:
    // 构造函数，初始化 UI 界面布局
    explicit VisualAlgorithm(QWidget *parent = nullptr);
};

#endif // VISUALALGORITHM_H
