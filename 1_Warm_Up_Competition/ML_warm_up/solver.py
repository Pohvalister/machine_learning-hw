import numpy as np
import pandas as pd

from sklearn.preprocessing import LabelEncoder, OneHotEncoder
from sklearn.preprocessing import minmax_scale

from sklearn.model_selection import train_test_split, RandomizedSearchCV
from sklearn.neighbors import KNeighborsClassifier
from sklearn.tree import DecisionTreeClassifier
from sklearn.linear_model import Perceptron, SGDClassifier
from sklearn.ensemble import RandomForestClassifier
from sklearn.svm import SVC


from sklearn.metrics import accuracy_score, make_scorer

from sklearn.feature_selection import SelectKBest, SelectFromModel
from sklearn.feature_selection import chi2, f_classif, mutual_info_classif
from sklearn.ensemble import ExtraTreesClassifier
from sklearn.svm import LinearSVC

# input
train_input = pd.read_csv("./input/train.csv", index_col=0)
test_input = pd.read_csv("./input/test.csv", index_col=0)

train_Xs = train_input.values[:, :27]
train_ys = train_input.values[:, -1]

le = LabelEncoder()
le.fit(['A', 'B', 'C', 'D', 'E', 'F', 'G'])
train_ys = le.transform(train_ys)


def transform_Xs(Xs):
    floatXs = Xs[:, :20]
    categXs = Xs[:, 20:27]


    for i in range(len(categXs)):
        for j in range(len(categXs[i])):
            categXs[i][j] = le.transform([categXs[i][j]])[0]
        categXs[i] = categXs[i].astype('int')

    ohe = OneHotEncoder(sparse=False, categorical_features='all')
    categXs = ohe.fit_transform(categXs)

    floatXs = minmax_scale(floatXs)#normalize
    return np.concatenate([floatXs, categXs], axis=1)


train_Xs = transform_Xs(train_Xs)

train_ys = train_ys.astype('int')
learn_Xs, check_Xs, learn_ys, check_ys = train_test_split(train_Xs, train_ys, test_size=0.5)

test_Xs = test_input.values[:, :27]
test_Xs = transform_Xs(test_Xs)

# experimentals - classifiers
cl_names = ["RandomForest"]#,"SVC","SGD", "DecisionTree", "KNeighbors", "Perceptron"]
classifiers = [RandomForestClassifier]#, SVC,SGDClassifier,DecisionTreeClassifier, KNeighborsClassifier, Perceptron]

params = [
    {"n_estimators": range(10,100,10),
     "criterion": ["gini", "entropy"],
     "min_samples_split": range(2, 20)
    },
    {"kernel": ["linear", "rbf", "poly", "sigmoid"],
     "shrinking": [True, False],
     "degree": range(1, 5)
    },
    {"loss": ["hinge", "log", "modified_huber", "squared_hinge", "perceptron"],
     "penalty": ["l1", "l2", "elasticnet"],
     "max_iter": range(5, 1000, 20)
    },
    {"criterion": ["gini", "entropy"],
     "splitter": ["best", "random"],
     "min_samples_split": range(2, 20),
     "max_depth": range(100, 1000),
     "min_samples_leaf": range(1, 100),
     "min_impurity_decrease": np.arange(0.0, 0.7, 0.01)
     },
    {"algorithm": ["auto", "ball_tree", "kd_tree", "brute"],
     "metric": ["cityblock", "euclidean", "l1", "l2", "manhattan", "minkowski"],
     "n_neighbors": range(2, 15),
     "leaf_size": range(5, 50),
     },
    {"penalty": ["l1", "l2", "elasticnet"],
     "max_iter": range(5, 2000),
     "tol": np.arange(1e-6, 1e-2, 1e-6),
     "alpha": np.arange(1e-6, 1e-2, 1e-6),
     "eta0": np.arange(1e-2, 10.0, 1e-2)
     }

]

scorer = make_scorer(accuracy_score)

# experimentals - featureSelectors
ft_names = []
fselectors = []

ft_names += ["keep all features"]
fselectors += [lambda X: X]

SKBS_names = ["chi2", "fclassif", "mutual_info_classif"]
SelKBestStrats = [chi2, f_classif, mutual_info_classif]

for i in range(len(SelKBestStrats)):
    for k in range(1,21,2):
        name = "Select KBest + " + SKBS_names[i] + " with k = " + str(k)
        ft_names += [name]

        transformer = SelectKBest(SelKBestStrats[i], k=k).fit(train_Xs, train_ys)
        func = lambda X: transformer.transform(X)
        fselectors += [func]


for i in range(50,101,5):
    name = "Select with model: ExtraTreesClassifier with n_estimators = " + str(i)
    transformer = (SelectFromModel(ExtraTreesClassifier(n_estimators=i).fit(train_Xs, train_ys), prefit=True))
    func = lambda X: transformer.transform(X)
    ft_names += [name]
    fselectors += [func]


for i in np.arange(0.01,0.15,0.02):
    name = "Select with model: LinearSVC with penalty = l1, C = " + str(i)
    lsvc = LinearSVC(C=i, penalty="l1", dual=False).fit(train_Xs, train_ys)
    transformer = SelectFromModel(lsvc, prefit=True)
    func = lambda X:  transformer.transform(X)
    ft_names += [name]
    fselectors += [func]

for i in np.arange(0.01,0.15,0.02):
    name = "Select with model: LinearSVC with penalty = l2, C = " + str(i)
    lsvc = LinearSVC(C=i, penalty="l2", dual=False).fit(train_Xs, train_ys)
    transformer = SelectFromModel(lsvc, prefit=True)
    func = lambda X:  transformer.transform(X)
    ft_names += [name]
    fselectors += [func]

# finding best config
best_fs = None
#best_cl = None
best_cl_score = None
best_cl_exist = False
best_i, best_j = 0, 0

for i in range(len(classifiers)):
    for j in range(len(fselectors)):

        print("\nTesting on classifier:", cl_names[i], "with feature selection strategy:", ft_names[j])

        test_cl = classifiers[i]

        cl_params = params[i]
        learn_Xs_fs = fselectors[j](learn_Xs)

        best_configured = RandomizedSearchCV(test_cl(), cl_params, scoring=scorer).fit(learn_Xs_fs, learn_ys)
        learn_score = scorer(best_configured, learn_Xs_fs, learn_ys)
        print("Found configuration with score:", learn_score)

        check_Xs_fs = fselectors[j](check_Xs)
        check_score = scorer(best_configured, check_Xs_fs, check_ys)

        print("Actual score:", check_score)
        if not (best_cl_exist) or (best_cl_score < check_score):
            best_i, best_j = i, j
            #best_cl = best_configured
            best_cl_score = check_score
            best_cl_exist = True

print("\nBest score found: ", best_cl_score, "with classifier:", cl_names[best_i], "and strategy:", ft_names[best_j])

# predicting

test_Xs_fs = fselectors[best_j](test_Xs)
train_Xs_fs = fselectors[best_j](train_Xs)

best_cl=RandomizedSearchCV(classifiers[best_i](), params[best_i], scoring=scorer).fit(train_Xs_fs, train_ys)
test_ys = best_cl.predict(test_Xs_fs)
answer_ys = le.inverse_transform(test_ys)

pd.DataFrame(data=answer_ys, index=range(1, 6634, 2), columns=['class']).to_csv("answer.csv", header=True,
                                                                                index_label='id')
