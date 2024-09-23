#include<bits/stdc++.h>
using namespace std;

//  FP tree Node structure 
struct FPTreeNode {
    string item;
    int count;
    FPTreeNode* parent;
    unordered_map<string, FPTreeNode*> children;
    FPTreeNode* next;
    
    // Node constructor
    FPTreeNode(string item = "", int count = 1, FPTreeNode* parent = nullptr) {
        this->item = item;
        this->count = count;
        this->parent = parent;
        this->next = nullptr;
    }
};

struct HeaderTableEntry {
    int support;
    FPTreeNode* node; 
    FPTreeNode* lastNode; 
    
    // HeaderTableEntry constructor
    HeaderTableEntry(int support = 0, FPTreeNode* node = nullptr) {
        this->support = support;
        this->node = node;
        this->lastNode = node;
    }
};

class FPTree {
public:
    FPTreeNode* root;
    unordered_map<string, HeaderTableEntry> headerTable;

    FPTree() {
        root = new FPTreeNode();  // Root created with empty item and count = 1
    }

    void insertTransaction(const vector<string>& transaction, int count = 1) {
        FPTreeNode* currentNode = root;

        for (const auto& item : transaction) {
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

// Function for mining frequent itemsets from a conditional FP-Tree
vector<pair<vector<string>, int>> frequentitemmine(FPTree& tree, const string& baseItem, int minsupport) {
    vector<pair<vector<string>, int>> conditionalPatternBase;
    FPTreeNode* currentNode = tree.headerTable[baseItem].node;

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

    // Print the conditional pattern base
    cout << "Conditional pattern base for " << baseItem << ": ";
    for (auto list : conditionalPatternBase) {
        cout << "{";
        for (auto i : list.first) {
            cout << i << " ";
        }
        cout << "}:" << list.second << "  ";
    }
    cout << endl;

    // Create a conditional FP-Tree from the conditional pattern base
    FPTree conditionalTree;
    for (auto pattern : conditionalPatternBase) {
        conditionalTree.insertTransaction(pattern.first, pattern.second);
    }

    vector<pair<vector<string>, int>> frequentItemsets;

    // Mine the frequent itemsets from the conditional FP-Tree
    for (auto entry : conditionalTree.headerTable) {
        string condItem = entry.first;
        int condSupport = entry.second.support;

        if (condSupport < minsupport) {
            continue;
        }

        vector<string> frequentSet = {condItem, baseItem};
        frequentItemsets.push_back({frequentSet, condSupport});

        // Print the frequent itemset
        cout << "Frequent itemset: {";
        for (auto i : frequentSet) {
            cout << i << " ";
        }
        cout << "} | Support: " << condSupport << endl;

        // Recursively mine the conditional FP-tree for this conditional item
        vector<pair<vector<string>, int>> subItemsets = frequentitemmine(conditionalTree, condItem, minsupport);

        for (auto subItemset : subItemsets) {
            subItemset.first.push_back(baseItem);
            frequentItemsets.push_back(subItemset);

            cout << "Frequent itemset: {";
            for (auto i : subItemset.first) {
                cout << i << " ";
            }
            cout << "} | Support: " << subItemset.second << endl;
        }
    }

    return frequentItemsets;
}

// Function for sorting items by their frequency in descending order
bool sortbysec(const pair<string,int> &a,const pair<string,int> &b) {
    return (a.second > b.second);
}

int main() {
    fstream fin("purchasedatabase.txt", ios::in);
    if (!fin) {
        cout << "Error opening the file";
        return 1;
    }

    int minsupport;
    cout << "Enter minimum support: ";
    cin >> minsupport;

    string transaction;
    vector<vector<string>> database;
    vector<string> all_items_list;

    while (getline(fin, transaction)) {
        int idx_flag = 1;
        stringstream ss(transaction);
        string item, index;
        vector<string> item_list;

        while (getline(ss, item, ' ')) {
            if (idx_flag) {
                index = item;
                idx_flag = 0;
                continue;
            }
            item_list.push_back(item);
            if (find(all_items_list.begin(), all_items_list.end(), item) == all_items_list.end()) {
                all_items_list.push_back(item);
            }
        }
        database.push_back(item_list);
    }

    vector<pair<string, int>> frequency;

    // First scan to calculate the frequency of each item
    for (auto trans : database) {
        for (auto item : trans) {
            auto it = find_if(frequency.begin(), frequency.end(), [item](const auto &p) {
                return p.first == item;
            });
            if (it != frequency.end()) {
                it->second++;
            } else {
                frequency.push_back({item, 1});
            }
        }
    }

    // Remove items with less than minimum support
    for (auto i = frequency.begin(); i != frequency.end();) {
        if (i->second < minsupport) {
            i = frequency.erase(i);
        } else {
            ++i;
        }
    }

    // Sort items by frequency
    sort(frequency.begin(), frequency.end());
    sort(frequency.begin(), frequency.end(), sortbysec);

    unordered_map<string, int> itempriolist;
    for (int i = 0; i < frequency.size(); i++) {
        itempriolist[frequency[i].first] = i + 1;
    }

    // Reorder items in the transactions based on their frequency
    vector<vector<string>> ord_database;
    for (auto trans : database) {
        vector<string> ord_trans = trans;
        sort(ord_trans.begin(), ord_trans.end(), [&](const string& a, const string& b) {
            return itempriolist[a] < itempriolist[b];
        });
        ord_database.push_back(ord_trans);
    }

    FPTree tree;
    for (const auto& transaction : ord_database) {
        tree.insertTransaction(transaction);
    }

    cout << "----------------->Frequent items and conditional pattern base<-----------------\n";
    for (auto item : frequency) {
        frequentitemmine(tree, item.first, minsupport);
        cout << endl;
    }

    return 0;
}
