#include <iostream>
#include <vector>
#include <set>
#include <map>
#include<string>
#include <algorithm>
#include<fstream>

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
vector<set<set<char>>> apriori(const vector<set<char>>& part_of_transaction, int minSupport) {
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
        if (item.second >= minSupport) {
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
            if (supportCount[candidate] >= minSupport) {
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

int main() {
    vector<set<char>> part_of_transaction;
    fstream myfile2("tdx.txt",ios::in);
    if(!myfile2){
    	cout<<"error: file opening error\n";
	}
//    myfile2.open("tdx.txt",ios::in);
    string line;
    char c;
    set<char> s;
    int minSupport,trans_per_part,m;
    cout<<"Enter minimum support count: \n";
    cin>>minSupport;
    cout<<"Enter transaction per partion: \n";
    cin>>trans_per_part;
    vector<vector<set<set<char>>>> local_frequentItemsets;
    vector<set<set<char>>> global_frequentItemsets;
    while(!myfile2.eof()){
    	m=trans_per_part;
    	while(m>0){
		if(!myfile2.eof()){
    	s.clear();
    	getline(myfile2,line);
    	//cout<<line<<endl;
    	for(int i=0;i<line.length();){
    		s.insert(line[i]);
    		i+=2;
		}
//		for(auto i:s)
//		cout<<i<<endl;
		part_of_transaction.push_back(s);
		m--;
	}
	}
	local_frequentItemsets.push_back(apriori(part_of_transaction, minSupport));
	}


   // vector<set<set<char>>> local_frequentItemsets = apriori(part_of_transaction, minSupport);

    cout << "Frequent itemsets:" << endl;
    for(auto local_frequentItemset:local_frequentItemsets){
	
    for (int i = 0; i < local_frequentItemset.size(); ++i) {
        cout << "Frequent " << i + 1 << "-itemsets:" << endl;
        for (const auto& itemset : local_frequentItemset[i]) {
            for (const auto& item : itemset) {
                cout << item << " ";
            }
            cout << endl;
        }
        cout << endl;
    }}

    return 0;
}
