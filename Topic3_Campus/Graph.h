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

using namespace std;

// Edge Structure
struct Edge {
    int destination;
    int weight;
};

// Vertex Structure
struct Vertex {
    int id;
    string name;
    string info;
    int popularity;
    vector<Edge> edges;
};

// Campus Graph Class
class CampusGraph {
private:
    vector<Vertex> vertices;
    map<int, int> idToIndex; // Map user ID to vector index

public:
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
            cout << "Invalid location IDs." << endl;
            return;
        }
        int uIdx = idToIndex[u];
        int vIdx = idToIndex[v];

        vertices[uIdx].edges.push_back({v, weight});
        vertices[vIdx].edges.push_back({u, weight}); // Undirected graph
    }

    Vertex* getLocation(int id) {
        if (idToIndex.find(id) == idToIndex.end()) return nullptr;
        return &vertices[idToIndex[id]];
    }

    // Sort locations by popularity (Descending) using std::sort (QuickSort/IntroSort)
    void printSortedByPopularity() {
        if (vertices.empty()) {
            cout << "No locations available." << endl;
            return;
        }

        vector<Vertex> sortedVertices = vertices;
        sort(sortedVertices.begin(), sortedVertices.end(), [](const Vertex& a, const Vertex& b) {
            return a.popularity > b.popularity;
        });

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

    // Dijkstra's Algorithm
    // Returns a pair: (Distances Map, Parents Map)
    // Distances: id -> min distance
    // Parents: id -> parent id (for path reconstruction)
    pair<map<int, int>, map<int, int>> dijkstra(int startId) {
        map<int, int> dist;
        map<int, int> parent;

        for (const auto& v : vertices) {
            dist[v.id] = INT_MAX;
            parent[v.id] = -1;
        }

        if (idToIndex.find(startId) == idToIndex.end()) return {dist, parent};

        dist[startId] = 0;

        // Priority Queue stores pair<distance, id>, ordered by distance (min-heap)
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        pq.push({0, startId});

        while (!pq.empty()) {
            int d = pq.top().first;
            int u = pq.top().second;
            pq.pop();

            if (d > dist[u]) continue;

            // Get actual vertex object to iterate edges
            Vertex& uVertex = vertices[idToIndex[u]];

            for (const auto& edge : uVertex.edges) {
                int v = edge.destination;
                int weight = edge.weight;

                if (dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    parent[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }
        return {dist, parent};
    }

    void printShortestPath(int startId, int endId) {
        if (idToIndex.find(startId) == idToIndex.end() || idToIndex.find(endId) == idToIndex.end()) {
            cout << "One or both location IDs are invalid." << endl;
            return;
        }

        auto result = dijkstra(startId);
        map<int, int> dist = result.first;
        map<int, int> parent = result.second;

        if (dist[endId] == INT_MAX) {
            cout << "No path exists between " << vertices[idToIndex[startId]].name
                 << " and " << vertices[idToIndex[endId]].name << "." << endl;
            return;
        }

        cout << "Shortest path length: " << dist[endId] << endl;
        cout << "Path: ";

        stack<int> s;
        int curr = endId;
        while (curr != -1) {
            s.push(curr);
            if (curr == startId) break; // Should happen naturally if logic is correct
            curr = parent[curr];
        }

        while (!s.empty()) {
            int id = s.top();
            s.pop();
            cout << vertices[idToIndex[id]].name;
            if (!s.empty()) cout << " -> ";
        }
        cout << endl;
    }

    // Helper to check if empty
    bool isEmpty() const {
        return vertices.empty();
    }
};

#endif // GRAPH_H
