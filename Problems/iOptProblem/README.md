# DataDownloader

Инструмент командной строки для загрузки датасетов в локальную или глобальную (проектную) директорию.

## Описание

`DataDownloader.py` — это скрипт на Python, который автоматизирует загрузку датасетов для задач ЭКГ (электрокардиография). Он определяет, куда сохранять данные (локально или в общую папку проекта `Globalizer`), создаёт необходимую структуру директорий и запускает загрузчик `Loader`.

Скрипт поддерживает два режима:

- **local** — данные сохраняются в папку `./datasets` в текущей рабочей директорией;
- **global** — данные сохраняются в подпапку `_bin/datasets` внутри корневой папки проекта `Globalizer` (поиск папки выполняется автоматически вверх по дереву).

## Поддерживаемые задачи

| Класс задачи | Ссылка на датасет |
| :--- | :--- |
| `ECGClassification` | [Яндекс.Диск](https://disk.yandex.ru/d/2G-w_EXbKFb7QA) |
| `ECGSegmentation` | [Яндекс.Диск](https://disk.yandex.ru/d/_-5_exJ9XUWAeA) |
| `ECGGeneration` | — (пока не задана) |
| `ECGPacemakerClassification` | — (пока не задана) |

## Требования

- **Python** 3.12+
- Модуль `Loader` (должен лежать рядом со скриптом)
- Стандартные библиотеки: `argparse`, `pathlib`, `typing`, `os`

Установка зависимостей (если появятся):

```bash
pip install -r requirements.txt
```

## Запуск

### Синтаксис

```bash
python DataDownloader.py --source {global,local} --class {ECGClassification,ECGSegmentation,ECGGeneration,ECGPacemakerClassification} [--verbose]
```

### Обязательные аргументы

| Аргумент | Сокращение | Значения | Описание |
| :--- | :--- | :--- | :--- |
| `--source` | `-s` | `global`, `local` | Куда загружать датасет |
| `--class` | `-c` | `ECGClassification`, `ECGSegmentation`, `ECGGeneration`, `ECGPacemakerClassification` | Класс задачи |


### Примеры запуска

```bash
# Загрузка датасета для классификации ЭКГ локально
python DataDownloader.py --source local --class ECGClassification

# Загрузка датасета для сегментации в глобальную папку проекта
python DataDownloader.py -s global -c ECGSegmentation

```

## Структура директорий

### Режим `local`

```
<текущая директория>/
└── datasets/
    └── ECGClassification/
        └── ... (файлы датасета)
```

### Режим `global`

Скрипт ищет папку `Globalizer` вверх по дереву (до 6 уровней), затем создаёт структуру:

```
<путь_до_Globalizer>/
└── _bin/
    └── datasets/
        └── ECGClassification/
            └── ... (файлы датасета)
```

## Основной функционал

1. **Парсинг аргументов** — `argparse` разбирает флаги `--source` и `--class`, проверяет допустимые значения через `choices`.
2. **Определение пути**:
   - для `local` — используется `./datasets`;
   - для `global` — функция `find_project_folder("Globalizer")` ищет корневую папку проекта вверх по дереву, затем `contains_build_folder()` проверяет наличие подпапки `_bin`.
3. **Создание директорий** — если папки `datasets` и подпапки под конкретный класс задачи не существует, они создаются.
4. **Загрузка** — вызывается `Loader.Load(path, task_links[task_class])`, который скачивает датасет по ссылке и сохраняет его в указанную папку.

### Ключевые функции

| Функция | Назначение |
| :--- | :--- |
| `find_project_folder(project_folder, max_level=6)` | Ищет папку с заданным именем вверх по дереву (не более `max_level` уровней) |
| `contains_build_folder(path)` | Проверяет наличие подпапки `_bin` и возвращает её путь или `None` |
| `parse_args(default_args=None)` | Разбирает аргументы командной строки (можно передать список вручную для тестов) |

## Возможные ошибки

| Сообщение | Причина | Решение |
| :--- | :--- | :--- |
| `FileNotFoundError: Папка не найдена! Возможно вы не собрали проект.` | В режиме `global` не найдена папка `Globalizer` или подпапка `_bin` | Проверьте, что проект собран и папка `Globalizer` есть в дереве каталогов |
| `ModuleNotFoundError: No module named 'Loader'` | Отсутствует файл `Loader.py` | Положите `Loader.py` рядом со скриптом или установите модуль |
| `argument --source/-s: invalid choice` | Передано недопустимое значение | Используйте `global` или `local` |
| `argument --class/-c: invalid choice` | Передано недопустимое значение | Используйте один из поддерживаемых классов задач |

## Расширение

### Добавление нового класса задачи

1. Добавьте название класса в список `choices` аргумента `-c/--class`.
2. Добавьте ссылку на датасет в словарь `task_links`.

```python
task_links = {
    "ECGClassification": "https://disk.yandex.ru/d/2G-w_EXbKFb7QA",
    "ECGSegmentation": "https://disk.yandex.ru/d/_-5_exJ9XUWAeA",
    "ECGGeneration": "---",
    "ECGPacemakerClassification": "---",
    "NewTask": "https://example.com/dataset",  # ← новая задача
}
```

### Изменение глубины поиска папки проекта

Параметр `max_level` в `find_project_folder()` задаёт, на сколько уровней вверх подниматься (по умолчанию — 6).
