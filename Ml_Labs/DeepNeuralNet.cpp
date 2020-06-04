#include <utility>
#include "iostream"
#include "vector"
#include "unordered_map"
#include "math.h"
#include "fstream"
 
using namespace std;

struct DNNClassifier{
    struct Neuron{
        int id;
        vector<Edge*> in;
        vector<Edge*> out;

        Neuron(int i):id(i){
        }
    };

    struct Edge{
        int id;
        Neuron* from, to;

        Edge(int id, Neuron* f, Neuron* t):id(id),from(f),t(to){
        }
    };

    vector<vector<Neuron*>> network;
    int NeuronCount=0;

    DNNClassifier(vector<int> layersNeuronsCount){
        for (int count : layersNeuronsCount){
            vector<Neuron*> tmpLayer;
            for (int j=0; j< count; j++)
                tmpLayer.push_back(new Neuron(NeuronCount++));
            network.push_back(tmpLayer);
        }
    }

}
 

int D, M, T;// deep, features, obj
vector<int> neurCount;
vector<vector<int>> objs;
vector<int> classes;
int main() {
 
    //ifstream cin("test", std::ifstream::in);

    cin>>D>>M;
    neurCount.push_back(M);
    for (int i=1; i< D; i++){
        int tmp; cin>>tmp;
        neurCount.push_back(tmp);
    }
    cin>>T;
    for(int i=0; i<T; i++){
        vector<int> tmpV;
        for (int i=0; i< M; i++){
            int tmp; cin>>tmp;
            tmpV.push_back(tmp);
        }
        int y; cin>>y;
        classes.push_back(y);
    }

    DNNClassifier classifier = DNNClassifier(neurCount);
    classifier.fit(objs,classes);

    printNet();

    return 0;
}