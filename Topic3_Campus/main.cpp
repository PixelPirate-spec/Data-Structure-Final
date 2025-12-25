#include <iostream>
#include <limits>
#include <fstream>
#include "Graph.h"

using namespace std;

void initCampus(CampusGraph& campus, const string& filename = "") {
    if (!filename.empty()) {
        campus.loadMapFromFile(filename);
    } else {
        // 默认演示数据
        campus.addLocation(1, "Main_Gate", "主入口", 80);
        campus.addLocation(2, "Library", "一个安静的学习场所", 95);
        campus.addLocation(3, "Canteen", "美味又便宜的食物", 90);
        campus.addPath(1, 2, 200);
        campus.addPath(2, 3, 100);
    }
}

int main(int argc, char *argv[]) {
    CampusGraph campus;
    
    // --- CLI 模式 (增强版) ---
    if (argc > 1) {
        string command = argv[1];
        
        // 自动识别最后一个参数是否为文件名
        string mapFile = "";
        if (argc >= 3 && string(argv[argc-1]).find(".txt") != string::npos) {
            mapFile = argv[argc-1];
        }
        initCampus(campus, mapFile);

        if (command == "path") {
            if (argc < 4) return 1;
            campus.printPathWithDistance(stoi(argv[2]), stoi(argv[3]));
        } else if (command == "search") {
            if (argc < 3) return 1;
            campus.searchSpot(argv[2]);
        } else if (command == "sort_pop") {
            // [新增功能] 按热度排序
            campus.printSortedByPopularity();
        } else if (command == "sort_id") {
            // [新增功能] 按ID排序
            campus.printSortedById();
        } else if (command == "locations") {
            campus.printLocationsCSV();
        }
        return 0;
    }

    // --- 交互菜单模式 ---
    initCampus(campus, "map_data.txt");
    int choice;
    do {
        cout << "\n=== 校园导航 ===" << endl;
        cout << "1. 加地点 2. 加路径 3. 查路径 4. 浏览(排序) 5. 搜地点 6. 载入 7. 导出 0. 退出" << endl;
        cout << "选项: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                int id, pop; string name, info;
                cout << "ID: "; cin >> id; cout << "名: "; cin >> name; cout << "热度: "; cin >> pop;
                cin.ignore(); cout << "简介: "; getline(cin, info);
                campus.addLocation(id, name, info, pop);
                break;
            }
            case 2: {
                int u, v, w; cout << "起点ID 终点ID 距离: "; cin >> u >> v >> w;
                campus.addPath(u, v, w); break;
            }
            case 3: {
                int s, e; cout << "起点 终点: "; cin >> s >> e;
                campus.printShortestPath(s, e); break;
            }
            case 4: {
                int t; cout << "1.热度 2.ID: "; cin >> t;
                if(t==1) campus.printSortedByPopularity(); else campus.printSortedById();
                break;
            }
            case 5: { string k; cout << "关键字: "; cin >> k; campus.searchSpot(k); break; }
            case 6: { string f; cout << "文件名: "; cin >> f; campus.loadMapFromFile(f); break; }
            // 7.导出功能略 (Python端已实现结果展示)
        }
    } while (choice != 0);
    return 0;
}