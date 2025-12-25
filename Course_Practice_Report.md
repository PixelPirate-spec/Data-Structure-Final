# Data Structure Practice Report

## 1. 题目与要求 (Problem & Requirements)

### 1.1 问题描述 (Problem Description)
本项目旨在解决三个独立但技术互补的数据结构问题，并通过一个统一的系统进行集成展示。
*   **专题 1：学生成绩管理系统 (Student Grade System)**：解决大规模学生数据的存储、排序与管理问题。利用线性表存储数据，通过高效排序算法（快速排序、堆排序）满足不同维度的查询需求。
*   **专题 2：字典查询系统 (Dictionary System)**：解决海量词条的快速检索与动态更新问题。利用二叉排序树 (BST) 的特性，实现高效的插入、删除、精确查找及前缀模糊匹配。
*   **专题 3：校园导航系统 (Campus Navigation)**：解决复杂环境下的路径规划问题。将校园地图抽象为带权无向图，利用 Dijkstra 算法计算两点间的最短路径，并提供基于地点热度的排序功能。

**创新性架构**：本项目采用了**前后端分离**的设计模式。
*   **后端 (Backend)**：使用 **C++** 编写，负责核心数据结构（Vector, BST, Graph）的定义与高性能算法（QuickSort, HeapSort, Dijkstra）的实现。后端程序被编译为独立的命令行工具 (CLI)。
*   **前端 (Frontend)**：使用 **Python (Streamlit)** 编写，负责用户交互、数据可视化（表格、树形图、网络图）以及数据文件的持久化管理。前端通过系统调用（Subprocess）与后端交互，实现了算法逻辑与界面展示的解耦。

### 1.2 涉及知识点 (Knowledge Points)
*   **线性表 (Linear List)**：使用 `std::vector` 实现动态数组。
*   **排序算法 (Sorting)**：
    *   **快速排序 (QuickSort)**：用于按学号（ID）升序排列，平均时间复杂度 $O(n \log n)$。
    *   **堆排序 (HeapSort)**：用于按成绩（Score）降序排列，利用最小堆特性，时间复杂度 $O(n \log n)$。
*   **树 (Tree)**：
    *   **二叉排序树 (BST)**：用于字典存储，支持 $O(h)$ 的查找与动态维护。
    *   **树的遍历 (Traversal)**：中序遍历实现有序输出，递归遍历实现 JSON 序列化与前缀搜索。
*   **图 (Graph)**：
    *   **邻接表 (Adjacency List)**：高效存储稀疏图结构。
    *   **Dijkstra 算法**：解决单源最短路径问题，利用优先队列优化至 $O(E \log V)$。
*   **文件 I/O (File I/O)**：C++ (`ifstream`/`ofstream`) 与 Python 的文本文件读写，实现数据持久化。
*   **混合编程 (Polyglot Programming)**：Python 通过 `subprocess` 调用 C++ CLI 程序，利用管道 (Pipe) 捕获标准输出 (stdout)。

### 1.3 功能要求 (Functional Requirements)
*   **Topic 1 (成绩)**：支持从文件加载学生数据；支持添加、删除学生；提供按 ID 升序和按成绩降序的视图。
*   **Topic 2 (字典)**：支持单词的插入与删除；提供精确查找（输出释义）与前缀模糊查找（如 "App" -> "Apple", "Appetite"）；支持 BST 结构的图形化展示。
*   **Topic 3 (导航)**：支持地图节点与路径的动态添加；提供基于 Dijkstra 的最短路径规划（输出路径序列及总距离）；支持按热度查看地点；支持校园地图拓扑的可视化。

---

## 2. 功能设计 (Function Design)

### 2.1 数据结构定义 (Data Structure Definitions)

**Topic 1: Student (学生)**
```cpp
struct Student {
    std::string id;    // 学号 (Key for QuickSort)
    std::string name;  // 姓名
    float score;       // 成绩 (Key for HeapSort)
};
```
*解释*：简单的记录结构，存储学生基本信息。

**Topic 2: BSTNode (二叉搜索树节点)**
```cpp
struct BSTNode {
    string word;       // 单词 (Key)
    string meaning;    // 释义
    BSTNode* left;     // 左子树指针 (Smaller)
    BSTNode* right;    // 右子树指针 (Larger)

    BSTNode(string w, string m) : word(w), meaning(m), left(nullptr), right(nullptr) {}
};
```
*解释*：链式存储的二叉树节点，`word` 遵循 BST 性质（左 < 根 < 右）。

**Topic 3: Vertex & Edge (图顶点与边)**
```cpp
struct Edge {
    int destination;   // 目标顶点 ID
    int weight;        // 路径权重 (距离)
};

struct Vertex {
    int id;            // 地点 ID
    string name;       // 地点名称
    string info;       // 简介
    int popularity;    // 人气值
    vector<Edge> edges;// 邻接边列表 (Adjacency List)
};
```
*解释*：采用邻接表表示图。`Vertex` 存储节点信息及从该节点出发的所有边 `edges`。这比邻接矩阵更节省空间，适合稀疏图。

