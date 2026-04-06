import argparse
from pathlib import Path
from typing import Optional
import os
import Loader as Loader

def check_global_build_folder(project_folder: str, target_folder: str, max_level=10) -> Optional[Path]:
    current = Path.cwd()
    i = 0
    while True:
        if i == max_level:
            break
        i += 1

        target_path = current / target_folder
        if target_path.exists() and target_path.is_dir():
            return target_path

        if current.name == project_folder:
            print("Достигнута папка проекта!")
            break

        parent = current.parent
        if parent == current:  # дошли до корня
            print("Достигнут корень системы - папка не найдена!")
            break
        current = parent
    return None

def parse_args(default_args = None):
    # Создаем парсер аргументов
    parser = argparse.ArgumentParser(
        description='Скрипт с выбором локализации загрузки датасета (global/local) и класса задачи',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Примеры:
  %(prog)s --source global --class ECGClassification
  %(prog)s -s local -c ECGSegmentation
  %(prog)s --source global --class ECGGeneration --verbose
        """
    )

    # Добавляем флаг --source
    parser.add_argument(
        '-s', '--source',
        type=str,
        choices=['global', 'local'],
        required=True,
        default='local',
        help='Выберите источник: global или local'
    )

    # Добавляем флаг --class
    parser.add_argument(
        '-c', '--class',
        type=str,
        choices=['ECGClassification', 'ECGSegmentation', 'ECGGeneration'],
        required=True,
        dest='task_class',  # Используем dest чтобы избежать конфликта с keyword 'class'
        default='ECGClassification',
        help='Выберите класс задачи: ECGClassification, ECGSegmentation или ECGGeneration'
    )

    # Дополнительные опциональные аргументы
    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='Подробный вывод информации'
    )

    # Парсим аргументы
    if default_args:
        return parser.parse_args(default_args)
    else :
        return parser.parse_args()

task_links = {
    "ECGClassification" : "https://disk.yandex.ru/d/2G-w_EXbKFb7QA",
    "ECGSegmentation" : "https://disk.yandex.ru/d/_-5_exJ9XUWAeA",
    "ECGGeneration" : "---"
}

if __name__ == '__main__':
    args = parse_args() #(['-s', 'local', '-c', 'ECGClassification', '-v'])

    path = None
    if args.source == 'local':
        path = Path("./datasets")
    if args.source == 'global':
        path = check_global_build_folder("Globalizer", "_bin", 10)
        if path:
            path = Path(os.path.join(path, 'datasets'))
            print(f"Папка найдена: {path}")
        else:
            raise FileNotFoundError("Папка не найдена! Возможно вы не собрали проект. Работа загрузчика остановлена ")

    # Создаём папку под датасеты если она отсутствовала
    if not path.exists():
        os.mkdir(path)

    #Создаём подпапку для датасета
    path = Path(os.path.join(path, args.task_class))
    if not path.exists():
        os.mkdir(path)

    Loader.Load(path, task_links[args.task_class])

