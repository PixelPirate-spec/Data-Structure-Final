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
    string word, meaning;

    // Pre-populate dictionary
    dictionary.insert("Apple", "A fruit");
    dictionary.insert("Banana", "A long yellow fruit");
    dictionary.insert("Cat", "A small domesticated carnivorous mammal");
    dictionary.insert("Dog", "A domesticated carnivorous mammal");
    dictionary.insert("Elephant", "A very large plant-eating mammal");

    do {
        cout << "\n========================================" << endl;
        cout << "Topic 2: Dictionary System (BST)" << endl;
        cout << "========================================" << endl;
        cout << "1. Insert New Word" << endl;
        cout << "2. Search Word" << endl;
        cout << "3. Delete Word" << endl;
        cout << "4. Print All Words (A-Z)" << endl;
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
            case 0:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }

        if (choice != 0) {
           // pause();
        }

    } while (choice != 0);

    return 0;
}
