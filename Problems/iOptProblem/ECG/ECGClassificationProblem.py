import warnings
warnings.filterwarnings('ignore', category=UserWarning)
from ClassificationScripts.dataset import ECGDataset
from ClassificationScripts.model import MobileNetV3Small1D
from trial import Point
from trial import FunctionValue
from problem import Problem
from sklearn.model_selection import train_test_split
import numpy as np
from torch.utils.data import Dataset, DataLoader
from pathlib import Path
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
    best_model_state = None
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
            print(f"Early stopping at epoch {epoch+1}")
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
    y = np.load(Path("ECG/datasets/ECGClassification/y.npy"))

    for i in range(len(y)):
        X.append(np.load(Path("ECG/datasets/ECGClassification/X", str(i) + ".npy")))

    X = np.array(X)  # (N, 5000, 12)
    X = X[:, :, 0:2]
    y = np.array(y)  # (N,)
    return X, y

class ECGClassificationProblem(Problem):
    def __init__(self, dimension: int, ProcRank:int=0):
        super(ECGClassificationProblem, self).__init__()
        print("Init ", dimension)
        self.dimension = dimension
        self.number_of_float_variables = dimension
        self.number_of_objectives = 1
        self.number_of_constraints = 0
        print(dimension, ProcRank)

        X, y = prepare_data()

        X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, stratify=y)

        X_train = np.moveaxis(X_train, [1], [2])
        X_test = np.moveaxis(X_test, [1], [2])

        train_dataset = ECGDataset(X_train, y_train)
        test_dataset = ECGDataset(X_test, y_test)

        self.train_loader = DataLoader(train_dataset, batch_size=32, shuffle=True)
        self.test_loader = DataLoader(test_dataset, batch_size=32)


        self.float_variable_names = np.array(["P parameter", "Features parameter"], dtype=str)
        self.lower_bound_of_float_variables = [0.0, 80.0]
        self.upper_bound_of_float_variables = [1.0, 200.0]

        GPU_count = torch.cuda.device_count()
        print(f"Доступно GPU: {GPU_count}")
        for i in range(GPU_count):
            print(f"GPU {i}: {torch.cuda.get_device_name(i)}")

        self.gpu_id = 0
        if GPU_count != 0:
            self.gpu_id = ProcRank % GPU_count

            self.device = torch.device(f"cuda:{self.gpu_id}")

            torch.device(f"{self.device}")

        else:
            self.device = "cpu"

        print(f"gpu_id = {self.gpu_id}")


    def calculate(self, point: Point, function_value: FunctionValue) -> FunctionValue:
        p, f = point.float_variables[0], point.float_variables[1]

        model = MobileNetV3Small1D(in_channels=2, num_classes=3, p=p, o_features=int(f))
        macro_F1 = train(model, self.train_loader, self.test_loader, epochs=100, lr=1e-3, gpu_id=self.gpu_id)

        function_value.value = -macro_F1

        print('p ' + f"{p:.9f}"+ '\tfeatures ' + f"{f:.9f}" + "\tvalue " + f"{function_value.value:.9f}", flush=True)
        return function_value
        
if __name__ == '__main__':
    problem_ecg_class = ECGClassificationProblem(2)