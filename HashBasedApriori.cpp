#include<iostream>
#include<fstream>
#include<sstream>
#include<map>
#include<vector>
#include<math.h>
#include<algorithm>

using namespace std;

//Choosing 13 as prime number to hash

map<int, int> bkt_cnt_freq_map;

//Create an order map to have id for items to hash
map<string, int> order;

//The item list being sent should be sorted
int getBucketCount(vector<string> item_list)
{
	int i = item_list.size()-1;
	int temp = 0;
	while(i>=0)
	{
		temp += order[item_list[item_list.size()-i-1]] * pow(10, i);
		i--;
	}
	return temp % 13;
}

vector<vector<string>> getNextCandidates(vector<vector<string>> prev_frequent_items_list, int expected_candidate_length, int min_support)
{
	vector<vector<string>> candidates_list;
	
	for(int i=0; i<prev_frequent_items_list.size()-1; i++)
	{
		for(int j=i+1; j<prev_frequent_items_list.size(); j++)
		{
		
			vector<string> union_result;
			
			int same = 1;
			
			for(int p=0; p<expected_candidate_length-2; p++)
			{
				if(prev_frequent_items_list[i][p]!=prev_frequent_items_list[j][p])
				{
					same = 0;
				}
			}
			if(same)
			{
				for(int p=0; p<expected_candidate_length-2; p++)
				{
					union_result.push_back(prev_frequent_items_list[i][p]);
				}
				union_result.push_back(prev_frequent_items_list[i][expected_candidate_length-2]);
				union_result.push_back(prev_frequent_items_list[j][expected_candidate_length-2]);
				sort(union_result.begin(), union_result.end());
				if(bkt_cnt_freq_map[getBucketCount(union_result)] >= min_support)
				{
					candidates_list.push_back(union_result);
				}
				
			}
			
		}
	}
		
		return candidates_list;
}



map<vector<string>, int> getSupportForAllCandidates(vector<vector<string>> candidate_list,map<string, vector<string>> database, int k)
{
	map<vector<string>, int> candidate_list_freq;
	bkt_cnt_freq_map.clear();
	for(int i=0; i<candidate_list.size(); i++)
	{
		for(int j=i+1; j<candidate_list.size(); j++)
		{
		
			vector<string> union_result;
			
			int same = 1;
			
			for(int p=0; p<k-1; p++)
			{
				if(candidate_list[i][p]!=candidate_list[j][p])
				{
					same = 0;
				}
			}
			if(same)
			{
				for(int p=0; p<k-1; p++)
				{
					union_result.push_back(candidate_list[i][p]);
				}
				union_result.push_back(candidate_list[i][k-1]);
				union_result.push_back(candidate_list[j][k-1]);
				sort(union_result.begin(), union_result.end());
				int c=0;
				
				
				
				for(auto& transaction: database)
				{
					int pr=1;
					for(auto& item: union_result)
					{
						if(find(transaction.second.begin(), transaction.second.end(), item)==transaction.second.end())
						{
							pr = 0;
						}
					}
					if(pr)
					c++;
				}
			
				bkt_cnt_freq_map[getBucketCount(union_result)]+=c;
				
			}
			
		}
		int count=0;
		
		
		for(auto& transaction: database)
		{
			int present = 1;
			for(auto& item: candidate_list[i])
			{
				if(find(transaction.second.begin(), transaction.second.end(), item)==transaction.second.end())
				{
					present = 0;	
				}
			}
			if(present)
				count++;
			
		}
		
		candidate_list_freq[candidate_list[i]] = count;
		
	}
	//printf("\nReturning Supports of %d -- Candidate %d-itemsets\n",candidate_list_freq.size(), k);
	return candidate_list_freq;
}


vector<vector<string>> getFrequentItemsList(vector<vector<string>> candidate_list, map<string, vector<string>> database, int min_support, int k)
{
	vector<vector<string>> frequent_item_sets;
	map<vector<string>, int> candidate_list_freq;
	//printf("\nGetting supports of %d --- Candidate %d-itemsets\n", candidate_list.size(), k);
	candidate_list_freq = getSupportForAllCandidates(candidate_list, database, k);
	
	for(auto& each: candidate_list_freq)
	{
		if(each.second >= min_support)
		{
			frequent_item_sets.push_back(each.first);
		}
	}
	return frequent_item_sets;
}

int main()
{
	ifstream fin("td.txt");
	if(!fin)
	{
		//If error is encountered in opening the file, exit with return 1
		cerr << "Errror: Error in opening the file";
		return 1;
	}
	
	string transaction;
	map<string, vector<string>> database;
	vector<string> all_items_list;
	
	//Read the file and store the file as a map : string --> vector<string> (i.e tid --> item_list) 
	while(getline(fin, transaction))
	{
		int idx_flag = 1;
		stringstream ss(transaction);
		string item, index;
		vector<string> item_list;
		while(getline(ss, item, ' '))
		{
			if(idx_flag)
			{
				index = item;
				idx_flag = 0;
				continue;
			}
			item_list.push_back(item);
			if(find(all_items_list.begin(), all_items_list.end(), item)==all_items_list.end())
			{
				all_items_list.push_back(item);
			}
		}
		database[index] = item_list;
	}
	
	//Display Database
	
	printf("=============> Displaying Database <=================\n");
	
	for(auto& transaction: database)
	{
		cout << transaction.first << " ";
		for(auto& each_item: transaction.second)
		{
			cout << each_item << " ";
		}
		cout << endl;
	}
	
	//Request the user for minimum support
	
	int min_support;
	cout << "Please enter the minimum support required : ";
	cin >> min_support;
	
	int total_no_of_transactions = database.size();
	int total_no_of_items = all_items_list.size();
	
	//Sort all_item_list
	sort(all_items_list.begin(), all_items_list.end());
	
	
	int o=1;
	for(auto& each_item: all_items_list)
	{
		order[each_item]=o++;
	}
	
	map<int, vector<vector<string>>> candidate_list;
	map<int, vector<vector<string>>> frequent_items_list;
	
	//Create 1-item candidate list from all item list
	for(auto& each_item: all_items_list)
	{
		candidate_list[1].push_back({each_item});
	}
	
	int k = 1;
	while(candidate_list[k].size()!=0 && k<=total_no_of_items)
	{
		//Extract Frequent Item List from Candidate Item List
		//printf("Generating frequent %d-itemsets........\n", k);
		frequent_items_list[k] = getFrequentItemsList(candidate_list[k], database, min_support, k);
		//printf("%d --> frequent %d-itemsets are created\n", frequent_items_list[k].size(),k);
		
		/*
		for(auto each:bkt_cnt_freq_map)
		{
			cout << each.first << " -- " << each.second << endl;
			
		}
		cout << endl << endl;
		*/
		//Generate Candidate Item List for next length
		candidate_list[k+1] = getNextCandidates(frequent_items_list[k], k+1, min_support);
		
		//Display Results
		for(auto& each: frequent_items_list[k])
		{
			cout << "< ";
			for(auto& p: each)
			{
				cout << p << " ";
			}
			cout << ">" << endl;
		}
		
		k++;
	}
	
	
	
	
	
	
	
	
	
	
	return 0;
}
