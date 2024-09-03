#include <iostream>
#include <vector>
#include <set>
#include <map>
#include<string>
#include <algorithm>
#include<fstream>

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
												//  set.find()->The function returns an iterator which points to the element which is searched in the set container. 
												//If the element is not found, then the iterator points to the position just after the last element in the set.         	
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
vector<set<set<char>>> apriori(const vector<set<char>>& transactions, int minSupport) {
    vector<set<set<char>>> frequentItemsets;
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
            currentFrequentItemsets.insert({item.first});
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
            }
        }

        if (!currentFrequentItemsets.empty()) {
            frequentItemsets.push_back(currentFrequentItemsets);
        }
        k++;
    }

    return frequentItemsets;
}

int main() {
    vector<set<char>> transactions;
    fstream myfile2("tdx.txt",ios::in);
//    myfile2.open("tdx.txt",ios::in);
    string line;
    char c;
    set<char> s;
    while(!myfile2.eof()){
    	s.clear();
    	getline(myfile2,line);
    	for(int i=0;i<line.length();){
    		s.insert(line[i]);
    		i+=2;
		}
		transactions.push_back(s);
	}


    int minSupport = 7;
    vector<set<set<char>>> frequentItemsets = apriori(transactions, minSupport);

    cout << "Frequent itemsets:" << endl;
    for (int i = 0; i < frequentItemsets.size(); ++i) {
        cout << "Frequent " << i + 1 << "-itemsets:" << endl;
        for (const auto& itemset : frequentItemsets[i]) {
            for (const auto& item : itemset) {
                cout << item << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

    return 0;
}
