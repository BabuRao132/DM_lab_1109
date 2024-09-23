#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <fstream>

using namespace std;

// Function to generate candidate itemsets of length k
set<set<char>> generateCandidates(const set<set<char>>& frequentItemsets, int k) {
    set<set<char>> candidates;
    for (auto it1 = frequentItemsets.begin(); it1 != frequentItemsets.end(); ++it1) {
        for (auto it2 = next(it1); it2 != frequentItemsets.end(); ++it2) {
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
set<set<char>> pruneCandidates(const set<set<char>>& candidates, const set<set<char>>& previousFrequentItemsets) {
    set<set<char>> prunedCandidates;
    for (const auto& candidate : candidates) {
        bool isValid = true;
        for (const auto& item : candidate) {
            set<char> subset = candidate;
            subset.erase(item);
            if (previousFrequentItemsets.find(subset) == previousFrequentItemsets.end()) {
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

// Function to calculate support count of itemsets in transactions
map<set<char>, int> calculateSupport(const vector<set<char>>& transactions, const set<set<char>>& candidates) {
    map<set<char>, int> supportCount;
    for (const auto& candidate : candidates) {
        for (const auto& transaction : transactions) {
            if (includes(transaction.begin(), transaction.end(), candidate.begin(), candidate.end())) {
                supportCount[candidate]++;
            }
        }
    }
    return supportCount;
}

// Apriori algorithm to find frequent itemsets
pair<vector<set<set<char>>>, map<set<char>, int>> apriori(const vector<set<char>>& transactions, int minSupport) {
    vector<set<set<char>>> frequentItemsets;
    map<set<char>, int> supportCountMap;
    set<set<char>> currentFrequentItemsets;

    // Generate frequent 1-itemsets
    map<char, int> itemCount;
    for (const auto& transaction : transactions) {
        for (const auto& item : transaction) {
            itemCount[item]++;
        }
    }

    for (const auto& item : itemCount) {
        if (item.second >= minSupport) {
            set<char> temp = {item.first};
            currentFrequentItemsets.insert(temp);
            supportCountMap[temp] = item.second;
        }
    }

    frequentItemsets.push_back(currentFrequentItemsets);
    int k = 2;

    // Generate higher order frequent itemsets
    while (!currentFrequentItemsets.empty()) {
        set<set<char>> candidates = generateCandidates(currentFrequentItemsets, k);
        candidates = pruneCandidates(candidates, currentFrequentItemsets);
        map<set<char>, int> supportCount = calculateSupport(transactions, candidates);
        currentFrequentItemsets.clear();
        for (const auto& candidate : candidates) {
            if (supportCount[candidate] >= minSupport) {
                currentFrequentItemsets.insert(candidate);
                supportCountMap[candidate] = supportCount[candidate];
            }
        }

        if (!currentFrequentItemsets.empty()) {
            frequentItemsets.push_back(currentFrequentItemsets);
        }
        k++;
    }

    return {frequentItemsets, supportCountMap};
}

// Function to generate all subsets of a set
vector<set<char>> generateSubsets(const set<char>& itemset) {
    vector<set<char>> subsets;
    vector<char> items(itemset.begin(), itemset.end());
    int n = items.size();
    for (int i = 1; i < (1 << n); ++i) { // Generate all non-empty subsets
        set<char> subset;
        for (int j = 0; j < n; ++j) {
            if (i & (1 << j)) {
                subset.insert(items[j]);
            }
        }
        if (!subset.empty() && subset.size() < itemset.size()) {
            subsets.push_back(subset);
        }
    }
    return subsets;
}

// Function to generate association rules and calculate confidence
void generateAssociationRules(const vector<set<set<char>>>& frequentItemsets, 
                              const map<set<char>, int>& supportCountMap, double minConfidence) {
    cout << "Association Rules (min confidence = " << minConfidence << "%):" << endl;
    for (const auto& itemsetGroup : frequentItemsets) {
        for (const auto& itemset : itemsetGroup) {
            vector<set<char>> subsets = generateSubsets(itemset);
            for (const auto& subset : subsets) {
                set<char> remaining;
                set_difference(itemset.begin(), itemset.end(), subset.begin(), subset.end(),
                               inserter(remaining, remaining.begin()));
                if (!remaining.empty()) {
                    int supportItemset = supportCountMap.at(itemset);
                    int supportSubset = supportCountMap.at(subset);
                    double confidence = (double(supportItemset) / supportSubset) * 100;

                    if (confidence >= minConfidence) {
                        for (const char& s : subset) cout << s << " ";
                        cout << "=> ";
                        for (const char& r : remaining) cout << r << " ";
                        cout << "[Confidence: " << confidence << "%]" << endl;
                    }
                }
            }
        }
    }
}

int main() {
    vector<set<char>> transactions;
    fstream myfile2("tdx.txt", ios::in);
    string line;
    set<char> s;
    while (getline(myfile2, line)) {
        s.clear();
        for (int i = 0; i < line.length(); i += 2) {
            s.insert(line[i]);
        }
        transactions.push_back(s);
    }

    int minSupport;
    double minConfidence;

    // Taking minimum support and confidence from the user
    cout << "Enter minimum support (integer): ";
    cin >> minSupport;

    cout << "Enter minimum confidence (percentage): ";
    cin >> minConfidence;

    auto [frequentItemsets, supportCountMap] = apriori(transactions, minSupport);

    cout << "Frequent itemsets:" << endl;
    for (int i = 0; i < frequentItemsets.size(); ++i) {
        cout << "Frequent " << i + 1 << "-itemsets:" << endl;
        for (const auto& itemset : frequentItemsets[i]) {
            for (const auto& item : itemset) {
                cout << item << " ";
            }
            cout << " (Support: " << supportCountMap.at(itemset) << ")" << endl;
        }
        cout << endl;
    }

    // Generate association rules
    generateAssociationRules(frequentItemsets, supportCountMap, minConfidence);

    return 0;
}
