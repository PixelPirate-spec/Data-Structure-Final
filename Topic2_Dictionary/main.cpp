#include <iostream>
#include <limits>
#include <fstream>
#include "BST.h"

using namespace std;

void pause() {
    cout << "按回车键继续...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void initDictionary(BST& dictionary) {
    string defaultFile = "dictionary.txt";
    ifstream f(defaultFile);
    if (f.good()) {
        f.close();
        dictionary.loadFromFile(defaultFile);
    } else {
        f.close();
        dictionary.insert("Apple", "苹果");
        dictionary.insert("Banana", "香蕉");
        dictionary.insert("Cat", "猫");
    }
}

int main(int argc, char* argv[]) {
    BST dictionary;
    initDictionary(dictionary);

    // --- CLI 模式 (供 Python 调用) ---
    if (argc > 1) {
        string command = argv[1];
        if (command == "search") {
            if (argc < 3) return 1;
            cout << dictionary.search(argv[2]) << endl;
        } else if (command == "fuzzy") {
            if (argc < 3) return 1;
            dictionary.searchByPrefix(argv[2]);
        } else if (command == "print_tree") {
            dictionary.printTree(); 
        } else if (command == "view_all") { 
            // [新增功能] 对应菜单 4: 打印所有单词
            dictionary.inOrder();
        } 
        return 0;
    }

    // --- 交互菜单模式 ---
    int choice;
    string word, meaning, filename;
    do {
        cout << "\n=== 字典系统 ===" << endl;
        cout << "1. 插入 2. 搜索 3. 删除 4. 浏览(A-Z) 5. 模糊搜索 6. 保存 7. 加载 8. 树结构 0. 退出" << endl;
        cout << "选项: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                cout << "单词: "; getline(cin, word);
                cout << "释义: "; getline(cin, meaning);
                dictionary.insert(word, meaning);
                break;
            case 2: cout << "搜索: "; getline(cin, word); cout << dictionary.search(word) << endl; break;
            case 3: cout << "删除: "; getline(cin, word); dictionary.remove(word); break;
            case 4: dictionary.inOrder(); break;
            case 5: cout << "前缀: "; getline(cin, word); dictionary.searchByPrefix(word); break;
            case 6: cout << "文件名: "; getline(cin, filename); dictionary.saveToFile(filename); break;
            case 7: cout << "文件名: "; getline(cin, filename); dictionary.loadFromFile(filename); break;
            case 8: dictionary.printTree(); break;
        }
    } while (choice != 0);
    return 0;
}