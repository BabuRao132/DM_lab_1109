#include<bits/stdc++.h>

using namespace std; 


vector<vector<string>> getSubsets(vector<string> transaction)
{
	vector<vector<string>> subsets;
	for(int i=0; i<(1<<transaction.size()); i++)
	{
		vector<string> temp;
		for(int j=0; j<transaction.size(); j++)
		{
			if(((i & (1<<j)) != 0) && (find(temp.begin(), temp.end(), transaction[j])==temp.end()))
			{
				temp.push_back(transaction[j]);
			}
		}
		sort(temp.begin(), temp.end());
		if(find(subsets.begin(), subsets.end(), temp)==subsets.end() && temp.size()!=0)
		subsets.push_back(temp);
	}
	
	return subsets;
}

map<string, int> itemPriorityMap;
struct Node
{
	string item;
	int support;
	struct Node * predecessor;
	vector<struct Node*> successor;
	struct Node * next;
};

struct headerNode
{
	int support;
	struct Node * next;
	struct Node * last;
};

map<string, struct  headerNode * > header_list; 

bool sortBySupport(pair<string, int>& a, pair<string, int> b)
{
	
	return a.second > b.second;
}

bool sortBasedOnPriority(string& a, string& b)
{
	//cout << "comparing " << a << ", " << b << endl;
	return itemPriorityMap[a] < itemPriorityMap[b];
}

struct Node * getItem(vector<struct Node *> list_of_nodes, string itm)
{
	
	for(auto each_item : list_of_nodes)
	{
		if(each_item -> item == itm)
			return each_item;
	}
	return nullptr;
}

struct Node * insert_tree(vector<string> transaction, struct Node * root)
{
	if(transaction.empty())
	return root;
	
	struct Node * curr;
	cout << "Entered function...\n";
	if(!((root -> successor).empty()))
	{
	  curr = getItem(root -> successor, transaction[0]);
	  //cout << "Current : " << (root -> successor).size() << " details : "<< curr -> item;
	}
		
		
	if(((root -> successor).empty()) || curr == nullptr)
	{
		struct Node * newnode = new Node;
		newnode -> item = transaction[0];
		newnode -> support = 1;
		newnode -> predecessor = root;
		newnode -> next = nullptr;
		if(header_list[transaction[0]] -> next == nullptr && header_list[transaction[0]] -> last==nullptr)
		{
			header_list[transaction[0]] -> next = newnode;
			header_list[transaction[0]] -> last = newnode;
			cout << "Updated ( next & last ) of header node for " << transaction[0] << "\n";
		}
		else
		{
			header_list[transaction[0]] -> last -> next = newnode;
			header_list[transaction[0]] -> last = newnode;
			cout << "Updated ( last ) of header node for " << transaction[0] << "\n";
		}
		(root -> successor).push_back(newnode);
		curr = newnode;
		
		cout << "Inserted value < " << transaction[0] << " > in FP Tree as child to "<< root -> item << "\n";
	}
	else
	{
		curr -> support += 1;
		cout << "Incremented support for < " << transaction[0] << " > to "<< curr -> support <<"\n";
	}
	
	vector<string> N(transaction.begin()+1, transaction.end());
	insert_tree(N, curr);
	return root;
}





