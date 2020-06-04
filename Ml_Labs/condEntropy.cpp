#include "iostream"
#include "algorithm"
#include "vector"
#include "unordered_map"
#include "math.h"

using namespace std;

const int MAXF = 100010;

struct State{
	unordered_map<int, int> ys;
	int amount = 0;

	void add(int y){
		amount++;
		if (ys.find(y) == ys.end())
			ys[y] = 0;
		ys[y]++;
	}

};

State states[MAXF];

int main(){
	int Kx, Ky, N;

	cin>>Kx>>Ky>>N;

	for (int i =0; i < N; i++){
		int x, y;
		cin>>x>>y;
		states[x-1].add(y);
	}

	double allSum = 0;
	for (int i =0; i<Kx; i++){
		if (!states[i].amount)
			continue;

		double sum = 0;
		for (const auto& [key,count] : states[i].ys){
			double value = (double)count/(double)states[i].amount;
			sum -= value * log(value);
		}
		allSum += (double)states[i].amount * sum/((double)N);
	}
	cout.precision(17);
	cout<<allSum;


	return 0;
}