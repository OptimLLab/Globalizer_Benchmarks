import optuna
import warnings
warnings.filterwarnings('ignore', category=UserWarning)

from ClassificationScripts.dataset import ECGDataset
from ClassificationScripts.model import MobileNetV3Small1D
from problem import Problem
from sklearn.model_selection import train_test_split
import numpy as np
from torch.utils.data import DataLoader
from pathlib import Path
from datetime import datetime
from sklearn.metrics import f1_score
import torch
import torch.nn as nn

def train(model, train_loader, val_loader, epochs=10, lr=1e-3, gpu_id=0, patience=100):
    device = torch.device(f"cuda:{gpu_id}")
    model = model.to(device)

    optimizer = torch.optim.Adam(model.parameters(), lr=lr)
    criterion = nn.CrossEntropyLoss()

    train_losses = []
    val_f1_scores = []

    best_val_f1 = 0
    patience_counter = 0

    for epoch in range(epochs):
        model.train()
        total_loss = 0
        for X_batch, y_batch in train_loader:
            X_batch = X_batch.to(device)
            y_batch = y_batch.to(device)

            optimizer.zero_grad()
            output = model(X_batch)
            loss = criterion(output, y_batch)
            loss.backward()
            optimizer.step()
            total_loss += loss.item()

        avg_loss = total_loss / len(train_loader)
        train_losses.append(avg_loss)

        val_f1 = evaluate(model, val_loader, device)
        val_f1_scores.append(val_f1)

        if val_f1 > best_val_f1:
            best_val_f1 = val_f1
            patience_counter = 0
        else:
            patience_counter += 1

        if patience_counter >= patience:
            print(f"Early stopping at epoch {epoch + 1}")
            break

    return best_val_f1


def evaluate(model, data_loader, device, f1_average='macro'):
    model.eval()
    all_preds = []
    all_labels = []

    with torch.no_grad():
        for X_batch, y_batch in data_loader:
            X_batch = X_batch.to(device)
            y_batch = y_batch.to(device)
            output = model(X_batch)
            preds = output.argmax(dim=1)

            all_preds.extend(preds.cpu().numpy())
            all_labels.extend(y_batch.cpu().numpy())

    return f1_score(all_labels, all_preds, average=f1_average)

def prepare_data():
    X = []
    y = np.load(Path("../datasets/ECGClassification/y.npy"))

    for i in range(len(y)):
        X.append(np.load(Path("../datasets/ECGClassification/X", str(i) + ".npy")))

    X = np.array(X)  # (N, 5000, 12)
    X = X[:, :, 0:2]
    y = np.array(y)  # (N,)
    return X, y

def save_trial_callback(study, trial):
    """Сохраняет информацию о завершенном испытании и текущем лучшем результате."""
    with open("classification_trials_log.txt", "a", encoding="utf-8") as f:
        # Получаем лучший результат на текущий момент
        best_trial = study.best_trial
        best_value = best_trial.value
        best_params = best_trial.params
        
        # Записываем информацию о текущем испытании и лучшем результате
        f.write("=" * 60 + "\n")
        f.write(f"Trial {trial.number}:\n")
        f.write(f"  Value: {trial.value}\n")
        f.write(f"  Params: {trial.params}\n")
        f.write(f"  State: {trial.state}\n")
        f.write(f"  Duration: {trial.duration}\n")
        f.write("-" * 60 + "\n")
        f.write(f"Best so far (Trial {best_trial.number}):\n")
        f.write(f"  Best Value: {best_value}\n")
        f.write(f"  Best Params: {best_params}\n")
        f.write("=" * 60 + "\n\n")
            