int main()
{
	ifstream fin("purchasedatabase.txt");
	if(!fin)
	{
		cerr << "Error : Error in opening the file";
		return 1;
	}
	
	string transaction;
	map<string, vector<string>> database;
	while(getline(fin, transaction))
	{
		int index_flag = 1;
		string item, index;
		vector<string> items_list;
		stringstream ss(transaction);
		while(getline(ss, item, ' '))
		{
			if(index_flag)
			{
				index = item;
				index_flag = 0;
				continue;
			}
			items_list.push_back(item);
		}
		database[index] = items_list;
	}
	
	int min_support;
	cout<< "Enter the minimum support : ";
	cin >> min_support;
	
	//Get supports of 1-itemsets by scanning database
	map<string, int> oneItemSetsFreqMap;
	for(auto transaction: database)
	{
		for(auto itm: transaction.second)
		{
			oneItemSetsFreqMap[itm]++;
		}
	}
	
	//Sort the 1-itemsets based on their support counts
	vector<pair<string, int>> sortedOneItemSetsFreq(oneItemSetsFreqMap.begin(), oneItemSetsFreqMap.end());
	sort(sortedOneItemSetsFreq.begin(), sortedOneItemSetsFreq.end(), sortBySupport);
	int priority=1;
	for(auto itemmap: sortedOneItemSetsFreq)
	{
		cout << itemmap.first << " - " << itemmap.second << " - Priority : " <<priority<<"\n" ;
		itemPriorityMap[itemmap.first] = priority++;
		struct headerNode * tmp = new headerNode;
		tmp -> support = itemmap.second;
		tmp -> last = nullptr;
		tmp -> next = nullptr;
		header_list[itemmap.first] = tmp;
	}
	
	cout << "Header list constructed with size : " << header_list.size() << endl;
	
	
	//Construct root of FP tree
	struct Node * root = new Node;
	root -> support = -1;
	root -> item = "blank";
	root -> predecessor = nullptr;
	(root -> successor).clear();
	root -> next = nullptr;
	
	
	//Construct FP Tree
	for(auto transaction: database)
	{
		
		cout << "------------------------------------------------------------- \n\n";
		sort(transaction.second.begin(), transaction.second.end(), sortBasedOnPriority);
		for(auto ei: transaction.second)
		{
			cout << ei << " ";
		}
		cout << "\n\n";
	
		root = insert_tree(transaction.second, root);
	}
	
	//Mine FP Tree for patterns
	
	map<string, vector<map<vector<string>, int>>> condition_pattern_base;
	
	for(auto item: header_list)
	{
		if((item.second) -> next == nullptr)
		continue;
		
		struct Node * head = (item.second) -> next;
		
		while(head!=nullptr)
		{
			struct Node* newhead = head;
			int supp = newhead -> support;
			vector<string> item_list;
			map<vector<string>, int> map_items_list;
			while(1)
			{
				newhead = newhead -> predecessor;
				if(newhead -> predecessor == nullptr)
					break;
				else
				{
					item_list.insert(item_list.begin(), newhead -> item);
				}
			}
			map_items_list[item_list] = supp;
			condition_pattern_base[item.first].push_back(map_items_list);
			head = head -> next;
		}
	}
	
	//Display Canditional Pattern Base
	
	cout << "------------------- Conditional Pattern Base ---------------------------\n";
	
	for(auto each: condition_pattern_base)
	{
		cout << each.first << "\t - \t";
		for(auto is: each.second)
		{
			for(auto p: is)
			{
				if(p.first.size()==0)
				  continue;
				cout <<"{ ";
				for(auto eis: p.first)
				{
					cout << eis << " ";
				}
				cout << "}  : ";			
				cout << p.second <<"\t";
			}
		}
		cout << "\n\n";
	}
	
	cout << "\n\n------------------ Frequent Patterns Generated -----------\n";
	map<string, vector<map<vector<string>, int>>> conditonal_fp;
	for(auto each: condition_pattern_base)
	{
		map<vector<string>, int> map;
		for(auto is: each.second)
		{
			for(auto p: is)
			{
				vector<vector<string>> subset = getSubsets(p.first);
				//subset.push_back({});
				for(auto ess: subset)
				{
					map[ess]+=p.second;
				}
			}
		}
		//Display on console
		for(auto eiss: map){
		
			if(eiss.second >= min_support)
			{
				conditonal_fp[each.first].push_back(map);
				cout << "{ ";
				vector<string> tmp(eiss.first.begin(), eiss.first.end());
				tmp.push_back(each.first);
				sort(tmp.begin(), tmp.end());
				for(auto i: tmp)
				{
					cout << i << " ";
				}
				cout << "} : " << eiss.second << "\n";
			}
		}
		cout << "\n";
	}
	
	return 0;
}
