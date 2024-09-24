#include <bits/stdc++.h>
using namespace std;

// FP tree Node structure 
struct FPTreeNode {
    string item;
    int count;
    FPTreeNode* parent;
    unordered_map<string, FPTreeNode*> children;
    FPTreeNode* next;

    // Constructor
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

    // Constructor
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
        root = new FPTreeNode();  // Root node
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

// Function to find the conditional pattern base and frequent itemsets
void findFrequentItemsets(FPTree& tree, const string& item, int minsupport) {
    vector<pair<vector<string>, int>> conditionalPatternBase;
    
    // Safety check: Ensure the item is in the header table before accessing it
    if (tree.headerTable.find(item) == tree.headerTable.end()) return;

    FPTreeNode* currentNode = tree.headerTable[item].node;

    // Collect conditional pattern base (itemsets leading to `item`)
    while (currentNode != nullptr) {
        vector<string> path;
        FPTreeNode* traverseNode = currentNode->parent;

        while (traverseNode != nullptr && traverseNode->item != "") {
            path.push_back(traverseNode->item);
            traverseNode = traverseNode->parent;
        }

        if (!path.empty()) {
            reverse(path.begin(), path.end());  // Reverse to maintain original order
            conditionalPatternBase.push_back({path, currentNode->count});
        }

        currentNode = currentNode->next;
    }

    // Print conditional pattern base for the given item
    cout << "Frequent itemsets for " << item << ": " << endl;
    map<vector<string>, int> frequentItemsets;  // Store itemsets with their counts

    // Generate the frequent itemsets from the conditional pattern base
    for (auto& pattern : conditionalPatternBase) {
        for (int i = 0; i < (1 << pattern.first.size()); ++i) {
            vector<string> subset;
            for (int j = 0; j < pattern.first.size(); ++j) {
                if (i & (1 << j)) {
                    subset.push_back(pattern.first[j]);
                }
            }

            if (!subset.empty()) {
                subset.push_back(item);  // Add the current item to the subset
                if (frequentItemsets.find(subset) == frequentItemsets.end()) {
                    frequentItemsets[subset] = pattern.second;
                } else {
                    frequentItemsets[subset] += pattern.second;
                }
            }
        }
    }

    // Print the frequent itemsets
    for (auto& frequentSet : frequentItemsets) {
        if (frequentSet.second >= minsupport) {
            cout << "{";
            for (size_t i = 0; i < frequentSet.first.size(); ++i) {
                cout << frequentSet.first[i];
                if (i != frequentSet.first.size() - 1) {
                    cout << ", ";
                }
            }
            cout << ": " << frequentSet.second << "}" << endl;
        }
    }
}

// Function for sorting items by descending frequency
bool sortbysec(const pair<string, int>& a, const pair<string, int>& b) {
    return (a.second > b.second);
}

int main() {
    fstream fin("purchasedatabase.txt", ios::in);
    if (!fin) {
        cout << "Error in opening the file." << endl;
        return 1;
    }

    int minsupport;
    cout << "Enter minimum support: ";
    cin >> minsupport;

    string transaction;
    vector<vector<string>> database;
    vector<string> all_items_list;

    // Read the file and store the database
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

    // First scan to calculate frequency of items
    for (auto& trans : database) {
        for (auto& item : trans) {
            auto it = find_if(frequency.begin(), frequency.end(), [item](const auto& p) {
                return p.first == item;
            });
            if (it != frequency.end()) {
                it->second++;
            } else {
                frequency.push_back({item, 1});
            }
        }
    }

    // Delete elements with less than minimum support
    for (auto i = frequency.begin(); i != frequency.end();) {
        if (i->second < minsupport) {
            i = frequency.erase(i);
        } else {
            ++i;
        }
    }

    // Sort elements by frequency, then alphabetically
    sort(frequency.begin(), frequency.end());
    sort(frequency.begin(), frequency.end(), sortbysec);
	for(auto item: frequency) {
		cout<<item.first<<"=>"<<item.second<<endl;
	}
    unordered_map<string, int> itempriolist;
    for (int i = 0; i < frequency.size(); i++) {
        itempriolist[frequency[i].first] = i + 1;
    }
    cout<<"priority  assigned"<<endl;
    for(auto item: itempriolist) {
		cout<<item.first<<"=>"<<item.second<<endl;
	}
    vector<vector<string>> ord_database;
    for (auto& trans : database) {
        vector<string> ord_trans = trans;
        sort(ord_trans.begin(), ord_trans.end(), [&](const string& a, const string& b) {
            return itempriolist[a] < itempriolist[b];
        });
        ord_database.push_back(ord_trans);
    }
    cout<<"\n orderd database\n";
 
	for(auto trans: ord_database){
		for(auto item: trans){
			//for(auto item:itemslist)
			cout<<item<<" ";
		}
		cout<<endl;
	}

    FPTree tree;
    for (const auto& transaction : ord_database) {
        tree.insertTransaction(transaction);
    }

    cout << "-----------------> Frequent items and conditional pattern base <-----------------\n";
    for (auto& item : frequency) {
        findFrequentItemsets(tree, item.first, minsupport);
        cout << endl;
    }

    return 0;
}