class OptunaECGClassification(Problem):
    def __init__(self, n_trials: int = 100, timeout: int = None):
        super().__init__()

        # Загрузка данных
        X, y = prepare_data()
        X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, stratify=y)

        X_train = np.moveaxis(X_train, [1], [2])
        X_test = np.moveaxis(X_test, [1], [2])

        train_dataset = ECGDataset(X_train, y_train)
        test_dataset = ECGDataset(X_test, y_test)

        self.train_loader = DataLoader(train_dataset, batch_size=32, shuffle=True)
        self.test_loader = DataLoader(test_dataset, batch_size=32)


        self.dimension = 2
        self.number_of_float_variables = 2
        self.number_of_objectives = 1
        self.number_of_constraints = 0


        self.float_variable_names = np.array(["P parameter", "Features parameter"], dtype=str)
        self.lower_bound_of_float_variables = [0.0, 80.0]
        self.upper_bound_of_float_variables = [1.0, 200.0]

        # Параметры для Optuna
        self.n_trials = n_trials
        self.timeout = timeout
        self.study = None
        self.best_params = None
        self.best_value = None

    def objective(self, trial: optuna.Trial) -> float:
        """Целевая функция для Optuna"""
        # Предлагаем значения гиперпараметров

        p = trial.suggest_float('P_Dropout',
                                 self.lower_bound_of_float_variables[0],
                                 self.upper_bound_of_float_variables[0])

        f = trial.suggest_float('features_count',
                                        self.lower_bound_of_float_variables[1],
                                        self.upper_bound_of_float_variables[1])

        model = MobileNetV3Small1D(in_channels=2, num_classes=3, p=p, o_features=int(f))
        acc = train(model, self.train_loader, self.test_loader, epochs=100, lr=1e-3)

        #with open('optuna_class_log.txt', 'a', encoding='utf-8') as file:
        #    file.write(f'{acc} - {p} - {f}')

        return -acc




    def optimize_with_optuna(self, show_plots: bool = True) -> None:
        # Создаем исследование
        self.study = optuna.create_study(
            study_name='ecg_classification_hyperparameter_optimization',
            direction='minimize',  # Минимизируем 1 - f1_macro
            sampler=optuna.samplers.TPESampler(seed=42),  # Детерминированный семплинг
            pruner=optuna.pruners.MedianPruner(),  # Отсечение бесперспективных trials
            storage=None  # Можно указать БД для сохранения: 'sqlite:///svm_study.db'
        )

        # Запускаем оптимизацию
        self.study.optimize(
            self.objective,
            n_trials=self.n_trials,
            timeout=self.timeout,
            show_progress_bar=True,
            callbacks=[save_trial_callback]
        )

        # Сохраняем лучшие результаты
        self.best_params = self.study.best_params
        self.best_value = self.study.best_value


        print(f"Лучшее значение целевой функции : {self.best_value:.4f}")

        print("\nЛучшие гиперпараметры:")
        for param, value in self.best_params.items():
            if param == 'P_Dropout':
                print(f"  p = {value}")
            elif param == 'Features count':
                print(f"  f = {value}")
            else:
                print(f"  {param}: {value}")



if __name__ == '__main__':

    #with open('optuna_class_log.txt', 'w', encoding='utf-8') as file:
    #    file.write('Логи\n')

    with open("classification_trials_log.txt", "a", encoding="utf-8") as f:
        current_datetime = datetime.now()

        # Записываем информацию о текущем испытании и лучшем результате
        f.write("=" * 60 + "\n")
        f.write(f"Start {current_datetime}:\n")
        f.write("=" * 60 + "\n\n")

    # Создаем оптимизатор
    svm_optimizer = OptunaECGClassification(
        n_trials=320,  # Количество испытаний
        timeout=10*3600  # Максимальное время в секундах (1 час)
    )



    # Выполняем оптимизацию
    svm_optimizer.optimize_with_optuna(show_plots=True)

    with open("classification_trials_log.txt", "a", encoding="utf-8") as f:
        current_datetime = datetime.now()

        # Записываем информацию о текущем испытании и лучшем результате
        f.write("=" * 60 + "\n")
        f.write(f"Finish {current_datetime}:\n")
        f.write("=" * 60 + "\n\n")

