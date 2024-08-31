#include <iostream>
using namespace std;
#include<fstream>
#include <cstdlib>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char** argv) {
	fstream myfile2("tdx.txt",ios::out);
	int n=6;	// to limit max no.of items per transaction
	int x;
	x=rand()%(10-3+1)+3;
	int y;
	string line;
	char item;
	for(int i=1;i<=x;i++){
		myfile2<<i<<" ";
		y=rand()%n+1;
		for (int j=1;j<=y;j++){
			item=char(rand()%(102-97+1)+97);
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
