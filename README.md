# Malware Scanner

Многопоточная CLI утилита для анализа директории на предмет наличия "вредоносных" файлов.
---

## Принцип работы

Утилита получает на вход директорию для проверки и базу хэшей "вредоносных" файлов.

Далее утилита проверяет каждый файл, высчитывает его хэш и делает вывод о "вредоносности" файла.

После завершения сканирования выводится общая статистика.


## 📦 Зависимости.
- **CMake** ≥ 3.21
- **C++20**
- **gtest**
- **OpenSSL** > 3.0.0
- **rapidcsv**

Рекомендуемый способ установки библиотек — **[vcpkg](https://github.com/microsoft/vcpkg)**.

```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh (или bootstrap-vcpkg.bat для Windows)
./vcpkg integrate install
./vcpkg install gtest
./vcpkg install openssl
./vcpkg install rapidcsv
```
Далее при сборке через CMake необходимо использовать ключ, напечатанный при выполнении команды
```bash
./vcpkg integrate install
```

## ⚙️ Сборка проекта

Для сборки **под Linux** выполните следующие действия, находясь в корне проекта:

```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=/path/to/your/toolchain/file
cd build
make
```
Для сборки **под Windows** необходимо выбрать желаемый генератор и скомпилировать проект через него.
```pwsh
cmake -S . -B build -G <generator-name> -DCMAKE_TOOLCHAIN_FILE=/path/to/your/toolchain/file
```

Проект разделен на 3 части: 
- библиотека libsacnner с основной логикой
- scanner_cli - консольная утилита
- tests - unit-тесты

**На Linux** после сборки исполняемый файл утилиты будет находится в директории **scanner_cli**.

**На Windows** после сборки все исполняемые файлы и библиотеки будут находиться в директории **bin**.

## Использование утилиты
Общий синтаксис улилиты: 
```
Usage:
  ./scanner_cli --base <hashes.csv> --path <root_dir> [--log <detections.log>]

Flags:
  --base   Path to CSV with MD5 base (required)
  --path   Root directory to scan (required)
  --log    Path to detections log file (optional, overwritten on each run)
  -h, --help  Show this help
```
Флаг ```--log``` является необязательным. В случае его отсутствия лог не будет записан.

### База вредоносных хэшей
Хеши “вредоносных” файлов и соответствующие им вердикты задаются в CSV-файле, путь к которому передается на вход утилите.

CSV-файл – это текстовый файл, состоящий из строк. Каждая строка содержит хеш и имя
вердикта, разделенные символом ';'.

Пример содержимого csv-файла:
```
a9963513d093ffb2bc7ceb9807771ad4;Exploit
ac6204ffeb36d2320e52f1d551cfa370;Dropper
8ee70903f43b227eeb971262268af5a8;Downloader
```

## Тестирование

Для запуска тестов необходимо перейти в директорию *tests* и выполнить команду ```ctest```.

1 из тестов выполняется долго, т.к. он проверяет возможность работы системы с файлами большого объема.

Запуск всех тестов, кроме "долгого"
```
ctest -L general
```
Запуск только "долгого" теста
```
ctest -L huge_file
```