### 2.2 模块图 (Module Diagram)

系统数据流向如下：

```mermaid
graph TD
    User[用户 (Web 浏览器)] -->|交互操作| FE[Python 前端 (Streamlit)]

    subgraph Frontend_Logic
        FE -->|读/写| Files[(数据文件 .txt)]
        FE -->|生成参数| CLI_Args[CLI 参数]
    end

    CLI_Args -->|Subprocess 调用| BE[C++ 后端 (Executable)]

    subgraph Backend_Logic
        BE -->|加载| Files
        BE -->|计算| Algo[核心算法 (Sort/Search/Dijkstra)]
        Algo -->|输出| Stdout[标准输出 (CSV/JSON/Text)]
    end

    Stdout -->|解析 & 渲染| FE
    FE -->|展示| Viz[可视化组件 (表格/Graphviz)]
```

1.  **数据管理**：Python 前端全权负责数据的**增删改**（直接操作 `.txt` 文件），确保数据持久化。
2.  **计算服务**：C++ 后端作为**无状态计算引擎**。每次调用时，C++ 程序启动，读取最新文件，执行算法（如排序或路径规划），输出结果后退出。
3.  **可视化**：Python 捕获 C++ 的输出（如 JSON 格式的树结构），利用 `graphviz` 库渲染图形呈现给用户。

---

## 3. 功能代码 (Function Code)

### 3.1 核心算法：快速排序 (Topic 1)
```cpp
// 划分函数：选取基准元素，将小于基准的放左边，大于的放右边
int partition(vector<Student>& students, int low, int high) {
    string pivot = students[high].id; // 选取最后一个元素作为基准
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        // 按 ID 升序排列
        if (students[j].id < pivot) {
            i++;
            swap(students[i], students[j]);
        }
    }
    swap(students[i + 1], students[high]);
    return (i + 1);
}

void quickSort(vector<Student>& students, int low, int high) {
    if (low < high) {
        int pi = partition(students, low, high); // 获取分区点
        quickSort(students, low, pi - 1);        // 递归排序左半部分
        quickSort(students, pi + 1, high);       // 递归排序右半部分
    }
}
```
*设计思路*：采用标准的分治策略。通过 `partition` 操作确定基准元素的最终位置，该操作的时间复杂度为 $O(n)$。整体递归深度平均为 $\log n$，因此总复杂度为 $O(n \log n)$。

### 3.2 核心算法：BST 递归插入与 JSON 序列化 (Topic 2)
```cpp
// 递归插入节点
BSTNode* insert(BSTNode* node, string word, string meaning) {
    if (node == nullptr) {
        return new BSTNode(word, meaning); // 找到空位，新建节点
    }
    if (word < node->word) {
        node->left = insert(node->left, word, meaning);
    } else if (word > node->word) {
        node->right = insert(node->right, word, meaning);
    } else {
        // 单词已存在，更新释义
        node->meaning = meaning;
    }
    return node;
}

// 辅助函数：生成 JSON 格式用于前端可视化
// 输出示例: {"name": "Root", "children": [{"name": "Left"}, {"name": "Right"}]}
void printJSON(BSTNode* node) {
     if (node == nullptr) return;
     cout << "{\"name\": \"" << node->word << "\"";
     if (node->left || node->right) {
         cout << ", \"children\": [";
         if (node->left) printJSON(node->left);
         if (node->left && node->right) cout << ", "; // 分隔符
         if (node->right) printJSON(node->right);
         cout << "]";
     }
     cout << "}";
}
```
*设计思路*：插入操作利用 BST 的有序性，递归寻找插入点。JSON 序列化也是一种递归遍历（先序遍历变种），它构建了树的层级描述字符串，使得 Python 前端可以直接解析并传递给 Graphviz 绘图引擎，无需了解 C++ 内部指针结构。

### 3.3 核心算法：Dijkstra 最短路径 (Topic 3)
```cpp
// 返回两个 Map：dist (ID->最短距离), parent (ID->前驱节点ID)
pair<map<int, int>, map<int, int>> dijkstra(int startId) {
    map<int, int> dist;
    map<int, int> parent;

    // 初始化
    for (const auto& v : vertices) {
        dist[v.id] = INT_MAX;
        parent[v.id] = -1;
    }
    dist[startId] = 0;

    // 最小堆优先队列: pair<distance, id>，距离小的在顶端
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push({0, startId});

    while (!pq.empty()) {
        int d = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (d > dist[u]) continue; // 懒惰删除：如果是过期的较长路径，跳过

        // 松弛操作 (Relaxation)
        Vertex& uVertex = vertices[idToIndex[u]];
        for (const auto& edge : uVertex.edges) {
            int v = edge.destination;
            int weight = edge.weight;
            if (dist[u] != INT_MAX && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }
    return {dist, parent};
}
```
*设计思路*：使用 STL 的 `priority_queue` 实现最小堆优化 Dijkstra 算法。通过贪心策略，每次从优先队列中取出当前距离起点最近的未访问节点，并松弛其邻接边。`parent` 映射表用于在算法结束后回溯重建具体路径。

