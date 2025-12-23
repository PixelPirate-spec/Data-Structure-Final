#include <iostream>
#include <limits>
#include "Graph.h"

using namespace std;

void pause()
{
    cout << "按回车键继续...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void initCampus(CampusGraph &campus)
{
    // Pre-populate campus data (Default)
    campus.addLocation(1, "大门", "学校正门", 80);
    campus.addLocation(2, "图书馆", "学习圣地", 95);
    campus.addLocation(3, "食堂", "美味佳肴", 90);
    campus.addLocation(4, "宿舍", "生活休息", 85);
    campus.addLocation(5, "教学楼", "上课区域", 88);
    campus.addLocation(6, "体育馆", "运动健身", 75);
    campus.addLocation(7, "行政楼", "办公区域", 50);

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

int main(int argc, char *argv[])
{
    CampusGraph campus;
    initCampus(campus);

    // CLI Mode
    if (argc > 1)
    {
        string command = argv[1];
        if (command == "path")
        {
            if (argc < 4)
            {
                cerr << "用法: ./app path <起点ID> <终点ID>" << endl;
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
        else
        {
            cerr << "未知命令: " << command << endl;
            return 1;
        }
        return 0; // Exit after CLI command
    }

    int choice;
    vector<int> lastPath; // Store the last calculated path for export

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
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer for getline

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
            cin.ignore(); // consume newline
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
            // Update lastPath
            lastPath = campus.getShortestPath(startId, endId);
            break;
        }
        case 4:
        {
            int sortChoice;
            cout << "1. 按热度排序 (降序)" << endl;
            cout << "2. 按 ID 排序 (升序)" << endl;
            cout << "选项: ";
            cin >> sortChoice;
            if (sortChoice == 1)
                campus.printSortedByPopularity();
            else if (sortChoice == 2)
                campus.printSortedById();
            else
                cout << "无效选项。" << endl;
            break;
        }
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
            {
                cout << "没有可导出的路径。请先运行查询 (选项 3)。" << endl;
            }
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
