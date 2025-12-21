#ifndef BST_H
#define BST_H

#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>

using namespace std;

// BST Node Structure
struct BSTNode
{
    string word;
    string meaning;
    BSTNode *left;
    BSTNode *right;

    BSTNode(string w, string m) : word(w), meaning(m), left(nullptr), right(nullptr) {}
};

// Binary Search Tree Class
class BST
{
private:
    BSTNode *root;

    // Helper: Insert recursively
    BSTNode *insert(BSTNode *node, string word, string meaning)
    {
        if (node == nullptr)
        {
            return new BSTNode(word, meaning);
        }

        if (word < node->word)
        {
            node->left = insert(node->left, word, meaning);
        }
        else if (word > node->word)
        {
            node->right = insert(node->right, word, meaning);
        }
        else
        {
            // Word already exists, update meaning
            node->meaning = meaning;
            // Removed verbose output for bulk operations
        }
        return node;
    }

    // Helper: Search recursively
    string search(BSTNode *node, string word)
    {
        if (node == nullptr)
        {
            return "Word not found in the dictionary.";
        }

        if (word == node->word)
        {
            return node->meaning;
        }
        else if (word < node->word)
        {
            return search(node->left, word);
        }
        else
        {
            return search(node->right, word);
        }
    }

    // Helper: Find minimum value node in a subtree (used for deletion)
    BSTNode *findMin(BSTNode *node)
    {
        while (node && node->left != nullptr)
        {
            node = node->left;
        }
        return node;
    }

