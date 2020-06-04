#include <iomanip>
#include "iostream"

using namespace std;

int const MAXV = 21;
double conf[MAXV][MAXV];
double cLines[MAXV];
double cColom[MAXV];

double f1(double pr, double rc) {
    if (pr + rc >= 0.00001)
        return 2 * (pr * rc) / (pr + rc);
    return 0;
}

int main() {
    int K;
    double overall = 0;
    cin >> K;

    for (int i = 0; i < K; i++) {
        for (int j = 0; j < K; j++) {
            cin >> conf[i][j];
            overall += conf[i][j];
            cColom[j] += conf[i][j];
            cLines[i] += conf[i][j];
        }
    }

    double sumPrecision = 0, sumRecall = 0, sumScore = 0;
    for (int i = 0; i < K; i++) {
        double d = conf[i][i];
        double precision = (d == 0 ? 0.0 : (d / cColom[i]));
        double recall = (d == 0 ? 0.0 : (d / cLines[i]));

        sumScore += f1(precision, recall) * cLines[i];

        sumPrecision += precision * cLines[i];
        sumRecall += recall * cLines[i];
    }

    cout << setprecision(12) << (f1(sumPrecision / overall, sumRecall / overall)) << '\n' << (sumScore / overall);

    return 0;
}