---

## 4. 调试与测试 (Debug & Test)

### 4.1 调试分析 (Debug Analysis)
*   **快速排序**：
    *   **复杂度**：时间 $O(n \log n)$，空间 $O(\log n)$（递归栈）。
    *   **分析**：在处理学生 ID 字符串比较时，C++ 的 `std::string` 比较操作是按字典序进行的，符合需求。
*   **BST 操作**：
    *   **复杂度**：查找/插入/删除均为 $O(h)$，其中 $h$ 为树高。最坏情况（退化成链表）为 $O(n)$，平均情况为 $O(\log n)$。
    *   **分析**：为了支持模糊查找（前缀匹配），我们实现了 `searchByPrefix`，它实际上是一次完整的中序遍历，时间复杂度为 $O(n)$。在数据量巨大时可优化为 Trie 树或剪枝搜索，但对于本课程设计规模，遍历 BST 足够高效。
*   **Dijkstra 算法**：
    *   **复杂度**：$O(E \log V)$，其中 $E$ 是边数，$V$ 是顶点数。
    *   **分析**：利用 `map` 将非连续的用户 ID 映射到连续的数组索引 (`idToIndex`)，解决了用户 ID 不从 0 开始或不连续的问题，增强了系统的鲁棒性。

### 4.2 用户手册 (User Manual)
**启动系统**：
1.  打开终端，进入项目根目录。
2.  确保已安装依赖：`pip install -r requirements.txt`。
3.  编译后端（如未编译）：`mkdir build && cd build && cmake .. && make`。
4.  运行前端：`streamlit run app.py`。

**操作流程**：
*   界面左侧为 **侧边栏 (Sidebar)**，用于切换三个专题。
*   **Topic 1**：选择 "Sort & View" 查看排序结果；选择 "Manage Data" 添加或删除学生。
*   **Topic 2**：在 "Search" 栏输入单词进行查询；点击 "Visualize Tree" 查看动态生成的树结构图。
*   **Topic 3**：在 "Navigation" 栏选择起点和终点，点击 "Find Path" 获取路径规划结果。在 "Map Editor" 中可添加新地点。

### 4.3 测试过程 (Test Process)

**测试用例 1：学生成绩排序**
*   **输入**：
    *   1003 Alice 85.5
    *   1001 Bob 92.0
*   **操作**：点击 "Sort by ID"。
*   **预期结果**：表格显示顺序变为 1001 (Bob), 1003 (Alice)。
*   **实际结果**：显示正确，且 CSV 解析无误。

**测试用例 2：字典树可视化**
*   **输入**：依次插入 "Apple", "Banana", "Appetite"。
*   **操作**：点击 "Visualize Tree"。
*   **预期结果**：Graphviz 渲染出一棵树，Root 为 "Apple"，左子树有 "Appetite"，右子树有 "Banana"。
*   **实际结果**：图形结构正确显示，JSON 解析成功。

**测试用例 3：最短路径规划**
*   **数据**：Gate(1) -> Lib(2) [Weight 500], Lib(2) -> Canteen(3) [Weight 200], Canteen(3) -> Dorm(4) [Weight 50]。
*   **操作**：查询 Gate(1) 到 Dorm(4)。
*   **预期结果**：路径 Gate -> Lib -> Canteen -> Dorm，总距离 750。如果存在 Gate(1) -> Gym(6) -> Dorm(4) [Total 800]，算法应自动选择前者。
*   **实际结果**：系统正确输出了 Total Distance: 750 及完整路径链。

---

## 5. 总结 (Conclusion)

本次课程实践中，我成功设计并实现了一个包含三个核心数据结构专题的综合系统。

**主要收获**：
1.  **工程化思维**：不同于以往单一的控制台程序，本次实践通过 Python 和 C++ 的结合，模拟了现代软件开发中“计算与展示分离”的架构。深刻体会到了模块化设计对于代码维护的重要性。
2.  **CLI 封装技巧**：学会了如何将复杂的 C++ 算法封装为可以通过命令行参数（`argc`, `argv`）控制的工具。例如，设计 `./app search <word>` 这样的接口，使得后端程序变得通用且易于测试。
3.  **数据可视化**：通过 Streamlit 和 Graphviz，将抽象的数据结构（如二叉树、图）转化为直观的图像，极大地辅助了对算法逻辑的理解和调试。

**遇到的困难与解决**：
*   **问题**：Python 修改数据文件后，C++ 读取的仍是旧数据。
    *   **解决**：这是因为文件读写存在缓存或同步问题。我在 Python 代码中确保每次写入后关闭文件句柄，且 C++ 端在每次 CLI 启动时都重新加载文件，保证了数据的一致性。
*   **问题**：Graphviz 可视化时，节点名称包含空格导致解析错误。
    *   **解决**：在生成 JSON 或 CSV 时，对包含空格的字符串进行了特殊处理（如替换为下划线），或者在前端解析时加强了健壮性。

通过本次实践，我不仅巩固了快排、BST、Dijkstra 等核心算法，更提升了全栈视角的开发能力。
