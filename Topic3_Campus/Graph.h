#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <climits>
#include <map>
#include <iomanip>
#include <stack>
#include <fstream>
#include <sstream>

using namespace std;

// 边结构体
struct Edge
{
    int destination;
    int weight;
};

// 顶点结构体
struct Vertex
{
    int id;
    string name;
    string info;
    int popularity;
    vector<Edge> edges;
};

// 校园图类
class CampusGraph
{
private:
    vector<Vertex> vertices;
    map<int, int> idToIndex; // 将用户ID映射到向量索引

public:
    void clear() {
        vertices.clear();
        idToIndex.clear();
        cout << "Map cleared." << endl;
    }

    void addLocation(int id, string name, string info, int popularity) {
        if (idToIndex.find(id) != idToIndex.end()) {
            cout << "Location ID " << id << " already exists." << endl;
            return;
        }
        Vertex v;
        v.id = id;
        v.name = name;
        v.info = info;
        v.popularity = popularity;
        idToIndex[id] = vertices.size();
        vertices.push_back(v);
    }

    void addPath(int u, int v, int weight) {
        if (idToIndex.find(u) == idToIndex.end() || idToIndex.find(v) == idToIndex.end()) {
            cout << "Invalid location IDs: " << u << ", " << v << endl;
            return;
        }
        int uIdx = idToIndex[u];
        int vIdx = idToIndex[v];

        // 检查重复？暂时假设输入有效。
        vertices[uIdx].edges.push_back({v, weight});
        vertices[vIdx].edges.push_back({u, weight}); // 无向图
    }

    Vertex* getLocation(int id) {
        if (idToIndex.find(id) == idToIndex.end()) return nullptr;
        return &vertices[idToIndex[id]];
    }

