import sys
from PacemakerScripts.Dataset import *
from PacemakerScripts.Model import *
from PacemakerScripts.MetricsCalculator import *
from PacemakerScripts.Trainer import *
import uuid
from typing import List
import warnings
from pathlib import Path
warnings.filterwarnings('ignore')
import json

sys.path.append(str(Path(__file__).resolve().parent.parent.parent))
from trial import Point
from trial import FunctionValue
from problem import Problem
from Globalizer_problem import GlobalizerProblem


class PacemakerClassificationProblem(Problem):
    def __init__(self, dimension: int=8, ProcRank:int=0):
        super(PacemakerClassificationProblem, self).__init__()
        print("Init ", dimension)
        self.dimension = dimension
        self.number_of_float_variables = dimension
        self.number_of_objectives = 1
        self.number_of_constraints = 0
        print(dimension, ProcRank)

        torch.manual_seed(42)
        np.random.seed(42)

        self.base_path = Path("dataset/ECGPacemakerClassification")
        train_dataset = SignalDataset(
            self.base_path, 'train',
            normalization_type='zscore',
            use_differential=False,
            augmentation=True,
            target_length=5000
        )
        val_dataset = SignalDataset(
            self.base_path, 'val',
            normalization_type='zscore',
            use_differential=False,
            augmentation=False,
            target_length=5000
        )
        test_dataset = SignalDataset(
            self.base_path, 'test',
            normalization_type='zscore',
            use_differential=False,
            augmentation=False,
            target_length=5000
        )

        self.class_counts = [len(train_dataset.noecs_files), len(train_dataset.ecs_files)]
        print(f"\nClass counts - NOECS: {self.class_counts[0]}, ECS: {self.class_counts[1]}")

        class_weights = 1.0 / torch.tensor(self.class_counts, dtype=torch.float)
        sample_weights = [class_weights[label] for _, label in train_dataset.files]
        sampler = WeightedRandomSampler(sample_weights, len(sample_weights))

        self.model_save_path = Path("models")
        if not Path.exists(self.model_save_path):
            Path.mkdir(self.model_save_path)

        batch_size = 128

        self.train_loader = DataLoader(train_dataset, batch_size=batch_size, sampler=sampler,
                                  num_workers=0, pin_memory=True)
        self.val_loader = DataLoader(val_dataset, batch_size=batch_size, shuffle=False,
                                num_workers=0, pin_memory=True)
        self.test_loader = DataLoader(test_dataset, batch_size=batch_size, shuffle=False,
                                 num_workers=0, pin_memory=True)


        self.float_variable_names = np.array(['num_blocks', 'growth_rate', 'dropout', 'learning_rate',
                                              'weight_decay', 'loss_alpha', 'loss_gamma', 'kernel_size'], dtype=str)
        self.lower_bound_of_float_variables = [2, 16, 0.2, 0.0001, 0.00005, 0.5, 1.0, 3]
        self.upper_bound_of_float_variables = [4, 64, 0.7, 0.002, 0.001, 0.9, 5.0, 9]


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
        num_blocks, growth_rate, dropout, learning_rate, weight_decay, loss_alpha, loss_gamma, kernel_size =  int(point.float_variables[0]), int(point.float_variables[1]), point.float_variables[2], point.float_variables[3],point.float_variables[4], point.float_variables[5], point.float_variables[6], int(point.float_variables[7])

        model_params = {
            'input_channels': 12,
            'num_blocks': num_blocks,
            'growth_rate': growth_rate,
            'kernel_size': kernel_size,
            'dropout': dropout,
            'use_batch_norm': True,
            'use_se': False
        }


        model = Xception1D(**model_params).to(self.device)

        optimizer = torch.optim.AdamW(model.parameters(), lr=learning_rate, weight_decay=weight_decay)

        criterion = WeightedFocalLoss(
            self.class_counts,
            alpha=loss_alpha,
            gamma=loss_gamma,
            beta=0.999
        ).to(self.device)

        scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(
            optimizer, mode='min', patience=20, factor=0.5
        )

        save_dir = './xception'
        os.makedirs(save_dir, exist_ok=True)
        trainer = Trainer(model, self.device, uuid.uuid4(), save_dir)


        history, best_target_metric = trainer.train(
            self.train_loader, self.val_loader, criterion, optimizer,
            epochs=100,
            patience=20,
            scheduler=scheduler
        )

        function_value.value = -best_target_metric
        self.save_model(model, function_value.value, model_params)

        return function_value

    def save_model(self, model, metric, params_dict):
        files = [f for f in self.model_save_path.iterdir() if f.is_file()]

        for f in files:
            f_metric = float(f.stem)
            if f_metric < metric:
                return

        print("Модель сохранена: ", metric, params_dict)
        local_path = self.model_save_path.joinpath(f"{metric}.pth")
        torch.save(model.state_dict(), local_path)
        with open(self.model_save_path.joinpath(f"{metric}.json"), "w", encoding="utf-8") as f:
            json.dump(params_dict, f, ensure_ascii=False, indent=2)

    def match_by_fields(self, d1, d2, fields):
        return all(d1.get(f) == d2.get(f) for f in fields)

    def finalize(self, coordinate: List[float], discreteCoordinate: List[str]) -> float:
        """
        The process that occurs after the solution search procedure
        return: Calculated values of target metric"""

        print("Проверка тестовой выборки в лучшей точке: ", coordinate, discreteCoordinate)

        num_blocks, growth_rate, dropout, learning_rate, weight_decay, loss_alpha, loss_gamma, kernel_size = int(coordinate[0]), int(coordinate[1]), coordinate[2], coordinate[3], coordinate[4], coordinate[5], coordinate[6], int(coordinate[7])

        model_params = None

        tmp_params_dict = {
            'num_blocks': num_blocks,
            'growth_rate': growth_rate,
            'kernel_size': kernel_size,
            'dropout': dropout
        }

        json_files = list(Path(self.model_save_path).glob("*.json"))
        for json_file in json_files:
            with open(json_file, "r", encoding="utf-8") as f:
                current_params_dict = json.load(f)
                if self.match_by_fields(current_params_dict, tmp_params_dict,
                                        ['num_blocks', 'growth_rate', 'kernel_size', 'dropout']):
                    model_params = current_params_dict
                    metric = float(json_file.stem)
                    break

        if model_params is None:
            print("Среди чекпоинтов не найден целевой!")
            return -1

        target_path = self.model_save_path.joinpath(f"{metric}.pth")
        print("Лучшая модель: ", target_path)

        model = Xception1D(**model_params).to(self.device)
        model.load_state_dict(torch.load(target_path, map_location=self.device))
        model.eval()

        all_preds = []
        all_labels = []

        with torch.no_grad():
            for X_batch, y_batch in self.test_loader:
                X_batch = X_batch.to(self.device)
                y_batch = y_batch.to(self.device)
                output = model(X_batch)
                preds = output.argmax(dim=1)
                all_preds.extend(preds.cpu().numpy())
                all_labels.extend(y_batch.cpu().numpy())

        finish_metric = f1_score(all_labels, all_preds, average='macro')
        print("Значение целевой метрики на валидационной выборке: ", abs(metric))
        print("Значение целевой метрики на тестовой выборке: ", finish_metric)

        return finish_metric

if __name__ == '__main__':
    pacemakerProblem = PacemakerClassificationProblem()
    problem = GlobalizerProblem(pacemakerProblem)
    result = problem.calculate([2, 16, 0.2, 0.0001, 0.00005, 0.5, 1.0, 3], [])
    problem.finalize([2, 16, 0.2, 0.0001, 0.00005, 0.5, 1.0, 3], [])
    print(result)
