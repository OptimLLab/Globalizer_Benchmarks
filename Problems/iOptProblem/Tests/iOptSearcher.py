import numpy as np


from Tests.hyperparams import Hyperparameter, Numerical, Categorial

from Tests.interface import Searcher, Point



class iOptSearcher(Searcher):
    def __init__(self, max_iter, *, is_deterministic=True, **kwargs):
        super().__init__(framework_name='iOpt',
                         max_iter=max_iter,
                         is_deterministic=is_deterministic)

        self.kwargs = kwargs



    def _get_searcher_params(self):
        return self.kwargs.copy()

    def split_hyperparams(self):
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
