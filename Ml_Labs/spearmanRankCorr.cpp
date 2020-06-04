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



	return 0;
}