    // Helper: Delete recursively
    BSTNode *remove(BSTNode *node, string word)
    {
        if (node == nullptr)
        {
            cout << "Word '" << word << "' not found." << endl;
            return nullptr;
        }

        // 1. Locate the node to be deleted
        if (word < node->word)
        {
            node->left = remove(node->left, word);
        }
        else if (word > node->word)
        {
            node->right = remove(node->right, word);
        }
        else
        {
            // Found the node to delete

            // Case 1: Node has no children (Leaf node)
            if (node->left == nullptr && node->right == nullptr)
            {
                delete node;
                node = nullptr;
            }
            // Case 2: Node has only one child
            else if (node->left == nullptr)
            {
                BSTNode *temp = node;
                node = node->right; // Replace with right child
                delete temp;
            }
            else if (node->right == nullptr)
            {
                BSTNode *temp = node;
                node = node->left; // Replace with left child
                delete temp;
            }
            // Case 3: Node has two children
            else
            {
                // Strategy: Find the Inorder Successor (smallest node in the right subtree)
                BSTNode *temp = findMin(node->right);

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
    void inOrder(BSTNode *node)
    {
        if (node == nullptr)
            return;

        inOrder(node->left);
        cout << left << setw(20) << node->word << ": " << node->meaning << endl;
        inOrder(node->right);
    }

    // Helper: Clear tree (destructor)
    void clear(BSTNode *node)
    {
        if (node == nullptr)
            return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    // Helper: Calculate positions for tree visualization
    void calcPos(BSTNode *node, int &x, int y, map<BSTNode *, pair<int, int>> &pos)
    {
        if (!node)
            return;
        calcPos(node->left, x, y + 2, pos);
        pos[node] = {x, y};
        x += node->word.length() + 2; // Spacing
        calcPos(node->right, x, y + 2, pos);
    }

    // Helper: Recursive fuzzy search
    void searchByPrefix(BSTNode *node, string prefix)
    {
        if (node == nullptr)
            return;

        // Optimized traversal:
        // If node's word is smaller than prefix, we only need to look at right subtree
        // But prefix matching is tricky because "apple" > "app".
        // Let's stick to standard In-Order traversal and check condition for simplicity and correctness

        searchByPrefix(node->left, prefix);

        // Check if node->word starts with prefix
        if (node->word.find(prefix) == 0)
        {
            cout << left << setw(20) << node->word << ": " << node->meaning << endl;
        }

        searchByPrefix(node->right, prefix);
    }

    // Helper: Save to file (In-order traversal)
    void saveToFile(BSTNode *node, ofstream &outFile)
    {
        if (node == nullptr)
            return;
        saveToFile(node->left, outFile);
        outFile << node->word << ":" << node->meaning << endl;
        saveToFile(node->right, outFile);
    }

public:
    BST() : root(nullptr) {}

    ~BST()
    {
        clear(root);
    }

    void insert(string word, string meaning)
    {
        root = insert(root, word, meaning);
    }

    string search(string word)
    {
        return search(root, word);
    }

    void remove(string word)
    {
        root = remove(root, word);
    }

    void inOrder()
    {
        if (root == nullptr)
        {
            cout << "Dictionary is empty." << endl;
        }
        else
        {
            cout << "----------------------------------------" << endl;
            cout << left << setw(20) << "Word" << "Meaning" << endl;
            cout << "----------------------------------------" << endl;
            inOrder(root);
            cout << "----------------------------------------" << endl;
        }
    }

    // Extension: Visualization
    void printTree()
    {
        if (root == nullptr)
        {
            cout << "Tree is empty." << endl;
            return;
        }

        map<BSTNode *, pair<int, int>> pos;
        int x = 0;
        calcPos(root, x, 0, pos);

        int height = 0;
        for (auto const &entry : pos)
        {
            if (entry.second.second > height)
                height = entry.second.second;
        }

        // Create canvas
        vector<string> canvas(height + 1, string(x, ' '));

        for (auto const &entry : pos)
        {
            BSTNode *node = entry.first;
            int px = entry.second.first;
            int py = entry.second.second;
            string w = node->word;

            // Print word
            for (int i = 0; i < w.length(); ++i)
            {
                if (px + i < x)
                    canvas[py][px + i] = w[i];
            }

            // Draw branches
            if (node->left)
            {
                pair<int, int> lp = pos[node->left];
                int lx_center = lp.first + node->left->word.length() / 2;

                if (py + 1 < canvas.size())
                {
                    if (px - 1 >= 0)
                        canvas[py + 1][px - 1] = '/';
                    for (int k = lx_center + 1; k < px - 1; k++)
                        canvas[py + 1][k] = '_';
                }
            }
            if (node->right)
            {
                pair<int, int> rp = pos[node->right];
                int rx_center = rp.first + node->right->word.length() / 2;
                int px_end = px + w.length();

                if (py + 1 < canvas.size())
                {
                    if (px_end < x)
                        canvas[py + 1][px_end] = '\\';
                    for (int k = px_end + 1; k < rx_center; k++)
                        canvas[py + 1][k] = '_';
                }
            }
        }

        for (const string &line : canvas)
        {
            cout << line << endl;
        }
    }

    // Extension: Fuzzy Search
    void searchByPrefix(string prefix)
    {
        cout << "Words starting with '" << prefix << "':" << endl;
        cout << "----------------------------------------" << endl;
        searchByPrefix(root, prefix);
        cout << "----------------------------------------" << endl;
    }

    // Extension: File I/O
    void saveToFile(string filename)
    {
        ofstream outFile(filename);
        if (!outFile)
        {
            cout << "Error opening file for writing: " << filename << endl;
            return;
        }
        saveToFile(root, outFile);
        outFile.close();
        cout << "Dictionary saved to " << filename << endl;
    }

    void loadFromFile(string filename)
    {
        ifstream inFile(filename);
        if (!inFile)
        {
            cout << "Error opening file for reading: " << filename << endl;
            return;
        }

        string line;
        int count = 0;
        while (getline(inFile, line))
        {
            size_t delimiterPos = line.find(':');
            if (delimiterPos != string::npos)
            {
                string word = line.substr(0, delimiterPos);
                string meaning = line.substr(delimiterPos + 1);
                insert(word, meaning);
                count++;
            }
        }
        inFile.close();
        cout << "Loaded " << count << " entries from " << filename << endl;
    }
};

#endif // BST_H
