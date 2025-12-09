from ClassificationScripts.dataset import ECGDataset
from ClassificationScripts.model import MobileNetV3Small1D
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

def train(model, train_loader, test_loader, epochs=10, lr=1e-3):
    device = torch.device("cuda")# if torch.cuda.is_available() else "cpu")
    model = model.to(device)
    optimizer = torch.optim.Adam(model.parameters(), lr=lr)
    criterion = nn.CrossEntropyLoss()

    train_losses = []
    test_accuracies = []
    acc = 0

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
        #print(f"Epoch {epoch + 1}/{epochs}, Loss: {avg_loss:.4f}")

        # Validation
        model.eval()
        correct = 0
        total = 0
        with torch.no_grad():
            for X_batch, y_batch in test_loader:
                X_batch = X_batch.to(device)
                y_batch = y_batch.to(device)
                output = model(X_batch)
                preds = output.argmax(dim=1)
                correct += (preds == y_batch).sum().item()
                total += y_batch.size(0)

        accuracy = correct / total
        acc = accuracy
        test_accuracies.append(accuracy)
        print(f"Test Accuracy: {accuracy:.4f}")
    return acc

def prepare_data():
    folder_name = "ecg_ludb_records"
    #os.makedirs(folder_name, exist_ok=True)
    record_names_path = os.path.abspath(folder_name)
    if not os.path.exists(record_names_path):
        wfdb.dl_database('ludb', record_names_path, keep_subdirs=False)
    dataset_raw = dict()
    for i in range(1, 201):
        signalpath = os.path.join(record_names_path, str(i))
        print(signalpath)
        dataset_raw[i] = wfdb.rdrecord(signalpath)
    rhythm_raw = dict()

    for patient_id, patient_record in dataset_raw.items():
        rhythm_str = patient_record.comments[3]
        if rhythm_str == "Rhythm: Sinus rhythm.":
            rhythm_raw[patient_id] = 0
        elif rhythm_str.startswith("Rhythm: Sinus"):
            rhythm_raw[patient_id] = 1
        else:
            rhythm_raw[patient_id] = 2
        print(f"{rhythm_str} = {rhythm_raw[patient_id]}")  # DEBUG

    # Подготовка данных
    X = []
    y = []

    for patient_id, patient_record in dataset_raw.items():
        X.append(patient_record.p_signal)
        y.append(rhythm_raw[patient_id])

    X = np.array(X)  # (N, 5000, 12)
    X = X[:, :, 0:2]
    y = np.array(y)  # (N,)
    return X, y

class ECGClassificationProblem(Problem):
    def __init__(self, dimension: int):
        super(ECGClassificationProblem, self).__init__()
        self.dimension = dimension
        self.number_of_float_variables = dimension
        self.number_of_objectives = 1
        self.number_of_constraints = 0


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




    def calculate(self, point: Point, function_value: FunctionValue) -> FunctionValue:
        p, f = point.float_variables[0], point.float_variables[1]

        model = MobileNetV3Small1D(in_channels=2, num_classes=3, p=p, o_features=int(f))
        acc = train(model, self.train_loader, self.test_loader, epochs=100, lr=1e-3)

        print('p ' + f"{p:.9f}")
        print('features ' + f"{f:.9f}")
        function_value.value = -acc
        print(-acc)
        return function_value