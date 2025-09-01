#import numpy as np
#import pandas as pd
#from scipy.integrate import ode, solve_ivp
#import matplotlib.pyplot as plt
#import time


dimension = 6

lower = [-1.]*dimension
upper = [1.]*dimension

def objective(x):
    result = 0
    for i in range(dimension):
        result += x[i]**2

    return result
