#include<iostream>
#include<sstream>
#include<fstream>
#include<set>
#include<vector>
#include<map>
#include<algorithm>

using namespace std;

//Define a candidate structure that store item_list, its support, its count
struct Candidate
{
	vector<string> item_set;
	int support;
	int ct;
	
};

int checkItemsetPresence(vector<Candidate> source, vector<string> v)
{
	for(auto candidate: source)
	{
		
		if(candidate.item_set.size() == v.size())
		{
			int flag = 1;
			for(string item: v)
			{
				if(find(candidate.item_set.begin(), candidate.item_set.end(), item)==candidate.item_set.end())
				{
					flag = 0;
				}
			}
			if(flag)
			{
				return 1;
			}
	
		}
	}
	return 0;
}

vector<Candidate> removeCandidateFromPosition(vector<Candidate> source, int k)
{
	vector<Candidate> destination;
	for(int i=0; i<source.size(); i++)
	{
		if(i!=k)
		{
			destination.push_back(source[i]);
		}
	}
	return destination;
}


int main()
{
	
	

	//Create file input stream
	ifstream fin("database_purchase.txt");
	if(!fin)
	{
		cerr<< "Error : Error in opening the file";
		return 1;
	}
	
	//Create the database using map(string -> vector<string>)
	vector<vector<string>> database;
	vector<string> all_items_list;
	string transaction;
	while(getline(fin, transaction))
	{
		stringstream ss(transaction);
		int index_flag = 1;
		string item;
		vector<string> item_list;
		while(getline(ss, item, ' '))
		{
			if(index_flag==1)
			{
				index_flag=0;
				continue;
			}
			item_list.push_back(item);
			if(find(all_items_list.begin(), all_items_list.end(), item)==all_items_list.end())
			all_items_list.push_back(item);
		}
		sort(item_list.begin(), item_list.end()); 
		database.push_back(item_list);
		
	}	
	
	//Displaying Database
	for(auto& transaction: database)
	{ 
		for(auto& item: transaction)
		{
			cout << item << " ";
		}
		cout << endl;
	}
	
	int M, min_support;
	cout << "Provide the minimum support : ";
	cin >> min_support;
	cout << "Number of transactions should be read at once : ";
	cin >> M;
	
	//Initialize all states to empty
	
	vector<Candidate> Solid_Squares;
	vector<Candidate> Solid_Circles;
	vector<Candidate> Dashed_Squares;
	vector<Candidate> Dashed_Circles;
	
	
	int transaction_count = database.size();
	
	//Insert all 1 item sets into dashed circles
	sort(all_items_list.begin(), all_items_list.end());
	for(auto item : all_items_list)
	{
		Candidate c;
		c.item_set.push_back(item);
		c.support = 0;
		c.ct = 0;
		Dashed_Circles.push_back(c);
	}
	
//	printf("Added 1-itemsets to Dashed Circles\n");
//	
//	printf("-------------Dashed Circles-------------\n");
//		for(int t=0; t<Dashed_Circles.size(); t++)
//		{
//			Candidate candidate = Dashed_Circles[t];
//			cout << "<";
//			for(auto e: candidate.item_set)
//			{
//				cout << e << " ";
//			}
//			cout << "> - support : "<< candidate.support <<" count : "<< candidate.ct<< endl;
//		}
//		
//		printf("-------------Dashed Squares-------------\n");
//		for(int t=0; t<Dashed_Squares.size(); t++)
//		{
//			Candidate candidate = Dashed_Squares[t];
//			cout << "<";
//			for(auto e: candidate.item_set)
//			{
//				cout << e << " ";
//			}
//			cout << "> - support : "<< candidate.support <<" count : "<< candidate.ct<< endl;
//		}
//		
//		printf("-------------Solid Circles-------------\n");
//		for(int t=0; t<Solid_Circles.size(); t++)
//		{
//			Candidate candidate = Solid_Circles[t];
//			cout << "<";
//			for(auto e: candidate.item_set)
//			{
//				cout << e << " ";
//			}
//			cout << "> - support : "<< candidate.support <<" count : "<< candidate.ct<< endl;
//		}
//		
//		printf("-------------Solid Squares-------------\n");
//		for(int t=0; t<Solid_Squares.size(); t++)
//		{
//			Candidate candidate = Solid_Squares[t];
//			cout << "<";
//			for(auto e: candidate.item_set)
//			{
//				cout << e << " ";
//			}
//			cout << "> - support : "<< candidate.support <<" count : "<< candidate.ct<< endl;
//		}
	
	
	int i=0;
	
	while(Dashed_Circles.size()!=0 || Dashed_Squares.size() != 0)
	{
		int ctr = 0;
		while(ctr < M)
		{
			vector<string> t = database[(i+ctr)%transaction_count];
			
			//printf("Reading Transaction %d .............\n", (i+ctr)%transaction_count);
			
			for(int k=0; k<Dashed_Circles.size(); k++)
			{
				Candidate candidate = Dashed_Circles[k];			
		
				//Increment the support for itemset if its present in transaction
				
				if(candidate.ct < transaction_count)
				{
				int presence=1;
				for(auto item: candidate.item_set)
				{
					if(find(t.begin(), t.end(), item)==t.end())
					{
						presence = 0;
					}
				}
				if(presence)
				{	
					//printf("Founding match incrementing support of Dashed_Circle item.......\n");
					candidate.support++;
				}
				//Increment count of candidate irrespective of its presence
				candidate.ct++;
				}
				Dashed_Circles[k]=candidate;
				
//				cout << "Candidate details : <";
//				for(auto each: candidate.item_set)
//				{
//				 cout << each << " ";
//				}
//				cout << ">    ";
//				cout << "support : "<< candidate.support<< "   ";
//				cout << "count :" << candidate.ct<< endl;
				
			}
			
			for(int k=0; k<Dashed_Squares.size(); k++)
			{
				Candidate candidate = Dashed_Squares[k];
				//Increment the support for itemset if its present in transaction
				//printf("Coming into loop of Dashed Squares............\n");
				
				if(candidate.ct < transaction_count)
				{
				int presence=1;
				for(auto item: candidate.item_set)
				{
					if(find(t.begin(), t.end(), item)==t.end())
					{
						presence = 0;
					}
				}
				if(presence)
				{	
					//printf("Founding match incrementing support of Dashed_Square item......\n");
					candidate.support++;
				}
				//Increment count of candidate irrespective of its presence
				candidate.ct++;
			}
			Dashed_Squares[k] = candidate;
			
//			cout << "\nCandidate details : <";
//				for(auto each: candidate.item_set)
//				{
//				 cout << each << " ";
//				}
//				cout << ">    ";
//				cout << "support : "<< candidate.support<< "   ";
//				cout << "count :" << candidate.ct<< endl;
//			
			}
			
			int endc = Dashed_Circles.size();
			for(int k = 0; k<endc; k++)
			{
				Candidate candidate = Dashed_Circles[k];
				
//				printf("Entering phase 2 .......");
//				//If a candidate in Dashed_Circles has support >= min_support, add that candidate to DashedSquares and remove from dashed circles
//				
//				cout << "Candidate details : " << endl;
//				for(auto each: candidate.item_set)
//				{
//				 cout << each << " ";
//				}
//				cout << endl;
//				cout << "support : "<< candidate.support<< endl;
//				cout << "count :" << candidate.ct<< endl;
				
				
				if(candidate.support >= min_support)
				{
					//printf("\n\n Adding %d th candidate from dashed_circles to dashed squares\n", k);
					Dashed_Squares.push_back(candidate);
					Dashed_Circles = removeCandidateFromPosition(Dashed_Circles, k);
					k--;
					endc--;
				
				
				int present_candidate_length = candidate.item_set.size();
				vector<vector<string>> present_size_item_list;
				for(int j=0; j< Dashed_Squares.size(); j++)
				{
					Candidate c = Dashed_Squares[j];
					if(present_candidate_length == c.item_set.size())
					{
						sort(c.item_set.begin(), c.item_set.end());
						if(!count(present_size_item_list.begin(), present_size_item_list.end(), c.item_set))
							present_size_item_list.push_back(c.item_set);
					}
					
				}
				for(int j=0; j< Solid_Squares.size(); j++)
				{
					Candidate c = Solid_Squares[j];
					if(present_candidate_length == c.item_set.size())
					{
						sort(c.item_set.begin(), c.item_set.end());
						if(!count(present_size_item_list.begin(), present_size_item_list.end(), c.item_set))
							present_size_item_list.push_back(c.item_set);
					}
					
				}
				/*
				printf("Printing present items list .......... \n");
				for(auto each: present_size_item_list)
				{
					cout<< "<";
					for(auto it: each)
					{
						cout << it << " ";
					}
					cout<< ">"<<endl;
				}
				*/
				
				
				
				for(int j=0; j<present_size_item_list.size()-1; j++)
				{
					for(int p=j+1; p<present_size_item_list.size(); p++)
					{
						int same = 1;
						for(int q=0; q<present_candidate_length-1; q++)
						{
							if(present_size_item_list[j][q]!=present_size_item_list[p][q])
							{
								same = 0;
							}
						}
						if(same)
						{
							vector<string> items;
							for(int q=0; q<present_candidate_length-1; q++)
							{
								items.push_back(present_size_item_list[j][q]);
							}
							items.push_back(present_size_item_list[j][present_candidate_length-1]);
							items.push_back(present_size_item_list[p][present_candidate_length-1]);
							sort(items.begin(), items.end());
							if(!checkItemsetPresence(Dashed_Circles, items) && !checkItemsetPresence(Dashed_Squares, items) && !checkItemsetPresence(Solid_Squares, items) && !checkItemsetPresence(Solid_Circles, items))
							{							
							Candidate c;
							c.item_set = items;
							c.support = 0;
							c.ct = 0;
							/*printf("Pusing the itemset to Dashed Circles.......... \n");
							for(auto each: items)
							{
								cout<< "<";
								
									cout << each << " ";
								
								cout<< ">"<<endl;
							}
							*/
							Dashed_Circles.push_back(c);
							}
						}
					}
				}
				
					
			}
				
				
			}
			
			int temp = Dashed_Squares.size();
			for(int k=0; k<temp; k++)
			{
				//printf("Entering phase 3 ....");
				Candidate candidate = Dashed_Squares[k];
				if(candidate.ct>=transaction_count)
				{
					Solid_Squares.push_back(candidate);
					Dashed_Squares = removeCandidateFromPosition(Dashed_Squares, k);
					k--;
					temp--;
				}
			}
			
			temp = Dashed_Circles.size();
			for(int k=0; k<temp; k++)
			{
				Candidate candidate = Dashed_Circles[k];
				if(candidate.ct>=transaction_count)
				{
					Solid_Circles.push_back(candidate);
					Dashed_Circles = removeCandidateFromPosition(Dashed_Circles, k);
					k--;
					temp--;
				}
			
			}
				
			ctr++;
		}
		
//		printf("After first M transactions.............\n");
//		printf("-------------Dashed Circles-------------\n");
//		for(int t=0; t<Dashed_Circles.size(); t++)
//		{
//			Candidate candidate = Dashed_Circles[t];
//			cout << "<";
//			for(auto e: candidate.item_set)
//			{
//				cout << e << " ";
//			}
//			cout << "> - support : "<< candidate.support <<" count : "<< candidate.ct<< endl;
//		}
//		
//		printf("-------------Dashed Squares-------------\n");
//		for(int t=0; t<Dashed_Squares.size(); t++)
//		{
//			Candidate candidate = Dashed_Squares[t];
//			cout << "<";
//			for(auto e: candidate.item_set)
//			{
//				cout << e << " ";
//			}
//			cout << "> - support : "<< candidate.support <<" count : "<< candidate.ct<< endl;
//		}
//		
//		printf("-------------Solid Circles-------------\n");
//		for(int t=0; t<Solid_Circles.size(); t++)
//		{
//			Candidate candidate = Solid_Circles[t];
//			cout << "<";
//			for(auto e: candidate.item_set)
//			{
//				cout << e << " ";
//			}
//			cout << "> - support : "<< candidate.support <<" count : "<< candidate.ct<< endl;
//		}
//		
//		printf("-------------Solid Squares-------------\n");
//		for(int t=0; t<Solid_Squares.size(); t++)
//		{
//			Candidate candidate = Solid_Squares[t];
//			cout << "<";
//			for(auto e: candidate.item_set)
//			{
//				cout << e << " ";
//			}
//			cout << "> - support : "<< candidate.support <<" count : "<< candidate.ct<< endl;
//		}
		
		//printf("After reading m transactions count of DS, DC, SS, SC: %d %d %d %d", Dashed_Squares.size(), Dashed_Circles.size(), Solid_Squares.size(), Solid_Circles.size());
		
		i=(i+ctr)%transaction_count;
	}
	
	for(auto ec: Solid_Squares)
	{
		cout << "< ";
		for(auto i: ec.item_set)
		{
			cout << i << " ";
		}
		cout<<">"<<endl;
	}
	
	/*
	//to remove duplcates
	set<set<string>> result;
	
	for(int p=0;p < Solid_Squares.size(); p++)
	{
		Candidate l = Solid_Squares[p];
		set<string> tmp;
		for(auto each: l.item_set)
		{
			tmp.insert(each);
		}
		result.insert(tmp);
	}
	
	
	//Display result
	for(auto e: result)
	{
		cout << "<";
		for(auto each: e )
		{
			cout << each << " ";
		}
		cout << ">"<< endl;
	}
	*/
	
	return 0;
}