    // 任务4：关键字搜索（前缀匹配）
    void searchSpot(string keyword)
    {
        cout << "'" << keyword << "' 的搜索结果:" << endl;
        cout << left << setw(5) << "ID"
             << setw(20) << "Name"
             << setw(10) << "Popularity"
             << "Info" << endl;
        cout << string(60, '-') << endl;

        bool found = false;
        for (const auto &v : vertices)
        {
            // 检查名称是否以关键字开头
            if (v.name.find(keyword) == 0)
            {
                cout << left << setw(5) << v.id
                     << setw(20) << v.name
                     << setw(10) << v.popularity
                     << v.info << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "No matching locations found." << endl;
        }
    }

    // 按热度对地点进行排序（降序）
    void printSortedByPopularity()
    {
        if (vertices.empty())
        {
            cout << "没有可用的地点。" << endl;
            return;
        }

        vector<Vertex> sortedVertices = vertices;
        sort(sortedVertices.begin(), sortedVertices.end(), [](const Vertex& a, const Vertex& b) {
            return a.popularity > b.popularity;
        });

        cout << "--- Sorted by Popularity (Descending) ---" << endl;
        cout << left << setw(5) << "ID"
             << setw(20) << "Name"
             << setw(10) << "Popularity"
             << "Info" << endl;
        cout << string(60, '-') << endl;

        for (const auto& v : sortedVertices) {
            cout << left << setw(5) << v.id
                 << setw(20) << v.name
                 << setw(10) << v.popularity
                 << v.info << endl;
        }
    }

    // 按ID对地点进行排序（升序）
    void printSortedById()
    {
        if (vertices.empty())
        {
            cout << "没有可用的地点。" << endl;
            return;
        }

        vector<Vertex> sortedVertices = vertices;
        sort(sortedVertices.begin(), sortedVertices.end(), [](const Vertex& a, const Vertex& b) {
            return a.id < b.id;
        });

        cout << "--- Sorted by ID (Ascending) ---" << endl;
        cout << left << setw(5) << "ID"
             << setw(20) << "Name"
             << setw(10) << "Popularity"
             << "Info" << endl;
        cout << string(60, '-') << endl;

        for (const auto& v : sortedVertices) {
            cout << left << setw(5) << v.id
                 << setw(20) << v.name
                 << setw(10) << v.popularity
                 << v.info << endl;
        }
    }

    // 扩展：打印地点CSV
    void printLocationsCSV()
    {
        cout << "id,name,popularity,info" << endl;
        for (const auto& v : vertices) {
            cout << v.id << "," << v.name << "," << v.popularity << "," << v.info << endl;
        }
    }

    // 扩展：为可视化打印边CSV
    // u,v,weight
    void printEdgesCSV() {
        cout << "u,v,weight" << endl;
        for (const auto &uVertex : vertices)
        {
            for (const auto &edge : uVertex.edges)
            {
                // 由于是无向图，仅在 u < v 时打印以避免重复
                // 或者如果 graphviz 逻辑需要，则打印两者。Graphviz 的“严格图”或只是手动去重？
                // Graphviz `graph` 处理严格/无向图。
                // 为简单起见，让我们全部打印，或去重。
                if (uVertex.id < vertices[idToIndex[edge.destination]].id)
                {
                    cout << uVertex.id << "," << vertices[idToIndex[edge.destination]].id << "," << edge.weight << endl;
                }
            }
        }
    }

    // Dijkstra算法
    pair<map<int, int>, map<int, int>> dijkstra(int startId)
    {
        map<int, int> dist;
        map<int, int> parent;

        for (const auto& v : vertices) {
            dist[v.id] = INT_MAX;
            parent[v.id] = -1;
        }

        if (idToIndex.find(startId) == idToIndex.end()) return {dist, parent};

        dist[startId] = 0;

        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        pq.push({0, startId});

        while (!pq.empty()) {
            int d = pq.top().first;
            int u = pq.top().second;
            pq.pop();

            if (d > dist[u]) continue;

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

    // 辅助函数，以ID向量形式获取路径
    vector<int> getShortestPath(int startId, int endId)
    {
        vector<int> path;
        if (idToIndex.find(startId) == idToIndex.end() || idToIndex.find(endId) == idToIndex.end())
        {
            return path; // 空路径
        }

        auto result = dijkstra(startId);
        map<int, int> dist = result.first;
        map<int, int> parent = result.second;

        if (dist[endId] == INT_MAX)
            return path; // 没有路径

        stack<int> s;
        int curr = endId;
        while (curr != -1) {
            s.push(curr);
            if (curr == startId) break;
            curr = parent[curr];
        }

        while (!s.empty()) {
            path.push_back(s.top());
            s.pop();
        }
        return path;
    }

    // 扩展：为Python打印带距离的路径
    // 格式：Path: A->B->C | Total Distance: 500
    void printPathWithDistance(int startId, int endId)
    {
        vector<int> path = getShortestPath(startId, endId);
        if (path.empty())
        {
            cout << "未找到路径。" << endl;
            return;
        }

        auto result = dijkstra(startId);
        int distance = result.first[endId];

        cout << "Path: ";
        for (size_t i = 0; i < path.size(); ++i)
        {
            cout << vertices[idToIndex[path[i]]].name;
            if (i < path.size() - 1)
                cout << "->";
        }
        cout << " | Total Distance: " << distance << endl;
    }

    // 扩展：打印路径名称CSV（单行或列表）
    // 格式：Name1,Name2,Name3...
    void printPathNamesCSV(int startId, int endId)
    {
        vector<int> path = getShortestPath(startId, endId);
        if (path.empty())
        {
            // 什么都不打印还是报错？“纯结果”通常意味着如果没有则为空。
            return;
        }
        for (size_t i = 0; i < path.size(); ++i)
        {
            cout << vertices[idToIndex[path[i]]].name;
            if (i < path.size() - 1)
                cout << ",";
        }
        cout << endl;
    }

    void printShortestPath(int startId, int endId) {
        vector<int> path = getShortestPath(startId, endId);

        if (path.empty()) {
            if (idToIndex.find(startId) == idToIndex.end() || idToIndex.find(endId) == idToIndex.end())
                 cout << "Invalid IDs." << endl;
            else
                 cout << "No path found." << endl;
            return;
        }

        auto result = dijkstra(startId);
        int distance = result.first[endId];
        cout << "Shortest path length: " << distance << endl;
        cout << "Path: ";
        for (size_t i = 0; i < path.size(); ++i) {
            cout << vertices[idToIndex[path[i]]].name;
            if (i < path.size() - 1) cout << " -> ";
        }
        cout << endl;
    }

    // 任务5：文件I/O
    void loadMapFromFile(string filename)
    {
        ifstream inFile(filename);
        if (!inFile) {
            cout << "Error opening file: " << filename << endl;
            return;
        }

        clear();
        string line;
        bool readingEdges = false;

        while (getline(inFile, line)) {
            if (line.empty()) continue;
            if (line == "LOCATIONS") {
                readingEdges = false;
                continue;
            }
            if (line == "EDGES") {
                readingEdges = true;
                continue;
            }

            stringstream ss(line);
            if (!readingEdges) {
                int id, popularity;
                string name, info;
                // 假设格式：id popularity name info(行尾剩余部分)

                ss >> id >> popularity >> name;
                string tempInfo;
                getline(ss, tempInfo);
                // 从info中修剪前导空格
                if (!tempInfo.empty() && tempInfo[0] == ' ')
                    tempInfo = tempInfo.substr(1);

                addLocation(id, name, tempInfo, popularity);
            } else {
                int u, v, w;
                ss >> u >> v >> w;
                addPath(u, v, w);
            }
        }
        cout << "Map loaded from " << filename << endl;
    }

    void exportPathToFile(string filename, const vector<int>& path) {
        if (path.empty()) {
            cout << "No path to export." << endl;
            return;
        }

        ofstream outFile(filename);
        if (!outFile) {
            cout << "Error opening file for writing: " << filename << endl;
            return;
        }

        outFile << "Path Export:" << endl;
        for (size_t i = 0; i < path.size(); ++i) {
            int id = path[i];
            outFile << vertices[idToIndex[id]].name;
            if (i < path.size() - 1) {
                 outFile << " -> ";
            }
        }
        outFile << endl;
        outFile.close();
        cout << "Path exported to " << filename << endl;
    }

    bool isEmpty() const {
        return vertices.empty();
    }
};

#endif // GRAPH_H
