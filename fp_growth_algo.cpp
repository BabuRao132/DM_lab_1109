#include<bits/stdc++.h>
using namespace std;

//  FP tree Node structure
//
struct FPTreeNode {
	string item;
	int count;
	FPTreeNode* parent;
	unordered_map<string, FPTreeNode*> children;
	FPTreeNode* next;
	// struct constructer . default item ="", count=1,parent = nullpointer
	FPTreeNode(string item = "", int count = 1, FPTreeNode* parent = nullptr) {
		this->item = item;
		this->count = count;
		this->parent = parent;
		this->next = nullptr;
	}
};
struct HeaderTableEntry {
	int support;
	//header pointer to node
	FPTreeNode* node;
	//last node pointer
	FPTreeNode* lastNode;
	// strcuture constructor
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
		root = new FPTreeNode();  // Root created with "" (null)
	}

	void insertTransaction(const vector<string>& transaction) {
		FPTreeNode* currentNode = root;
		// iterate through the each transaction
		for (const auto& item : transaction) {
			//if the item is not fount in children list
			// create new node
			if (currentNode->children.find(item) == currentNode->children.end()) {
				FPTreeNode* newNode = new FPTreeNode(item, 1, currentNode);
				currentNode->children[item] = newNode;
				// if element not found in hashtable then add a entry in hashtable
				if (headerTable.find(item) == headerTable.end()) {
					headerTable[item] = HeaderTableEntry(1, newNode);
				} else {
					// if found in hashtable then inccremet the support and add new node to end of list
					headerTable[item].support++;
					headerTable[item].lastNode->next = newNode;  // Directly add new node at the end
					headerTable[item].lastNode = newNode;        // Update last node pointer
				}
			} else {
				//if item is found in children list then increment the count
				currentNode->children[item]->count++;
			}
			currentNode = currentNode->children[item];
		}
	}
};

// generating subsets 

//vector<vector<string>> Subsets(vector<string> transaction)
//{
//	vector<vector<string>> subsets;
//	for(int i=0; i<(1<<transaction.size()); i++)
//	{
//		vector<string> temp;
//		for(int j=0; j<transaction.size(); j++)
//		{
//			if(((i & (1<<j)) != 0) && (find(temp.begin(), temp.end(), transaction[j])==temp.end()))
//			{
//				temp.push_back(transaction[j]);
//			}
//		}
//		sort(temp.begin(), temp.end());
//		if(find(subsets.begin(), subsets.end(), temp)==subsets.end() && temp.size()!=0)
//		subsets.push_back(temp);
//	}
//	
//	return subsets;
//}

//mining the frequent patterns 

//void frequentpatterns(int minsupport,string item,vector<pair<vector<string>, int>> conditionalPatternBase){
////	map<string,vector<map<vector<string>,int>>> con_pat_base;
////	con_pat_base[item]=conditionalPatternBase;
//	map<string,vector<map<vector<string>,int>>> frequentpatterns;
//	//for(auto pat_map: con_pat_base){
//		map<vector<string>,int> map;
//		for(auto pat: conditionalPatternBase){
//			//for(auto p: pat){
//				vector<vector<string>> subsets= Subsets(pat.first);
//				for(auto subset: subsets)
//					map[subset]+=pat.second;
//				
//			//}
//		for(auto pat_subset: map){
//			if (pat_subset.second>=minsupport){
//				frequentpatterns[item].push_back(map);
//				cout<<"{";
//				vector<string> temp(pat_subset.first.begin(),pat_subset.first.end());
//				//temp.push_back(pat.first);
//				sort(temp.begin(),temp.end());
//				for(auto i: temp)
//				   cout<<i<<" ";
//				cout<<"}"<<":"<<pat_subset.second<<endl;
//			}
//		}
//		cout<<"\n\n";
//	}
//}
// find the conditional pattern base

