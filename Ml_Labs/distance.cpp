#include "iostream"
#include "algorithm"
#include "vector"

using namespace std;

bool y_first(pair<long,long> a, pair<long,long> b){
	if (a.second == b.second)
		return a.first < b.first;
	return a.second < b.second;
}

bool x_first(pair<long, long> a, pair<long,long> b){
	if (a.first == b. first)
		return a.second < b.second;
	return a.first < b.first;
}

vector<pair<long,long>> features;

int main(){
	long x,y;
	int K,N;
	cin>>K>>N;

	for (int i=0; i< N; i++){
		cin>>x>>y;
		features.push_back(make_pair(x,y));
	}

	sort(features.begin(),features.end(),x_first);

	long additional_dist = 0, overall_dist = 0;
	for (int i = 1; i<N; i++){
		additional_dist += (features[i].first - features[i-1].first) * i;
		overall_dist += additional_dist;
	}

	sort(features.begin(), features.end(), y_first);

	long overall_aggreg_dist = 0, aggreg_dist = 0, same_y_count = 1; 
	for (int i = 1; i< N; i++){
		if (features[i].second == features[i-1].second){
			aggreg_dist += (features[i].first - features[i-1].first) * same_y_count;
			overall_aggreg_dist += aggreg_dist;
			same_y_count +=1;
		}else{
			aggreg_dist = 0;
			same_y_count = 1;
		}
	}

	cout<<2*overall_aggreg_dist<<'\n'<<2*(overall_dist - overall_aggreg_dist);

	return 0;
}