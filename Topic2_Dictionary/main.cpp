#include <iostream>
#include <limits>
#include "BST.h"

using namespace std;

void pause() {
    cout << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

int main() {
    BST dictionary;
    int choice;
    string word, meaning, filename;

    // Pre-populate dictionary
    dictionary.insert("Apple", "A fruit");
    dictionary.insert("Banana", "A long yellow fruit");
    dictionary.insert("Cat", "A small domesticated carnivorous mammal");
    dictionary.insert("Dog", "A domesticated carnivorous mammal");
    dictionary.insert("Elephant", "A very large plant-eating mammal");
    dictionary.insert("Application", "A formal request to be considered for a position");
    dictionary.insert("Appetite", "A natural desire to satisfy a bodily need, especially for food");


    do {
        cout << "\n========================================" << endl;
        cout << "Topic 2: Dictionary System (BST)" << endl;
        cout << "========================================" << endl;
        cout << "1. Insert New Word" << endl;
        cout << "2. Search Word" << endl;
        cout << "3. Delete Word" << endl;
        cout << "4. Print All Words (A-Z)" << endl;
        cout << "5. Fuzzy Search (Prefix)" << endl;
        cout << "6. Save to File" << endl;
        cout << "7. Load from File" << endl;
        cout << "8. Print Tree Structure" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer for getline

        switch (choice) {
            case 1:
                cout << "Enter word: ";
                getline(cin, word);
                cout << "Enter meaning: ";
                getline(cin, meaning);
                dictionary.insert(word, meaning);
                cout << "Word inserted/updated successfully." << endl;
                break;
            case 2:
                cout << "Enter word to search: ";
                getline(cin, word);
                cout << "Result: " << dictionary.search(word) << endl;
                break;
            case 3:
                cout << "Enter word to delete: ";
                getline(cin, word);
                dictionary.remove(word);
                cout << "Operation completed." << endl;
                break;
            case 4:
                dictionary.inOrder();
                break;
            case 5:
                cout << "Enter prefix to search: ";
                getline(cin, word);
                dictionary.searchByPrefix(word);
                break;
            case 6:
                cout << "Enter filename to save (e.g., dict.txt): ";
                getline(cin, filename);
                dictionary.saveToFile(filename);
                break;
            case 7:
                cout << "Enter filename to load (e.g., dict.txt): ";
                getline(cin, filename);
                dictionary.loadFromFile(filename);
                break;
            case 8:
                cout << "Tree Structure Visualization:" << endl;
                dictionary.printTree();
                break;
            case 0:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 0);

    return 0;
}
