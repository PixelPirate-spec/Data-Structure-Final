#include <iostream>
#include <limits>
#include <fstream>
#include "Graph.h"

using namespace std;

void pause() {
    cout << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void initCampus(CampusGraph& campus) {
    string defaultFile = "map_data.txt";
    ifstream f(defaultFile);
    if (f.good()) {
        f.close();
        campus.loadMapFromFile(defaultFile);
    } else {
        f.close();
        // Pre-populate campus data (Default)
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
}

int main(int argc, char* argv[]) {
    CampusGraph campus;
    initCampus(campus);

    // CLI Mode
    if (argc > 1) {
        string command = argv[1];
        if (command == "path") {
            if (argc < 4) {
                cerr << "Usage: ./app path <startId> <endId>" << endl;
                return 1;
            }
            int startId = stoi(argv[2]);
            int endId = stoi(argv[3]);
            campus.printPathWithDistance(startId, endId);
        } else if (command == "locations") {
            campus.printLocationsCSV();
        } else if (command == "edges") {
            campus.printEdgesCSV();
        } else if (command == "search") {
             if (argc < 3) {
                cerr << "Usage: ./app search <keyword>" << endl;
                return 1;
            }
            string keyword = argv[2];
            campus.searchSpot(keyword);
        } else {
            cerr << "Unknown command: " << command << endl;
            return 1;
        }
        return 0; // Exit after CLI command
    }

    int choice;
    vector<int> lastPath; // Store the last calculated path for export

    do {
        cout << "\n========================================" << endl;
        cout << "Topic 3: Campus Navigation System" << endl;
        cout << "========================================" << endl;
        cout << "1. Add Location" << endl;
        cout << "2. Add Path" << endl;
        cout << "3. Query Shortest Path" << endl;
        cout << "4. Browse Locations (Sort by Popularity/ID)" << endl;
        cout << "5. Keyword Search (Prefix)" << endl;
        cout << "6. Load Map from File" << endl;
        cout << "7. Export Last Path to File" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer for getline

        switch (choice) {
            case 1: {
                int id, pop;
                string name, info;
                cout << "Enter ID: "; cin >> id;
                cout << "Enter Name (No spaces, use _): "; cin >> name;
                cout << "Enter Popularity (0-100): "; cin >> pop;
                cin.ignore(); // consume newline
                cout << "Enter Info: "; getline(cin, info);
                campus.addLocation(id, name, info, pop);
                cout << "Location added." << endl;
                break;
            }
            case 2: {
                int u, v, w;
                cout << "Enter Start ID: "; cin >> u;
                cout << "Enter End ID: "; cin >> v;
                cout << "Enter Weight: "; cin >> w;
                campus.addPath(u, v, w);
                cout << "Path added." << endl;
                break;
            }
            case 3: {
                int startId, endId;
                cout << "Enter Start Location ID: ";
                cin >> startId;
                cout << "Enter End Location ID: ";
                cin >> endId;
                campus.printShortestPath(startId, endId);
                // Update lastPath
                lastPath = campus.getShortestPath(startId, endId);
                break;
            }
            case 4: {
                int sortChoice;
                cout << "1. Sort by Popularity (Descending)" << endl;
                cout << "2. Sort by ID (Ascending)" << endl;
                cout << "Choice: ";
                cin >> sortChoice;
                if (sortChoice == 1) campus.printSortedByPopularity();
                else if (sortChoice == 2) campus.printSortedById();
                else cout << "Invalid choice." << endl;
                break;
            }
            case 5: {
                string keyword;
                cout << "Enter keyword (prefix): ";
                cin >> keyword;
                campus.searchSpot(keyword);
                break;
            }
            case 6: {
                string filename;
                cout << "Enter filename: ";
                cin >> filename;
                campus.loadMapFromFile(filename);
                lastPath.clear();
                break;
            }
            case 7: {
                if (lastPath.empty()) {
                    cout << "No path available to export. Please run query (Option 3) first." << endl;
                } else {
                    string filename;
                    cout << "Enter filename to save path: ";
                    cin >> filename;
                    campus.exportPathToFile(filename, lastPath);
                }
                break;
            }
            case 0:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 0);

    return 0;
}
