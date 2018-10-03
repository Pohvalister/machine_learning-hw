import numpy as np
import random
import pandas as pd

from sklearn.preprocessing import LabelEncoder

from sklearn.model_selection import train_test_split, RandomizedSearchCV
from sklearn.neighbors import KNeighborsClassifier
from sklearn.tree import DecisionTreeClassifier
from sklearn.linear_model import Perceptron

from sklearn.metrics import accuracy_score, make_scorer

from sklearn.feature_selection import SelectKBest


#input
train_input = pd.read_csv("./input/train.csv", index_col=0)
test_input = pd.read_csv("./input/test.csv", index_col=0)

train_Xs = train_input.values[:, :27]
train_ys = train_input.values[:, -1]

converter = dict(zip(['A','B','C','D','E','F','G'], range(7)))

for i in range(len(train_ys)):
    train_ys[i]=converter[train_ys[i]]
    for j in range (20, len(train_Xs[i])):
        train_Xs[i][j]=converter[train_Xs[i][j]]

train_ys=train_ys.astype('int')
learn_Xs, check_Xs, learn_ys, check_ys = train_test_split(train_Xs,train_ys)

test_Xs = test_input.values[:, :27]
#experimentals
cl_names = ["DecisionTree", "KNeighbors", "Perceptron"]
classifiers = [ DecisionTreeClassifier,KNeighborsClassifier, Perceptron]

params = [
    {"criterion":["gini", "entropy"],
     "splitter":["best", "random"],
     "min_samples_split": range(2, 20),
     "max_depth": range(100, 1000),
     "min_samples_leaf": range(1,100),
     "min_impurity_decrease": np.arange(0.0, 0.7,0.1)
     },
    {"algorithm":["auto", "ball_tree", "kd_tree", "brute"],
     "metric":["cityblock", "euclidean", "l1", "l2", "manhattan", "minkowski"],
     "n_neighbors": range(2,15),
     "leaf_size": range(5,50),
     },
    {"penalty":["l1", "l2", "elasticnet"],
     "max_iter": range(5,2000)
    }
]

scorer = make_scorer(accuracy_score)

ft_names = ["SelectKBest", "RFECV"]





#finding best config
best_cl = None
best_cl_score = None
best_cl_exist = False

for i in range (len(classifiers)):

    print("\nTesting on ", cl_names[i])
    test_cl = classifiers[i]

    cl_params=params[i]

    best_configured = RandomizedSearchCV(test_cl(), cl_params, scoring=scorer).fit(learn_Xs, learn_ys)
    learn_score = scorer(best_configured, learn_Xs,learn_ys)
    print("\nFound configuration with score: ", learn_score)

    check_score = scorer(best_configured, check_Xs, check_ys)
    print("Actual score: ", check_score)
    if not (best_cl_exist) or (best_cl_score < check_score):
        best_cl=best_configured
        best_cl_score = check_score
        best_cl_exist = True


print("\nBest score found: ", best_cl_score)


