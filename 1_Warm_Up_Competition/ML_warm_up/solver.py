import numpy as np
import pandas as pd

from sklearn.preprocessing import LabelEncoder
from sklearn.preprocessing import minmax_scale

from sklearn.model_selection import train_test_split, RandomizedSearchCV
from sklearn.neighbors import KNeighborsClassifier
from sklearn.tree import DecisionTreeClassifier
from sklearn.linear_model import Perceptron

from sklearn.metrics import accuracy_score, make_scorer

from sklearn.feature_selection import SelectKBest, RFECV
from sklearn.feature_selection import chi2, f_classif, mutual_info_classif

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

    floatXs = minmax_scale(floatXs)

    for i in range(len(categXs)):
        for j in range(len(categXs[i])):
            categXs[i][j] = le.transform([categXs[i][j]])[0]
        categXs[i] = categXs[i].astype('int')

    return np.concatenate([floatXs, categXs], axis=1)


train_Xs = transform_Xs(train_Xs)

train_ys = train_ys.astype('int')
learn_Xs, check_Xs, learn_ys, check_ys = train_test_split(train_Xs, train_ys)

test_Xs = test_input.values[:, :27]
test_Xs = transform_Xs(test_Xs)

# chi2: ValueError: Input X must be non-negative.


# experimentals - classifiers
cl_names = ["DecisionTree", "KNeighbors", "Perceptron"]
classifiers = [DecisionTreeClassifier, KNeighborsClassifier, Perceptron]

params = [
    {"criterion": ["gini", "entropy"],
     "splitter": ["best", "random"],
     "min_samples_split": range(2, 20),
     "max_depth": range(100, 1000),
     "min_samples_leaf": range(1, 100),
     "min_impurity_decrease": np.arange(0.0, 0.7, 0.1)
     },
    {"algorithm": ["auto", "ball_tree", "kd_tree", "brute"],
     "metric": ["cityblock", "euclidean", "l1", "l2", "manhattan", "minkowski"],
     "n_neighbors": range(2, 15),
     "leaf_size": range(5, 50),
     },
    {"penalty": ["l1", "l2", "elasticnet"],
     "max_iter": range(5, 2000)
     }
]

scorer = make_scorer(accuracy_score)

# experimentals - featureSelectors
ft_names = []
fselectors = []

ft_names += ["keep all features"]
fselectors += [lambda X, y: X]

SKBS_names = ["chi2", "fclassif", "mutual_info_classif"]
SelKBestStrats = [chi2, f_classif, mutual_info_classif]

for i in range(len(SelKBestStrats)):
    name = "SelectKBest + " + SKBS_names[i]
    ft_names += [name]

    func = lambda X, y: SelectKBest(SelKBestStrats[i]).fit_transform(X, y)
    fselectors += [func]

# finding best config
best_ft = None
best_cl = None
best_cl_score = None
best_cl_exist = False

for i in range(len(classifiers)):
    for j in range(len(fselectors)):

        print("\nTesting on classifier:", cl_names[i], "with featrue selection strategy:", ft_names[j])

        test_cl = classifiers[i]

        cl_params = params[i]
        learn_Xs_fs = fselectors[j](learn_Xs, learn_ys)

        best_configured = RandomizedSearchCV(test_cl(), cl_params, scoring=scorer).fit(learn_Xs_fs, learn_ys)
        learn_score = scorer(best_configured, learn_Xs_fs, learn_ys)
        print("\nFound configuration with score:", learn_score)

        check_Xs_fs = fselectors[j](check_Xs, check_ys)
        check_score = scorer(best_configured, check_Xs_fs, check_ys)

        print("Actual score:", check_score)
        if not (best_cl_exist) or (best_cl_score < check_score):
            best_ft = fselectors[j]
            best_cl = best_configured
            best_cl_score = check_score
            best_cl_exist = True

print("\nBest score found: ", best_cl_score)
