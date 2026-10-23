# Бенчмарк оптимизации гиперпараметров ML-моделей

Проект представляет собой **бенчмарк для задач глобальной оптимизации гиперпараметров** моделей машинного обучения. Поиск гиперпараметров формулируется как задача оптимизации, совместимая с фреймворками глобальной оптимизации (например, iOpt), и включает 14 табличных датасетов × 3 семейства моделей с непрерывными, целочисленными и категориальными переменными поиска.

---

## Содержание

- [Обзор](#обзор)
- [Структура проекта](#структура-проекта)
- [Датасеты](#датасеты)
- [Модели и гиперпараметры](#модели-и-гиперпараметры)
- [Метрики](#метрики)
- [Пример использования](#пример-использования)
- [Как это работает](#как-это-работает)
- [Расширение бенчмарка](#расширение-бенчмарка)
- [Известные ограничения](#известные-ограничения)
- [Зависимости](#зависимости)

---

## Обзор

Каждый вызов `TestsProblem.calculate(point, function_value)` обучает модель с заданными гиперпараметрами и оценивает её через кросс-валидацию. Цель оптимизатора — **максимизировать качество модели** (accuracy или F1-score).

Задача предоставляется через интерфейс `Problem` / `GlobalizerProblem`, поэтому её можно подключить к любому глобальному оптимизатору, поддерживающему этот контракт.

---

## Структура проекта

```text
.
TestsProblem.py        # Постановка задачи оптимизации (класс TestsProblem)
scripts/
   ├── hyperparams.py         # Типы гиперпараметров: Numerical, Categorial
   ├── interface.py           # Датакласс Point + абстрактный Searcher
   ├── iOptSearcher.py        # Реализация Searcher на базе iOpt
   ├── metrics.py             # Классы метрик + маппинг DATASET_TO_METRIC
   └── data/                  # Классы датасетов (Balance, Digits, Zoo, ...)
         ├── loader.py        # Загрузка и парсинг датасетов (TextParser, ArffParser)
         └── datasets/        # Скачанные файлы датасетов (создаётся при первом запуске)
```

---

## Установка

```bash
pip install numpy scikit-learn xgboost arff requests
```

Датасеты скачиваются автоматически при первом запуске с Яндекс.Диска (см. блок `__main__` в `loader.py`) и распаковываются в `scripts/datasets/`.

---

## Датасеты

14 табличных датасетов из UCI / ARFF / scikit-learn:

| Ключ | Датасет | Тип задачи | Метрика |
|---|---|---|---|
| `balance` | Balance Scale | многоклассовая | F1 (macro) |
| `bank-marketing` | Bank Marketing | бинарная | Accuracy |
| `banknote` | Banknote Authentication | бинарная | F1 (binary) |
| `breast-cancer` | Breast Cancer (sklearn) | бинарная | F1 (binary) |
| `car-evaluation` | Car Evaluation | многоклассовая | F1 (macro) |
| `cnae9` | CNAE-9 | многоклассовая | Accuracy |
| `credit-approval` | Credit Approval | бинарная | F1 (binary) |
| `digits` | Digits (sklearn) | многоклассовая | Accuracy |
| `ecoli` | Ecoli | многоклассовая | F1 (macro) |
| `parkinsons` | Parkinsons | бинарная | F1 (binary) |
| `semeion` | Semeion | многоклассовая | Accuracy |
| `statlog-segmentation` | Statlog Segmentation | многоклассовая | Accuracy |
| `wilt` | Wilt | бинарная | F1 (binary) |
| `zoo` | Zoo | многоклассовая | F1 (macro) |

Все метрики используют препроцессинг `StandardScaler` и кросс-валидацию `StratifiedKFold(shuffle=True, random_state=42)`.

---

## Модели и гиперпараметры

Поддерживаются три семейства моделей. Переменные поиска делятся на **непрерывные** (оптимизируются численно, опционально в логарифмической шкале) и **дискретные** (категориальные или малые целочисленные диапазоны).

### SVC (`svc`)

| Гиперпараметр | Тип | Диапазон | Лог-шкала |
|---|---|---|---|
| `gamma` | float | `1e-9 … 1e-6` | ✅ |
| `C` | int | `1 … 1e10` | ✅ |
| `kernel` | категориальный | `poly`, `rbf`, `sigmoid` | — |

### XGBClassifier (`xgb`)

| Гиперпараметр | Тип | Диапазон | Лог-шкала |
|---|---|---|---|
| `n_estimators` | int | `10 … 200` | — |
| `max_depth` | int | `5 … 20` | — |
| `min_child_weight` | int | `1 … 10` | — |
| `gamma` | float | `0.01 … 0.6` | — |
| `subsample` | float | `0.05 … 0.95` | — |
| `colsample_bytree` | float | `0.05 … 0.95` | — |
| `learning_rate` | float | `0.001 … 0.1` | ✅ |

### MLPClassifier (`mlp`)

| Гиперпараметр | Тип | Диапазон | Лог-шкала |
|---|---|---|---|
| `hidden_layer_sizes` | int | `2 … 150` | — |
| `activation` | категориальный | `identity`, `logistic`, `tanh`, `relu` | — |
| `solver` | категориальный | `lbfgs`, `sgd`, `adam` | — |
| `alpha` | float | `1e-9 … 1e-1` | ✅ |

> **Правило дискретизации:** параметр `Numerical` считается дискретным, только если он `int`, не в лог-шкале и содержит не более 5 значений. В противном случае он оптимизируется как непрерывная переменная (с округлением до `int` после оценки). Параметры `Categorial` всегда дискретны.

---

## Метрики

Определены в `metrics.py`:

- **`Accuracy`** — доля правильных ответов.
- **`F1`** — F1-score с настраиваемым `average` (`binary` / `macro` / `micro`).

Обе вычисляются через `cross_val_score` на пайплайне с препроцессингом.

---

## Пример использования

```python
from TestsProblem import TestsProblem
from Globalizer_problem import GlobalizerProblem

# 1. Создаём задачу: SVC на датасете Balance
problem = TestsProblem(database='balance', method='svc')

# 2. Оборачиваем для глобального оптимизатора
globalizer_problem = GlobalizerProblem(problem)

# 3. Изучаем пространство поиска
discrete_params = globalizer_problem.get_discrete_params()
start_point = globalizer_problem.get_start_value()

# 4. Оцениваем точку: gamma=1e-7, C=1e2, kernel='rbf'
result = globalizer_problem.calculate([1e-7, 1e2], ['rbf'])
print(result)
```

Непрерывные аргументы соответствуют `gamma` и `C` (в указанном порядке), дискретный — `kernel`.

---

## Как это работает

```text
Оптимизатор (Globalizer)
        │
        ▼
GlobalizerProblem(TestsProblem)
        │  get_discrete_params() / get_start_value()
        ▼
TestsProblem.calculate(point, function_value)
        │
        ▼
__get_argument_dict(point)  ──►  словарь гиперпараметров
        │                       (exp для лог-шкалы, round для int)
        ▼
iOptSearcher._calculate_metric(arguments)
        │
        ├──► estimator(**arguments)      # SVC / XGB / MLP
        ├──► metric(model, dataset)      # cross_val_score
        └──► Point(timepoint, value, params)
        │
        ▼
function_value.value = -Point.value      # минимизация отрицания метрики
```

**Ключевые шаги:**

1. `TestsProblem.__init__` загружает датасет, выбирает оценщик и его набор гиперпараметров, делит их на непрерывные/дискретные группы.
2. Метаданные задачи (`dimension`, границы, типы, имена переменных) заполняются для оптимизатора.
3. При оценке точка оптимизатора декодируется обратно в аргументы оценщика.
4. Модель обучается и оценивается; возвращается **отрицательная** метрика, чтобы минимизатор максимизировал качество.


---


## Зависимости

| Пакет | Назначение |
|---|---|
| `numpy` | Операции с массивами |
| `scikit-learn` | Модели, метрики, кросс-валидация, препроцессинг, sklearn-датасеты |
| `xgboost` | `XGBClassifier` |
| `arff` | Парсинг ARFF-датасетов |
| `requests` | Скачивание датасетов |

**Python:** 3.12+.