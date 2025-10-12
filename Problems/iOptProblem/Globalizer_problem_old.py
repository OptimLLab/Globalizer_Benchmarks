from typing import List

from rastrigin import Rastrigin
from trial import Point
from trial import FunctionValue
from problem import Problem

import numpy as np

class GlobalizerProblem:

    def __init__(self):
        self.dimension = 2
        self.problem: Problem = Rastrigin(dimension=self.dimension)
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
        res = self.problem.lower_bound_of_float_variables.tolist()
        return res

    def get_upper_bounds(self) -> List[float]:
        res = self.problem.upper_bound_of_float_variables.tolist()
        return res

    def set_dimension(self, dimension: int):
        self.dimension = dimension
        self.problem: Problem = Rastrigin(dimension=dimension)
        self.current_coordinate = [0] * dimension
        self.point: Point = Point(float_variables=np.array(self.current_coordinate), discrete_variables=None)
        self.function_value: FunctionValue = FunctionValue()
        self.result = self.problem.calculate(self.point, self.function_value)
        self.result_value = float(self.result.value)

if __name__ == "__main__":
    problem = GlobalizerProblem()
    bound = problem.get_lower_bounds()
    print(bound)
    result = problem.calculate([0.5, 0.5], 0)
    print(result)
    resultAllFunction = problem.calculate_all_functionals([0.5, 0.5])
    print(resultAllFunction)