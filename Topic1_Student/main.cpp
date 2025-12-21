#include <iostream>
#include <vector>
#include <algorithm>
#include "Student.h"

using namespace std;

// --- QuickSort Implementation (Ascending by ID) ---

int partition(vector<Student>& students, int low, int high) {
    string pivot = students[high].id;
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        // Ascending order: if current element is smaller than pivot, increment i and swap
        if (students[j].id < pivot) {
            i++;
            swap(students[i], students[j]);
        }
    }
    swap(students[i + 1], students[high]);
    return (i + 1);
}

void quickSort(vector<Student>& students, int low, int high) {
    if (low < high) {
        int pi = partition(students, low, high);
        quickSort(students, low, pi - 1);
        quickSort(students, pi + 1, high);
    }
}

// --- HeapSort Implementation (Descending by Score) ---

// To sort descending, we want the smallest elements at the end.
// A Min-Heap has the smallest element at the root.
// If we swap the root (min) with the last element, the last element becomes the smallest.
// Then we reduce the heap size and heapify.
// Repeating this will place elements in descending order: [Largest, ..., Smallest]
// Wait.
// Step 1: Build Min Heap. Root = Min.
// Step 2: Swap Root with End. End = Min.
// Step 3: Reduce heap size by 1.
// Repeat.
// Array state: [Heap ... | Sorted Mins]
// Final state: [Largest ... Smallest]

void heapify(vector<Student>& students, int n, int i) {
    int smallest = i; // Initialize smallest as root
    int l = 2 * i + 1; // left = 2*i + 1
    int r = 2 * i + 2; // right = 2*i + 2

    // If left child is smaller than root
    if (l < n && students[l].score < students[smallest].score)
        smallest = l;

    // If right child is smaller than smallest so far
    if (r < n && students[r].score < students[smallest].score)
        smallest = r;

    // If smallest is not root
    if (smallest != i) {
        swap(students[i], students[smallest]);
        // Recursively heapify the affected sub-tree
        heapify(students, n, smallest);
    }
}

void heapSort(vector<Student>& students) {
    int n = students.size();

    // Build heap (rearrange array)
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(students, n, i);

    // One by one extract an element from heap
    for (int i = n - 1; i > 0; i--) {
        // Move current root to end
        swap(students[0], students[i]);

        // call max heapify on the reduced heap
        heapify(students, i, 0);
    }
}

// --- Main Function ---

// Custom function to check if the student list is empty, as requested.
bool isStudentListEmpty(const vector<Student>& students) {
    return students.empty();
}

void printStudents(const vector<Student>& students) {
    printStudentHeader();
    for (const auto& s : students) {
        printStudent(s);
    }
    cout << endl;
}

void inputStudents(vector<Student>& students) {
    int n;
    cout << "Enter number of students to add: ";
    if (!(cin >> n)) {
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }

    for (int i = 0; i < n; ++i) {
        Student s;
        cout << "Student " << (i + 1) << ":" << endl;
        cout << "  ID: ";
        cin >> s.id;
        cout << "  Name: ";
        cin >> s.name;
        cout << "  Score: ";
        cin >> s.score;
        students.push_back(s);
    }
    cout << n << " students added." << endl;
}

int main() {
    vector<Student> students;

    // // Pre-populate with some data for testing if empty
    // students.push_back({"1003", "Alice", 85.5});
    // students.push_back({"1001", "Bob", 92.0});
    // students.push_back({"1005", "Charlie", 78.5});
    // students.push_back({"1002", "David", 88.0});
    // students.push_back({"1004", "Eve", 95.5});

    int choice;
    do {
        cout << "========================================" << endl;
        cout << "Topic 1: Student Grade System" << endl;
        cout << "========================================" << endl;
        cout << "1. Add Students" << endl;
        cout << "2. View Students (Current Order)" << endl;
        cout << "3. QuickSort by ID (Ascending)" << endl;
        cout << "4. HeapSort by Score (Descending)" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            choice = -1;
        }

        switch (choice) {
            case 1:
                inputStudents(students);
                break;
            case 2:
                if (isStudentListEmpty(students)) {
                    cout << "No students to display." << endl;
                } else {
                    printStudents(students);
                }
                break;
            case 3:
                if (!isStudentListEmpty(students)) {
                    quickSort(students, 0, students.size() - 1);
                    cout << "Sorted by ID (Ascending):" << endl;
                    printStudents(students);
                } else {
                    cout << "No students to display." << endl;
                }
                break;
            case 4:
                if (!isStudentListEmpty(students)) {
                    heapSort(students);
                    cout << "Sorted by Score (Descending):" << endl;
                    printStudents(students);
                } else {
                    cout << "No students to display." << endl;
                }
                break;
            case 0:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
        cout << endl;

    } while (choice != 0);

    return 0;
}
