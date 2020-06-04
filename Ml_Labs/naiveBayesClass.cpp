#include <utility>
#include "iostream"
#include "vector"
#include "unordered_map"
#include "math.h"
#include "fstream"

using namespace std;

struct NaiveBayesClassifier {
    struct LetterClass {

        const double SMOOTH = 0.01;

        unordered_map<int, int> wordCounter;

        int textAmount = 0;
        int wordAmount = 0;

        void insertNewText(vector<int> text) {
            textAmount++;
            for (int word : text) {
                wordAmount++;
                if (wordCounter.find(word) == wordCounter.end())
                    wordCounter[word] = 1;
                else
                    wordCounter[word]++;
            }
        }

        //adaptive smoothing
        double getLogCredibility(vector<int> text, int featuresAmount) {
            double logs = 0;
            for (int word : text) {
                int wordCount = (wordCounter.find(word) != wordCounter.end() ? wordCounter[word] : 0);//P(fi|cj) = count(fi,cj) + z / sum(count(fk, cj)) + z*q
                logs += log((wordCounter[word] + SMOOTH) /
                            ((double) wordAmount + SMOOTH * (double) featuresAmount));
            }

            return logs;
        }
    };

    LetterClass spamClass;
    double spamPrioriProb;
    LetterClass legiClass;
    double legiPrioriProb;


    int wordAmount = 0;

    vector<vector<int>> objs;
    vector<char> ans;

    double off = 0;

    NaiveBayesClassifier(vector<vector<int>> xs, vector<char> ys) : objs(std::move(xs)), ans(std::move(ys)) {
        //fit
        for (int i = 0; i < objs.size(); i++) {
            if (ans[i] == 'S')
                spamClass.insertNewText(objs[i]);
            else
                legiClass.insertNewText(objs[i]);
        }
        wordAmount = spamClass.wordAmount + legiClass.wordAmount;

        spamPrioriProb =
                (double) spamClass.textAmount / ((double) spamClass.textAmount + (double) legiClass.textAmount);
        legiPrioriProb =
                (double) legiClass.textAmount / ((double) spamClass.textAmount + (double) legiClass.textAmount); //c = ln(P(c)) + sum (ln(P(fi|c)))

    }

    char predict(const vector<int> &obj) {
        /* double diff = spamClass.getLogCredibility(obj, wordAmount) + log(spamPrioriProb)
                     - legiClass.getLogCredibility(obj, wordAmount) - log(legiPrioriProb);
         if (off < diff)*/
        double legiLikeHood = legiClass.getLogCredibility(obj, wordAmount) + log(legiPrioriProb);
        double spamLikeHood = spamClass.getLogCredibility(obj, wordAmount) + log(spamPrioriProb);
        if (spamLikeHood >= legiLikeHood * 0.95)
            return 'S';
        else
            return 'L';
    }
};

int N;
vector<char> legits;
vector<vector<int>> letters;

int main() {

    //ifstream cin("test", std::ifstream::in);

    cin >> N;
    for (int i = 0; i < N; i++) {
        int k;
        char c;
        cin >> k >> c;
        legits.push_back(c);
        vector<int> tmpV;
        for (int j = 0; j < k; j++) {
            int tmp;
            cin >> tmp;
            tmpV.push_back(tmp);
        }
        letters.push_back(tmpV);
    }

    NaiveBayesClassifier cl = NaiveBayesClassifier(letters, legits);

    cin >> N;
    for (int i = 0; i < N; i++) {
        int k;
        cin >> k;
        vector<int> tmpV;
        for (int j = 0; j < k; j++) {
            int tmp;
            cin >> tmp;
            tmpV.push_back(tmp);
        }
        cout << cl.predict(tmpV) << '\n';
    }

    return 0;
}