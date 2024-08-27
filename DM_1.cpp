#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char** argv) {
	//task1: creation of transaction database
	fstream myfile("td.txt",ios::out);
	myfile<<"T1 a b c e"<<endl;
	myfile<<"T2 b d f"<<endl;
	myfile<<"T3 a c d f"<<endl;
	myfile<<"T4 d f"<<endl;
	myfile<<"T5 c d e"<<endl;
	myfile.close();
	//task2: reading transaction database
	string line;
	myfile.open("td.txt",ios::in);
	while(!myfile.eof()){
		getline(myfile,line);
		cout<<line<<endl;
	}
	myfile.close();
	//task3: count of item
	char c;
	cout<<"enter the item"<<endl;
	cin>>c;
	myfile.open("td.txt",ios::in);
	int count=0;
	while(!myfile.eof()){
		getline(myfile,line);
		for(int i=0;i<line.length();i++){
			if(line[i]==c){
				count++;
			}
		}
	}
	cout<<c<<" "<<count<<endl;
	myfile.close();
	
	//task4:transaction list for a given item
	char ch;
	cout<<"enter the item for trscn list"<<endl;
	cin>>ch;
	myfile.open("td.txt",ios::in);
	cout<<ch<<" ";
	while(!myfile.eof()){
		getline(myfile,line);
		for(int i=0;i<line.length();i++){
			if(line[i]==ch){
				cout<<line[0]<<line[1]<<" ";
			}
		}
	}
	cout<<endl;
	myfile.close();
	
	//task5:generating random no.of transactions
	fstream myfile2("tdx.txt",ios::out);
	int N=6;   //to limit no.of transactions
	int n=4;	// to limit max no.of items per transaction
	int x= rand()%N;
	int y=rand()%n;
	string t;
	char item;
	cout<<"no.of transactions will be read: "<<x<<endl;
	cout<<"no.of items per transaction will be read: "<<y<<endl;
	for(int i=0;i<x;i++){
		cout<<"enter transtn number"<<endl;
		cin>>t;
		myfile2<<t<<" ";
		for (int j=1;j<=y;j++){
			cout<<"enter item"<<j<<" ";
			cin>>item;
			myfile2<<item<<" ";
		}
		myfile2<<endl;
	}
	myfile2.close();
	myfile2.open("tdx.txt",ios::in);
	while(!myfile2.eof()){
		getline(myfile2,line);
		cout<<line<<endl;
	}
	myfile2.close();
	
	return 0;
}