vector<pair<vector<string>, int>> ConditionalPatternBase(FPTree& tree, const string& item,int minsupport) {
	vector<pair<vector<string>, int>> conditionalPatternBase;
	FPTreeNode* currentNode = tree.headerTable[item].node;
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
	for(auto list: conditionalPatternBase) {
		cout<<"{";
		for(auto item: list.first) {
			cout<<item<<" ";
		}
		cout<<"}";
		cout<<":"<<list.second<<"  ";
	}
	//frequentpatterns(minsupport,item,conditionalPatternBase );
	return conditionalPatternBase;
}
// printing fp Tree
void printTree(FPTreeNode* node, int depth = 0) {
    if (node == nullptr) return;

    // Indent to represent tree levels
    for (int i = 0; i < depth; ++i) {
        cout << "  ";
    }
    cout << node->item << " (" << node->count << ")" << endl;

    // Recur for each child
    for (const auto& child : node->children) {
        printTree(child.second, depth + 1);
    }
}

// function for sorting items with descending  frequency
bool sortbysec(const pair<string,int> &a,const pair<string,int> &b) {
	return (a.second > b.second);
}

int main() {
	fstream fin("purchasedatabase.txt",ios::in);
	if(!fin) {
		cout<<"error in opening the file";
		return 1;
	}
	//minimum support value
	int minsupport;
	cout<<"enter minimum support: \n";
	cin>>minsupport;

	string transaction;
	vector<vector<string>> database;
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
		database.push_back(item_list);
	}
//	cout<<"\n---------- database----------------------\n";
//
//	for(auto trans: database){
//		for(auto item: trans){
//			//for(auto item:itemslist)
//			cout<<item<<" ";
//		}
//		cout<<endl;
//	}
	vector<pair<string,int>> frequency;

	// 1st scan to database for calculating frquency of items
	for(auto trans: database) {
		for(auto item: trans) {
			//for(auto item:itemslist)
			//cout<<item<<" ";
			auto it = find_if(frequency.begin(),frequency.end(),[item] (const auto &p) {
				return p.first ==item;
			});
			if (it!=frequency.end()) {
				it->second++;
			}
			else {
				frequency.push_back({item,1});
			}

		}
		//cout<<endl;
	}
	// sorting the elements by their frequency
	// if tie then sort by their alphabetical order
	sort(frequency.begin(),frequency.end());
	sort(frequency.begin(),frequency.end(),sortbysec);
	cout<<"items with their frequency in sorted order\n\n";
	for(auto item: frequency) {
		cout<<item.first<<"=>"<<item.second<<endl;
	}
	// assign the priority to the item based on their frequency
	unordered_map<string,int> itempriolist;
	for(int i=0; i<frequency.size(); i++) {
		itempriolist[frequency[i].first]=i+1;
	}
	cout<<"item priority list\n";
	for(auto item: itempriolist) {	
		cout<<item.first<<"=>"<<item.second<<endl;
	}

	// sort items in the transaction based on their frequency
	//2nd scan t database
	vector<vector<string>> ord_database;
	for(auto trans: database) {
		vector<string> ord_trans=trans;
		sort(ord_trans.begin(),ord_trans.end(),[&] (const string& a,const string& b) {
			return 	itempriolist[a]<itempriolist[b];
		});
		ord_database.push_back(ord_trans);
	}
	cout<<"\n orderd database\n";

	for(auto trans: ord_database) {
		for(auto item: trans) {
			//for(auto item:itemslist)
			cout<<item<<" ";
		}
		cout<<endl;
	}
	FPTree tree;
	for (const auto& transaction : ord_database) {
		tree.insertTransaction(transaction);
	}
	cout<<"--------fp tree--------\n\n";
	printTree(tree.root);
	cout<<"           ->conditionalPatternBase<-              \n";
	for(auto item: frequency) {
		cout<<item.first<<"     -    ";
		ConditionalPatternBase(tree,item.first,minsupport);
		cout<<endl;
 }
 

	return 0;


}
