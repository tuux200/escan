#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "include/Cli.hpp"
#include "include/Elf.hpp"
#include "include/ElfParse.hpp"
#include "include/NameTables.hpp"
#include "include/ParseSegments.hpp"
#include "include/UI.hpp"

static const char* ESCAN_VERSION = "1.0.0";

static void printUsage(const char* program) {
    std::cout << "Usage: " << program << " [options] <file>" << '\n';
    std::cout << "Options:" << '\n';
    std::cout << "  -A            ELF analysis" << '\n';
    std::cout << "  -S            sections and name tables" << '\n';
    std::cout << "  -P            segments and shared objects" << '\n';
    std::cout << "  --version     show version" << '\n';
    std::cout << "  -h, --help    show this help" << '\n';
}

template <typename Func>
static void runView(const std::string& title, Func func) {
    std::ostringstream output;
    std::streambuf* oldOut = std::cout.rdbuf(output.rdbuf());
    std::streambuf* oldErr = std::cerr.rdbuf(output.rdbuf());

    try {
        func();
    } catch (...) {
    }

    std::cout.rdbuf(oldOut);
    std::cerr.rdbuf(oldErr);

    std::cout << renderView(title, output.str());
}

static bool readHeader(std::ifstream& file, elf::Elf32_Ehdr& header) {
    file.clear();
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    return file.gcount() == static_cast<std::streamsize>(sizeof(header));
}

static bool readHeader(std::ifstream& file, elf::Elf64_Ehdr& header) {
    file.clear();
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    return file.gcount() == static_cast<std::streamsize>(sizeof(header));
}

int runCli(int argc, char** argv) {
    bool analysis = false;
    bool sections = false;
    bool segments = false;
    std::string path;

    for (int i = 1; i < argc; ++i) {
        std::string argument = argv[i];

        if (argument == "--version") {
            std::cout << "escan " << ESCAN_VERSION << '\n';
            return 0;
        } else if (argument == "-h" || argument == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (argument == "-A") {
            analysis = true;
        } else if (argument == "-S") {
            sections = true;
        } else if (argument == "-P") {
            segments = true;
        } else if (!argument.empty() && argument[0] == '-') {
            std::cerr << "Unknown option: " << argument << '\n';
            printUsage(argv[0]);
            return 1;
        } else if (path.empty()) {
            path = argument;
        } else {
            std::cerr << "Error: Multiple files specified" << '\n';
            return 1;
        }
    }

    if (path.empty()) {
        printUsage(argv[0]);
        return 1;
    }

    if (!analysis && !sections && !segments) {
        analysis = true;
    }

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Cannot open file: " << path << '\n';
        return 1;
    }

    unsigned char ident[elf::EI_NIDENT]{};
    file.read(reinterpret_cast<char*>(ident), elf::EI_NIDENT);

    if (file.gcount() != elf::EI_NIDENT) {
        std::cerr << "Too small for ELF file!" << '\n';
        return 1;
    }

    if (!checkMAGIC(ident)) {
        std::cerr << "This is not an ELF file!" << '\n';
        return 1;
    }

    if (analysis) {
        runView("ELF analysis", [&file]() {
            readElf(file);
        });
    }

    if (ident[elf::EI_CLASS] == elf::ELFCLASS32) {
        elf::Elf32_Ehdr header{};

        if (!readHeader(file, header)) {
            std::cerr << "Error: Failed to read ELF32 header" << '\n';
            return 1;
        }

        if (sections) {
            parseSections(file, header);
        }

        if (segments) {
            runView("Segments and shared objects", [&file, &header]() {
                parseSegments(file, header);
            });
        }
    } else if (ident[elf::EI_CLASS] == elf::ELFCLASS64) {
        elf::Elf64_Ehdr header{};

        if (!readHeader(file, header)) {
            std::cerr << "Error: Failed to read ELF64 header" << '\n';
            return 1;
        }

        if (sections) {
            parseSections(file, header);
        }

        if (segments) {
            runView("Segments and shared objects", [&file, &header]() {
                parseSegments(file, header);
            });
        }
    } else {
        std::cerr << "Error: Unsupported ELF class" << '\n';
        return 1;
    }

    return 0;
}
