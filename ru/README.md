<div align="center">

# escan

**Быстрый кросс-платформенный анализатор ELF — современная альтернатива `readelf`.**

[![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-064F8C?logo=cmake&logoColor=white)](https://cmake.org/)
[![FTXUI](https://img.shields.io/badge/UI-FTXUI-2E8B57)](https://github.com/ArthurSonzogni/FTXUI)
[![yaml-cpp](https://img.shields.io/badge/config-yaml--cpp-E3780C?logo=yaml&logoColor=white)](https://github.com/jbeder/yaml-cpp)
[![Linux](https://img.shields.io/badge/platform-Linux-FCC624?logo=linux&logoColor=black)](https://www.kernel.org/)
[![macOS](https://img.shields.io/badge/platform-macOS-000000?logo=apple&logoColor=white)](https://www.apple.com/macos/)
[![Windows](https://img.shields.io/badge/platform-Windows-0078D6?logo=windows&logoColor=white)](https://www.microsoft.com/windows/)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](../LICENSE)
[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](https://github.com/tuux200/escan/releases)

[English](../README.md) | **Русский** | [English (en/)](../en/README.md)

</div>

## Оглавление

1. [О проекте](#1-о-проекте)
2. [Возможности](#2-возможности)
3. [Требования](#3-требования)
4. [Сборка](#4-сборка)
5. [Использование](#5-использование)
6. [Конфигурация](#6-конфигурация)
7. [Структура проекта](#7-структура-проекта)
8. [Лицензия](#8-лицензия)

## 1. О проекте

`escan` — консольный инструмент для чтения и анализа ELF-файлов: исполняемых
бинарников, разделяемых библиотек (`.so`, `.dylib`, `.dll`) и объектных
файлов (`.o`). Он покрывает повседневные задачи GNU `readelf`, но поставляется
в виде одного портируемого бинарника с аккуратным выводом в рамке в терминале.

В отличие от `readelf`, `escan`:

- читает **ELF32 и ELF64** одной сборкой, без вариантов `-m` и привязки к
  архитектуре хоста;
- работает на **Linux, macOS и Windows** — один CMake-проект собирается на
  всех платформах;
- отображает каждый отчёт в **тонкой цветной рамке** в терминале, цвет берётся
  из YAML-конфигурации;
- деманглит имена C++-символов, так что вместо `_ZN3Foo3barEv` выводится
  `Foo::bar()`;
- имеет минимум зависимостей: `FTXUI`, `yaml-cpp` и стандартная библиотека C++.

## 2. Возможности

| Флаг | Отчёт |
| ---- | ----- |
| `-A` | Анализ заголовка ELF: magic, class, data encoding, version, OS/ABI, type, machine, entry point и layout |
| `-S` | Таблица секций и таблицы имён (демангленные имена функций и символов) |
| `-P` | Программные сегменты, флаги сегментов и зависимости от разделяемых объектов (`DT_NEEDED`, `DT_SONAME`) |
| `-h`, `--help` | Справка по использованию |
| `--version` | Версия программы |

Дополнительно:

- автоматическое определение класса ELF (`ELFCLASS32` / `ELFCLASS64`) и
  вызов соответствующего парсера;
- вычисление смещений адресов сегментов в файле для таблиц динамических строк;
- корректная обработка stripped и статически слинкованных бинарников;
- настройка цвета рамки через `escan.yaml` без перекомпиляции.

## 3. Требования

| Компонент | Версия |
| --------- | ------ |
| CMake | 3.20 или новее |
| Компилятор C++ | GCC 10+, Clang 12+, MSVC 2019+ (C++20) |
| yaml-cpp | опционально — скачивается автоматически, если не найден |
| Сеть | нужна при первой конфигурации для загрузки `FTXUI` |

## 4. Сборка

```sh
git clone https://github.com/tuux200/escan.git
cd escan
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Итоговый бинарник: `build/escan` (`build\escan.exe` на Windows).

При первой конфигурации скачивается `FTXUI` и, если в системе отсутствует
`yaml-cpp`, также `yaml-cpp`. Последующие сборки работают офлайн.

## 5. Использование

```
escan [опции] <файл>
```

### Опции

| Опция | Описание |
| ----- | -------- |
| `-A` | Анализ заголовка ELF |
| `-S` | Секции и таблицы имён |
| `-P` | Сегменты и разделяемые объекты |
| `-h`, `--help` | Показать справку |
| `--version` | Показать версию |

Флаги можно комбинировать в любом порядке.

### Примеры

```sh
escan -A /bin/ls          # анализ заголовка
escan -S /bin/ls          # секции и имена функций, без рамки
escan -P /bin/ls          # сегменты и зависимости .so
escan -A -S -P ./a.out    # всё сразу
escan --version
```

Пример вывода `-A`:

```
┌──────────────────────────────────────────────────┐
│ELF analysis                                      │
│Magic: 7f E L F                                   │
│Class: ELF64                                      │
│Data: 2's complement, little endian               │
│Version: 1 (current)                              │
│OS/ABI: UNIX - GNU                                │
│Type: DYN (Shared object file / PIE)              │
│Machine: AMD x86-64                               │
│Entry point address: 0x10d70                      │
│Start of program headers: 64 (bytes into file)    │
│Start of section headers: 381504 (bytes into file)│
│Flags: 0x0                                        │
│Size of this header: 64 (bytes)                   │
│Size of program headers: 56 (bytes)               │
│Number of program headers: 15                     │
│Size of section headers: 64 (bytes)               │
│Number of section headers: 33                     │
│Section header string table index: 32             │
└──────────────────────────────────────────────────┘
```

## 6. Конфигурация

Цвет рамки отчёта читается из `escan.yaml` в текущей рабочей директории:

```yaml
color: "#94ebff"
```

Принимается любое значение формата `#RRGGBB`. Если файл отсутствует или ключ
не задан, используется значение по умолчанию `#94ebff`.

## 7. Структура проекта

```
escan/
├── CMakeLists.txt          портируемая сборка, FTXUI + yaml-cpp
├── escan.yaml              конфигурация цвета рамки
├── include/
│   ├── Elf.hpp             структуры и константы ELF32 / ELF64
│   ├── ElfParse.hpp        API анализа заголовка
│   ├── NameTables.hpp      API секций и таблиц имён
│   ├── ParseSegments.hpp   API сегментов и динамической секции
│   ├── UI.hpp              рендеринг рамок FTXUI
│   └── Cli.hpp             точка входа CLI
└── src/
    ├── ElfParse.cpp        отчёт -A
    ├── NameTables.cpp      отчёт -S
    ├── ParseSegments.cpp   отчёт -P
    ├── UI.cpp              рендеринг рамок и цвет из YAML
    ├── cli.cpp             разбор опций и диспетчеризация
    └── main.cpp
```

Код намеренно процедурный: свободные функции и простые структуры, без иерархий
классов.

## 8. Лицензия

Проект распространяется под [лицензией MIT](../LICENSE).

---

<div align="center">

[![GitHub](https://img.shields.io/badge/GitHub-tuux200%2Fescan-181717?logo=github&logoColor=white)](https://github.com/tuux200/escan)

</div>
