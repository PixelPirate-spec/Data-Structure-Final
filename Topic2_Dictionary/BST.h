#ifndef BST_H
#define BST_H

#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// BST Node Structure
struct BSTNode {
    string word;
    string meaning;
    BSTNode* left;
    BSTNode* right;

    BSTNode(string w, string m) : word(w), meaning(m), left(nullptr), right(nullptr) {}
};

// Binary Search Tree Class
class BST {
private:
    BSTNode* root;

    // Helper: Insert recursively
    BSTNode* insert(BSTNode* node, string word, string meaning) {
        if (node == nullptr) {
            return new BSTNode(word, meaning);
        }

        if (word < node->word) {
            node->left = insert(node->left, word, meaning);
        } else if (word > node->word) {
            node->right = insert(node->right, word, meaning);
        } else {
            // Word already exists, update meaning
            node->meaning = meaning;
            cout << "Word '" << word << "' already exists. Meaning updated." << endl;
        }
        return node;
    }

    // Helper: Search recursively
    string search(BSTNode* node, string word) {
        if (node == nullptr) {
            return "Word not found in the dictionary.";
        }

        if (word == node->word) {
            return node->meaning;
        } else if (word < node->word) {
            return search(node->left, word);
        } else {
            return search(node->right, word);
        }
    }

    // Helper: Find minimum value node in a subtree (used for deletion)
    BSTNode* findMin(BSTNode* node) {
        while (node && node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    // Helper: Delete recursively
    BSTNode* remove(BSTNode* node, string word) {
        if (node == nullptr) {
            cout << "Word '" << word << "' not found." << endl;
            return nullptr;
        }

        // 1. Locate the node to be deleted
        if (word < node->word) {
            node->left = remove(node->left, word);
        } else if (word > node->word) {
            node->right = remove(node->right, word);
        } else {
            // Found the node to delete

            // Case 1: Node has no children (Leaf node)
            if (node->left == nullptr && node->right == nullptr) {
                delete node;
                node = nullptr;
            }
            // Case 2: Node has only one child
            else if (node->left == nullptr) {
                BSTNode* temp = node;
                node = node->right; // Replace with right child
                delete temp;
            } else if (node->right == nullptr) {
                BSTNode* temp = node;
                node = node->left; // Replace with left child
                delete temp;
            }
            // Case 3: Node has two children
            else {
                // Strategy: Find the Inorder Successor (smallest node in the right subtree)
                BSTNode* temp = findMin(node->right);

                // Copy the successor's content to this node
                node->word = temp->word;
                node->meaning = temp->meaning;

                // Delete the successor node from the right subtree
                node->right = remove(node->right, temp->word);
            }
        }
        return node;
    }

    // Helper: In-order traversal
    void inOrder(BSTNode* node) {
        if (node == nullptr) return;

        inOrder(node->left);
        cout << left << setw(20) << node->word << ": " << node->meaning << endl;
        inOrder(node->right);
    }

    // Helper: Clear tree (destructor)
    void clear(BSTNode* node) {
        if (node == nullptr) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

public:
    BST() : root(nullptr) {}

    ~BST() {
        clear(root);
    }

    void insert(string word, string meaning) {
        root = insert(root, word, meaning);
    }

    string search(string word) {
        return search(root, word);
    }

    void remove(string word) {
        root = remove(root, word);
    }

    void inOrder() {
        if (root == nullptr) {
            cout << "Dictionary is empty." << endl;
        } else {
            cout << "----------------------------------------" << endl;
            cout << left << setw(20) << "Word" << "Meaning" << endl;
            cout << "----------------------------------------" << endl;
            inOrder(root);
            cout << "----------------------------------------" << endl;
        }
    }
};

#endif // BST_H
