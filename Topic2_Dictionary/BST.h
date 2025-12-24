#ifndef BST_H
#define BST_H

#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>

using namespace std;
struct BSTNode
{
    string word;
    string meaning;
    BSTNode *left;
    BSTNode *right;

    BSTNode(string w, string m) : word(w), meaning(m), left(nullptr), right(nullptr) {}
};

// 二叉搜索树类
class BST
{
private:
    BSTNode *root;
    // 插入节点
    BSTNode *insert(BSTNode *node, string word, string meaning)
    {
        // 当这是叶子节点是创建新节点并返回，供上层递归调用
        if (node == nullptr)
        {
            return new BSTNode(word, meaning);
        }

        if (word < node->word)
        {
            node->left = insert(node->left, word, meaning);
        }
        else if (word > node->word)
        {
            node->right = insert(node->right, word, meaning);
        }
        else
        {
            // 单词已存在-》去重+更新
            node->meaning = meaning;
        }
        return node;
    }

    string search(BSTNode *node, string word)
    {
        if (node == nullptr)
        {
            return "字典中未找到该单词。";
        }

        if (word == node->word)
        {
            return node->meaning;
        }
        else if (word < node->word)
        {
            return search(node->left, word);
        }
        else
        {
            return search(node->right, word);
        }
    }

    BSTNode *findMin(BSTNode *node)
    {
        while (node && node->left != nullptr)
        {
            node = node->left;
        }
        return node;
    }

    BSTNode *remove(BSTNode *node, string word)
    {
        if (node == nullptr)
        {
            cout << "单词 '" << word << "' 未找到。" << endl;
            return nullptr;
        }

        if (word < node->word)
        {
            node->left = remove(node->left, word);
        }
        else if (word > node->word)
        {
            node->right = remove(node->right, word);
        }
        else
        {
            // Case 1: 此节点没有孩子
            if (node->left == nullptr && node->right == nullptr)
            {
                delete node;
                node = nullptr;
            }
            // Case 2: 节点有一个孩子
            else if (node->left == nullptr)
            {
                BSTNode *temp = node;
                node = node->right; 
                delete temp;
            }
            else if (node->right == nullptr)
            {
                BSTNode *temp = node;
                node = node->left; // 替换为左子节点
                delete temp;
            }
            // Case 3: 节点有两个孩子
            else
            {
                // 查找柚子树最小节点（中序后继节点）
                // 中序后继节点的关键字大于待删除节点，且其要么没有子节点，要么只有右子节点（不可能有左子节点，因为它是右子树的最小值），后续删除该后继节点只需处理简单场景（无子女 / 仅有一个子女）。
                BSTNode *temp = findMin(node->right);

                // 用后继节点覆盖这个节点的值
                node->word = temp->word;
                node->meaning = temp->meaning;

                // 精准定位，删除冗余节点
                node->right = remove(node->right, temp->word);
            }
        }
        return node;
    }

    // 中序遍历
    void inOrder(BSTNode *node)
    {
        if (node == nullptr)
            return;

        inOrder(node->left);
        cout << left << setw(20) << node->word << ": " << node->meaning << endl;
        inOrder(node->right);
    }

