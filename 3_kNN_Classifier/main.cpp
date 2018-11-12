#include <iostream>
#include <utility>
#include <cmath>

//Neighbors Searchers - structures to handle nearest neighbors founding
template<std::size_t N>
class KNeighborsAlgorithm{
    //virtual KNeighborsAlgorithm(int* X[N], int* y, int a_obj, double distance(int[N],int[N])) = 0;
    virtual std::pair<int, double> *search(int x[N], int neigh_amount) = 0;
};

template<std::size_t N>
class KDTreeStruct : KNeighborsAlgorithm<N>{

};
//Metrics - evaluates distances betwwen features
template <typename T, std::size_t N>
double euclidian_distance(T a[N], T b[N]){
    double answ=0;
    for (int i=0; i<N;i++)
        answ += (a[i] - b[i])*(a[i] - b[i]);
    return sqrt(answ);
}

template <typename T, std::size_t N>
double manhattan_distance(T a[N], T b[N]){
    double answ = 0;
    for (int i=0; i<N; i++)
        answ += abs(a[i] - b[i]);
    return answ;
}

template <typename T, std::size_t N>
double chebyshev_distance(T a[N], T b[N]){
    double answ = 0;
    for (int i=0; i< N; i++)
        answ = max(answ, abs(a[i] - b[i]));
    return answ;
}

//Weights - adds weights to distances
void uniform_weight(double* distances, int K){
    for (int i=0; i<K; i++)
        distances[i]=(distances[i] <= 1 ? 1 : 0);
}

void triangle_weight(double* distances, int K){
    for (int i=0; i<K; i++)
        distances[i] = distances[i] <= 1 ? 1.0 - abs(distances[i]) : 0;
}

void exponent_weight(double* distances, int K){
    for (int i=0; i<K; i++)
        distances[i] = distances[i] > 0 ? 1.0 / abs(distances[i]) : 1;
}

void tricube_weight(double* distances, int K){
    for (int i=0; i<K; i++)
        distances[i] = distances[i] <= 1 ? 0.86419753 * pow(1 - pow(distances[i], 3), 3) : 0;
}

//CLassifier - ML structure
template <class A, std::size_t N, typename = void>
struct KNeighborsClassifier;

template<class KNA, std::size_t N>
struct KNeighborsClassifier<KNA, N, typename std::enable_if<std::is_base_of<KNeighborsAlgorithm<N>, KNA>::value>::type> {
private:
    int n_count;
    double w_scale;
    void (*w_func)(double*, int);
    KNA n_algo;
    double (*distance_func)(int[N], int[N]);

public:
    KNeighborsClassifier(int num_neigbours, double window_scale, void weight_func(double*, int), double distance(int[N],int[N]))
            : n_count(num_neigbours)
            , w_func(weight_func)
            , distance_func(distance)
            , w_scale(window_scale)
    {};

    void fit (int* X[N], int* y, int a_obj){
        n_algo = KNA(X, y, a_obj, distance_func);
    }
    int predict(int x[N]){
        std::pair<int, double> pairs[n_count];
        pairs = n_algo.search(x, n_count);
        double* distance= new double[n_count];
        for (int i=0;i< n_count; i++){
            distance[i]=pairs[i].second* w_scale;
        }
        w_func(distance, n_count);
        int max_type=0;
        double max_weight=0;
        for (int i = 0; i<n_count; i++){
            double w = 0;
            for (int j=i; j<n_count; j++){//!!!!!!!
                if (pairs[j].first==pairs[i].first){
                    w+= pairs[j].second * distance[j];
                }
            }
            if (max_weight<w){
                max_weight=w;
                max_type = pairs[i].first;
            }
        }
        return max_type;
    }
};

//Main Function
const int MAXCLAS = 26;
const int MAXFEAT = 201;
const int MAXOBJE = 401;

const int WEIGHT_C = 4;
void (*weights[WEIGHT_C])(double*, int) = {uniform_weight, triangle_weight, exponent_weight, tricube_weight};

const int DISTANCE_C = 3;
template <typename T, std::size_t N>
double (*distances[DISTANCE_C])(T[N], T[N]) = {euclidian_distance<T,N>, manhattan_distance<T,N>, chebyshev_distance<T,N>};


KNeighborsClassifier<KNeighborsAlgorithm<MAXFEAT>,MAXFEAT>* train(int classes_c, int neigh_c, int window_c, int metric_c, int weight_c);

int X[MAXOBJE][MAXFEAT];
int y[MAXOBJE];

int X_test[MAXOBJE][MAXFEAT];
int y_test[MAXOBJE];

int main() {
    int FEAT_C, CLASS_C, OBJ_C;
    std::cin>>FEAT_C>>CLASS_C>>OBJ_C;

    for (int i=0;i< OBJ_C; i++)
        for (int j=0; j< MAXFEAT; i++)
            if (j <FEAT_C)
                std::cin>>X[i][j];
            else {
                if (j == FEAT_C)
                    std::cin >> y[i];
                X[i][j] = 0;
            }

    int TEST_C;
    std::cin>>TEST_C;

    for (int i=0;i< TEST_C; i++)
        for (int j=0; j< MAXFEAT; i++)
            if (j <FEAT_C)
                std::cin>>X_test[i][j];
            else
                X_test[i][j]=0;



    return 0;
}

KNeighborsClassifier<KNeighborsAlgorithm<MAXFEAT>,MAXFEAT>* train(int classes_c, int neigh_c, int window_c, int metric_c, int weight_c){
    for (int nc = 1; nc< neigh_c; nc++)
        for (int mc=0; mc< metric_c; mc++)
            for (int wec=0; wec < weight_c; wec++)
                for (int wic=0; wic < window_c; wic++){
                    double win_scale = 0.1 + (5 - 0.1) / (window_c*wic);///!!!!!
                    auto classifier = new KNeighborsClassifier<KDTreeStruct<MAXFEAT>, MAXFEAT>(nc, win_scale, weights[wec],distances<int,MAXFEAT>[mc]);


                }
}
