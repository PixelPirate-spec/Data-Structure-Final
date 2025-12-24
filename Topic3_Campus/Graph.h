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

// Edge Structure
struct Edge
{
    int destination;
    int weight;
};

// Vertex Structure
struct Vertex
{
    int id;
    string name;
    string info;
    int popularity;
    vector<Edge> edges;
};

// Campus Graph Class
class CampusGraph
{
private:
    vector<Vertex> vertices;
    map<int, int> idToIndex; // Map user ID to vector index

public:
    void clear()
    {
        vertices.clear();
        idToIndex.clear();
        cout << "地图已清空。" << endl;
    }

    void addLocation(int id, string name, string info, int popularity)
    {
        if (idToIndex.find(id) != idToIndex.end())
        {
            cout << "地点 ID " << id << " 已存在。" << endl;
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

    void addPath(int u, int v, int weight)
    {
        if (idToIndex.find(u) == idToIndex.end() || idToIndex.find(v) == idToIndex.end())
        {
            cout << "无效的地点 ID: " << u << ", " << v << endl;
            return;
        }
        int uIdx = idToIndex[u];
        int vIdx = idToIndex[v];

        // Check for duplicates? For now assume valid input.
        vertices[uIdx].edges.push_back({v, weight});
        vertices[vIdx].edges.push_back({u, weight}); // Undirected graph
    }

    Vertex *getLocation(int id)
    {
        if (idToIndex.find(id) == idToIndex.end())
            return nullptr;
        return &vertices[idToIndex[id]];
    }

    // Task 4: Keyword Search (Prefix Matching)
    void searchSpot(string keyword)
    {
        cout << "'" << keyword << "' 的搜索结果:" << endl;
        cout << left << setw(5) << "ID"
             << setw(20) << "名称"
             << setw(10) << "热度"
             << "信息" << endl;
        cout << string(60, '-') << endl;

        bool found = false;
        for (const auto &v : vertices)
        {
            // Check if name starts with keyword
            if (v.name.find(keyword) == 0)
            {
                cout << left << setw(5) << v.id
                     << setw(20) << v.name
                     << setw(10) << v.popularity
                     << v.info << endl;
                found = true;
            }
        }
        if (!found)
        {
            cout << "未找到匹配的地点。" << endl;
        }
    }

    // Sort locations by popularity (Descending)
    void printSortedByPopularity()
    {
        if (vertices.empty())
        {
            cout << "没有可用的地点。" << endl;
            return;
        }

        vector<Vertex> sortedVertices = vertices;
        sort(sortedVertices.begin(), sortedVertices.end(), [](const Vertex &a, const Vertex &b)
             { return a.popularity > b.popularity; });

        cout << "--- 按热度排序 (降序) ---" << endl;
        cout << left << setw(5) << "ID"
             << setw(20) << "名称"
             << setw(10) << "热度"
             << "信息" << endl;
        cout << string(60, '-') << endl;

        for (const auto &v : sortedVertices)
        {
            cout << left << setw(5) << v.id
                 << setw(20) << v.name
                 << setw(10) << v.popularity
                 << v.info << endl;
        }
    }

    // Sort locations by ID (Ascending)
    void printSortedById()
    {
        if (vertices.empty())
        {
            cout << "没有可用的地点。" << endl;
            return;
        }

        vector<Vertex> sortedVertices = vertices;
        sort(sortedVertices.begin(), sortedVertices.end(), [](const Vertex &a, const Vertex &b)
             { return a.id < b.id; });

        cout << "--- 按 ID 排序 (升序) ---" << endl;
        cout << left << setw(5) << "ID"
             << setw(20) << "名称"
             << setw(10) << "热度"
             << "信息" << endl;
        cout << string(60, '-') << endl;

        for (const auto &v : sortedVertices)
        {
            cout << left << setw(5) << v.id
                 << setw(20) << v.name
                 << setw(10) << v.popularity
                 << v.info << endl;
        }
    }

    // Extension: Print Locations CSV
    void printLocationsCSV()
    {
        cout << "id,name,popularity,info" << endl;
        for (const auto &v : vertices)
        {
            cout << v.id << "," << v.name << "," << v.popularity << "," << v.info << endl;
        }
    }

    // Extension: Print Edges CSV for visualization
    // u,v,weight
    void printEdgesCSV()
    {
        cout << "u,v,weight" << endl;
        for (const auto &uVertex : vertices)
        {
            for (const auto &edge : uVertex.edges)
            {
                // Since undirected, print only if u < v to avoid duplicates
                // Or print both if required by graphviz logic. Graphviz "strict graph" or just manual deduplication?
                // Graphviz `graph` handles strict/undirected.
                // Let's print all for simplicity, or deduplicate.
                if (uVertex.id < vertices[idToIndex[edge.destination]].id)
                {
                    cout << uVertex.id << "," << vertices[idToIndex[edge.destination]].id << "," << edge.weight << endl;
                }
            }
        }
    }

    // Dijkstra's Algorithm
    pair<map<int, int>, map<int, int>> dijkstra(int startId)
    {
        map<int, int> dist;
        map<int, int> parent;

        for (const auto &v : vertices)
        {
            dist[v.id] = INT_MAX;
            parent[v.id] = -1;
        }

        if (idToIndex.find(startId) == idToIndex.end())
            return {dist, parent};

        dist[startId] = 0;

        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        pq.push({0, startId});

        while (!pq.empty())
        {
            int d = pq.top().first;
            int u = pq.top().second;
            pq.pop();

            if (d > dist[u])
                continue;

            Vertex &uVertex = vertices[idToIndex[u]];

            for (const auto &edge : uVertex.edges)
            {
                int v = edge.destination;
                int weight = edge.weight;

                if (dist[u] != INT_MAX && dist[u] + weight < dist[v])
                {
                    dist[v] = dist[u] + weight;
                    parent[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }
        return {dist, parent};
    }

    // Helper to get path as a vector of IDs
    vector<int> getShortestPath(int startId, int endId)
    {
        vector<int> path;
        if (idToIndex.find(startId) == idToIndex.end() || idToIndex.find(endId) == idToIndex.end())
        {
            return path; // Empty path
        }

        auto result = dijkstra(startId);
        map<int, int> dist = result.first;
        map<int, int> parent = result.second;

        if (dist[endId] == INT_MAX)
            return path; // No path

        stack<int> s;
        int curr = endId;
        while (curr != -1)
        {
            s.push(curr);
            if (curr == startId)
                break;
            curr = parent[curr];
        }

        while (!s.empty())
        {
            path.push_back(s.top());
            s.pop();
        }
        return path;
    }

    // Extension: Print Path with Distance for Python
    // Format: Path: A->B->C | Total Distance: 500
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

    // Extension: Print Path Names CSV (Single line or List)
    // Format: Name1,Name2,Name3...
    void printPathNamesCSV(int startId, int endId)
    {
        vector<int> path = getShortestPath(startId, endId);
        if (path.empty())
        {
            // Print nothing or error? "pure results" usually implies empty if nothing.
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

    void printShortestPath(int startId, int endId)
    {
        vector<int> path = getShortestPath(startId, endId);

        if (path.empty())
        {
            if (idToIndex.find(startId) == idToIndex.end() || idToIndex.find(endId) == idToIndex.end())
                cout << "无效的 ID。" << endl;
            else
                cout << "未找到路径。" << endl;
            return;
        }

        auto result = dijkstra(startId);
        int distance = result.first[endId];
        cout << "最短路径长度: " << distance << endl;
        cout << "路径: ";
        for (size_t i = 0; i < path.size(); ++i)
        {
            cout << vertices[idToIndex[path[i]]].name;
            if (i < path.size() - 1)
                cout << " -> ";
        }
        cout << endl;
    }

    // Task 5: File I/O
    void loadMapFromFile(string filename)
    {
        ifstream inFile(filename);
        if (!inFile)
        {
            cout << "打开文件失败: " << filename << endl;
            return;
        }

        clear();
        string line;
        bool readingEdges = false;

        while (getline(inFile, line))
        {
            if (line.empty())
                continue;
            if (line == "LOCATIONS")
            {
                readingEdges = false;
                continue;
            }
            if (line == "EDGES")
            {
                readingEdges = true;
                continue;
            }

            stringstream ss(line);
            if (!readingEdges)
            {
                int id, popularity;
                string name, info;
                // Assuming format: id popularity name info(rest of line)

                ss >> id >> popularity >> name;
                string tempInfo;
                getline(ss, tempInfo);
                // Trim leading space from info
                if (!tempInfo.empty() && tempInfo[0] == ' ')
                    tempInfo = tempInfo.substr(1);

                addLocation(id, name, tempInfo, popularity);
            }
            else
            {
                int u, v, w;
                ss >> u >> v >> w;
                addPath(u, v, w);
            }
        }
        cout << "地图已从 " << filename << " 加载。" << endl;
    }

    void exportPathToFile(string filename, const vector<int> &path)
    {
        if (path.empty())
        {
            cout << "没有可导出的路径。" << endl;
            return;
        }

        ofstream outFile(filename);
        if (!outFile)
        {
            cout << "写入文件打开失败: " << filename << endl;
            return;
        }

        outFile << "路径导出:" << endl;
        for (size_t i = 0; i < path.size(); ++i)
        {
            int id = path[i];
            outFile << vertices[idToIndex[id]].name;
            if (i < path.size() - 1)
            {
                outFile << " -> ";
            }
        }
        outFile << endl;
        outFile.close();
        cout << "路径已导出至 " << filename << endl;
    }

    bool isEmpty() const
    {
        return vertices.empty();
    }
};

#endif // GRAPH_H
