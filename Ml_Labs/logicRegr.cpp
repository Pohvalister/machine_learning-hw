#include "iostream"
#include "fstream"
#include "vector"
#include "algorithm"
#include "unordered_map"
#include "math.h"

using namespace std;

vector<int> table;

struct Neuron{
    vector<double> inWeights;
    double b;
};

int genBitVector(int value, vector<int>& v, int len){
    if (len == -1){
        return value;
    }
    value = genBitVector(value,v,len-1);
    v.push_back(value%2);
    return value/2;
}

int main(){
    //ifstream cin("test", std::ifstream::in);
    int M, b;
    cin>>M;

    int N = pow (2, M);
    for (int i=0; i<N; i++){
        cin>>b;
        table.push_back(b);
    }

    vector<Neuron> layer;
    for (int i=0; i<N; i++)
        if (table[i]){
            vector<int> bits;
            genBitVector(i,bits,M-1);
            Neuron neuron;
            neuron.b = 0.5;
            for (int bit : bits){
                if (bit) {
                    neuron.inWeights.push_back(1);
                    neuron.b -= 1.0;
                }else
                    neuron.inWeights.push_back(-1);
            }
            layer.push_back(neuron);
        }

    if (layer.empty()){
        printf("2 %d 1\n", M);
        for (int i=0; i<M; i++)
            printf("%f", 0.0);
        printf("%f", -0.5);
    }else{
        printf("3 %d %d 1\n", M, layer.size());
        for (Neuron n : layer){
            for (double w : n.inWeights)
                printf("%f ", w);
            printf("%f ", n.b);
        }
        printf("\n");
        for (int i=0; i<layer.size(); i++)
            printf("%f ", 1.0);
        printf("%f", -0.5);
    }

    return 0;
}