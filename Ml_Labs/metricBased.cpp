#include <iomanip>

#include "iostream"
#include "math.h"
#include "vector"
#include "unordered_map"
#include "functional"
#include "fstream"

using namespace std;

int M, K, N;//feat, class, obj

typedef vector<vector<double>> matrix;
typedef function<double(double)> Kernel;
typedef function<double(vector<double>, vector<double>)> Metric;
typedef function<int(vector<double>)> Classifier;
typedef function<Classifier(int ignore_place)> ClassifierIgn;
typedef function<double(const vector<double>&)> Denominator;
typedef function<double(double)> Normalizer;

matrix trainSet;
vector<int> trainClasses;
matrix testSet;
vector<int> testClasses;

//classifier factory
unordered_map<int, double> distanceCounter;
bool sortBySecond (pair<int,double> a, pair<int, double> b){
    if (a.second == b.second)
        return a.first>b.first;
    return a.second<b.second;
}
ClassifierIgn buildClassifierIgn(const Metric& metric, const Kernel& kernel, const Denominator& denominator){
    return [=](int ignore_place){

        return [=](vector<double> features){

            vector<pair<int, double>> pairs;
            for (int o=0; o<N; o++)
                if (o!=ignore_place)
                    pairs.emplace_back(make_pair(o,metric(trainSet[o],features)));

            sort(pairs.begin(),pairs.end(),sortBySecond);

            vector<double> distances;
            for (const auto& [objId, dist] : pairs)
                distances.push_back(dist);


            distanceCounter.clear();
            double window = denominator(distances);

            unordered_map<int,double> classCounter;
            for (int i=0; i< min<size_t>(20, pairs.size());i++){
                auto & [objId, distance] = pairs[i];
                double newDistance = kernel(distance / window);
                distanceCounter[objId] = newDistance;
                int classId = trainClasses[objId];
                if (classCounter.find(classId)==classCounter.end())
                    classCounter[classId]= newDistance;
                else
                    classCounter[classId]+= newDistance;
            }
            return (*max_element(classCounter.begin(), classCounter.end(),sortBySecond)).first;
        };
    };
}

//hyperparams
vector<Normalizer> normalizers;
vector<Kernel> kernels{
        [](double d){return (abs(d)<=1.? 3./4.*(1.-d*d):0);},
        [](double d){return (abs(d)<=1.? 15./16.*(1.-d*d)*(1.-d*d):0);},
        [](double d){return (abs(d)<=1.? 1. - abs(d):0);}
};
vector<Metric> metrics{
        [](vector<double> x1, vector<double> x2){//euclidean
            double answer=0;
            for (int i=0; i< x1.size(); i++)
                answer+= (x1[i]-x2[i])*(x1[i]-x2[i]);
            return sqrt(answer);
        },
        [](const vector<double>& a, const vector<double>& b)
        {
            double result = 0.;
            for (size_t i = 0; i < a.size(); i++) {
                result += sqrt(abs(a[i] - b[i]));
            }
            return result * result;
        },
        [](vector<double> x1, vector<double > x2){//manhattan
            double answer=0;
            for (int i=0; i<x1.size();i++)
                answer += abs(x1[i]-x2[i]);
            return answer;
        }
};
vector<Denominator> denominators;

//f task
double f1(double pr, double rc) {
    if (pr + rc >= 0.00001)
        return 2 * (pr * rc) / (pr + rc);
    return 0;
}

double calc_f_score(int k, const vector<vector<int>> & conf){
    vector<double> cColom(k), cLines(k);
    double overall = 0;

    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            overall += conf[i][j];
            cColom[j] += conf[i][j];
            cLines[i] += conf[i][j];
        }
    }

    double sumPrecision = 0, sumRecall = 0, sumScore = 0;
    for (int i = 0; i < k; i++) {
        double d = conf[i][i];
        double precision = (d == 0 ? 0.0 : (d / cColom[i]));
        double recall = (d == 0 ? 0.0 : (d / cLines[i]));

        sumScore += f1(precision, recall) * cLines[i];

        sumPrecision += precision * cLines[i];
        sumRecall += recall * cLines[i];
    }

    //return (f1(sumPrecision / overall, sumRecall / overall));// << '\n' <<
    return (sumScore / overall);
}
//testings
double testClassifier(const ClassifierIgn& classifierIgn){
    vector<vector<int>> classCompare(K, vector<int>(K,0));
    for (int i=0; i<N; i++){
        Classifier classifier = classifierIgn(i);
        int c = classifier(trainSet[i]);
        classCompare[trainClasses[i]][c]++;
    }
    return calc_f_score(K, classCompare);
}


int main(){

    //ifstream cin("test", std::ifstream::in);

    cin>>M>>K>>N;

    for (int o=0; o< N; o++){//input
        vector<double> tmpV;
        for (int f=0; f<M; f++){
            double tmp;
            cin>>tmp;
            tmpV.push_back(tmp);
        }
        int c;
        cin>>c;
        trainClasses.push_back(c - 1);
        trainSet.push_back(tmpV);
    }
    for (int f=0; f<M; f++){//normalize
        double maxVal=trainSet[0][f], minVal=trainSet[0][f];
        for (int o=1; o<N; o++){
            maxVal = max(trainSet[o][f],maxVal);
            minVal = min(trainSet[o][f],minVal);
        }
        normalizers.emplace_back([maxVal,minVal](double feat){return ((maxVal-minVal!=0)?(2*(feat - minVal)/(maxVal-minVal))-1:0);});

        for (int o=0; o<N; o++)
            trainSet[o][f] = normalizers[f](trainSet[o][f]);
    }
    for (double d = 0.1; d <= 1.6; d += 0.5)
        denominators.emplace_back([d](const vector<double> &distances){return d;});
    for (int i = 0; i < min(21, N); i++)
        denominators.emplace_back([i](const vector<double> &distances){return distances[i];});

    double bestScore = -1.;
    ClassifierIgn bestClassifierIgn;
    for (const auto& metric : metrics)
        for (const auto& kernel : kernels)
            for (const auto& denominator : denominators){
                ClassifierIgn newClassifierIgn = buildClassifierIgn(metric, kernel, denominator);
                double newScore = testClassifier(newClassifierIgn);
                if (newScore > bestScore){
                    bestScore = newScore;
                    bestClassifierIgn = newClassifierIgn;
                }
            }

    Classifier bestClassifier = bestClassifierIgn(-1);
    int N1;
    cin>>N1;
    for (int o=0; o< N1; o++){//output
        vector<double> tmpV;
        for (int f=0; f<M; f++){
            double tmp;
            cin>>tmp;
            tmpV.push_back(normalizers[f](tmp));
        }
        int result = bestClassifier(tmpV);

        cout<<setprecision(10)<<fixed;
        cout<<distanceCounter.size()<<' ';
        for (const auto& [classId, distance] : distanceCounter)
            cout<<classId+1<<' '<<distance<<' ';
        cout<<'\n';
        //cout<<result+1<<'\n';
    }
    return 0;
}