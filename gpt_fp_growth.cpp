#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>

using namespace std;

// Structure for FP-Tree Node
struct FPTreeNode {
    string item;
    int count;
    FPTreeNode* parent;
    unordered_map<string, FPTreeNode*> children;
    FPTreeNode* next;

    FPTreeNode(string item = "", int count = 1, FPTreeNode* parent = nullptr)
        : item(item), count(count), parent(parent), next(nullptr) {}
};

// Structure for Header Table Entry
struct HeaderTableEntry {
    int support;
    FPTreeNode* node;
    FPTreeNode* lastNode;

    HeaderTableEntry(int support = 0, FPTreeNode* node = nullptr)
        : support(support), node(node), lastNode(node) {}
};

// Structure for the FP-Tree
struct FPTree {
    FPTreeNode* root;
    unordered_map<string, HeaderTableEntry> headerTable;

    FPTree() {
        root = new FPTreeNode();  // The root is an empty node
    }

    // Insert a transaction into the FP-Tree
    void insertTransaction(const vector<string>& transaction, int count = 1) {
        FPTreeNode* currentNode = root;

        for (const string& item : transaction) {
            if (currentNode->children.find(item) == currentNode->children.end()) {
                FPTreeNode* newNode = new FPTreeNode(item, count, currentNode);
                currentNode->children[item] = newNode;

                if (headerTable.find(item) == headerTable.end()) {
                    headerTable[item] = HeaderTableEntry(count, newNode);
                } else {
                    headerTable[item].support += count;
                    headerTable[item].lastNode->next = newNode;
                    headerTable[item].lastNode = newNode;
                }
            } else {
                currentNode->children[item]->count += count;
            }
            currentNode = currentNode->children[item];
        }
    }
};

// Function declarations
vector<pair<vector<string>, int>> findConditionalPatternBase(FPTree& tree, const string& item);
void buildConditionalFPTree(FPTree& originalTree, const string& item);
void mineFPTree(FPTree& tree);
vector<vector<string>> sortTransactions(const vector<vector<string>>& transactions, const unordered_map<string, int>& frequency);
unordered_map<string, int> findFrequentItems(const vector<vector<string>>& transactions, int minSupport);

// Function to find the conditional pattern base
vector<pair<vector<string>, int>> findConditionalPatternBase(FPTree& tree, const string& item) {
    vector<pair<vector<string>, int>> conditionalPatternBase;
    auto it = tree.headerTable.find(item);
    if (it == tree.headerTable.end()) {
        // Item not found in the header table
        cout << "Item " << item << " not found in header table." << endl;
        return conditionalPatternBase;
    }
    FPTreeNode* currentNode = it->second.node;

    while (currentNode != nullptr) {
        vector<string> path;
        FPTreeNode* traverseNode = currentNode->parent;

        while (traverseNode != nullptr && traverseNode->item != "") {
            path.push_back(traverseNode->item);
            traverseNode = traverseNode->parent;
        }

        if (!path.empty()) {
            conditionalPatternBase.push_back({path, currentNode->count});
        }

        currentNode = currentNode->next;
    }

    return conditionalPatternBase;
}

// Function to build a conditional FP-Tree for a given item
void buildConditionalFPTree(FPTree& originalTree, const string& item) {
    vector<pair<vector<string>, int>> conditionalPatternBase = findConditionalPatternBase(originalTree, item);

    if (conditionalPatternBase.empty()) return;

    FPTree conditionalTree;

    for (const auto& pattern : conditionalPatternBase) {
        conditionalTree.insertTransaction(pattern.first, pattern.second);
    }

    mineFPTree(conditionalTree);
}

// Function to mine the frequent itemsets from the FP-Tree
void mineFPTree(FPTree& tree) {
    for (const auto& entry : tree.headerTable) {
        const string& item = entry.first;
        int support = entry.second.support;

        // Frequent itemset for the single item
        cout << "Frequent itemset: " << item << " | Support: " << support << endl;

        // Build and mine the conditional FP-Tree for the current item
        buildConditionalFPTree(tree, item);
    }
}

// Function to sort the transactions based on frequency of items
vector<vector<string>> sortTransactions(const vector<vector<string>>& transactions, const unordered_map<string, int>& frequency) {
    vector<vector<string>> sortedTransactions;

    for (const auto& transaction : transactions) {
        vector<string> sortedTransaction = transaction;

        // Sort items in the transaction by descending frequency
        sort(sortedTransaction.begin(), sortedTransaction.end(), [&frequency](const string& a, const string& b) {
            auto itA = frequency.find(a);
            auto itB = frequency.find(b);
            int freqA = (itA != frequency.end()) ? itA->second : 0;
            int freqB = (itB != frequency.end()) ? itB->second : 0;
            return freqA > freqB;
        });

        sortedTransactions.push_back(sortedTransaction);
    }

    return sortedTransactions;
}

// Function to find frequent items from transactions and their frequencies
unordered_map<string, int> findFrequentItems(const vector<vector<string>>& transactions, int minSupport) {
    unordered_map<string, int> frequency;

    // Count frequency of each item
    for (const auto& transaction : transactions) {
        for (const auto& item : transaction) {
            frequency[item]++;
        }
    }

    // Remove infrequent items (support < minSupport)
    for (auto it = frequency.begin(); it != frequency.end();) {
        if (it->second < minSupport) {
            it = frequency.erase(it);
        } else {
            ++it;
        }
    }

    return frequency;
}

// FP-Growth main function
void fpgrowth(const vector<vector<string>>& transactions, int minSupport) {
    // Step 1: Find frequent items and their frequencies
    unordered_map<string, int> frequency = findFrequentItems(transactions, minSupport);

    // Step 2: Sort transactions based on item frequency
    vector<vector<string>> sortedTransactions = sortTransactions(transactions, frequency);

    // Step 3: Build the FP-Tree
    FPTree tree;
    for (const auto& transaction : sortedTransactions) {
        tree.insertTransaction(transaction);
    }

    // Step 4: Mine frequent itemsets from the FP-Tree
    mineFPTree(tree);
}

int main() {
    // Sample transactions
    vector<vector<string>> transactions = {
        {"A", "B", "C"},
        {"A", "B", "D"},
        {"A", "C", "E"},
        {"B", "C", "E"},
        {"A", "B", "C", "E"}
    };

    // Minimum support threshold
    int minSupport = 2;

    // Run FP-Growth
    fpgrowth(transactions, minSupport);

    return 0;
}
