#include "QuizDatabase.h"

// 辅助宏，简化选择题定义
#define ADD_CHOICE(qlist, qtext, optA, optB, optC, optD, correctIdx, expl) \
    qlist.push_back({qtext, {optA, optB, optC, optD}, correctIdx, expl, false, "", ""})

std::vector<QuizQuestion> QuizDatabase::getQuestionsFor(Topic topic, const QList<int>& frequentErrors) {
    std::vector<QuizQuestion> q;

    if (topic == Topic::Vector) {
        // --- 10道选择题 ---
        ADD_CHOICE(q, "1. std::vector 的底层物理内存分配方式是？", "链式非连续空间", "连续内存空间", "哈希分块空间", "分块树状空间", 1, "vector 在内存中以连续的数组块进行存储。");
        ADD_CHOICE(q, "2. std::vector 末尾插入元素 (push_back) 的均摊时间复杂度为？", "O(1)", "O(log N)", "O(N)", "O(N log N)", 0, "通常为 O(1)，但如果触发扩容，需要重新分配内存并迁移，均摊后仍为 O(1)。");
        ADD_CHOICE(q, "3. 当 vector 的 size 小于 capacity 时，调用 resize(n) 且 n 小于当前 size，会发生什么？", "重新分配物理内存", "析构尾部多余元素，但 capacity 不变", "清空所有元素", "编译报错", 1, "resize 缩小尺寸时只析构多余元素并改变 size，底层的连续内存块大小（Capacity）绝不缩减。");
        ADD_CHOICE(q, "4. 在 std::vector 中部插入元素 (insert) 的最坏时间复杂度为？", "O(1)", "O(log N)", "O(N)", "O(N^2)", 2, "在中间插入元素时，需要将插入点之后的所有元素向后移动一位，因此最坏复杂度为 O(N)。");
        ADD_CHOICE(q, "5. std::vector 中 at() 与 operator[] 的主要区别是？", "at() 时间复杂度是 O(N)", "operator[] 会自动扩容", "at() 会执行越界检查并抛出异常，而 [] 不进行检查", "二者完全等价", 2, "at() 是安全的，越界时抛出 std::out_of_range，而 [] 为了极佳性能不作安全性检查。");
        ADD_CHOICE(q, "6. vector 触发扩容重分配（reallocate）后，原有的指针、引用和迭代器？", "依然全部有效", "只有头指针有效", "全部失效", "只有迭代器有效", 2, "扩容会申请全新的连续大内存，旧内存块将被释放，因此所有指向旧内存的指针、迭代器与引用都会失效。");
        ADD_CHOICE(q, "7. 调用 reserve(100) 对 std::vector 的 size() 产生什么影响？", "Size 变成 100", "Size 不发生改变", "Size 归零", "Size 加倍", 1, "reserve 只会预留 capacity（物理容量），不会改变容器的 size（逻辑大小），也不会构造任何元素。");
        ADD_CHOICE(q, "8. 如何彻底释放一个 vector 占用的多余 Capacity 物理空间？", "调用 clear()", "调用 resize(0)", "调用 shrink_to_fit() 或与空 vector 交换", "调用 erase", 2, "clear 和 resize 都不缩减 capacity。彻底释放空间可以调用 shrink_to_fit() 或 `vector<int>().swap(v)`。");
        ADD_CHOICE(q, "9. std::vector<bool> 在标准库中的特殊表现是？", "它是一个标准 vector 容器", "它是模板特化，内部通过位压缩存储且不符合标准容器要求", "它不支持随机访问", "它只支持正数", 1, "vector<bool> 是高度特化的空间优化实现（一个 bit 存一个 bool），其迭代器无法返回常规的 bool&，常带来某些模板编译坑。");
        ADD_CHOICE(q, "10. 如果已知所需元素数量，为防止 vector 多次扩容复制，最佳实践是？", "多次调用 push_back", "在插入前使用 reserve() 预留空间", "调用 resize()", "无需任何操作", 1, "预先调用 reserve(N) 一次性申请足量空间，可以完全消除后续 push_back 带来的多次扩容与内存复制开销。");

        // --- 5道编程题 ---
        q.push_back({
            "编程题 1: 计算 std::vector 的所有元素之和。若数组为空，返回 0。",
            QStringList(), -1,
            "int sumVector(const std::vector<int>& v) {\n    int sum = 0;\n    for(int x : v) sum += x;\n    return sum;\n}",
            true,
            "int sumVector(const std::vector<int>& v) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            int sumVector(const std::vector<int>& v);
            int main() {
                if (sumVector({1, 2, 3, 4}) != 10) return 1;
                if (sumVector({}) != 0) return 2;
                if (sumVector({-5, 5}) != 0) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 2: 原地删除 vector 中所有的奇数元素，保持偶数元素的相对顺序不变。",
            QStringList(), -1,
            "void removeOdds(std::vector<int>& v) {\n    v.erase(std::remove_if(v.begin(), v.end(), [](int x){ return x % 2 != 0; }), v.end());\n}",
            true,
            "void removeOdds(std::vector<int>& v) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            void removeOdds(std::vector<int>& v);
            int main() {
                std::vector<int> v1 = {1, 2, 3, 4, 5};
                removeOdds(v1);
                if (v1 != std::vector<int>{2, 4}) return 1;
                std::vector<int> v2 = {1, 3, 5};
                removeOdds(v2);
                if (!v2.empty()) return 2;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 3: 寻找 vector 中的最大元素。若数组为空，返回 -1。",
            QStringList(), -1,
            "int findMax(const std::vector<int>& v) {\n    if (v.empty()) return -1;\n    int mx = v[0];\n    for(int x : v) if(x > mx) mx = x;\n    return mx;\n}",
            true,
            "int findMax(const std::vector<int>& v) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            int findMax(const std::vector<int>& v);
            int main() {
                if (findMax({3, 1, 9, 4}) != 9) return 1;
                if (findMax({}) != -1) return 2;
                if (findMax({-10, -5}) != -5) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 4: 原地将 vector 中每一个元素的值翻倍（例如：[1, 2, 3] 变为 [2, 4, 6]）。",
            QStringList(), -1,
            "void doubleElements(std::vector<int>& v) {\n    for(int &x : v) x *= 2;\n}",
            true,
            "void doubleElements(std::vector<int>& v) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            void doubleElements(std::vector<int>& v);
            int main() {
                std::vector<int> v = {1, 2, 3};
                doubleElements(v);
                if (v != std::vector<int>{2, 4, 6}) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 5: 原地反转一个 vector 元素（例如：[1, 2, 3] 变为 [3, 2, 1]），要求不使用标准库 std::reverse。",
            QStringList(), -1,
            "void reverseVector(std::vector<int>& v) {\n    int l = 0, r = (int)v.size() - 1;\n    while(l < r) {\n        int t = v[l]; v[l] = v[r]; v[r] = t;\n        l++; r--;\n    }\n}",
            true,
            "void reverseVector(std::vector<int>& v) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            void reverseVector(std::vector<int>& v);
            int main() {
                std::vector<int> v = {1, 2, 3, 4};
                reverseVector(v);
                if (v != std::vector<int>{4, 3, 2, 1}) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });
    }
    else if (topic == Topic::List) {
        ADD_CHOICE(q, "1. std::list 的底层数据结构是？", "单向链表", "双向循环链表", "数组", "跳表", 1, "C++ 的 std::list 通常由双向循环链表实现，每个节点都存有 prev 和 next 指针。");
        ADD_CHOICE(q, "2. 在 std::list 的头部插入元素 (push_front) 的时间复杂度为？", "O(1)", "O(log N)", "O(N)", "O(N log N)", 0, "由于双向循环链表直接持有头尾节点的指针，因此头部插入仅需常数时间 O(1)。");
        ADD_CHOICE(q, "3. 对 std::list 执行插入和删除操作后，已有的迭代器？", "全部失效", "除了被删除节点的迭代器失效外，其余仍保持有效", "只有头尾迭代器失效", "没有规律", 1, "链表的物理空间是不连续的，插入/删除节点不影响其他节点的物理地址，因此除被删除节点的迭代器外，其余均不失效。");
        ADD_CHOICE(q, "4. std::list 是否支持 [] 下标随机访问？", "支持，复杂度 O(1)", "支持，复杂度 O(N)", "不支持", "看编译器实现", 2, "链表不支持随机存取，只能从头或尾逐个节点遍历，因此未重载 operator[]。");
        ADD_CHOICE(q, "5. 为何 std::list 内部有自己专用的成员函数 list::sort() 而非调用 std::sort？", "内部算法更快", "std::sort 需要 RandomAccessIterator，而 list 只有 BidirectionalIterator", "list 没有数据域", "语法糖限制", 1, "std::sort 依赖于快速的随机访问特性。list 没有连续的物理地址，因此必须提供专门用于链表的归并排序 sort()。");
        ADD_CHOICE(q, "6. 在 std::list 中查找特定元素最坏的时间复杂度是？", "O(1)", "O(log N)", "O(N)", "O(N log N)", 2, "没有索引和有序查找机制，只能从头到尾进行线性遍历，复杂度为 O(N)。");
        ADD_CHOICE(q, "7. std::list 相对 std::vector 的最大优势是？", "节省内存空间", "支持常数时间 O(1) 的任意位置高效插入/删除", "支持下标随机访问", "支持高速缓存局部性", 1, "链表插入删除仅需改变指针指向，而不需要像 vector 一样大范围搬移元素，是其核心优势。");
        ADD_CHOICE(q, "8. 下列哪个 STL 链表只支持单向遍历？", "std::list", "std::vector", "std::forward_list", "std::deque", 2, "C++11 引入的 forward_list 是单向链表，只存有 next 指针，以最大限度地压缩内存开销。");
        ADD_CHOICE(q, "9. std::list 节点内部的物理地址在内存中？", "必然是连续的", "基本上是连续的", "极大概率是不连续的，分布于堆内存的各个碎片中", "与元素值相关", 2, "节点是在运行时动态申请的，由系统内存分配器随机放置于堆空间上，物理地址极度零散。");
        ADD_CHOICE(q, "10. 调用 list::clear() 后，其底层发生了什么？", "改变了逻辑 size，未释放内存", "逐个节点释放堆空间并调用析构，且释放哨兵节点", "逐个释放数据节点空间并析构，保留哨兵节点且 size 归零", "没有任何开销", 2, "释放所有的元素节点，保留哨兵节点以便下次使用，容器 size 恢复为 0。");

        // 5道编程题
        q.push_back({
            "编程题 1: 原地移除 std::list<int> 中的所有偶数元素。",
            QStringList(), -1,
            "void removeEvens(std::list<int>& lst) {\n    lst.remove_if([](int x){ return x % 2 == 0; });\n}",
            true,
            "void removeEvens(std::list<int>& lst) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <list>
            #include <iostream>
            void removeEvens(std::list<int>& lst);
            int main() {
                std::list<int> lst = {1, 2, 3, 4, 5, 6};
                removeEvens(lst);
                if (lst != std::list<int>{1, 3, 5}) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 2: 统计链表中特定值 val 出现的次数。",
            QStringList(), -1,
            "int countVal(const std::list<int>& lst, int val) {\n    int cnt = 0;\n    for(int x : lst) if(x == val) cnt++;\n    return cnt;\n}",
            true,
            "int countVal(const std::list<int>& lst, int val) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <list>
            #include <iostream>
            int countVal(const std::list<int>& lst, int val);
            int main() {
                std::list<int> lst = {2, 3, 2, 5, 2};
                if (countVal(lst, 2) != 3) return 1;
                if (countVal(lst, 9) != 0) return 2;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 3: 逆序打印链表元素（返回一个逆序后的新 vector 数组）。",
            QStringList(), -1,
            "std::vector<int> reversePrint(const std::list<int>& lst) {\n    std::vector<int> res(lst.begin(), lst.end());\n    std::reverse(res.begin(), res.end());\n    return res;\n}",
            true,
            "std::vector<int> reversePrint(const std::list<int>& lst) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <list>
            #include <vector>
            #include <iostream>
            std::vector<int> reversePrint(const std::list<int>& lst);
            int main() {
                std::list<int> lst = {1, 2, 3};
                std::vector<int> res = reversePrint(lst);
                if (res != std::vector<int>{3, 2, 1}) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 4: 查找链表中第一个值为 val 的元素的索引位置（从 0 开始），找不到则返回 -1。",
            QStringList(), -1,
            "int findIndex(const std::list<int>& lst, int val) {\n    int idx = 0;\n    for(int x : lst) {\n        if(x == val) return idx;\n        idx++;\n    }\n    return -1;\n}",
            true,
            "int findIndex(const std::list<int>& lst, int val) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <list>
            #include <iostream>
            int findIndex(const std::list<int>& lst, int val);
            int main() {
                std::list<int> lst = {10, 20, 30, 20};
                if (findIndex(lst, 20) != 1) return 1;
                if (findIndex(lst, 50) != -1) return 2;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 5: 合并两个已排序的单增链表到一个新的链表中，并保持排序（升序）。",
            QStringList(), -1,
            "std::list<int> mergeLists(std::list<int>& l1, std::list<int>& l2) {\n    std::list<int> res = l1;\n    res.merge(l2);\n    return res;\n}",
            true,
            "std::list<int> mergeLists(std::list<int>& l1, std::list<int>& l2) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <list>
            #include <iostream>
            std::list<int> mergeLists(std::list<int>& l1, std::list<int>& l2);
            int main() {
                std::list<int> l1 = {1, 3, 5};
                std::list<int> l2 = {2, 4};
                std::list<int> res = mergeLists(l1, l2);
                if (res != std::list<int>{1, 2, 3, 4, 5}) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });
    }
    else if (topic == Topic::StackQueue) {
        ADD_CHOICE(q, "1. 栈 (std::stack) 的核心数据访问原则是？", "先进先出 (FIFO)", "后进先出 (LIFO)", "随机访问", "按值大小排列", 1, "栈是 LIFO (Last In First Out) 适配器，最后压入的数据最先被弹出。");
        ADD_CHOICE(q, "2. 队列 (std::queue) 的核心数据访问原则是？", "先进先出 (FIFO)", "后进先出 (LIFO)", "随机访问", "按优先级顺序", 0, "队列是 FIFO (First In First Out) 适配器，最早写入的数据最先被移出。");
        ADD_CHOICE(q, "3. 栈和队列在 C++ 标准库中被归类为？", "序列容器 (Sequence Containers)", "容器适配器 (Container Adapters)", "关联容器 (Associative Containers)", "无序容器", 1, "它们不直接实现底层物理存储，而是通过封装别的容器（默认是 std::deque）来实现特定逻辑接口，因此是容器适配器。");
        ADD_CHOICE(q, "4. std::stack 的默认底层基础容器是？", "std::vector", "std::list", "std::deque", "std::array", 2, "C++ 的 stack 和 queue 默认都用 std::deque 作为底层基础，支持双端高效增删。");
        ADD_CHOICE(q, "5. 为什么 std::vector 不太适合直接作为 std::queue 的底层容器？", "不支持末尾插入", "头部删除 (pop_front) 需要 O(N) 搬移代价，效率太低", "会造成死锁", "不支持随机访问", 1, "队列需要 pop_front() 操作。而 vector 没有 pop_front 接口，在头部删除元素代价是 O(N) 的。");
        ADD_CHOICE(q, "6. C++ 的 std::stack 为何没有直接提供 clear() 成员函数？", "库设计遗漏", "为了保持 LIFO 的极致纯粹与语义精简，防止越权操作", "底层实现冲突", "历史包袱", 1, "栈和队列为了保持标准的受限物理形态，只允许读写顶部/头部，不支持大范围的直接清空。可以用 `s = {}` 来一键清空。");
        ADD_CHOICE(q, "7. 双栈模拟队列时，如果输入栈有 1,2,3 压入，经过输出栈后，弹出的顺序是？", "3, 2, 1", "1, 2, 3", "2, 3, 1", "无规律", 1, "1,2,3 在入栈1后是 [1,2,3(顶)]。倒入栈2后变为 [3,2,1(顶)]，依次弹出即为 1,2,3，完美还原了队列 FIFO 语义。");
        ADD_CHOICE(q, "8. 下列关于 std::priority_queue (优先队列) 的表述，正确的是？", "底层以链表为基础", "本质上是一个堆 (Heap) 结构，默认是大顶堆", "只能是 FIFO 访问", "支持中途修改权重", 1, "优先队列底层默认使用 `std::vector` + `std::make_heap` 算法实现堆，默认优先级最高的元素最先出队。");
        ADD_CHOICE(q, "9. 栈的入栈 push() 和出栈 pop() 的时间复杂度都是？", "O(1)", "O(log N)", "O(N)", "O(N log N)", 0, "入栈和出栈都只在尾部/顶部进行操作，执行的是底层容器常数级别的 pop_back/push_back。");
        ADD_CHOICE(q, "10. 如果想要快速用空适配器重置清空一个 stack<int> s，最优雅的方式是？", "循环调用 pop()", "s = std::stack<int>();", "调用 s.clear()", "无法清空", 1, "`s = std::stack<int>();` 利用移动赋值将一个全新干净的临时栈赋给 s，是常数时间清空的最快做法。");

        // 5道编程题
        q.push_back({
            "编程题 1: 利用 std::stack 将输入的字符串反转（例如：\"hello\" 变为 \"olleh\"）。",
            QStringList(), -1,
            "std::string reverseString(std::string s) {\n    std::stack<char> st;\n    for(char c : s) st.push(c);\n    std::string res = \"\";\n    while(!st.empty()) {\n        res += st.top();\n        st.pop();\n    }\n    return res;\n}",
            true,
            "std::string reverseString(std::string s) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <stack>
            #include <string>
            #include <iostream>
            std::string reverseString(std::string s);
            int main() {
                if (reverseString("abc") != "cba") return 1;
                if (reverseString("") != "") return 2;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 2: 利用 std::stack 校验括号匹配。仅包含 '(' 和 ')'，括号必须完全闭合配对（如 \"(())\" 合法，\"())\" 不合法）。",
            QStringList(), -1,
            "bool isBalanced(std::string s) {\n    std::stack<char> st;\n    for(char c : s) {\n        if(c == '(') st.push(c);\n        else {\n            if(st.empty()) return false;\n            st.pop();\n        }\n    }\n    return st.empty();\n}",
            true,
            "bool isBalanced(std::string s) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <stack>
            #include <string>
            #include <iostream>
            bool isBalanced(std::string s);
            int main() {
                if (!isBalanced("(())")) return 1;
                if (isBalanced("(()")) return 2;
                if (isBalanced(")")) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 3: 用一个队列将数据倒入另一个栈（输入一个 queue，将其所有元素倒入一个 stack 中并返回）。",
            QStringList(), -1,
            "std::stack<int> queueToStack(std::queue<int>& q) {\n    std::stack<int> s;\n    while(!q.empty()) {\n        s.push(q.front());\n        q.pop();\n    }\n    return s;\n}",
            true,
            "std::stack<int> queueToStack(std::queue<int>& q) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <queue>
            #include <stack>
            #include <iostream>
            std::stack<int> queueToStack(std::queue<int>& q);
            int main() {
                std::queue<int> q;
                q.push(1); q.push(2);
                std::stack<int> s = queueToStack(q);
                if (s.size() != 2 || s.top() != 2) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 4: 寻找栈中最小的值，要求保持原栈结构不变（利用辅助栈/容器恢复）。若为空返回 -1。",
            QStringList(), -1,
            "int getStackMin(std::stack<int>& s) {\n    if(s.empty()) return -1;\n    std::vector<int> tmp;\n    int mn = s.top();\n    while(!s.empty()) {\n        int val = s.top();\n        if(val < mn) mn = val;\n        tmp.push_back(val);\n        s.pop();\n    }\n    for(auto it = tmp.rbegin(); it != tmp.rend(); ++it) s.push(*it);\n    return mn;\n}",
            true,
            "int getStackMin(std::stack<int>& s) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <stack>
            #include <vector>
            #include <iostream>
            int getStackMin(std::stack<int>& s);
            int main() {
                std::stack<int> s;
                s.push(10); s.push(5); s.push(20);
                if (getStackMin(s) != 5) return 1;
                if (s.top() != 20) return 2; // 检查原栈结构是否完整还原
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 5: 约瑟夫环模拟：n 个人（标号 0 到 n-1）围成一圈，从 0 开始报数，每报到第 m 个人（报数 1 到 m）出局，求最后留下来的人的编号（利用 queue 实现）。",
            QStringList(), -1,
            "int josephus(int n, int m) {\n    std::queue<int> q;\n    for(int i = 0; i < n; ++i) q.push(i);\n    while(q.size() > 1) {\n        for(int i = 1; i < m; ++i) {\n            q.push(q.front());\n            q.pop();\n        }\n        q.pop();\n    }\n    return q.front();\n}",
            true,
            "int josephus(int n, int m) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <queue>
            #include <iostream>
            int josephus(int n, int m);
            int main() {
                if (josephus(5, 3) != 3) return 1; // 0,1,2,3,4 中出队顺序: 2, 0, 4, 1, 剩 3
                if (josephus(6, 2) != 4) return 2;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });
    }
    else if (topic == Topic::Sort) {
        ADD_CHOICE(q, "1. C++ 标准库中的 std::sort 底层采用的复合排序算法是？", "快速排序 + 归并排序", "内省排序 (Introsort)", "双轴快排", "红黑树排序", 1, "Introsort 结合了快速排序（大部分情况）、堆排序（深度过深退化时以维持最坏 O(N log N)）以及插入排序（小数组优化）。");
        ADD_CHOICE(q, "2. std::stable_sort (稳定排序) 底层通常首选的算法是？", "快速排序", "归并排序", "基数排序", "希尔排序", 1, "归并排序（Merge Sort）是稳定的，且 stable_sort 会尝试申请额外内存作为缓冲区。");
        ADD_CHOICE(q, "3. 快速排序在最坏情况下的时间复杂度是？", "O(N)", "O(N log N)", "O(N^2)", "O(N^3)", 2, "如果每次划分都分到了最极端的边界（例如已排序数组且选端点为主元），复杂度退化为 O(N^2)。");
        ADD_CHOICE(q, "4. 堆排序 (Heap Sort) 空间复杂度是？", "O(1)", "O(log N)", "O(N)", "O(N log N)", 0, "堆排序是原地排序（In-place），只需要常数个辅助变量，空间复杂度为 O(1)。");
        ADD_CHOICE(q, "5. 插入排序适用于什么样的数据特征？", "随机分布的大数据", "完全逆序的数据", "基本有序的小数组", "具有浮点数的数据", 2, "在接近有序或元素极少时，插入排序执行的移动非常少，耗时接近线性 O(N)，效率极高。");
        ADD_CHOICE(q, "6. 下列排序算法中，哪一个是稳定的？", "快速排序", "堆排序", "选择排序", "归并排序", 3, "归并排序在合并相等的元素时保持了相对位置，是稳定的。而快排、堆排、选择排序都是非稳定的。");
        ADD_CHOICE(q, "7. 快速排序采用的算法思想是？", "贪心算法", "动态规划", "分治法 (Divide and Conquer)", "暴力搜索", 2, "通过 Pivot 将数组划分为左右两个独立子区间，分别递归解决，属于经典的分治法思想。");
        ADD_CHOICE(q, "8. 对 8 个元素进行堆排序，构建初始最大堆的时间复杂度是？", "O(1)", "O(N)", "O(N log N)", "O(N^2)", 1, "通过自底向上的下滤建堆（Floyd建堆法），时间复杂度严格证明为线性 O(N)。");
        ADD_CHOICE(q, "9. std::sort 要求传入的迭代器类型至少是？", "Forward Iterator", "Bidirectional Iterator", "Random Access Iterator", "Input Iterator", 2, "因为要进行大跨度的双向随机跳跃（计算 pivot），必须支持随机访问迭代器。");
        ADD_CHOICE(q, "10. 选择排序 (Selection Sort) 的核心设计逻辑是？", "逐个将未排序元素挪入已排序区间", "每次扫描未排序部分以锁定极值并放到首端", "两两交换相邻元素", "利用哈希映射", 1, "每次遍历在剩余区间选出一个最小（或最大）元素，与当前区间的首元素交换位置。");

        // 5道编程题
        q.push_back({
            "编程题 1: 校验给定数组是否有序（升序）。",
            QStringList(), -1,
            "bool isSorted(const std::vector<int>& arr) {\n    for(size_t i = 1; i < arr.size(); ++i) {\n        if(arr[i] < arr[i-1]) return false;\n    }\n    return true;\n}",
            true,
            "bool isSorted(const std::vector<int>& arr) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            bool isSorted(const std::vector<int>& arr);
            int main() {
                if (!isSorted({1, 3, 5, 5, 9})) return 1;
                if (isSorted({1, 3, 2})) return 2;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 2: 实现经典的冒泡排序算法（原地修改数组）。",
            QStringList(), -1,
            "void bubbleSort(std::vector<int>& arr) {\n    int n = arr.size();\n    for(int i = 0; i < n-1; ++i) {\n        for(int j = 0; j < n-1-i; ++j) {\n            if(arr[j] > arr[j+1]) {\n                int t = arr[j]; arr[j] = arr[j+1]; arr[j+1] = t;\n            }\n        }\n    }\n}",
            true,
            "void bubbleSort(std::vector<int>& arr) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            void bubbleSort(std::vector<int>& arr);
            int main() {
                std::vector<int> arr = {4, 1, 3, 9, 7};
                bubbleSort(arr);
                if (arr != std::vector<int>{1, 3, 4, 7, 9}) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 3: 实现经典的选择排序算法（原地修改数组）。",
            QStringList(), -1,
            "void selectionSort(std::vector<int>& arr) {\n    int n = arr.size();\n    for(int i = 0; i < n-1; ++i) {\n        int minIdx = i;\n        for(int j = i+1; j < n; ++j) {\n            if(arr[j] < arr[minIdx]) minIdx = j;\n        }\n        if(minIdx != i) {\n            int t = arr[i]; arr[i] = arr[minIdx]; arr[minIdx] = t;\n        }\n    }\n}",
            true,
            "void selectionSort(std::vector<int>& arr) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            void selectionSort(std::vector<int>& arr);
            int main() {
                std::vector<int> arr = {5, 2, 8, 3, 1};
                selectionSort(arr);
                if (arr != std::vector<int>{1, 2, 3, 5, 8}) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 4: 寻找数组中第 K 大的元素（例如：k=1 表示寻找最大元素）。",
            QStringList(), -1,
            "int findKthLargest(std::vector<int>& arr, int k) {\n    std::sort(arr.begin(), arr.end());\n    return arr[arr.size() - k];\n}",
            true,
            "int findKthLargest(std::vector<int>& arr, int k) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            int findKthLargest(std::vector<int>& arr, int k);
            int main() {
                std::vector<int> arr = {3, 2, 1, 5, 6, 4};
                if (findKthLargest(arr, 2) != 5) return 1;
                if (findKthLargest(arr, 1) != 6) return 2;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 5: 统计数组中的逆序对个数（即满足 i < j 且 arr[i] > arr[j] 的对数）。",
            QStringList(), -1,
            "int countInversions(const std::vector<int>& arr) {\n    int cnt = 0;\n    int n = arr.size();\n    for(int i = 0; i < n; ++i) {\n        for(int j = i+1; j < n; ++j) {\n            if(arr[i] > arr[j]) cnt++;\n        }\n    }\n    return cnt;\n}",
            true,
            "int countInversions(const std::vector<int>& arr) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            int countInversions(const std::vector<int>& arr);
            int main() {
                if (countInversions({2, 4, 3, 1}) != 4) return 1; // (2,1), (4,3), (4,1), (3,1)
                if (countInversions({1, 2, 3}) != 0) return 2;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });
    }
    else if (topic == Topic::LowerBound) {
        ADD_CHOICE(q, "1. std::lower_bound 查找的要求是？", "数组必须无序", "数组必须已排序（升序）", "数组大小必须为偶数", "必须是双向链表", 1, "它使用二分查找法，前提是查找区间必须呈有序排布。");
        ADD_CHOICE(q, "2. std::lower_bound 寻找到的目标值满足什么条件？", "大于目标值的第一个元素", "小于目标值的最后一个元素", "大于或等于 (≥) 目标值的第一个元素", "完全等于目标值的元素", 2, "返回的是第一个不小于（即大于或等于）给定值的元素迭代器。");
        ADD_CHOICE(q, "3. 如果查找区间中所有元素都严格小于目标值，lower_bound 将返回？", "指向首元素的迭代器", "指向末尾有效元素的迭代器", "区间的尾后迭代器 (last/end)", "空指针", 2, "若无满足条件的元素，则收缩至区间末端，返回尾后迭代器（如 `v.end()`）。");
        ADD_CHOICE(q, "4. 对 std::vector 使用 lower_bound，其时间复杂度为？", "O(1)", "O(log N)", "O(N)", "O(N log N)", 1, "vector 支持随机访问迭代器，折半比较与迭代器跳跃均为常数级，总复杂度为 O(log N)。");
        ADD_CHOICE(q, "5. 对 std::list 使用 std::lower_bound，其效率表现为？", "O(log N) 比较，O(log N) 移动", "O(log N) 比较，O(N) 移动", "编译报错", "O(1)", 1, "链表不支持随机存取，虽然折半比较次数是 O(log N)，但迭代器内部做 `++` 推进的次数是线性 O(N)。");
        ADD_CHOICE(q, "6. std::upper_bound 与 std::lower_bound 的核心区别是？", "返回类型不同", "lower_bound 找的是第一个大于等于目标值的，而 upper_bound 找的是第一个严格大于目标值的", "只针对降序", "二者算法完全不同", 1, "如目标是3，在 [1, 3, 3, 5] 中，lower_bound 返回索引1（第一个3），upper_bound 返回索引3（元素5）。");
        ADD_CHOICE(q, "7. 二分查找循环中，`len >> 1` 代表？", "将区间右移 1 位，相当于除以 2 且速度极快", "将区间乘 2", "取余数", "溢出重置", 0, "这是位运算优化，右移一位相当于除以 2，速度明显快于常规的整除 `/2`。");
        ADD_CHOICE(q, "8. 用 binary search 查找 [1, 2, 4, 8]，查找目标 5，最终收敛迭代器指向？", "2", "4", "8", "1", 2, "大于或等于 5 的第一个元素是 8（索引 3）。");
        ADD_CHOICE(q, "9. GCC 底层的 lower_bound 循环使用 (len > 0) 相比经典 (left <= right) 的好处是？", "代码行数多", "彻底规避了 `(left + right) / 2` 计算时大整数加法溢出整型的潜在风险", "只在堆内存中生效", "支持降序", 1, "直接利用 `first` 加上 `half` 偏移量定位中点，避免直接加两个大数组下标。");
        ADD_CHOICE(q, "10. 如果要在有序 vector 中定位某个元素是否存在并返回布尔值，标准库对应的算法是？", "std::find", "std::binary_search", "std::lower_bound", "std::adjacent_find", 1, "`std::binary_search` 返回布尔值，在底层也是基于 `std::lower_bound` 判定实现的。");

        // 5道编程题
        q.push_back({
            "编程题 1: 实现标准的折半查找。若元素存在返回其索引，不存在则返回 -1（升序数组）。",
            QStringList(), -1,
            "int binarySearch(const std::vector<int>& arr, int target) {\n    int l = 0, r = (int)arr.size() - 1;\n    while(l <= r) {\n        int mid = l + (r - l) / 2;\n        if(arr[mid] == target) return mid;\n        if(arr[mid] < target) l = mid + 1;\n        else r = mid - 1;\n    }\n    return -1;\n}",
            true,
            "int binarySearch(const std::vector<int>& arr, int target) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            int binarySearch(const std::vector<int>& arr, int target);
            int main() {
                std::vector<int> arr = {1, 3, 5, 7, 9};
                if (binarySearch(arr, 5) != 2) return 1;
                if (binarySearch(arr, 4) != -1) return 2;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 2: 手写实现 std::lower_bound：返回升序数组中第一个大于或等于 target 的元素索引，若都小于它则返回数组大小。",
            QStringList(), -1,
            "int myLowerBound(const std::vector<int>& arr, int target) {\n    int first = 0, len = arr.size();\n    while(len > 0) {\n        int half = len >> 1;\n        int mid = first + half;\n        if(arr[mid] < target) {\n            first = mid + 1;\n            len = len - half - 1;\n        } else {\n            len = half;\n        }\n    }\n    return first;\n}",
            true,
            "int myLowerBound(const std::vector<int>& arr, int target) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            int myLowerBound(const std::vector<int>& arr, int target);
            int main() {
                std::vector<int> arr = {1, 3, 3, 5, 7};
                if (myLowerBound(arr, 3) != 1) return 1;
                if (myLowerBound(arr, 4) != 3) return 2;
                if (myLowerBound(arr, 9) != 5) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 3: 手写实现 std::upper_bound：返回升序数组中第一个严格大于 target 的元素索引，若均不满足则返回数组大小。",
            QStringList(), -1,
            "int myUpperBound(const std::vector<int>& arr, int target) {\n    int first = 0, len = arr.size();\n    while(len > 0) {\n        int half = len >> 1;\n        int mid = first + half;\n        if(!(target < arr[mid])) {\n            first = mid + 1;\n            len = len - half - 1;\n        } else {\n            len = half;\n        }\n    }\n    return first;\n}",
            true,
            "int myUpperBound(const std::vector<int>& arr, int target) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            int myUpperBound(const std::vector<int>& arr, int target);
            int main() {
                std::vector<int> arr = {1, 3, 3, 5, 7};
                if (myUpperBound(arr, 3) != 3) return 1; // 索引3的元素是5
                if (myUpperBound(arr, 7) != 5) return 2;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 4: 给定有序（升序）数组，求特定目标元素 target 在数组中出现的次数，要求利用二分查找实现以达到 O(log N) 的效率。",
            QStringList(), -1,
            "int countOccurrences(const std::vector<int>& arr, int target) {\n    auto l = std::lower_bound(arr.begin(), arr.end(), target);\n    auto r = std::upper_bound(arr.begin(), arr.end(), target);\n    return r - l;\n}",
            true,
            "int countOccurrences(const std::vector<int>& arr, int target) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            int countOccurrences(const std::vector<int>& arr, int target);
            int main() {
                std::vector<int> arr = {1, 2, 2, 2, 3, 4};
                if (countOccurrences(arr, 2) != 3) return 1;
                if (countOccurrences(arr, 9) != 0) return 2;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 5: 给定有序（升序）无重复元素的数组，若将 target 插入其中并继续保持有序，返回其应该插入的正确索引位置。",
            QStringList(), -1,
            "int searchInsert(const std::vector<int>& arr, int target) {\n    return std::lower_bound(arr.begin(), arr.end(), target) - arr.begin();\n}",
            true,
            "int searchInsert(const std::vector<int>& arr, int target) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            int searchInsert(const std::vector<int>& arr, int target);
            int main() {
                std::vector<int> arr = {1, 3, 5, 6};
                if (searchInsert(arr, 5) != 2) return 1;
                if (searchInsert(arr, 2) != 1) return 2;
                if (searchInsert(arr, 7) != 4) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });
    }
    else if (topic == Topic::Reverse) {
        ADD_CHOICE(q, "1. std::reverse 反转操作的时间复杂度是？", "O(1)", "O(log N)", "O(N)", "O(N log N)", 2, "算法准确执行 N/2 次首尾交换，因此总时间复杂度为 O(N)。");
        ADD_CHOICE(q, "2. std::reverse 的空间复杂度是？", "O(1)", "O(log N)", "O(N)", "O(N^2)", 0, "它在原有数组上就地修改，只需要常数个辅助变量，空间复杂度为 O(1)。");
        ADD_CHOICE(q, "3. std::reverse 接受的迭代器最低要求是？", "Input Iterator", "Forward Iterator", "Bidirectional Iterator", "Random Access Iterator", 2, "算法需要向后递增（++first）且向前递减（--last）以向中间靠拢，因此最低要求是双向迭代器。");
        ADD_CHOICE(q, "4. 为什么单链表容器 std::forward_list 无法调用 std::reverse？", "因为它只支持向后遍历，不满足 BidirectionalIterator 的双向移动要求", "因为元素太多", "没有分配空间", "类型冲突", 0, "单向链表仅支持 ++ 迭代，无法进行 --last 的回退移动，因此不能使用此模板。");
        ADD_CHOICE(q, "5. 下列哪个数据结构可以直接完美使用 std::reverse？", "std::list", "std::stack", "std::queue", "std::priority_queue", 0, "std::list 的迭代器属于双向迭代器（支持 ++ 与 --），故可直接使用该算法。适配器类不支持迭代器访问。");
        ADD_CHOICE(q, "6. 反转半开区间 `[first, last)` 对应的 C++ 底层终止条件判定是？", "first == last", "first == last || first == --last", "first > last", "first.next == last", 1, "如果是偶数个元素，两指针在中间相遇时 `first == last`；若是奇数，`first` 与自减后的 `last` 重合时 `first == --last`，两者皆终止。");
        ADD_CHOICE(q, "7. 原地交换迭代器指向的两个元素的底层 STL 函数是？", "std::swap", "std::iter_swap", "std::copy", "std::move", 1, "`std::iter_swap` 专为交换两个迭代器指向的内容而设计。");
        ADD_CHOICE(q, "8. 对一个大小为 5 的数组进行 reverse，共执行了几次元素交换？", "5", "4", "2", "3", 2, "执行 N/2 也就是 5/2 = 2 次首尾元素交换。");
        ADD_CHOICE(q, "9. std::reverse_copy 的作用是？", "把数组反转两遍", "不改变原数组，将反转结果拷贝输出到另一个目标容器中", "原地反转并拷贝", "清空数组", 1, "reverse_copy 是非破坏性算法，源区间不变，反转结果顺序写入到目标输出迭代器中。");
        ADD_CHOICE(q, "10. 如果需要对 vector 里的某一部分子区间（如索引 1 到 4）进行翻转，调用方式是？", "std::reverse(v.begin(), v.end())", "std::reverse(v.begin() + 1, v.begin() + 5)", "std::reverse(v.begin() + 1, v.begin() + 4)", "std::reverse(v.begin(), v.begin() + 4)", 1, "半开区间表示为 `[v.begin() + 1, v.begin() + 5)`，对应的即是元素 1, 2, 3, 4。");

        // 5道编程题
        q.push_back({
            "编程题 1: 原地翻转给定的整型数组（不使用 std::reverse，手写双指针交换）。",
            QStringList(), -1,
            "void myReverse(std::vector<int>& arr) {\n    int l = 0, r = (int)arr.size() - 1;\n    while(l < r) {\n        std::swap(arr[l], arr[r]);\n        l++; r--;\n    }\n}",
            true,
            "void myReverse(std::vector<int>& arr) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            void myReverse(std::vector<int>& arr);
            int main() {
                std::vector<int> arr = {1, 2, 3, 4, 5};
                myReverse(arr);
                if (arr != std::vector<int>{5, 4, 3, 2, 1}) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 2: 判断给定数组是否是“回文数”对称数组（如 [1, 2, 2, 1] 属于回文）。要求使用双指针实现。",
            QStringList(), -1,
            "bool isPalindrome(const std::vector<int>& arr) {\n    int l = 0, r = (int)arr.size() - 1;\n    while(l < r) {\n        if(arr[l] != arr[r]) return false;\n        l++; r--;\n    }\n    return true;\n}",
            true,
            "bool isPalindrome(const std::vector<int>& arr) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            bool isPalindrome(const std::vector<int>& arr);
            int main() {
                if (!isPalindrome({1, 2, 3, 2, 1})) return 1;
                if (isPalindrome({1, 2, 3, 4})) return 2;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 3: 将数组循环左移 k 位（例如：[1, 2, 3, 4, 5] 左移 2 位变为 [3, 4, 5, 1, 2]）。",
            QStringList(), -1,
            "void rotateLeft(std::vector<int>& arr, int k) {\n    int n = arr.size();\n    if (n == 0) return;\n    k %= n;\n    std::reverse(arr.begin(), arr.begin() + k);\n    std::reverse(arr.begin() + k, arr.end());\n    std::reverse(arr.begin(), arr.end());\n}",
            true,
            "void rotateLeft(std::vector<int>& arr, int k) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            void rotateLeft(std::vector<int>& arr, int k);
            int main() {
                std::vector<int> arr = {1, 2, 3, 4, 5};
                rotateLeft(arr, 2);
                if (arr != std::vector<int>{3, 4, 5, 1, 2}) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 4: 将数组循环右移 k 位（例如：[1, 2, 3, 4, 5] 右移 2 位变为 [4, 5, 1, 2, 3]）。",
            QStringList(), -1,
            "void rotateRight(std::vector<int>& arr, int k) {\n    int n = arr.size();\n    if (n == 0) return;\n    k %= n;\n    std::reverse(arr.begin(), arr.end());\n    std::reverse(arr.begin(), arr.begin() + k);\n    std::reverse(arr.begin() + k, arr.end());\n}",
            true,
            "void rotateRight(std::vector<int>& arr, int k) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            void rotateRight(std::vector<int>& arr, int k);
            int main() {
                std::vector<int> arr = {1, 2, 3, 4, 5};
                rotateRight(arr, 2);
                if (arr != std::vector<int>{4, 5, 1, 2, 3}) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 5: 原地翻转给定的字符串 `std::string`，不申请额外空间。",
            QStringList(), -1,
            "void reverseStringInPlace(std::string& s) {\n    int l = 0, r = (int)s.size() - 1;\n    while(l < r) {\n        char t = s[l]; s[l] = s[r]; s[r] = t;\n        l++; r--;\n    }\n}",
            true,
            "void reverseStringInPlace(std::string& s) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <string>
            #include <iostream>
            void reverseStringInPlace(std::string& s);
            int main() {
                std::string s = "antigravity";
                reverseStringInPlace(s);
                if (s != "ytivargitna") return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });
    }
    else if (topic == Topic::Unique) {
        ADD_CHOICE(q, "1. std::unique 算法的主要作用是？", "彻底删除数组中的所有重复元素，减小容器大小", "去重相邻的重复元素，将不重复的元素覆盖移动到前面", "自动对其进行升序排序", "将重复元素移到末尾并自动调用析构", 1, "unique 只去重“相邻”的重复元素，它是将后面不重复的覆盖写入前面，并不缩减容器的物理容量。");
        ADD_CHOICE(q, "2. 为了能够对包含任意重复元素的 vector 彻底去重，通常应先调用？", "std::reverse", "std::sort", "std::merge", "std::unique", 1, "由于 unique 只进行相邻去重，非相邻的重复无法识别。因此必须先通过 sort 使相同的元素挨在一起。");
        ADD_CHOICE(q, "3. std::unique 返回的迭代器指向？", "去重后首个元素的迭代器", "去重后最后一个有效元素之后的位置（即逻辑新尾后位置）", "容器本身的 end()", "原数组末尾", 1, "返回的是新的逻辑终点迭代器。原数组从该迭代器到 `end()` 的内容为无用的垃圾数据。");
        ADD_CHOICE(q, "4. 彻底删除 unique 过滤后的尾部冗余“垃圾元素”的经典 STL 惯用法是？", "Erase-Remove 惯用法：`v.erase(std::unique(v.begin(), v.end()), v.end());`", "调用 `v.clear()`", "调用 `v.resize(0)`", "无需调用任何接口，自动完成", 0, "这是 STL 的经典组合拳，利用 vector::erase 将 unique 返回的逻辑尾后到物理尾后区间的无效元素彻底清除。");
        ADD_CHOICE(q, "5. std::unique 算法的时间复杂度是？", "O(1)", "O(log N)", "O(N)", "O(N log N)", 2, "算法只需要对区间进行一次单向扫描，时间复杂度为 O(N)。");
        ADD_CHOICE(q, "6. std::unique 底层所使用的核心算法设计是？", "双指针快慢指针就地覆盖", "二分查找", "额外开辟一块辅助内存空间", "多路归并", 0, "使用一个慢指针 `dest` 指向当前去重后的有效数组边界，快指针 `first` 扫描整个区间，不相等时前移并覆盖写入。");
        ADD_CHOICE(q, "7. 如果对已排序数组 `[1, 1, 2, 2, 3]` 调用 unique，去重后前三个元素是？", "1, 2, 3", "1, 1, 2", "2, 2, 3", "3, 2, 1", 0, "前三个会被覆盖写为不重复的 1, 2, 3，后面的元素则为遗留的失效数据。");
        ADD_CHOICE(q, "8. std::unique 的空间复杂度是？", "O(1)", "O(log N)", "O(N)", "O(N log N)", 0, "它是一次扫描原地修改，空间复杂度为常数级 O(1)。");
        ADD_CHOICE(q, "9. std::unique_copy 算法的主要功能是？", "删除重复元素并返回真假", "不修改原区间，只将去重后的序列复制输出到另一个目标容器", "自动在链表上完成删除", "无此函数", 1, "它是非破坏性的拷贝版，将原区间相邻去重后的序列输出至目标迭代器。");
        ADD_CHOICE(q, "10. 如果直接对 `[2, 1, 2, 3]` 执行 unique (未先排序)，结果是？", "2, 1, 3", "2, 1, 2, 3", "1, 2, 3", "崩溃", 1, "因为没有相同的相邻元素，去重无效果，结果依然是 2, 1, 2, 3。");

        // 5道编程题
        q.push_back({
            "编程题 1: 对已排序的 vector 执行去重，并彻底删除尾部的无用冗余数据（实现 Erase-Remove 惯用法）。",
            QStringList(), -1,
            "void removeDuplicates(std::vector<int>& v) {\n    v.erase(std::unique(v.begin(), v.end()), v.end());\n}",
            true,
            "void removeDuplicates(std::vector<int>& v) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            void removeDuplicates(std::vector<int>& v);
            int main() {
                std::vector<int> v = {1, 1, 2, 2, 3};
                removeDuplicates(v);
                if (v != std::vector<int>{1, 2, 3}) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 2: 给定已排序的数组，求其中有多少个不重复的元素，要求不使用 std::unique（手写双指针统计）。",
            QStringList(), -1,
            "int countUnique(const std::vector<int>& arr) {\n    if (arr.empty()) return 0;\n    int cnt = 1;\n    for(size_t i = 1; i < arr.size(); ++i) {\n        if(arr[i] != arr[i-1]) cnt++;\n    }\n    return cnt;\n}",
            true,
            "int countUnique(const std::vector<int>& arr) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            int countUnique(const std::vector<int>& arr);
            int main() {
                if (countUnique({1, 1, 2, 2, 3}) != 3) return 1;
                if (countUnique({}) != 0) return 2;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 3: 给定已排序数组，除某一个元素仅出现一次外，其他所有元素均出现了两次，请找出这个仅出现一次的元素。",
            QStringList(), -1,
            "int findSingleNumber(const std::vector<int>& arr) {\n    for(size_t i = 0; i < arr.size(); i += 2) {\n        if(i == arr.size() - 1 || arr[i] != arr[i+1]) return arr[i];\n    }\n    return -1;\n}",
            true,
            "int findSingleNumber(const std::vector<int>& arr) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            int findSingleNumber(const std::vector<int>& arr);
            int main() {
                if (findSingleNumber({1, 1, 2, 2, 3, 4, 4}) != 3) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 4: 删除字符串中相邻的重复字符（例如：\"aabbcc\" 变为 \"abc\"），要求就地修改字符串。",
            QStringList(), -1,
            "void removeConsecutiveDuplicates(std::string& s) {\n    s.erase(std::unique(s.begin(), s.end()), s.end());\n}",
            true,
            "void removeConsecutiveDuplicates(std::string& s) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <string>
            #include <algorithm>
            #include <iostream>
            void removeConsecutiveDuplicates(std::string& s);
            int main() {
                std::string s = "aaabbbccc";
                removeConsecutiveDuplicates(s);
                if (s != "abc") return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 5: 给定一个数组，将所有的 0 移动到数组的末尾，同时保持非零元素的相对顺序不变（原地修改，要求复杂度为 O(N) 且不使用辅助数组）。",
            QStringList(), -1,
            "void moveZeroes(std::vector<int>& arr) {\n    int dest = 0;\n    for(int x : arr) {\n        if(x != 0) {\n            arr[dest++] = x;\n        }\n    }\n    while(dest < (int)arr.size()) {\n        arr[dest++] = 0;\n    }\n}",
            true,
            "void moveZeroes(std::vector<int>& arr) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            void moveZeroes(std::vector<int>& arr);
            int main() {
                std::vector<int> arr = {0, 1, 0, 3, 12};
                moveZeroes(arr);
                if (arr != std::vector<int>{1, 3, 12, 0, 0}) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });
    }
    else if (topic == Topic::Merge) {
        ADD_CHOICE(q, "1. std::merge 操作的前提是？", "两个数组必须完全无序", "两个输入区间必须都是已经排序的（升序）", "两个数组长度必须一致", "输出容器必须已满", 1, "merge 依赖于输入区间的有序性，它是经典的二路归并算法步。");
        ADD_CHOICE(q, "2. std::merge 合并两个大小分别为 N1 和 N2 的数组，最坏比较次数是？", "O(N1 * N2)", "N1 + N2 - 1", "log(N1 + N2)", "N1 * log N2", 1, "双指针线性比较，在最坏情况下，会执行 N1 + N2 - 1 次元素比较。");
        ADD_CHOICE(q, "3. std::merge 的稳定性表现是？", "不稳定的", "稳定的，若元素相等则优先拷贝前一个区间的元素", "取决于容器类型", "随机的", 1, "merge 是稳定的。当两个输入区间中有相同的元素值时，第一区间的元素始终优先写入结果，保证其原有顺序。");
        ADD_CHOICE(q, "4. std::merge 算法在合并时是否允许输入区间与目标输出区间重叠？", "允许", "不允许，行为未定义", "只允许一部分重叠", "取决于内存大小", 1, "标准规定，目标输出区间不能与任何一个输入区间发生重叠，否则会造成读写冲突和未定义行为。");
        ADD_CHOICE(q, "5. 如果你想在同一个数组中就地合并两个相邻的已排序区间，应该调用？", "std::merge", "std::inplace_merge", "std::sort", "std::copy", 1, "`std::inplace_merge` 是专门为同一容器中相邻有序片段的就地归并设计的，它会自动尝试申请临时缓存以优化性能。");
        ADD_CHOICE(q, "6. std::merge 要求输入的迭代器类型至少是？", "Input Iterator", "Forward Iterator", "Bidirectional Iterator", "Random Access Iterator", 0, "它只需要单向递增读取数据，因此最低要求是输入迭代器（Input Iterator），输出为输出迭代器。");
        ADD_CHOICE(q, "7. 归并排序 (Merge Sort) 算法的核心支撑步骤是？", "Partition 划分", "Merge 归并", "建堆 heapify", "二分查找", 1, "归并排序的步骤是：先分治递归左右部分，然后再调用归并操作将左右两个有序半区组合成大有序区。");
        ADD_CHOICE(q, "8. 合并 `[1, 3, 5]` 和 `[2, 4]`，当 i1 指向 3，i2 指向 2 时，下一个被放入结果的元素是？", "3", "2", "4", "5", 1, "比较 3 和 2，由于 2 较小，2 优先被移入结果数组，i2 前进。");
        ADD_CHOICE(q, "9. std::merge 的时间复杂度为？", "O(N1 + N2)", "O(N log N)", "O(N^2)", "O(1)", 0, "对两个区间各进行一次线性扫描，因此时间复杂度为 O(N1 + N2)。");
        ADD_CHOICE(q, "10. 如果目标容器在 merge 前没有预先 resize 分配好足够的空间，我们需要调用什么来动态写入？", "std::back_inserter", "v.begin()", "v.end()", "std::front_inserter", 0, "若目标容器大小不足，直接传入 `begin()` 会导致越界崩溃。传入 `std::back_inserter(v)` 可以将写入操作重定向为 `push_back` 自动扩容。");

        // 5道编程题
        q.push_back({
            "编程题 1: 合并两个已排序数组到第三个数组中（手写双指针归并）。",
            QStringList(), -1,
            "std::vector<int> mergeArrays(const std::vector<int>& a, const std::vector<int>& b) {\n    std::vector<int> res;\n    int i = 0, j = 0;\n    while(i < (int)a.size() && j < (int)b.size()) {\n        if(a[i] <= b[j]) res.push_back(a[i++]);\n        else res.push_back(b[j++]);\n    }\n    while(i < (int)a.size()) res.push_back(a[i++]);\n    while(j < (int)b.size()) res.push_back(b[j++]);\n    return res;\n}",
            true,
            "std::vector<int> mergeArrays(const std::vector<int>& a, const std::vector<int>& b) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            std::vector<int> mergeArrays(const std::vector<int>& a, const std::vector<int>& b);
            int main() {
                std::vector<int> a = {1, 3, 5};
                std::vector<int> b = {2, 4, 6};
                std::vector<int> res = mergeArrays(a, b);
                if (res != std::vector<int>{1, 2, 3, 4, 5, 6}) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 2: 就地归并相邻的两个已排序子区间：数组前 part1Size 个元素已排序，后半段也已排序，原地归并使整个数组升序。要求不使用辅助数组之外的方法（空间 O(N)）。",
            QStringList(), -1,
            "void inplaceMerge(std::vector<int>& arr, int part1Size) {\n    std::inplace_merge(arr.begin(), arr.begin() + part1Size, arr.end());\n}",
            true,
            "void inplaceMerge(std::vector<int>& arr, int part1Size) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            void inplaceMerge(std::vector<int>& arr, int part1Size);
            int main() {
                std::vector<int> arr = {1, 4, 5, 2, 3}; // 前 3 个有序，后 2 个有序
                inplaceMerge(arr, 3);
                if (arr != std::vector<int>{1, 2, 3, 4, 5}) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 3: 求两个已排序（升序）数组的“交集”元素（相同元素仅保留一份，结果保持升序）。要求使用双指针达到 O(N) 的时间复杂度。",
            QStringList(), -1,
            "std::vector<int> getIntersection(const std::vector<int>& a, const std::vector<int>& b) {\n    std::vector<int> res;\n    int i = 0, j = 0;\n    while(i < (int)a.size() && j < (int)b.size()) {\n        if(a[i] == b[j]) {\n            if(res.empty() || res.back() != a[i]) res.push_back(a[i]);\n            i++; j++;\n        } else if(a[i] < b[j]) i++;\n        else j++;\n    }\n    return res;\n}",
            true,
            "std::vector<int> getIntersection(const std::vector<int>& a, const std::vector<int>& b) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            std::vector<int> getIntersection(const std::vector<int>& a, const std::vector<int>& b);
            int main() {
                std::vector<int> a = {1, 2, 2, 4};
                std::vector<int> b = {2, 2, 3, 4};
                std::vector<int> res = getIntersection(a, b);
                if (res != std::vector<int>{2, 4}) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 4: 求两个已排序（升序）数组的“并集”元素（相同元素去重，结果保持升序）。要求使用双指针达到 O(N) 的时间复杂度。",
            QStringList(), -1,
            "std::vector<int> getUnion(const std::vector<int>& a, const std::vector<int>& b) {\n    std::vector<int> res;\n    int i = 0, j = 0;\n    auto add = [&](int x) {\n        if(res.empty() || res.back() != x) res.push_back(x);\n    };\n    while(i < (int)a.size() && j < (int)b.size()) {\n        if(a[i] == b[j]) { add(a[i]); i++; j++; }\n        else if(a[i] < b[j]) { add(a[i++]); }\n        else { add(b[j++]); }\n    }\n    while(i < (int)a.size()) add(a[i++]);\n    while(j < (int)b.size()) add(b[j++]);\n    return res;\n}",
            true,
            "std::vector<int> getUnion(const std::vector<int>& a, const std::vector<int>& b) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            std::vector<int> getUnion(const std::vector<int>& a, const std::vector<int>& b);
            int main() {
                std::vector<int> a = {1, 2, 3};
                std::vector<int> b = {2, 3, 4};
                std::vector<int> res = getUnion(a, b);
                if (res != std::vector<int>{1, 2, 3, 4}) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 5: 给定一个包含若干个闭区间（例如：[[1,3], [2,6], [8,10]]）的 vector，合并所有重叠的区间并返回（已按起点升序排好序，如重叠合并为 [[1,6], [8,10]]）。",
            QStringList(), -1,
            "std::vector<std::pair<int,int>> mergeIntervals(const std::vector<std::pair<int,int>>& intervals) {\n    std::vector<std::pair<int,int>> res;\n    if(intervals.empty()) return res;\n    res.push_back(intervals[0]);\n    for(size_t i = 1; i < intervals.size(); ++i) {\n        auto& last = res.back();\n        if(intervals[i].first <= last.second) {\n            last.second = std::max(last.second, intervals[i].second);\n        } else {\n            res.push_back(intervals[i]);\n        }\n    }\n    return res;\n}",
            true,
            "std::vector<std::pair<int,int>> mergeIntervals(const std::vector<std::pair<int,int>>& intervals) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <utility>
            #include <algorithm>
            #include <iostream>
            std::vector<std::pair<int,int>> mergeIntervals(const std::vector<std::pair<int,int>>& intervals);
            int main() {
                std::vector<std::pair<int,int>> intervals = {{1, 3}, {2, 6}, {8, 10}};
                auto res = mergeIntervals(intervals);
                if (res.size() != 2 || res[0].first != 1 || res[0].second != 6 || res[1].first != 8) return 1;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });
    }
    else if (topic == Topic::Permutation) {
        ADD_CHOICE(q, "1. std::next_permutation 的作用是什么？", "将区间按字典序降序排列", "将区间重排为下一个字典序排列，若已是最大排列则回绕到最小排列并返回 false", "随机打乱区间元素", "找到前一个字典序排列", 1, "next_permutation 将区间就地修改为下一个字典序排列。若当前已是最大排列，则重置为最小排列（升序）并返回 false。");
        ADD_CHOICE(q, "2. std::next_permutation 对迭代器类型的最低要求是？", "Input Iterator", "Forward Iterator", "Bidirectional Iterator", "Random Access Iterator", 2, "算法需要从右往左扫描（回退）以找到降序断点，必须支持双向迭代器。");
        ADD_CHOICE(q, "3. 对排列 [3, 2, 1] 调用 next_permutation，返回值和区间变化是？", "返回 true，区间变为 [3, 2, 1]", "返回 false，区间变为 [1, 2, 3]", "返回 true，区间变为 [1, 2, 3]", "编译错误", 1, "[3,2,1] 已是三元素的最大字典序排列，因此返回 false 并将区间重置为最小排列 [1,2,3]。");
        ADD_CHOICE(q, "4. 若要枚举 {1,2,3} 的所有 6 个全排列，必须先？", "将数组随机打乱", "将数组排成最大序（降序）", "将数组排成最小序（升序）", "无需预处理", 2, "next_permutation 从当前排列出发生成下一个。若从升序（最小）开始，连续调用直到返回 false，性能最佳，且能遍历所有 N! 个排列。");
        ADD_CHOICE(q, "5. next_permutation 内部第一步从右往左找到什么？", "第一个比右侧相邻元素大的元素（降序破坏点 i）", "第一个比右侧相邻元素小的元素（升序破坏点 i）", "数组的中间点", "最大元素", 1, "从尾部往左扫描，找到第一个满足 arr[i] < arr[i+1] 的下标 i（降序断点）。若不存在，则当前已是最大排列。");
        ADD_CHOICE(q, "6. 找到断点 i 后，下一步是？", "直接将 i 右侧区间整体翻转", "在 i+1 到末尾中找第一个比 arr[i] 大的元素 j，与 arr[i] 交换，再翻转 i+1 到末尾", "删除 arr[i]，在后面插入最大元素", "将 arr[i] 移到队首", 1, "第二步：在 [i+1, end) 中从右往左找第一个比 arr[i] 大的元素 j，交换 arr[i] 与 arr[j]；第三步：翻转 [i+1, end) 使其变为最小序。");
        ADD_CHOICE(q, "7. next_permutation 的时间复杂度是？", "O(1)", "O(log N)", "O(N)", "O(N log N)", 2, "扫描、交换和翻转三步的合计最坏时间都是线性 O(N)。");
        ADD_CHOICE(q, "8. std::prev_permutation 与 next_permutation 的关系是？", "二者完全相同", "prev_permutation 生成字典序上的前一个排列，交易与 next_permutation 方向相反", "prev_permutation 只适用于降序区间", "prev_permutation 返回类型不同", 1, "prev_permutation 生成前一个字典序排列；若当前已是最小排列则重置为最大排列并返回 false。");
        ADD_CHOICE(q, "9. 对含重复元素的数组 [1,1,2] 调用 next_permutation，将枚举多少个不同排列？", "6 个", "3 个", "2 个", "无法枚举", 1, "含重复元素时，[1,1,2] 共有 3!/2! = 3 个不同排列：[1,1,2]、[1,2,1]、[2,1,1]。next_permutation 会自动跳过重复情况。");
        ADD_CHOICE(q, "10. 正确枚举字符串所有排列的方式是？", "对字符串直接调用 std::sort 后打印", "先将字符串排序，然后在 do { ... } while(next_permutation(...)) 循环中打印", "调用 next_permutation 一次即可得到所有排列", "只能手写递归回溯", 1, "标准用法：先排序得到最小字典序，再用 do-while 循环调用 next_permutation，直到它返回 false 为止。");

        // --- 5道编程题 ---
        q.push_back({
            "编程题 1: 枚举数组的所有全排列，将每个排列以 vector 形式存入结果数组（按字典序升序排列）。",
            QStringList(), -1,
            "std::vector<std::vector<int>> allPermutations(std::vector<int> arr) {\n    std::sort(arr.begin(), arr.end());\n    std::vector<std::vector<int>> res;\n    do {\n        res.push_back(arr);\n    } while (std::next_permutation(arr.begin(), arr.end()));\n    return res;\n}",
            true,
            "std::vector<std::vector<int>> allPermutations(std::vector<int> arr) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            std::vector<std::vector<int>> allPermutations(std::vector<int> arr);
            int main() {
                auto res = allPermutations({1, 2, 3});
                if (res.size() != 6) return 1;
                if (res[0] != std::vector<int>{1, 2, 3}) return 2;
                if (res[5] != std::vector<int>{3, 2, 1}) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 2: 给定整数数组，找到其下一个字典序排列；若为最大排列则返回最小排列（升序）。",
            QStringList(), -1,
            "std::vector<int> nextPerm(std::vector<int> arr) {\n    std::next_permutation(arr.begin(), arr.end());\n    return arr;\n}",
            true,
            "std::vector<int> nextPerm(std::vector<int> arr) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            std::vector<int> nextPerm(std::vector<int> arr);
            int main() {
                if (nextPerm({1, 2, 3}) != std::vector<int>{1, 3, 2}) return 1;
                if (nextPerm({3, 2, 1}) != std::vector<int>{1, 2, 3}) return 2;
                if (nextPerm({1, 1, 2}) != std::vector<int>{1, 2, 1}) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 3: 统计数组中有多少个不同的全排列（利用 next_permutation 计数，不使用额外 set）。",
            QStringList(), -1,
            "int countPermutations(std::vector<int> arr) {\n    std::sort(arr.begin(), arr.end());\n    int cnt = 0;\n    do {\n        cnt++;\n    } while (std::next_permutation(arr.begin(), arr.end()));\n    return cnt;\n}",
            true,
            "int countPermutations(std::vector<int> arr) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            int countPermutations(std::vector<int> arr);
            int main() {
                if (countPermutations({1, 2, 3}) != 6) return 1;
                if (countPermutations({1, 1, 2}) != 3) return 2;
                if (countPermutations({1, 1, 1}) != 1) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 4: 手写实现 next_permutation 算法（不调用标准库），返回是否成功找到下一个排列（false 表示已是最大排列并已翻转回最小）。",
            QStringList(), -1,
            "bool myNextPermutation(std::vector<int>& arr) {\n    int n = (int)arr.size();\n    int i = n - 2;\n    while (i >= 0 && arr[i] >= arr[i+1]) i--;\n    if (i < 0) {\n        std::reverse(arr.begin(), arr.end());\n        return false;\n    }\n    int j = n - 1;\n    while (arr[j] <= arr[i]) j--;\n    std::swap(arr[i], arr[j]);\n    std::reverse(arr.begin() + i + 1, arr.end());\n    return true;\n}",
            true,
            "bool myNextPermutation(std::vector<int>& arr) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            bool myNextPermutation(std::vector<int>& arr);
            int main() {
                std::vector<int> a1 = {1, 2, 3}; if (!myNextPermutation(a1) || a1 != std::vector<int>{1, 3, 2}) return 1;
                std::vector<int> a2 = {3, 2, 1}; if (myNextPermutation(a2) || a2 != std::vector<int>{1, 2, 3}) return 2;
                std::vector<int> a3 = {1, 3, 2}; if (!myNextPermutation(a3) || a3 != std::vector<int>{2, 1, 3}) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 5: 给定字符串，按字典序返回所有不重复全排列（结果按字典序升序排列）。",
            QStringList(), -1,
            "std::vector<std::string> stringPermutations(std::string s) {\n    std::sort(s.begin(), s.end());\n    std::vector<std::string> res;\n    do {\n        res.push_back(s);\n    } while (std::next_permutation(s.begin(), s.end()));\n    return res;\n}",
            true,
            "std::vector<std::string> stringPermutations(std::string s) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <string>
            #include <algorithm>
            #include <iostream>
            std::vector<std::string> stringPermutations(std::string s);
            int main() {
                auto r1 = stringPermutations("abc");
                if (r1.size() != 6 || r1[0] != "abc" || r1[5] != "cba") return 1;
                auto r2 = stringPermutations("aab");
                if (r2.size() != 3 || r2[0] != "aab" || r2[2] != "baa") return 2;
                auto r3 = stringPermutations("a");
                if (r3.size() != 1 || r3[0] != "a") return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });
    }

    else if (topic == Topic::Remove) {
        ADD_CHOICE(q, "1. std::remove 的作用是什么？", "删除容器中所有等于 val 的元素并释放内存", "将序列中不等于 val 的元素移动 to 前面，并返回新的逻辑尾部", "清空整个容器", "统计等于 val 的元素重复次数", 1, "std::remove 并不真正删除元素，而是将保留的元素移动到前面，返回新的逻辑尾部迭代器，需要配合 erase 才能释放空间。");
        ADD_CHOICE(q, "2. Erase-Remove 惯用法的正确写法是？", "v.erase(v.begin(), v.end())", "v.erase(std::remove(v.begin(), v.end(), val), v.end())", "std::remove(v.begin(), v.end(), val)", "v.remove(val)", 1, "两步算法缺一不可：remove 只移动元素，erase 才能删除尾部冗余，合并两步才能真正删除。");
        ADD_CHOICE(q, "3. std::remove 的时间复杂度是？", "O(1)", "O(log N)", "O(N)", "O(N^2)", 2, "进行一次线性扫描，使用双指针技巧。");
        ADD_CHOICE(q, "4. std::remove 要求的最低迭代器是？", "Input Iterator", "Forward Iterator", "Bidirectional Iterator", "Random Access Iterator", 1, "remove 只要求 Forward Iterator，因为只需要向前读取和写入。");
        ADD_CHOICE(q, "5. std::remove 的返回值是什么？", "void", "bool", "指向新逻辑尾部的迭代器", "int", 2, "返回指向新逻辑尾部（第一个冗余元素所在位置）的迭代器。");
        ADD_CHOICE(q, "6. 调用 std::remove 后容器的大小如何变化？", "size 缩小", "capacity 缩小", "size 和 capacity 均不改变，需要 erase 才会改变 size", "size 增大但 capacity 不变", 2, "remove 只移动元素，不能改变容器结构，size 和 capacity 均不变。");
        ADD_CHOICE(q, "7. std::remove_if 与 std::remove 的区别是？", "remove_if 移除满足谓词条件的元素，remove 移除特定固定值的元素", "两者完全相同", "remove_if 效率更低", "remove_if 只适用于 vector", 0, "remove 按值过滤，remove_if 按谓词（函数/仿函数/lambda）过滤。");
        ADD_CHOICE(q, "8. std::remove 执行后尾部冗余元素的状态是？", "全部为 0", "仍有原来的值，处于有效但未指定的状态", "全为最大值", "抛出异常", 1, "remove 将保留元素移到前面，尾部元素仍然存在于内存中，处于有效但未指定的状态。");
        ADD_CHOICE(q, "9. 关于 Erase-Remove 哪个顺序是正确的？", "erase 和 remove 可以互换顺序", "remove 先整理数据，erase 删除最末端的冗余", "remove 会触发内存重新分配", "先调用 erase 再调用 remove", 1, "正确顺序是先 remove 整理元素，再由 erase 将尾部冗余删掉。");
        ADD_CHOICE(q, "10. std::list 是否推荐使用 std::remove？", "可以，与 vector 用法相同", "推荐使用 list 自己的 remove 成员函数，效率更高", "list 的 remove 效率较低", "不支持", 1, "list 有自己专用的 remove 成员函数（直接操作节点指针），效率更高。");

        q.push_back({
            "编程题 1: 从 vector 中移除所有特定值的元素，使用 erase-remove 惯用法。",
            QStringList(), -1,
            "void removeValue(std::vector<int>& v, int val) {\n    v.erase(std::remove(v.begin(), v.end(), val), v.end());\n}",
            true,
            "void removeValue(std::vector<int>& v, int val) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            void removeValue(std::vector<int>& v, int val);
            int main() {
                std::vector<int> v1 = {1, 2, 3, 2, 4}; removeValue(v1, 2); if (v1 != std::vector<int>{1, 3, 4}) return 1;
                std::vector<int> v2 = {}; removeValue(v2, 5); if (!v2.empty()) return 2;
                std::vector<int> v3 = {5, 5, 5}; removeValue(v3, 5); if (!v3.empty()) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 2: 从 vector 中移除所有偶数，使用 remove_if 和 lambda 表达式。",
            QStringList(), -1,
            "void removeEvens(std::vector<int>& v) {\n    v.erase(std::remove_if(v.begin(), v.end(), [](int x){ return x % 2 == 0; }), v.end());\n}",
            true,
            "void removeEvens(std::vector<int>& v) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            void removeEvens(std::vector<int>& v);
            int main() {
                std::vector<int> v1 = {1, 2, 3, 4, 5, 6}; removeEvens(v1); if (v1 != std::vector<int>{1, 3, 5}) return 1;
                std::vector<int> v2 = {2, 4, 6}; removeEvens(v2); if (!v2.empty()) return 2;
                std::vector<int> v3 = {1, 3, 5}; removeEvens(v3); if (v3 != std::vector<int>{1, 3, 5}) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 3: 从 vector 中移除所有小于 0 的负数。",
            QStringList(), -1,
            "void removeNegatives(std::vector<int>& v) {\n    v.erase(std::remove_if(v.begin(), v.end(), [](int x){ return x < 0; }), v.end());\n}",
            true,
            "void removeNegatives(std::vector<int>& v) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            void removeNegatives(std::vector<int>& v);
            int main() {
                std::vector<int> v1 = {-1, 2, -3, 4, -5}; removeNegatives(v1); if (v1 != std::vector<int>{2, 4}) return 1;
                std::vector<int> v2 = {-1, -2, -3}; removeNegatives(v2); if (!v2.empty()) return 2;
                std::vector<int> v3 = {1, 2, 3}; removeNegatives(v3); if (v3 != std::vector<int>{1, 2, 3}) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 4: 手动实现 remove 算法（双指针），不使用 std::remove。",
            QStringList(), -1,
            "int myRemove(std::vector<int>& arr, int val) {\n    int dest = 0;\n    for (int x : arr) {\n        if (x != val) {\n            arr[dest++] = x;\n        }\n    }\n    return dest;\n}",
            true,
            "int myRemove(std::vector<int>& arr, int val) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            int myRemove(std::vector<int>& arr, int val);
            int main() {
                std::vector<int> a1 = {1, 2, 3, 2, 4}; int n1 = myRemove(a1, 2); if (n1 != 3 || a1[0] != 1 || a1[1] != 3 || a1[2] != 4) return 1;
                std::vector<int> a2 = {}; int n2 = myRemove(a2, 5); if (n2 != 0) return 2;
                std::vector<int> a3 = {5, 5, 5}; int n3 = myRemove(a3, 5); if (n3 != 0) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 5: 将所有的 0 移动到数组末尾，同时保持非零元素相对顺序不变（双指针原地修改）。",
            QStringList(), -1,
            "void moveZeroes(std::vector<int>& arr) {\n    int dest = 0;\n    for (int x : arr) {\n        if (x != 0) arr[dest++] = x;\n    }\n    while (dest < (int)arr.size()) arr[dest++] = 0;\n}",
            true,
            "void moveZeroes(std::vector<int>& arr) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <iostream>
            void moveZeroes(std::vector<int>& arr);
            int main() {
                std::vector<int> a1 = {0, 1, 0, 3, 12}; moveZeroes(a1); if (a1 != std::vector<int>{1, 3, 12, 0, 0}) return 1;
                std::vector<int> a2 = {}; moveZeroes(a2); if (!a2.empty()) return 2;
                std::vector<int> a3 = {0, 0, 5}; moveZeroes(a3); if (a3 != std::vector<int>{5, 0, 0}) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });
    }

    else if (topic == Topic::Rotate) {
        ADD_CHOICE(q, "1. std::rotate 的功能是什么？", "逆转整个数组", "将 [mid, last) 的元素移到前面，[first, mid) 的元素移到后面", "随机打乱数组元素", "将元素计数并排序", 1, "rotate 将 [mid, last) 的元素移到前面，[first, mid) 的元素移到后面。");
        ADD_CHOICE(q, "2. std::rotate 使用的经典算法是？", "三次翻转", "三次翻转法（Three-Reverse）", "三路法", "动态规划", 1, "三次翻转法使 rotate 以 O(N) 时间、O(1) 空间实现。");
        ADD_CHOICE(q, "3. 对大小为 N 的数组，std::rotate 的时间复杂度是？", "O(1)", "O(log N)", "O(N)", "O(N log N)", 2, "rotate 确切执行 N 次交换，时间复杂度 O(N)。");
        ADD_CHOICE(q, "4. std::rotate 要求的最低迭代器是？", "Input Iterator", "Forward Iterator", "Bidirectional Iterator", "Output Iterator", 1, "基础版本只需要 Forward Iterator，但 RandomAccessIterator 有优化版本。");
        ADD_CHOICE(q, "5. 若 mid == first 或 mid == last，std::rotate 的行为是？", "抛出异常", "未定义行为", "什么也不做，直接返回", "崩溃", 2, "若 mid 等于 first 或 last，不需要旋转，直接返回。");
        ADD_CHOICE(q, "6. std::rotate 的返回值是什么？", "void", "指向原数组第一个元素的迭代器", "指向旋转后原 first 位置元素的新位置的迭代器", "bool", 2, "返回指向旋转前 first 位置元素旋转后新位置的迭代器。");
        ADD_CHOICE(q, "7. std::rotate 的空间复杂度是？", "O(1)", "O(N)", "O(log N)", "O(N^2)", 0, "使用三次翻转，原地操作，只需要常数级空间。");
        ADD_CHOICE(q, "8. 以下哪种容器无法直接使用 std::rotate？", "std::vector", "std::list", "std::deque", "std::forward_list", 3, "forward_list 是单向链表，不支持三次翻转法所需的 -- 操作。");
        ADD_CHOICE(q, "9. 旋转数组 [1,2,3,4,5]，mid 指向索引3（元素4），结果是？", "[3,4,5,1,2]", "[4,5,1,2,3]", "[1,2,4,5,3]", "[5,4,3,2,1]", 1, "[mid,last)=[4,5] 移到前面，[first,mid)=[1,2,3] 移到后面，结果为 [4,5,1,2,3]。");
        ADD_CHOICE(q, "10. 三次翻转法实现 rotate 的正确顺序是？", "reverse(0,n); reverse(0,mid); reverse(mid,n)", "reverse(0,mid); reverse(mid,n); reverse(0,n)", "reverse(0,mid); reverse(0,n); reverse(mid,n)", "reverse(mid,n); reverse(0,mid); reverse(0,n)", 1, "先翻转前段，再翻转后段，最后翻转整体，三步结合得到旋转效果。");

        q.push_back({
            "编程题 1: 使用 std::rotate 将数组的后半段元素移到前面。",
            QStringList(), -1,
            "void rotateByMid(std::vector<int>& v) {\n    int mid = v.size() / 2;\n    std::rotate(v.begin(), v.begin() + mid, v.end());\n}",
            true,
            "void rotateByMid(std::vector<int>& v) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            void rotateByMid(std::vector<int>& v);
            int main() {
                std::vector<int> v1 = {1, 2, 3, 4, 5, 6}; rotateByMid(v1); if (v1 != std::vector<int>{4, 5, 6, 1, 2, 3}) return 1;
                std::vector<int> v2 = {1, 2}; rotateByMid(v2); if (v2 != std::vector<int>{2, 1}) return 2;
                std::vector<int> v3 = {1}; rotateByMid(v3); if (v3 != std::vector<int>{1}) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 2: 手动实现三次翻转的 rotate 算法，不使用 std::rotate。",
            QStringList(), -1,
            "void myRotate(std::vector<int>& v, int k) {\n    k = k % (int)v.size();\n    std::reverse(v.begin(), v.begin() + k);\n    std::reverse(v.begin() + k, v.end());\n    std::reverse(v.begin(), v.end());\n}",
            true,
            "void myRotate(std::vector<int>& v, int k) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            void myRotate(std::vector<int>& v, int k);
            int main() {
                std::vector<int> v1 = {1, 2, 3, 4, 5}; myRotate(v1, 2); if (v1 != std::vector<int>{3, 4, 5, 1, 2}) return 1;
                std::vector<int> v2 = {1, 2, 3}; myRotate(v2, 0); if (v2 != std::vector<int>{1, 2, 3}) return 2;
                std::vector<int> v3 = {1, 2, 3, 4}; myRotate(v3, 6); if (v3 != std::vector<int>{3, 4, 1, 2}) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 3: 数组循环右移 k 位（每个元素向右移动 k 个位置）。",
            QStringList(), -1,
            "void rotateRight(std::vector<int>& v, int k) {\n    if (v.empty()) return;\n    k = k % (int)v.size();\n    std::reverse(v.begin(), v.end());\n    std::reverse(v.begin(), v.begin() + k);\n    std::reverse(v.begin() + k, v.end());\n}",
            true,
            "void rotateRight(std::vector<int>& v, int k) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            void rotateRight(std::vector<int>& v, int k);
            int main() {
                std::vector<int> v1 = {1, 2, 3, 4, 5, 6, 7}; rotateRight(v1, 3); if (v1 != std::vector<int>{5, 6, 7, 1, 2, 3, 4}) return 1;
                std::vector<int> v2 = {-1, -100, 3, 99}; rotateRight(v2, 2); if (v2 != std::vector<int>{3, 99, -1, -100}) return 2;
                std::vector<int> v3 = {1, 2}; rotateRight(v3, 1); if (v3 != std::vector<int>{2, 1}) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 4: 判断两个数组是否互为旋转关系。",
            QStringList(), -1,
            "bool areRotations(const std::vector<int>& a, const std::vector<int>& b) {\n    if (a.size() != b.size()) return false;\n    if (a.empty()) return true;\n    std::vector<int> doubled = a;\n    doubled.insert(doubled.end(), a.begin(), a.end());\n    auto it = std::search(doubled.begin(), doubled.end(), b.begin(), b.end());\n    return it != doubled.end();\n}",
            true,
            "bool areRotations(const std::vector<int>& a, const std::vector<int>& b) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <vector>
            #include <algorithm>
            #include <iostream>
            bool areRotations(const std::vector<int>& a, const std::vector<int>& b);
            int main() {
                std::vector<int> a1 = {1, 2, 3, 4}, b1 = {3, 4, 1, 2}; if (!areRotations(a1, b1)) return 1;
                std::vector<int> a2 = {1, 2, 3}, b2 = {1, 2, 4}; if (areRotations(a2, b2)) return 2;
                std::vector<int> a3 = {}, b3 = {}; if (!areRotations(a3, b3)) return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });

        q.push_back({
            "编程题 5: 将字符串的前 k 个字符移到末尾（不使用额外空间）。",
            QStringList(), -1,
            "void shiftString(std::string& s, int k) {\n    if (s.empty() || k <= 0) return;\n    k = k % (int)s.size();\n    std::reverse(s.begin(), s.begin() + k);\n    std::reverse(s.begin() + k, s.end());\n    std::reverse(s.begin(), s.end());\n}",
            true,
            "void shiftString(std::string& s, int k) {\n    // 请在此处编写代码\n    \n}",
            R"raw(
            #include <string>
            #include <algorithm>
            #include <iostream>
            void shiftString(std::string& s, int k);
            int main() {
                std::string s1 = "abcdef"; shiftString(s1, 2); if (s1 != "cdefab") return 1;
                std::string s2 = ""; shiftString(s2, 5); if (s2 != "") return 2;
                std::string s3 = "hello"; shiftString(s3, 0); if (s3 != "hello") return 3;
                std::cout << "ALL_TESTS_PASSED" << std::endl;
                return 0;
            }
            )raw"
        });
    }

    if (!frequentErrors.isEmpty()) {
        std::vector<QuizQuestion> prioritized;
        std::vector<QuizQuestion> remaining;
        for (int i = 0; i < (int)q.size(); ++i) {
            if (frequentErrors.contains(i)) {
                prioritized.push_back(q[i]);
            } else {
                remaining.push_back(q[i]);
            }
        }
        prioritized.insert(prioritized.end(), remaining.begin(), remaining.end());
        q = prioritized;
    }

    return q;
}
