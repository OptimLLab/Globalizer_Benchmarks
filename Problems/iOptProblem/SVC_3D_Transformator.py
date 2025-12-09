import numpy as np
from trial import Point
from trial import FunctionValue
from problem import Problem
from sklearn.svm import SVC
from sklearn.model_selection import cross_val_score
from typing import Dict, List
from sklearn.utils import shuffle
import csv
from sklearn.model_selection import StratifiedKFold

def factory_dataset():
    x = []
    y = []
    with open(r"Datasets/transformator_state.csv") as rrrr_file:
        file_reader = csv.reader(rrrr_file, delimiter=",")
        for row in file_reader:
            x_row = []
            for i in range(len(row)-1):
                x_row.append(row[i])
            x.append(x_row)
            y.append(row[len(row)-1])
    return shuffle(np.array(x), np.array(y), random_state=42)

class SVC_3D(Problem):
    def __init__(self):

        super(SVC_3D, self).__init__()

        x_dataset, y_dataset = factory_dataset()

        self.dimension = 3
        self.number_of_float_variables = 2
        self.number_of_discrete_variables = 1
        self.number_of_objectives = 1
        self.number_of_constraints = 0
        if x_dataset.shape[0] != y_dataset.shape[0]:
            raise ValueError('The input and output sample sizes do not match.')
        self.x = x_dataset
        self.y = y_dataset
        self.float_variable_names = ["Regularization parameter", "Kernel coefficient"]
        self.lower_bound_of_float_variables = [5.0, -3.0]
        self.upper_bound_of_float_variables = [9.0, 1.0]
        self.discrete_variable_names.append('kernel')
        self.discrete_variable_values.append(['rbf', 'sigmoid', 'poly'])
        self.cv = StratifiedKFold(shuffle=True, random_state=42)


    def calculate(self, point: Point, function_value: FunctionValue) -> FunctionValue:
        cs, gammas = point.float_variables[0], point.float_variables[1]
        kernel_type = point.discrete_variables[0]
        clf = SVC(C=10 ** cs, gamma=10 ** gammas, kernel=kernel_type)
        function_value.value = -cross_val_score(clf, self.x, self.y, cv=self.cv, scoring='f1_macro').mean()
        return function_value
