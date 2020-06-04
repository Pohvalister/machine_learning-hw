#include "iostream"
#include "math.h"

using namespace std;

int const MAXN = 100010;

int features[MAXN][2];

int main(){
	int N;
	cin>>N;

	double avgX=0, avgY=0;
	double x,y;

	for (int i=0; i<N; i++){
		cin>>x>>y;
		features[i][0] = x; features[i][1] = y;
		avgX += x; avgY += y;
	}
	avgX/=double(N); avgY/=double(N);

	double sumM = 0, sumM_X = 0, sumM_Y = 0;
	for (int i=0; i<N; i++){
		x = features[i][0] - avgX;
		y = features[i][1] - avgY;

		sumM_X += x*x;
		sumM_Y += y*y;
		sumM   += x*y;
	}

	double pearson = sumM / sqrt(sumM_X * sumM_Y);

	cout<<pearson;

	return 0;
}