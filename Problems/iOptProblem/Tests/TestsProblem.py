from functools import partial

from ClassificationScripts.dataset import ECGDataset
from ClassificationScripts.model import MobileNetV3Small1D
from Tests.hyperparams import Numerical, Categorial, Hyperparameter
from Tests.iOptSearcher import iOptSearcher
from Tests.metrics import DATASET_TO_METRIC
from trial import Point
from trial import FunctionValue
from problem import Problem
from typing import Dict
from sklearn.model_selection import train_test_split
import numpy as np
import wfdb
from torch.utils.data import Dataset, DataLoader
import torch
import torch.nn as nn
import os
import Tests.data as data

from sklearn.svm import SVC
from xgboost import XGBClassifier
from sklearn.neural_network import MLPClassifier

NAME_TO_DATASET = {
    'balance': data.Balance,
    'bank-marketing': data.BankMarketing,
    'banknote': data.Banknote,
    'breast-cancer': data.BreastCancer,
    'car-evaluation': data.CarEvaluation,
    'cnae9': data.CNAE9,
    'credit-approval': data.CreditApproval,
    'digits': data.Digits,
    'ecoli': data.Ecoli,
    'parkinsons': data.Parkinsons,
    'semeion': data.Semeion,
    'statlog-segmentation': data.StatlogSegmentation,
    'wilt': data.Wilt,
    'zoo': data.Zoo
}

METHOD_TO_HYPERPARAMS = {
    SVC: {
        'gamma': Numerical('float', 1e-9, 1e-6, is_log_scale=False),#
        'C': Numerical('int', 1, 1e10, is_log_scale=False)#
    },

    XGBClassifier: {
        'n_estimators': Numerical('int', 10, 200),
        'max_depth': Numerical('int', 5, 20),
        'min_child_weight': Numerical('int', 1, 10),
        'gamma': Numerical('float', 0.01, 0.6),
        'subsample': Numerical('float', 0.05, 0.95),
        'colsample_bytree': Numerical('float', 0.05, 0.95),
        'learning_rate': Numerical('float', 0.001, 0.1, is_log_scale=False)
    },

    MLPClassifier: {
        'alpha': Numerical('float', 1e-9, 1e-1, is_log_scale=False)
    }
}

def get_datasets(name: str) -> data.Dataset:
    try:
        return NAME_TO_DATASET[name]
    except KeyError:
        raise ValueError(f' Dataset "{name}" do not support')

def get_estimator(name: str) -> SVC | XGBClassifier | MLPClassifier:
    if name == 'svc':
        return partial(SVC, max_iter=1000)
    elif name == 'xgb':
        return partial(XGBClassifier, n_jobs=1)
    elif name == 'mlp':
        return MLPClassifier
    raise ValueError(f'Estimator "{name}" do not support')


class TestsProblem(Problem):
    def __init__(self, database: str, method: str):
        super(TestsProblem, self).__init__()

        dataset_class = get_datasets(database)

        instance = dataset_class()
        self.dataset = instance.load_dataset()

        self.estimator = get_estimator(method)
        if isinstance(self.estimator, partial):
            self.estimator = self.estimator.func
        self.metric = DATASET_TO_METRIC[dataset_class]
        self.hyperparams = METHOD_TO_HYPERPARAMS[self.estimator]

        float_hyperparams, discrete_hyperparams = self._split_hyperparams()


        self._searchsBuilder()

        self.number_of_float_variables = len(float_hyperparams)
        self.number_of_discrete_variables = len(discrete_hyperparams)
        self.dimension = len(float_hyperparams) + len(discrete_hyperparams)
        self.number_of_objectives = 1

        self.float_variables_types, self.is_log_float = [], []


        for name, p in float_hyperparams.items():
            self.float_variable_names.append(name)
            self.float_variables_types.append(p.type)
            self.lower_bound_of_float_variables.append(np.log(p.min_value) if p.is_log_scale else p.min_value)
            self.upper_bound_of_float_variables.append(np.log(p.max_value) if p.is_log_scale else p.max_value)
            self.is_log_float.append(p.is_log_scale)

        for name, p in discrete_hyperparams.items():
            self.discrete_variable_names.append(name)
            if isinstance(p, Numerical):
                assert type == 'int', 'Type must be int'
                assert not p.is_log_scale, 'Log must be off'
                self.discrete_variable_values.append([str(x) for x in range(p.min_value, p.max_value + 1)])
            elif isinstance(p, Categorial):
                self.discrete_variable_values.append(p.values)


    def _split_hyperparams(self):
        floats, discretes = {}, {}
        for name, x in self.hyperparams.items():
            if self.is_discrete_hyperparam(x):
                discretes[name] = x
            else:
                floats[name] = x
        return floats, discretes

    @staticmethod
    def is_discrete_hyperparam(p: Hyperparameter):
        if isinstance(p, Numerical):
            return (p.type == 'int') and (not p.is_log_scale) and (p.max_value - p.min_value + 1 <= 5)
        return True

    def _searchsBuilder(self):
        self.searcher = iOptSearcher(100, r=3, refine_solution=True, proportion_of_global_iterations=0.75)
        self.searcher.tune(self.estimator, self.hyperparams, self.dataset, self.metric)


    def calculate(self, point: Point, function_value: FunctionValue) -> FunctionValue:
        arguments = self.__get_argument_dict(point)

        custom_point = self.searcher._calculate_metric(arguments)

        function_value.value = -custom_point.value
        return function_value

    def __get_argument_dict(self, point):
        arguments = {}
        for name, type, value, log in zip(self.float_variable_names, self.float_variables_types,
                                          point.float_variables,
                                          self.is_log_float):
            value = np.exp(value) if log else value
            value = int(value + 0.5) if type == 'int' else value
            arguments[name] = value
        if point.discrete_variables is not None:
            for name, value in zip(self.discrete_variable_names, point.discrete_variables):
                arguments[name] = int(value) if value.isnumeric() else value
        return arguments