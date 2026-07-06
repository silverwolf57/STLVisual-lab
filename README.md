# STL Visual Lab - C++ STL 容器与算法可视化学习平台

[![C++ Version](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Framework](https://img.shields.io/badge/Qt-6.11%2B-green.svg)](https://www.qt.io/)
[![Build Status](https://img.shields.io/badge/Build-Passed-brightgreen.svg)]()
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)]()

这里是北京大学程序设计实习课程第64组的QT大作业，组长是覃耀纬，组员是叶子成和曹梓钊。关于项目的讲解视频详细可见北大网盘链接:https://disk.pku.edu.cn/link/AA582CCD3E002C483B86A470D47FC1F8EB

我们的成果是`STL Visual Lab` ，一款基于 **Qt 6 (C++17)** 开发的**交互式 C++ 容器与算法可视化学习平台**。该平台专门面向 C++ 初学者，旨在解决 STL (Standard Template Library) 底层内存模型、迭代器变化和算法执行流程抽象难懂的痛点。通过**实时动画演示**、**单步调试跟踪（支持撤销）**、**代码高亮对照**以及**智能错题评测编译系统**，帮助用户直观透彻地掌握 STL 的底层运转机理。

---

## 🌟 核心功能

### 1. 📊 主控中心仪表盘 (Dashboard)
- 采用现代科技感暗色扁平化（Fusion Dark）UI 设计，提供“数据结构”与“STL算法”两大核心板块的直观入口，配备精美的高清科技感图示，交互微动效平滑自然。

### 2. 📦 数据结构可视化 (Data Structures)
- **std::vector**：动态演示 `push_back`、`insert` 触发的内存申请、元素拷贝及 `capacity` 动态倍增扩容全过程。
- **std::list**：以图形化节点和动态箭头指针，展示插入、删除时“双向链表断开与重连”的动画细节。
- **std::stack & queue**：动态模拟栈的“后进先出 (LIFO)”与队列的“先进先出 (FIFO)”流水线吞吐流程。

### 3. 🔄 算法流程可视化 (Algorithms)
涵盖 8 种核心 C++ 标准模板库算法的单步动画演练：
- **std::sort**：包含快速排序、插入排序、堆排序等多种底层实现选择。
- **std::lower_bound**：二分查找的双指针区间收缩过程。
- **std::reverse**：首尾双指针交换的对称翻转动画。
- **std::unique**：快慢指针去重覆盖过程。
- **std::merge**：双指针归并两个有序区间的过程。
- **std::remove**：元素前移与逻辑尾部迭代器返回流程。
- **std::rotate**：三阶段翻转旋转算法的优雅步骤。
- **std::next_permutation**：字典序全排列生成动画，全步骤高亮交换数和翻转区间。

### 4. ⚙️ 多功能动画控制器
- 支持自动播放/暂停、单步前进、**单步后退（基于快照栈机制的 Undo 撤销）**、重置动画。
- 支持一键导出当前画布为 `.png` 图片，方便学习者截取并整理学习笔记。

### 5. 📝 智能测验与本地编译沙盒
- 包含 **11 个模块**，总计 **165 道专项练习题**（每个模块 10 道单选题 + 5 道编程实践题）。
- **本地安全编译沙盒**：编程题支持直接在应用内置的编辑器中编写 C++ 代码。后台自动检测系统 PATH 中的 `g++` 编译器，或读取自定义环境变量 `STL_GPP_COMPILER`，将代码写入安全隔离的临时目录进行多线程异步编译运行，不卡顿 UI。
- **安全性防护**：具备 8 秒编译超时和 2 秒执行超时硬性截杀（Timeout Limit）、64KB 输出大小截断上限（Output Limit），防止死循环和内存爆满。

### 6. 🧠 智能错题推荐评测
- 系统自动记录用户的错题历史，并保存到本地的 `user_profile.json` 中。
- 在每次进入小测验时，系统根据错题频次对题库进行**动态重排**，优先展示用户答错次数较多的题目，实现个性化智能复习。

---

## 🛠️ 项目技术栈

- **语言标准**：C++17
- **GUI 框架**：Qt 6.11+ (兼容 Core, Widgets, Gui 模块)
- **编译工具**：qmake / MinGW (g++ 8.0+) 或其他支持 C++17 的编译器
- **绘图引擎**：QPainter (2D 自定义矢量渲染，平滑插值动画)
- **测试环境**：Windows / macOS / Linux 跨平台兼容

---

## 📂 项目文件目录结构

```text
STLVisualLab/
├── STLVisualLab.pro          # Qt 项目配置文件
├── main.cpp                  # 程序入口及深色主题样式定义
├── MainWindow.cpp / .h       # 主窗口控制与导航逻辑
├── VisualBase.h              # 抽象可视化组件基类 (定义 stepForward/stepBackward)
├── VisualVector.cpp / .h     # std::vector 扩容与元素操作可视化
├── VisualList.cpp / .h       # std::list 链表重连可视化
├── VisualStackQueue.cpp / .h # std::stack & std::queue 可视化
├── VisualSort.cpp / .h       # 排序算法可视化
├── VisualLowerBound.cpp / .h # 二分查找区间收缩可视化
├── VisualReverse.cpp / .h    # 逆序翻转可视化
├── VisualUnique.cpp / .h     # 去重算法可视化
├── VisualMerge.cpp / .h      # 归并算法可视化
├── VisualRemove.cpp / .h     # 移出算法可视化
├── VisualRotate.cpp / .h     # 旋转算法可视化
├── VisualPermutation.cpp / .h# next_permutation 可视化
├── SubQuizWidget.cpp / .h    # 测验界面及多线程沙盒编译器接口
├── QuizDatabase.cpp / .h     # 165道单选/编程题数据库
├── UserProfile.cpp / .h      # 本地用户配置与错题频次管理
├── CppHighlighter.h          # 实时 C++ 源码高亮渲染器
├── resources.qrc             # 静态图标资源文件
└── 1-作业报告.md              # 课程设计详细技术报告
``

---

## 🚀 编译与运行指南

### 前提条件
1. 安装 **Qt 6.x** 软件开发包 (带有 Qt Creator 即可)。
2. 确保系统已配置 **MinGW / GCC 编译器** 并加入了环境变量 `PATH`。

### 方法一：使用 Qt Creator 集成开发环境 (推荐)
1. 打开 Qt Creator，选择 `Open File or Project`，并定位到项目中的 `STLVisualLab.pro` 文件。
2. 配置构建套件（Kits），推荐选择带有 `g++` 的 `Desktop Qt 6.x.x MinGW 64-bit`。
3. 点击左下角的 **绿色运行按钮** 编译并启动程序。

### 方法二：使用命令行编译 (Windows PowerShell / Cmd)
打开终端，确保 `qmake` 和 `make` (或 `mingw32-make`) 工具已加入环境变量：

```powershell
# 1. 克隆或进入项目根目录
cd STLVisualLab

# 2. 生成 Makefile
qmake STLVisualLab.pro

# 3. 编译项目 (Windows 下使用 mingw32-make，Linux/macOS 下直接使用 make)
mingw32-make -j8

# 4. 运行可执行程序
./release/STLVisualLab.exe
```

> 💡 **提示**：如果系统无法自动找到 `g++` 编译器，可以在操作系统中设置环境变量 `STL_GPP_COMPILER`，并将其指向您本地 `g++.exe` 的绝对路径（例如 `C:\msys64\mingw64\bin\g++.exe`）。

---

## 🎨 界面演示与视觉预览

- **主仪表盘界面**：Fusion 霓虹深色主题仪表盘。
- **Vector 扩容动画**：清晰展示逻辑空间与物理内存的倍增对比。
- **二分查找区间收缩**：双指针对撞高亮，代码随光标同步跳动。
- **本地编译与错题反馈**：代码编辑器侧边栏实时报错及测试用例拦截。

---

## 📜 许可证

本项目遵循 [MIT License](LICENSE) 许可协议。仅供学术交流与 C++ STL 可视化辅助教学使用。
