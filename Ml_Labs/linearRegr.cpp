#include "iostream"
#include "algorithm"
#include "vector"
#include "unordered_map"
#include "math.h"

using namespace std;

const double EPS = 0.000000001;
const double tau = 0.05;

void calcWithGauss (vector<vector<double>>& m){//gauss

    for (int rowId=0; rowId<m.size(); rowId++){//down
        double dividor = m[rowId][rowId];
        m[rowId][rowId] = 1;
        for (int colId = rowId+1; colId<m[rowId].size(); colId++)
            m[rowId][colId]/=dividor;

        for (int rowId2 = rowId+1; rowId2 <m.size(); rowId2++){
            if (abs(m[rowId2][rowId])<EPS)
                continue;
            double toRemove = m[rowId2][rowId];
            m[rowId2][rowId] = 0;
            for (int colId = rowId +1; colId < m[rowId2].size(); colId++)
                m[rowId2][colId] = (m[rowId2][colId]/toRemove) - m[rowId][colId];
        }
    }

    for (int rowId = m.size()-1; rowId>=0; rowId--){//up
        for (int rowId2 = rowId -1; rowId2>=0; rowId2--){
            if (abs(m[rowId2][rowId])<EPS)
                continue;
            double mult = m[rowId2][rowId];
            for (int colId = m[rowId].size(); colId > rowId2; colId--)
                m[rowId2][colId] = m[rowId2][colId] - m[rowId][colId]*mult;
        }
    }
}

struct Matrix{
    vector<vector<double>> matr;

    Matrix(int r, int c){
        matr = vector<vector<double>>(r, vector<double>(c, 0.));
    }

    Matrix(Matrix const &other){
        matr = other.matr;
    }

    void transponse(){
        vector<vector<double>> tmp = vector<vector<double>>(matr[0].size(), vector<double>(matr.size()));
        for (int i = 0; i<matr.size(); i++)
            for (int j=0; j<matr[0].size(); j++)
                tmp[j][i] = matr[i][j];

        swap(tmp,matr);
    }

    void multiply(const Matrix &other){
        vector<vector<double>> tmp(other.matr.size(), vector<double>(matr[0].size(), 0));
        for (int i=0;i<other.matr.size(); i++)
            for (int j=0; j<matr[0].size(); j++)
                for (int k=0; k< other.matr[0].size();k++)
                    tmp[i][j] += other.matr[i][k] * matr[k][j];

        swap(matr,tmp);
    }

    void inverse(){
        unsigned long S1 = matr.size();
        for (int i=0; i< S1; i++){
            for (int j=0; j< S1; j++)
                matr[i].push_back(0);
            matr[i][S1+i] = 1;
        }

        calcWithGauss(matr);
        vector<vector<double>> tmp;
        for (int i = 0; i< S1; i++) {//inverse
            vector<double> tmp2(S1);
            copy(matr[i].begin() + S1, matr[i].end(), tmp2.begin());
            tmp.emplace_back(tmp2);
        }
        swap(tmp,matr);
    }

    vector<double> multiply(const vector<double> &v){
        vector<double> answer(matr.size(), 0);
        for (int i=0; i< matr.size(); i++)
            for (int j=0; j<matr[i].size();j++)
                answer[i] += matr[i][j] * v[j];

        return answer;
    }
};


vector<double> ys;


int main(){
    int M, N;//feat, obj
    cin>>M>>N;

    Matrix m(N,M+1);

    for (int i=0; i< N; i++){
        for (int j = 0; j< M; j++)
            cin>>m.matr[i][j];
        m.matr[i][M] = 1;
        double y;
        cin>>y;
        ys.push_back(y);
    }

    // w = (X^T * X) ^-1 * X^T * y
    Matrix mT = Matrix(m);
    mT.transponse();
    m.multiply(mT);
    for (int i=0; i< m.matr.size();i++)
        m.matr[i][i] += tau;

    m.inverse();
    mT.multiply(m);
    vector coefs = mT.multiply(ys);
    cout.precision(11);
    for (const auto& c : coefs)
        cout<<c<<'\n';

    return 0;
}