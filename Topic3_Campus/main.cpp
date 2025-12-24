#include <iostream>
#include <limits>
#include <fstream>
#include "Graph.h"

using namespace std;

void pause()
{
    cout << "按回车键继续...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// 修改：不再自动加载默认文件，而是由 main 函数控制加载
void initCampus(CampusGraph &campus, const string &filename = "")
{
    if (filename.empty())
    {
        // 如果没有提供文件，加载硬编码的默认数据作为演示
        campus.addLocation(1, "Main_Gate", "The main entrance.", 80);
        campus.addLocation(2, "Library", "A quiet place to study.", 95);
        campus.addLocation(3, "Canteen", "Tasty and cheap food.", 90);
        campus.addLocation(4, "Dormitory", "Where students live.", 85);
        campus.addLocation(5, "Teaching_Bldg", "Classes are held here.", 88);
        campus.addLocation(6, "Gymnasium", "Sports and events.", 75);
        campus.addLocation(7, "Admin_Bldg", "Administrative offices.", 50);

        campus.addPath(1, 5, 200);
        campus.addPath(1, 6, 500);
        campus.addPath(5, 2, 100);
        campus.addPath(5, 3, 150);
        campus.addPath(2, 4, 300);
        campus.addPath(3, 4, 50);
        campus.addPath(6, 3, 250);
        campus.addPath(6, 7, 400);
        campus.addPath(7, 2, 100);
    }
    else
    {
        campus.loadMapFromFile(filename);
    }
}

int main(int argc, char *argv[])
{
    CampusGraph campus;

    // 命令行模式 (CLI Mode) - 供 Python 调用
    if (argc > 1)
    {
        string command = argv[1];

        // 约定：最后一个参数如果是文件名，则尝试加载
        // 例如: ./app path 0 4 build/map_data.txt
        string mapFile = "";
        if (argc >= 3 && string(argv[argc - 1]).find(".txt") != string::npos)
        {
            mapFile = argv[argc - 1];
        }

        // 根据是否有文件参数进行初始化
        if (!mapFile.empty())
        {
            campus.loadMapFromFile(mapFile);
        }
        else
        {
            initCampus(campus); // 加载默认
        }

        if (command == "path")
        {
            // ./app path <start> <end> [filename]
            if (argc < 4) // 至少需要 start 和 end
            {
                cerr << "用法: ./app path <起点ID> <终点ID> [数据文件]" << endl;
                return 1;
            }
            int startId = stoi(argv[2]);
            int endId = stoi(argv[3]);
            campus.printPathWithDistance(startId, endId);
        }
        else if (command == "locations")
        {
            campus.printLocationsCSV();
        }
        else if (command == "edges")
        {
            campus.printEdgesCSV();
        }
        else if (command == "search")
        {
            if (argc < 3)
            {
                cerr << "Usage: ./app search <keyword> [filename]" << endl;
                return 1;
            }
            string keyword = argv[2];
            campus.searchSpot(keyword);
        }
        else
        {
            cerr << "Unknown command: " << command << endl;
            return 1;
        }
        return 0; // CLI 执行完退出
    }

    // 交互模式 (Interactive Mode)
    initCampus(campus, "map_data.txt"); // 尝试加载当前目录的默认文件

    int choice;
    vector<int> lastPath;

    do
    {
        cout << "\n========================================" << endl;
        cout << "主题 3：校园导航系统" << endl;
        cout << "========================================" << endl;
        cout << "1. 添加地点" << endl;
        cout << "2. 添加路径" << endl;
        cout << "3. 查询最短路径" << endl;
        cout << "4. 浏览地点 (按热度/ID排序)" << endl;
        cout << "5. 关键字搜索 (前缀)" << endl;
        cout << "6. 从文件加载地图" << endl;
        cout << "7. 导出上次路径到文件" << endl;
        cout << "0. 退出" << endl;
        cout << "请输入选项: ";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice)
        {
        case 1:
        {
            int id, pop;
            string name, info;
            cout << "输入 ID: ";
            cin >> id;
            cout << "输入名称 (无空格): ";
            cin >> name;
            cout << "输入热度 (0-100): ";
            cin >> pop;
            cin.ignore();
            cout << "输入简介: ";
            getline(cin, info);
            campus.addLocation(id, name, info, pop);
            cout << "地点已添加。" << endl;
            break;
        }
        case 2:
        {
            int u, v, w;
            cout << "输入起点 ID: ";
            cin >> u;
            cout << "输入终点 ID: ";
            cin >> v;
            cout << "输入权重 (距离): ";
            cin >> w;
            campus.addPath(u, v, w);
            cout << "路径已添加。" << endl;
            break;
        }
        case 3:
        {
            int startId, endId;
            cout << "输入起点 ID: ";
            cin >> startId;
            cout << "输入终点 ID: ";
            cin >> endId;
            campus.printShortestPath(startId, endId);
            lastPath = campus.getShortestPath(startId, endId);
            break;
        }
        case 4:
            campus.printSortedByPopularity();
            break;
        case 5:
        {
            string keyword;
            cout << "输入关键字 (前缀): ";
            cin >> keyword;
            campus.searchSpot(keyword);
            break;
        }
        case 6:
        {
            string filename;
            cout << "输入文件名: ";
            cin >> filename;
            campus.loadMapFromFile(filename);
            lastPath.clear();
            break;
        }
        case 7:
        {
            if (lastPath.empty())
                cout << "没有可导出的路径。" << endl;
            else
            {
                string filename;
                cout << "输入保存路径的文件名: ";
                cin >> filename;
                campus.exportPathToFile(filename, lastPath);
            }
            break;
        }
        case 0:
            cout << "正在退出..." << endl;
            break;
        default:
            cout << "无效选项，请重试。" << endl;
        }

    } while (choice != 0);

    return 0;
}