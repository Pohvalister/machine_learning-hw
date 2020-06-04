#include "iostream"
#include "vector"
#include "algorithm"

using namespace std;

const int MAXF = 100010;

vector<int> feat_y_to_x[MAXF];
vector<int> feat_x;

long distanceBetween(vector<int> v){
	long answer=0;
	int n = v.size();
	if (n==0)
		return 0;

	sort(v.begin(), v.end());

	vector<long> prefSum(n+1);
	vector<long> sufSum(n);
	prefSum[0] = 0;
	sufSum[n-1] = 0;

	for(int i=0; i< n-1; i++){
		prefSum[i+1] = (long) v[i] + prefSum[i];
		sufSum[n-i-2] = (long) v[n-i-1] + sufSum[n-i-1];

	}
	prefSum[n] = (long) v[n-1] + prefSum[n-1];

	for (int i=0; i< n; i++)
		answer += sufSum[i] - prefSum[i] - (long) (n-2*i-1)*(long)v[i];
	return answer;

}


int main(){
	int K, N;
	cin>>K>>N;

	for (int i=0; i< N; i++){
		int x, y;
		cin>>x>>y;
		feat_y_to_x[y-1].push_back(x);
		feat_x.push_back(x);
	}

	long overall_dist = distanceBetween(feat_x);
	long overall_aggreg_dist = 0;
	for (int i=0; i<K+1; i++){
		overall_aggreg_dist += distanceBetween(feat_y_to_x[i]);
	}
	cout<<overall_aggreg_dist<<'\n'<<overall_dist-overall_aggreg_dist;


	return 0;
}