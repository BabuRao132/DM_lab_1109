#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>

using namespace std;

// Function to generate candidate itemsets of size 2 using hashing
unordered_map<int, int> generateHashTable(const vector<vector<int>>& transactions) {
    unordered_map<int, int> hashTable;

    for (const auto& transaction : transactions) {
        int n = transaction.size();
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                int hashKey = (transaction[i] * 10 + transaction[j]);
                hashTable[hashKey]++;
            }
        }
    }

    return hashTable;
}

// Function to generate frequent itemsets of size 1
vector<int> findFrequent1Itemsets(const vector<vector<int>>& transactions, int supportThreshold) {
    unordered_map<int, int> itemCount;
    for (const auto& transaction : transactions) {
        for (int item : transaction) {
            itemCount[item]++;
        }
    }

    vector<int> frequentItems;
    for (const auto& item : itemCount) {
        if (item.second >= supportThreshold) {
            frequentItems.push_back(item.first);
        }
    }

    return frequentItems;
}

// Function to generate candidate itemsets of size k using previous frequent itemsets
vector<vector<int>> generateCandidates(const vector<vector<int>>& prevFrequentItemsets) {
    vector<vector<int>> candidates;
    int n = prevFrequentItemsets.size();

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            vector<int> candidate = prevFrequentItemsets[i];

            for (int item : prevFrequentItemsets[j]) {
                if (find(candidate.begin(), candidate.end(), item) == candidate.end()) {
                    candidate.push_back(item);
                }
            }

            sort(candidate.begin(), candidate.end());

            if (candidate.size() == prevFrequentItemsets[0].size() + 1) {
                candidates.push_back(candidate);
            }
        }
    }

    return candidates;
}

// Function to filter candidates based on support threshold using a hash table
vector<vector<int>> filterCandidates(const vector<vector<int>>& candidates, const vector<vector<int>>& transactions, int supportThreshold) {
    unordered_map<int, int> hashTable;

    for (const auto& transaction : transactions) {
        set<int> transactionSet(transaction.begin(), transaction.end());

        for (const auto& candidate : candidates) {
            bool isSubset = true;
            for (int item : candidate) {
                if (transactionSet.find(item) == transactionSet.end()) {
                    isSubset = false;
                    break;
                }
            }

            if (isSubset) {
                int hashKey = 0;
                for (int item : candidate) {
                    hashKey = hashKey * 10 + item;
                }
                hashTable[hashKey]++;
            }
        }
    }

    vector<vector<int>> frequentItemsets;
    for (const auto& item : hashTable) {
        if (item.second >= supportThreshold) {
            vector<int> frequentItemset;
            int key = item.first;
            while (key > 0) {
                frequentItemset.insert(frequentItemset.begin(), key % 10);
                key /= 10;
            }
            frequentItemsets.push_back(frequentItemset);
        }
    }

    return frequentItemsets;
}

int main() {
    // Example transactions
    vector<vector<int>> transactions = {
        {1, 2, 3},
        {2, 3, 4},
        {1, 2},
        {2, 3},
        {1, 3, 4}
    };

    int supportThreshold = 2;

    // Find frequent 1-itemsets
    vector<int> frequent1Itemsets = findFrequent1Itemsets(transactions, supportThreshold);
    vector<vector<int>> frequentItemsets;

    for (int item : frequent1Itemsets) {
        frequentItemsets.push_back({item});
    }

    int k = 2;
    while (!frequentItemsets.empty()) {
        cout << "Frequent " << k - 1 << "-itemsets:" << endl;
        for (const auto& itemset : frequentItemsets) {
            for (int item : itemset) {
                cout << item << " ";
            }
            cout << endl;
        }

        // Generate candidate itemsets of size k
        vector<vector<int>> candidates = generateCandidates(frequentItemsets);

        // Filter candidates based on support threshold
        frequentItemsets = filterCandidates(candidates, transactions, supportThreshold);
        k++;
    }

    return 0;
}