    // 清空树（析构函数）
    void clear(BSTNode *node)
    {
        if (node == nullptr)
            return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    // 计算树可视化位置
    void calcPos(BSTNode *node, int &x, int y, map<BSTNode *, pair<int, int>> &pos)
    {
        if (!node)
            return;
        // 1. 递归处理左子树：计算左子树所有节点的坐标
        calcPos(node->left, x, y + 2, pos);
        // 2. 为当前节点分配并存储坐标
        pos[node] = {x, y};
        // 3. 更新x坐标：为后续节点预留水平空间
        x += node->word.length() + 2; // 间距
        calcPos(node->right, x, y + 2, pos);
    }

    // 递归模糊搜索
    void searchByPrefix(BSTNode *node, string prefix)
    {
        if (node == nullptr)
            return;

        // 优化遍历：
        // 如果节点的单词小于前缀，我们只需要查看右子树
        // 但是前缀匹配很棘手，因为 "apple" > "app"。
        // 为简单和正确起见，我们坚持使用标准中序遍历并检查条件

        searchByPrefix(node->left, prefix);

        // 检查节点单词是否以该前缀开头
        if (node->word.find(prefix) == 0)
        {
            cout << left << setw(20) << node->word << ": " << node->meaning << endl;
        }

        searchByPrefix(node->right, prefix);
    }

    // 保存到文件（中序遍历）
    void saveToFile(BSTNode *node, ofstream &outFile)
    {
        if (node == nullptr)
            return;
        saveToFile(node->left, outFile);
        outFile << node->word << ":" << node->meaning << endl;
        saveToFile(node->right, outFile);
    }

    // 序列化为 JSON
    void serializeJSON(BSTNode *node, bool isLast)
    {
        if (node == nullptr)
            return;

        cout << "{";
        cout << "\"name\": \"" << node->word << "\"";

        if (node->left != nullptr || node->right != nullptr)
        {
            cout << ", \"children\": [";
            if (node->left)
            {
                serializeJSON(node->left, node->right == nullptr);
            }
            if (node->right)
            {
                if (node->left)
                    cout << ", ";
                serializeJSON(node->right, true);
            }
            cout << "]";
        }

        cout << "}";
        if (!isLast)
            cout << ", "; 
    }

    // 处理序列化为 JSON
    void printJSON(BSTNode *node)
    {
        if (node == nullptr)
            return;

        cout << "{\"name\": \"" << node->word << "\"";
        if (node->left || node->right)
        {
            cout << ", \"children\": [";
            if (node->left)
            {
                printJSON(node->left);
            }
            if (node->left && node->right)
            {
                cout << ", ";
            }
            if (node->right)
            {
                printJSON(node->right);
            }
            cout << "]";
        }
        cout << "}";
    }

public:
    BST() : root(nullptr) {}

    ~BST()
    {
        clear(root);
    }

    void insert(string word, string meaning)
    {
        root = insert(root, word, meaning);
    }

    string search(string word)
    {
        return search(root, word);
    }

    void remove(string word)
    {
        root = remove(root, word);
    }

    void inOrder()
    {
        if (root == nullptr)
        {
            cout << "字典为空。" << endl;
        }
        else
        {
            cout << "----------------------------------------" << endl;
            cout << left << setw(20) << "单词" << "释义" << endl;
            cout << "----------------------------------------" << endl;
            inOrder(root);
            cout << "----------------------------------------" << endl;
        }
    }

    // 可视化
    void printTree()
    {
        if (root == nullptr)
        {
            cout << "树为空。" << endl;
            return;
        }

        map<BSTNode *, pair<int, int>> pos;
        int x = 0;
        calcPos(root, x, 0, pos);

        int height = 0;
        for (auto const &entry : pos)
        {
            if (entry.second.second > height)
                height = entry.second.second;
        }

        // 创建画布
        vector<string> canvas(height + 1, string(x, ' '));

        for (auto const &entry : pos)
        {
            BSTNode *node = entry.first;
            int px = entry.second.first;
            int py = entry.second.second;
            string w = node->word;

            // 打印单词
            for (int i = 0; i < w.length(); ++i)
            {
                if (px + i < x)
                    canvas[py][px + i] = w[i];
            }

            // 绘制分支
            if (node->left)
            {
                pair<int, int> lp = pos[node->left];
                int lx_center = lp.first + node->left->word.length() / 2;

                if (py + 1 < canvas.size())
                {
                    if (px - 1 >= 0)
                        canvas[py + 1][px - 1] = '/';
                    for (int k = lx_center + 1; k < px - 1; k++)
                        canvas[py + 1][k] = '_';
                }
            }
            if (node->right)
            {
                pair<int, int> rp = pos[node->right];
                int rx_center = rp.first + node->right->word.length() / 2;
                int px_end = px + w.length();

                if (py + 1 < canvas.size())
                {
                    if (px_end < x)
                        canvas[py + 1][px_end] = '\\';
                    for (int k = px_end + 1; k < rx_center; k++)
                        canvas[py + 1][k] = '_';
                }
            }
        }

        for (const string &line : canvas)
        {
            cout << line << endl;
        }
    }

    // 用于 CLI 的 JSON 可视化
    void printTreeJSON()
    {
        if (root == nullptr)
        {
            cout << "{}" << endl;
            return;
        }
        printJSON(root);
        cout << endl;
    }

    // 模糊搜索
    void searchByPrefix(string prefix)
    {
        cout << "以 '" << prefix << "' 开头的单词：" << endl;
        cout << "----------------------------------------" << endl;
        searchByPrefix(root, prefix);
        cout << "----------------------------------------" << endl;
    }

    // 文件 I/O
    void saveToFile(string filename)
    {
        ofstream outFile(filename);
        if (!outFile)
        {
            cout << "写入文件打开失败: " << filename << endl;
            return;
        }
        saveToFile(root, outFile);
        outFile.close();
        cout << "字典已保存至 " << filename << endl;
    }

    void loadFromFile(string filename)
    {
        ifstream inFile(filename);
        if (!inFile)
        {
            cout << "读取文件打开失败: " << filename << endl;
            return;
        }

        string line;
        int count = 0;
        while (getline(inFile, line))
        {
            size_t delimiterPos = line.find(':');
            if (delimiterPos != string::npos)
            {
                string word = line.substr(0, delimiterPos);
                string meaning = line.substr(delimiterPos + 1);
                insert(word, meaning);
                count++;
            }
        }
        inFile.close();
        cout << "从 " << filename << " 加载了 " << count << " 条记录。" << endl;
    }
};

#endif // BST_H
