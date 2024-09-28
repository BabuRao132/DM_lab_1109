#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <fstream>

using namespace std;

// Function to generate candidate itemsets of length k
set<set<char>> generateCandidates(const set<set<char>>& local_frequentItemsets, int k) {
    set<set<char>> candidates;
    for (auto it1 = local_frequentItemsets.begin(); it1 != local_frequentItemsets.end(); ++it1) {
        for (auto it2 = next(it1); it2 != local_frequentItemsets.end(); ++it2) {
            set<char> candidate = *it1;
            candidate.insert(it2->begin(), it2->end());
            if (candidate.size() == k) {
                candidates.insert(candidate);
            }
        }
    }
    return candidates;
}

// Function to prune candidates by ensuring all subsets are frequent
set<set<char>> pruneCandidates(const set<set<char>>& candidates, const set<set<char>>& previouslocal_frequentItemsets) {
    set<set<char>> prunedCandidates;
    for (const auto& candidate : candidates) {
        bool isValid = true;
        for (const auto& item : candidate) {
            set<char> subset = candidate;
            subset.erase(item);
            if (previouslocal_frequentItemsets.find(subset) == previouslocal_frequentItemsets.end()) {
                isValid = false;
                break;
            }
        }
        if (isValid) {
            prunedCandidates.insert(candidate);
        }
    }
    return prunedCandidates;
}

// Function to calculate support count of itemsets in part_of_transaction
map<set<char>, int> calculateSupport(const vector<set<char>>& part_of_transaction, const set<set<char>>& candidates) {
    map<set<char>, int> supportCount;
    for (const auto& candidate : candidates) {
        for (const auto& transaction : part_of_transaction) {
            if (includes(transaction.begin(), transaction.end(), candidate.begin(), candidate.end())) {
                supportCount[candidate]++;
            }
        }
    }
    return supportCount;
}

// Apriori algorithm to find frequent itemsets
vector<set<set<char>>> apriori(const vector<set<char>>& part_of_transaction, float local_min_sup) {
    vector<set<set<char>>> local_frequentItemsets;
    set<set<char>> currentlocal_frequentItemsets;

    // Generate frequent 1-itemsets
    map<char, int> itemCount;
    for (const auto& transaction : part_of_transaction) {
        for (const auto& item : transaction) {
            itemCount[item]++;
        }
    }

    for (const auto& item : itemCount) {
        if (item.second >= local_min_sup) {
            currentlocal_frequentItemsets.insert({item.first});
        }
    }

    local_frequentItemsets.push_back(currentlocal_frequentItemsets);
    int k = 2;

    // Generate higher order frequent itemsets
    while (!currentlocal_frequentItemsets.empty()) {
        set<set<char>> candidates = generateCandidates(currentlocal_frequentItemsets, k);
        candidates = pruneCandidates(candidates, currentlocal_frequentItemsets);
        map<set<char>, int> supportCount = calculateSupport(part_of_transaction, candidates);
        currentlocal_frequentItemsets.clear();
        for (const auto& candidate : candidates) {
            if (supportCount[candidate] >= local_min_sup) {
                currentlocal_frequentItemsets.insert(candidate);
            }
        }

        if (!currentlocal_frequentItemsets.empty()) {
            local_frequentItemsets.push_back(currentlocal_frequentItemsets);
        }
        k++;
    }

    return local_frequentItemsets;
}

// Function to merge local frequent itemsets and calculate global support
map<set<char>, int> mergeFrequentItemsets(const vector<vector<set<set<char>>>>& all_local_frequentItemsets, const vector<set<char>>& all_transactions) {
    map<set<char>, int> global_support;

    // Traverse through all local frequent itemsets and calculate support on the entire dataset
    for (const auto& partition_frequent_itemsets : all_local_frequentItemsets) {
        for (const auto& level_itemsets : partition_frequent_itemsets) {
            for (const auto& itemset : level_itemsets) {
                if (global_support.find(itemset) == global_support.end()) {
                    // Calculate support for each itemset across all transactions once
                    global_support[itemset] = 0;
                    for (const auto& transaction : all_transactions) {
                        if (includes(transaction.begin(), transaction.end(), itemset.begin(), itemset.end())) {
                            global_support[itemset]++;
                        }
                    }
                }
            }
        }
    }

    return global_support;
}


int main() {
    ifstream myfile2("tdx.txt");
    if (!myfile2) {
        cout << "error: file opening error\n";
        return 1;
    }

    string line;
    set<char> s;
    int global_sup, trans_per_part, trans_count = 0;
    float rel_min_sup, local_min_sup;

    cout << "Enter minimum support count: \n";
    cin >> global_sup;
    cout << "Enter transaction per partition: \n";
    cin >> trans_per_part;

    vector<vector<set<set<char>>>> all_local_frequentItemsets;
    vector<set<char>> all_transactions;

    // Read all transactions from the file
    while (getline(myfile2, line)) {
        s.clear();
        trans_count++;
        for (int i = 0; i < line.length(); i += 2) { // Assuming items are separated by spaces
            s.insert(line[i]);
        }
        all_transactions.push_back(s);
    }
    myfile2.close();

    // Calculate relative and local minimum support
    rel_min_sup = global_sup * 1.0 / trans_count;
    local_min_sup = rel_min_sup * trans_per_part;

    // Process transactions in partitions
    for (int i = 0; i < all_transactions.size(); i += trans_per_part) {
        vector<set<char>> part_of_transaction;

        // Read one partition at a time
        for (int j = i; j < i + trans_per_part && j < all_transactions.size(); ++j) {
            part_of_transaction.push_back(all_transactions[j]);
        }

        // Print the partition number
        cout << "Processing Partition " << (i / trans_per_part) + 1 << ":\n";

        // Run Apriori on this partition and store local frequent itemsets
        all_local_frequentItemsets.push_back(apriori(part_of_transaction, local_min_sup));
    }

    // Output frequent itemsets for all partitions
    cout << "\nFrequent itemsets by partition:\n";
    for (size_t partition = 0; partition < all_local_frequentItemsets.size(); ++partition) {
        cout << "Partition " << partition + 1 << ":\n";
        for (const auto& itemset_level : all_local_frequentItemsets[partition]) {
            for (const auto& itemset : itemset_level) {
                for (const auto& item : itemset) {
                    cout << item << " ";
                }
                cout << endl;
            }
        }
        cout << endl;
    }

    // Merge local frequent itemsets and calculate global frequent itemsets
    map<set<char>, int> global_frequentItemsets = mergeFrequentItemsets(all_local_frequentItemsets, all_transactions);

    // Output global frequent itemsets
    cout << "\nGlobal frequent itemsets (with support counts):\n";
    for (const auto& itemset : global_frequentItemsets) {
        if (itemset.second >= global_sup) {
            for (const auto& item : itemset.first) {
                cout << item << " ";
            }
            cout << " -> Support: " << itemset.second << endl;
        }
    }

    return 0;
}
