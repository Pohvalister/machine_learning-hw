#include "iostream"
#include "unordered_map"

using namespace std;

const int MAXF = 100010;

template<typename T>
bool check(const unordered_map<int, T> &map, int value){
	return map.find(value) != map.end();
}

void increase (unordered_map<int, int> &map, int value){
	if (!check(map,value))
		map[value] = 0;
	map[value]++;
}

unordered_map<int, unordered_map<int,int>*> from2to1;
unordered_map<int, int> amount1;
unordered_map<int, int> amount2;

int main(){
	int K1, K2, N;

	cin>>K1>>K2>>N;

	for (int i =0; i< N; i++){
		int x1,x2;
		cin>>x1>>x2;
		if (!check(from2to1, x2)){
			from2to1[x2]=new unordered_map <int,int>;
			from2to1[x2]->operator[](x1) =0;
		}
		increase(*from2to1[x2], x1);
		increase(amount1, x1);
		increase(amount2, x2);
	}

	double answer = 0;

	for (const auto& [x2,countx2] : amount2){
		double imp = (double) countx2 / (double) N;
		double totalx1Amount = 0;

		for (const auto& [x1, count] : *from2to1[x2]){
			int countx1 = amount1[x1];

			double fe = imp * (double) countx1;
			double fo = (double) count;
			double b = (fo - fe);

			totalx1Amount += (double) countx1;
			answer += b * b/fe;

		}
		answer += (double)countx2 - imp * totalx1Amount;

	}

	cout.precision(17);
	cout<<answer;
	return 0;
}