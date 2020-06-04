#include <utility>

#include "iostream"
#include "fstream"
#include "vector"
#include "algorithm"
#include "unordered_map"

using namespace std;

const int MAX_DEPTH = 11;


int M,K;//M - feat; K = class
int N;//obj

struct Object{
    Object(vector<int> f, int c):features(std::move(f)),classId(c){
    }
    vector<int> features;
    int classId;
};

struct Node{
    int id;

    Node* left;
    Node* right;
    vector<Object*> elems;

    bool isLeaf;
    int classId;

    double gini;
    pair<int,double> rule;

    Node(vector<Object*> elements):elems(std::move(elements)){
        gini = calcGini();
    }

    double calcGini(){//1 - sum (pi^2); pi - вероятность нахождения системы в i-ом состоянии
        double answer=0;
        unordered_map<int, int> counter;
        for (auto elem : elems)
            (counter.find(elem->classId)==counter.end()	?counter[elem->classId] = 1
                                                           : counter[elem->classId]++);

        int total = elems.size();
        for (const auto& [classId, amount] : counter)
            answer += ((double) amount / (double) total)*((double) amount / (double) total);

        answer = 1. - answer;
        return answer;
    }

    void setId(int id){
        this->id = id;
    }

    void splitNode(pair<int, double> rule){
        this->rule = rule;
        int featId = rule.first;
        double featVal = rule.second;


        unsigned n = count_if (elems.begin(), elems.end(), [featId, featVal](Object* o){return (double)o->features[featId]<featVal;});
        vector<Object*> l(n), r(elems.size() - n);
        partition_copy(elems.begin(), elems.end(),l.begin(), r.begin(),
                       [featId, featVal](Object* o){return (double)o->features[featId]<featVal;});

        left = new Node(l);
        right = new Node(r);
    }

};

struct FeatureReference{
    int featureId;
    int featureValue;
    int classId;
    FeatureReference(int fi, int fv, int ci):featureId(fi),featureValue(fv),classId(ci){
    }
};
struct GiniScorer{
    unordered_map<int, int> counter;
    int GiniSum = 0;
    int featAmount = 0;

    GiniScorer(vector<FeatureReference> feat){
        for (FeatureReference f : feat)
            incClass(f.classId);
    }

    GiniScorer(){
    }

    void incClass(int fId){
        if (counter.find(fId) == counter.end())
            counter[fId] = 0;
        GiniSum -= (counter[fId]*counter[fId]);
        counter[fId]++;
        featAmount ++;
        GiniSum += (counter[fId]*counter[fId]);
    }

    void decClass(int fId){
        GiniSum -= (counter[fId]*counter[fId]);
        counter[fId]--;
        featAmount --;
        GiniSum += (counter[fId]*counter[fId]);
    }

    double getGini(){
        if (featAmount == 0)
            return 1;
        return 1 - (double) GiniSum / (double) (featAmount*featAmount);
    }


};

pair<int,double> getBestFeature(vector<Object*> elems, double gini){
    double bestFeatureValue=0, bestIG = 0;
    int bestFeatureId;
    for (int i=0; i<M; i++){
        vector<FeatureReference> featSet;
        for (auto elem : elems)
            featSet.emplace_back(i,elem->features[i],elem->classId);

        stable_sort(featSet.begin(), featSet.end(),[](FeatureReference a, FeatureReference b)
        {return a.featureValue < b.featureValue;});

        GiniScorer left, right(featSet);
        int elemsAmount = elems.size();
        for (int j=0; j< featSet.size()-1; j++){
            left.incClass(featSet[j].classId);
            right.decClass(featSet[j].classId);

            double curIG = gini - (double)left.featAmount / (double)elemsAmount * left.getGini()
                           - (double)right.featAmount / (double)elemsAmount * right.getGini();
            if (curIG>bestIG||(i==0&&j==0)){
                bestIG = curIG;
                bestFeatureId = i;
                bestFeatureValue = (double) (featSet[j].featureValue + featSet[j+1].featureValue) /2;
            }
        }
    }
    return make_pair(bestFeatureId, bestFeatureValue);
}

enum splitNode {SPLIT, ONECLASS, NOCLASS, TOODEEP};

splitNode checkSplitNessesity(Node* node, int depth){
    if (node->elems.empty())
        return NOCLASS;
    if (depth >= MAX_DEPTH)
        return TOODEEP;

    bool sameClassed = true;
    int checkClass = node->elems[0]->classId;
    for (const auto& elem : node->elems)
        sameClassed  = (elem->classId == checkClass) && sameClassed;

    return (sameClassed ? ONECLASS: SPLIT);
}

int nodes_amount = 1;
void buildDT(Node* node, int depth){
    node->setId(nodes_amount++);

    splitNode desision = checkSplitNessesity(node,depth);
    node->isLeaf = true;

    switch(desision){
        case SPLIT:{
            node->isLeaf = false;
            pair<int,double> ruleInfo = getBestFeature(node->elems, node -> gini);
            node->splitNode(ruleInfo);

            buildDT(node->left,depth+1);
            buildDT(node->right,depth+1);
            break;
        }
        case NOCLASS:{
            node->classId = 1;
            break;
        }
        case ONECLASS:{
            node->classId = node->elems[0]->classId;
            break;
        }
        case TOODEEP:{
            unordered_map<int, int> counter;
            for (auto elem : node->elems)
                (counter.find(elem->classId)==counter.end()	? counter[elem->classId] = 1
                                                               : counter[elem->classId]++);
            node->classId = (*max_element(counter.begin(),counter.end(), [](auto a, auto b){ return a.second < b.second;})).first;
            break;
        }
    }
}

Node* buildDT(vector<Object*> elems){
    Node* root = new Node(std::move(elems));
    buildDT(root,2);
    return root;
}

void coutTree(Node* node){
    if (node-> isLeaf)
        printf("C %d\n",node->classId);
        //cout<<"C "<<node->classId<<'\n';
    else{
        printf("Q %d %f %d %d\n", node->rule.first + 1, node->rule.second,
               node->left->id,node->right->id);
        /*cout<<"Q "<<node->rule.first +1<<' '<<node->rule.second<<' ';
        cout<<node->left->id<<' '<<node->right->id<<'\n';*/
        coutTree(node->left);
        coutTree(node->right);
    }
}

vector<Object*> objs;

int main(){
    //ifstream cin("test", std::ifstream::in);

    cin>>M>>K>>N;

    for (int i=0; i<N; i++){
        int f,c;
        vector<int> features;
        for (int j=0; j<M; j++){
            cin>>f;
            features.push_back(f);
        }
        cin>>c;
        objs.push_back(new Object(features, c));
    }

    Node* root = buildDT(objs);
    cout<<nodes_amount - 1<<'\n';
    coutTree(root);

    return 0;
}