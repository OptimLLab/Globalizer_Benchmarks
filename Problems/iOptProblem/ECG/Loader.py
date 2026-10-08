import requests
import zipfile
import tarfile
import os
import sys
from pathlib import Path
from tqdm import tqdm


def download_file(url, load_dir):
    try:
        response = requests.get(url, stream=True, allow_redirects=True)
        response.raise_for_status()
        total_size = int(response.headers.get('content-length', 0))

        with open(load_dir, 'wb') as file:
            with tqdm(
                    desc=f"Скачивание {os.path.basename(load_dir)}",
                    total=total_size,
                    unit='B',
                    unit_scale=True,
                    unit_divisor=1024,
            ) as progress_bar:
                for chunk in response.iter_content(chunk_size=8192):
                    if chunk:
                        file.write(chunk)
                        progress_bar.update(len(chunk))

        print(f"Датасет скачан в {load_dir}")
        return True
    except requests.exceptions.RequestException as e:
        print(f"Ошибка при скачивании: {e}")
        return False


def extract_archive(archive_path, extract_to):
    try:
        if archive_path.endswith('.zip'):
            with zipfile.ZipFile(archive_path, 'r') as zip_ref:
                files = zip_ref.namelist()
                with tqdm(
                        desc="Распаковка ZIP архива",
                        total=len(files),
                        unit='файл'
                ) as progress_bar:
                    for file in files:
                        zip_ref.extract(file, extract_to)
                        progress_bar.update(1)

        elif archive_path.endswith('.tar.gz') or archive_path.endswith('.tgz'):
            with tarfile.open(archive_path, 'r:gz') as tar_ref:
                members = tar_ref.getmembers()

                with tqdm(
                        desc="Распаковка TAR.GZ архива",
                        total=len(members),
                        unit='файл'
                ) as progress_bar:
                    for member in members:
                        tar_ref.extract(member, extract_to)
                        progress_bar.update(1)

        elif archive_path.endswith('.tar'):
            with tarfile.open(archive_path, 'r') as tar_ref:
                members = tar_ref.getmembers()
                with tqdm(
                        desc="Распаковка TAR архива",
                        total=len(members),
                        unit='файл'
                ) as progress_bar:
                    for member in members:
                        tar_ref.extract(member, extract_to)
                        progress_bar.update(1)
        else:
            print(f"Неподдерживаемый формат архива: {archive_path}")
            return False

        print(f"Архив успешно распакован в: {extract_to}")
        return True

    except Exception as e:
        print(f"Ошибка при распаковке архива: {e}")
        return False


def get_direct_download_link(yandex_url):
    if 'get?uid' in yandex_url or 'download' in yandex_url:
        return yandex_url
    if '/d/' in yandex_url:
        public_key = yandex_url.split('/d/')[-1].split('?')[0]
        download_url = f"https://cloud-api.yandex.net/v1/disk/public/resources/download?public_key=https://disk.yandex.ru/d/{public_key}"
        try:
            response = requests.get(download_url)
            response.raise_for_status()
            return response.json().get('href')
        except:
            return yandex_url
    return yandex_url


def Load(save_path, datasets_url):
    yandex_url = datasets_url
    dataset_dir = save_path
    download_url = get_direct_download_link(yandex_url)
    archive_name = "downloaded_archive"
    try:
        head_response = requests.head(download_url, allow_redirects=True)
        content_type = head_response.headers.get('content-type', '')
        if 'zip' in content_type:
            archive_name += ".zip"
        elif 'gzip' in content_type or 'tar' in content_type:
            archive_name += ".tar.gz"
        else:
            archive_name += ".zip"
    except:
        archive_name += ".zip"


    archive_path = os.path.join(save_path, archive_name)

    print(f"Загрузка архива...")
    if not download_file(download_url, archive_path):
        return

    print(f"Распаковку архива...")
    extract_to = os.path.join(os.getcwd(), dataset_dir)

    if extract_archive(archive_path, extract_to):
        print(f"Датасет подготовлен: {extract_to}")
        os.remove(archive_path)
        print(f"Архив {archive_name} удален")
    else:
        print("Произошла ошибка при распаковке архива")


