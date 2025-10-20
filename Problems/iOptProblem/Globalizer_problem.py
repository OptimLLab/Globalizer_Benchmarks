from typing import List
import Cardio2D
from rastrigin import Rastrigin
from trial import Point
from trial import FunctionValue
from problem import Problem
import SVC_Fixed_Kernel
from sklearn.datasets import load_breast_cancer
from sklearn.utils import shuffle
import requests
from tqdm import tqdm
import os
from pathlib import Path
import hashlib
import numpy as np
import shutil
from ECGClassificationProblem import ECGClassificationProblem


def _get_hash(path: Path) -> str:
    file_hash = hashlib.sha256()
    with open(path, "rb") as f:
        while chunk := f.read(8192):
            file_hash.update(chunk)
    return file_hash.hexdigest()

def download(path: Path, public_key: str) -> None:
    url = "https://cloud-api.yandex.net/v1/disk/public/resources"
    params = {"public_key": f"https://disk.yandex.ru/d/{public_key}"}

    response = requests.get(url, params=params).json()
    download_url = response["file"]
    file_size = response["size"]
    sha256 = response["sha256"]

    response = requests.get(download_url, stream=True)

    if path.is_file() and os.path.getsize(path) == file_size:
        print(f"File already downloaded: {path}")
        if _get_hash(path) == sha256:
            return

    with tqdm(total=file_size, unit="B", unit_scale=True) as progress_bar:
        with open(path, "wb") as f:
            for data in response.iter_content(1024):
                progress_bar.update(len(data))
                f.write(data)

class GlobalizerProblem:

    def __init__(self, target_problem: Problem):
        self.dimension = target_problem.dimension
        self.problem: Problem = target_problem #Rastrigin(dimension=self.dimension)
        self.current_coordinate = List[float]
        self.point: Point = Point(float_variables=np.array(self.current_coordinate), discrete_variables=None)
        self.function_value: FunctionValue = FunctionValue()
        self.result : FunctionValue = FunctionValue()
        self.result_value = 0.0

        self.set_dimension(self.dimension)

    def calculate(self, coordinate: List[float], fNumber: int = 0) -> float:
        self.current_coordinate = coordinate
        self.point.float_variables = np.array(self.current_coordinate)
        self.function_value.functionID = fNumber
        self.result = self.problem.calculate(self.point, self.function_value)
        self.result_value = float(self.result.value)
        return self.result_value

    def calculate_all_functionals(self, coordinate: List[float]) -> List[float]:
        self.current_coordinate = coordinate
        self.point.float_variables = np.array(self.current_coordinate)
        self.function_value.functionID = 0
        self.result = self.problem.calculate(self.point, self.function_value)
        self.result_value = float(self.result.value)
        return [self.result_value]

    def get_dimension(self) -> int:
        return self.dimension

    def get_number_of_functions(self) -> int:
        return self.get_number_of_criterions() + self.get_number_of_constraints()

    def get_number_of_constraints(self) -> int:
        return 0

    def get_number_of_criterions(self) -> int:
        return 1

    def get_start_y(self) -> List[float]:
        return [0.5] * self.dimension

    def get_start_value(self) -> List[float]:
        return [self.calculate(self.get_start_y(), 0)]

    def get_lower_bounds(self) -> List[float]:
        res = self.problem.lower_bound_of_float_variables
        return res

    def get_upper_bounds(self) -> List[float]:
        res = self.problem.upper_bound_of_float_variables
        return res

    def set_dimension(self, dimension: int):
        self.dimension = dimension
        #self.problem: Problem = Rastrigin(dimension=dimension)
        self.current_coordinate = [0] * dimension
        self.point: Point = Point(float_variables=np.array(self.current_coordinate), discrete_variables=None)
        self.function_value: FunctionValue = FunctionValue()
        self.result = self.problem.calculate(self.point, self.function_value)
        self.result_value = float(self.result.value)


def load_breast_cancer_data():
    dataset = load_breast_cancer()
    x_raw, y_raw = dataset['data'], dataset['target']
    inputs, outputs = shuffle(x_raw, y_raw ^ 1, random_state=42)
    return inputs, outputs

def test_ecg_classification_problem():

    p_value_bound = {'low': 0.0, 'up': 1.0}
    f_value_bound = {'low': 80.0, 'up': 200.0}
    problem_ecg_class = ECGClassificationProblem(p_value_bound, f_value_bound)

    problem = GlobalizerProblem(problem_ecg_class)

    bound = problem.get_lower_bounds()
    print(bound)
    result = problem.calculate([0.5, 100], 0)
    print(result)
    result_all_function = problem.calculate_all_functionals([0.5, 100])
    print(result_all_function)

def test_svc1d_problem():
    x, y = load_breast_cancer_data()
    kernel_coefficient = -5
    regularization_value_bound = {'low': 1, 'up': 6}
    problem_svc = SVC_Fixed_Kernel.SVC_Fixed_Kernel(x, y, kernel_coefficient, regularization_value_bound)

    problem = GlobalizerProblem(problem_svc)

    bound = problem.get_lower_bounds()
    print(bound)
    result = problem.calculate([2], 0)
    print(result)
    result_all_function = problem.calculate_all_functionals([2])
    print(result_all_function)

def test_segmentation_problem():
    if not os.path.exists('data'):
        path = Path('data.zip')
        download(path, 'Oqxcid6uX58kYQ')
        shutil.unpack_archive('data.zip', 'data', format="zip")
        os.remove('data.zip')

    p_value_bound = {'low': 0.0, 'up': 1.0}
    q_value_bound = {'low': 1.0, 'up': 1.6}
    problem_nn = Cardio2D.Cardio2D(p_value_bound, q_value_bound)

    problem = GlobalizerProblem(problem_nn)

    bound = problem.get_lower_bounds()
    print(bound)
    result = problem.calculate([0.5, 1.3], 0)
    print(result)
    result_all_function = problem.calculate_all_functionals([0.5, 1.3])
    print(result_all_function)

def test_rastrigin():
    r_problem = Rastrigin(5)
    problem = GlobalizerProblem(r_problem)
    bound = problem.get_lower_bounds()
    print(bound)
    result = problem.calculate([0.5] * problem.get_dimension(), 0)
    print(result)
    resultAllFunction = problem.calculate_all_functionals([0.5] * problem.get_dimension())
    print(resultAllFunction)

if __name__ == "__main__":
    test_ecg_classification_problem()
    #test_svc1d_problem()
    #test_segmentation_problem()
    #test_rastrigin()