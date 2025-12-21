#include <iostream>
#include <limits>
#include "Graph.h"

using namespace std;

void pause() {
    cout << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

int main() {
    CampusGraph campus;
    int choice;

    // Pre-populate campus data
    // IDs: 1:Gate, 2:Library, 3:Canteen, 4:Dorm, 5:Classroom, 6:Gym, 7:Admin
    campus.addLocation(1, "Main Gate", "The main entrance.", 80);
    campus.addLocation(2, "Library", "A quiet place to study.", 95);
    campus.addLocation(3, "Canteen", "Tasty and cheap food.", 90);
    campus.addLocation(4, "Dormitory", "Where students live.", 85);
    campus.addLocation(5, "Teaching Bldg", "Classes are held here.", 88);
    campus.addLocation(6, "Gymnasium", "Sports and events.", 75);
    campus.addLocation(7, "Admin Bldg", "Administrative offices.", 50);

    // Add paths (Undirected)
    // Gate(1) - [200] -> Teaching(5)
    // Gate(1) - [500] -> Gym(6)
    // Teaching(5) - [100] -> Library(2)
    // Teaching(5) - [150] -> Canteen(3)
    // Library(2) - [300] -> Dorm(4)
    // Canteen(3) - [50] -> Dorm(4)
    // Gym(6) - [250] -> Canteen(3)
    // Gym(6) - [400] -> Admin(7)
    // Admin(7) - [100] -> Library(2) -- Making a loop

    campus.addPath(1, 5, 200);
    campus.addPath(1, 6, 500);
    campus.addPath(5, 2, 100);
    campus.addPath(5, 3, 150);
    campus.addPath(2, 4, 300);
    campus.addPath(3, 4, 50);
    campus.addPath(6, 3, 250);
    campus.addPath(6, 7, 400);
    campus.addPath(7, 2, 100);

    do {
        cout << "\n========================================" << endl;
        cout << "Topic 3: Campus Navigation System" << endl;
        cout << "========================================" << endl;
        cout << "1. Find Shortest Path" << endl;
        cout << "2. View All Locations (By Popularity)" << endl;
        cout << "3. Find Location Info" << endl;
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
                int startId, endId;
                cout << "Enter Start Location ID: ";
                cin >> startId;
                cout << "Enter End Location ID: ";
                cin >> endId;
                campus.printShortestPath(startId, endId);
                break;
            }
            case 2:
                campus.printSortedByPopularity();
                break;
            case 3: {
                int id;
                cout << "Enter Location ID: ";
                cin >> id;
                Vertex* v = campus.getLocation(id);
                if (v) {
                    cout << "Name: " << v->name << endl;
                    cout << "Info: " << v->info << endl;
                    cout << "Popularity: " << v->popularity << endl;
                } else {
                    cout << "Location not found." << endl;
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
