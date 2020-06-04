#include "iostream"
#include "vector"

const int MAXCOUNT = 100010;
using namespace std;
std::vector<int> classes[MAXCOUNT];
std::vector<int> sets[MAXCOUNT];

int main(){
	int N,M,K;
	cin>>N>>M>>K;

	for (int i=0; i<N;i++){
		int cl;
		cin>>cl;
		classes[cl-1].push_back(i+1);
	}

	int output_point=0;
	for (int i=0; i< M;i++){
		for (int j=0; j< classes[i].size(); j++){
			sets[output_point].push_back(classes[i][j]);
			output_point=(output_point+1)%K;
		}
	}

	for (int i=0; i< K; i++){
		cout<<sets[i].size();
		for (int j=0;j< sets[i].size();j++)
			cout<<' '<<sets[i][j];
		cout<<'\n';
	}


	return 0;
}