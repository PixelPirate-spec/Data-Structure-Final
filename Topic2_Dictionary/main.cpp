#include <iostream>
#include <limits>
#include "BST.h"

using namespace std;

void pause()
{
    cout << "按回车键继续...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void initDictionary(BST &dictionary)
{
    // Pre-populate dictionary
    dictionary.insert("Apple", "苹果");
    dictionary.insert("Banana", "香蕉");
    dictionary.insert("Cat", "猫");
    dictionary.insert("Dog", "狗");
    dictionary.insert("Elephant", "大象");
    dictionary.insert("Application", "申请");
    dictionary.insert("Appetite", "食欲");
}

int main(int argc, char *argv[])
{
    BST dictionary;
    initDictionary(dictionary);

    // CLI Mode
    if (argc > 1)
    {
        string command = argv[1];
        if (command == "search")
        {
            if (argc < 3)
            {
                cerr << "用法: ./app search <单词>" << endl;
                return 1;
            }
            string word = argv[2];
            cout << dictionary.search(word) << endl;
        }
        else if (command == "fuzzy")
        {
            if (argc < 3)
            {
                cerr << "用法: ./app fuzzy <前缀>" << endl;
                return 1;
            }
            string prefix = argv[2];
            dictionary.searchByPrefix(prefix);
        }
        else if (command == "print_tree")
        {
            dictionary.printTreeJSON();
        }
        else
        {
            cerr << "未知命令: " << command << endl;
            return 1;
        }
        return 0; // Exit after CLI command
    }

    int choice;
    string word, meaning, filename;

    do
    {
        cout << "\n========================================" << endl;
        cout << "主题 2：字典系统 (BST)" << endl;
        cout << "========================================" << endl;
        cout << "1. 插入新单词" << endl;
        cout << "2. 搜索单词" << endl;
        cout << "3. 删除单词" << endl;
        cout << "4. 打印所有单词 (A-Z)" << endl;
        cout << "5. 模糊搜索 (前缀)" << endl;
        cout << "6. 保存到文件" << endl;
        cout << "7. 从文件加载" << endl;
        cout << "8. 打印树形结构" << endl;
        cout << "0. 退出" << endl;
        cout << "请输入选项: ";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer for getline

        switch (choice)
        {
        case 1:
            cout << "输入单词: ";
            getline(cin, word);
            cout << "输入释义: ";
            getline(cin, meaning);
            dictionary.insert(word, meaning);
            cout << "单词已插入/更新。" << endl;
            break;
        case 2:
            cout << "输入要搜索的单词: ";
            getline(cin, word);
            cout << "结果: " << dictionary.search(word) << endl;
            break;
        case 3:
            cout << "输入要删除的单词: ";
            getline(cin, word);
            dictionary.remove(word);
            cout << "操作完成。" << endl;
            break;
        case 4:
            dictionary.inOrder();
            break;
        case 5:
            cout << "输入要搜索的前缀: ";
            getline(cin, word);
            dictionary.searchByPrefix(word);
            break;
        case 6:
            cout << "输入保存文件名 (如 dict.txt): ";
            getline(cin, filename);
            dictionary.saveToFile(filename);
            break;
        case 7:
            cout << "输入加载文件名 (如 dict.txt): ";
            getline(cin, filename);
            dictionary.loadFromFile(filename);
            break;
        case 8:
            cout << "树形结构可视化:" << endl;
            dictionary.printTree();
            break;
        case 0:
            cout << "正在退出..." << endl;
            break;
        default:
            cout << "无效选项，请重试。" << endl;
        }

    } while (choice != 0);

    return 0;
}
