<div align="center">

# escan

**Fast, cross-platform ELF analyzer — a modern alternative to `readelf`.**

[![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-064F8C?logo=cmake&logoColor=white)](https://cmake.org/)
[![FTXUI](https://img.shields.io/badge/UI-FTXUI-2E8B57)](https://github.com/ArthurSonzogni/FTXUI)
[![yaml-cpp](https://img.shields.io/badge/config-yaml--cpp-E3780C?logo=yaml&logoColor=white)](https://github.com/jbeder/yaml-cpp)
[![Linux](https://img.shields.io/badge/platform-Linux-FCC624?logo=linux&logoColor=black)](https://www.kernel.org/)
[![macOS](https://img.shields.io/badge/platform-macOS-000000?logo=apple&logoColor=white)](https://www.apple.com/macos/)
[![Windows](https://img.shields.io/badge/platform-Windows-0078D6?logo=windows&logoColor=white)](https://www.microsoft.com/windows/)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](https://github.com/tuux200/escan/releases)

**English** | [Русский](ru/README.md) | [English (en/)](en/README.md)

</div>

## Table of contents

1. [About project](#1-about-project)
2. [Features](#2-features)
3. [Requirements](#3-requirements)
4. [Build](#4-build)
5. [Usage](#5-usage)
6. [Configuration](#6-configuration)
7. [Project structure](#7-project-structure)
8. [License](#8-license)

## 1. About project

`escan` is a command line tool for reading and analysing ELF object files —
executables, shared libraries (`.so` / `.dylib` / `.dll`) and relocatable
objects (`.o`). It covers the everyday tasks of GNU `readelf`, but ships as a
single portable binary with a clean, framed terminal output.

Unlike `readelf`, `escan`:

- reads **ELF32 and ELF64** with one build, no `-m` variants and no host
  architecture assumptions;
- works out of the box on **Linux, macOS and Windows** — the same CMake
  project builds everywhere;
- renders every report inside a **thin coloured frame** in the terminal, with
  the colour taken from a YAML config;
- demangles C++ symbol names, so `Foo::bar()` is printed instead of
  `_ZN3Foo3barEv`;
- stays dependency light: only `FTXUI`, `yaml-cpp` and the C++ standard
  library.

## 2. Features

| Flag | Report |
| ---- | ------ |
| `-A` | ELF header analysis: magic, class, data encoding, version, OS/ABI, type, machine, entry point and header layout |
| `-S` | Section table and name tables (demangled function and symbol names) |
| `-P` | Program segments, segment flags and shared object dependencies (`DT_NEEDED`, `DT_SONAME`) |
| `-h`, `--help` | Usage summary |
| `--version` | Version string |

Additional capabilities:

- automatic ELF class detection (`ELFCLASS32` / `ELFCLASS64`) and dispatch to
  the matching parser;
- segment address to file offset resolution for dynamic string tables;
- graceful reporting of stripped and static binaries;
- colour configuration through `escan.yaml` without recompiling.

## 3. Requirements

| Component | Version |
| --------- | ------- |
| CMake | 3.20 or newer |
| C++ compiler | GCC 10+, Clang 12+, MSVC 2019+ (C++20) |
| yaml-cpp | optional — fetched automatically when not installed |
| Network | required on the first configure step to fetch `FTXUI` |

## 4. Build

```sh
git clone https://github.com/tuux200/escan.git
cd escan
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

The resulting binary is `build/escan` (`build\escan.exe` on Windows).

The first configure downloads `FTXUI` and, if the system `yaml-cpp` is
missing, `yaml-cpp` as well. Later builds work offline.

## 5. Usage

```
escan [options] <file>
```

### Options

| Option | Description |
| ------ | ----------- |
| `-A` | ELF header analysis |
| `-S` | Sections and name tables |
| `-P` | Segments and shared objects |
| `-h`, `--help` | Show help |
| `--version` | Show version |

Flags may be combined in any order.

### Examples

```sh
escan -A /bin/ls          # header analysis
escan -S /bin/ls          # sections and function names, no frame
escan -P /bin/ls          # segments and .so dependencies
escan -A -S -P ./a.out    # everything at once
escan --version
```

Sample `-A` report:

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

## 6. Configuration

The report frame colour is read from `escan.yaml` in the current working
directory:

```yaml
color: "#94ebff"
```

Any `#RRGGBB` value is accepted. When the file is absent or the key is
missing, the default `#94ebff` is used.

## 7. Project structure

```
escan/
├── CMakeLists.txt          portable build, FTXUI + yaml-cpp
├── escan.yaml              frame colour configuration
├── include/
│   ├── Elf.hpp             ELF32 / ELF64 structures and constants
│   ├── ElfParse.hpp        header analysis API
│   ├── NameTables.hpp      sections and name tables API
│   ├── ParseSegments.hpp   segments and dynamic section API
│   ├── UI.hpp              FTXUI frame rendering
│   └── Cli.hpp             command line entry point
└── src/
    ├── ElfParse.cpp        -A report
    ├── NameTables.cpp      -S report
    ├── ParseSegments.cpp   -P report
    ├── UI.cpp              frame rendering and YAML colour
    ├── cli.cpp             option parsing and dispatch
    └── main.cpp
```

The code is deliberately procedural: free functions and plain structs, no
class hierarchies.

## 8. License

Released under the [MIT License](LICENSE).

---

<div align="center">

[![GitHub](https://img.shields.io/badge/GitHub-tuux200%2Fescan-181717?logo=github&logoColor=white)](https://github.com/tuux200/escan)

</div>
