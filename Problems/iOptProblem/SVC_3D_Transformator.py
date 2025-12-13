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
from pathlib import Path
from urllib.parse import urlencode
import requests
import zipfile
import os

def factory_dataset():
    path = Path(__file__).parent    
    x = []
    y = []
    with open(path / 'datasets' / 'transformator_state.csv') as rrrr_file:
        file_reader = csv.reader(rrrr_file, delimiter=",")
        for row in file_reader:
            x_row = []
            for i in range(len(row)-1):
                x_row.append(row[i])
            x.append(x_row)
            y.append(row[len(row)-1])
    return shuffle(np.array(x), np.array(y), random_state=42)


def downloadTransformatorsDataset():
    path = Path(__file__).parent

    base_url = 'https://cloud-api.yandex.net/v1/disk/public/resources/download?'
    public_key = 'https://disk.yandex.ru/d/X_YlA0dJ-OsI8g'

    final_url = base_url + urlencode(dict(public_key=public_key))
    response = requests.get(final_url)
    download_url = response.json()['href']

    download_response = requests.get(download_url)
    with open(path / 'datasets.zip', 'wb') as f:
        f.write(download_response.content)

    with zipfile.ZipFile(path / 'datasets.zip') as f:
        f.extractall(path=path)

    os.remove(path / 'datasets.zip')

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
        self.discrete_variable_values.append(('sigmoid', 'rbf', 'poly'))
        self.cv = StratifiedKFold(shuffle=True, random_state=42)


    def calculate(self, point: Point, function_value: FunctionValue) -> FunctionValue:
        cs, gammas = point.float_variables[0], point.float_variables[1]
        kernel_type = point.discrete_variables[0]
        clf = SVC(C=10 ** cs, gamma=10 ** gammas, kernel=kernel_type)
        function_value.value = 1-cross_val_score(clf, self.x, self.y, cv=self.cv, scoring='f1_macro').mean()
        #print("Arguments: C=\t", 10 ** cs, "\tgamma=\t", 10 ** gammas, "\tkernel=\t",
        #      kernel_type, "\tValue:\t", function_value.value)
        return function_value

    def default_calculate(self)-> float:
        """
        Метод расчёта значения целевой функции со значениями по умолчанию

        """
        clf = SVC()
        value = 1-cross_val_score(clf, self.x, self.y, cv=self.cv, scoring='f1_macro').mean()
        return value
        
if __name__ == '__main__':
    downloadTransformatorsDataset()