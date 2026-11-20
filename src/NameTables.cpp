#include <iomanip>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#if defined(_MSC_VER)
#include <dbghelp.h>
#else
#include <cxxabi.h>
#endif
#include "include/Elf.hpp"
#include "include/NameTables.hpp"

static std::string demangleName(const char* name) {
#if defined(_MSC_VER)
    char buffer[1024];
    if (UnDecorateSymbolName(name, buffer, sizeof(buffer), UNDNAME_COMPLETE) != 0) {
        return buffer;
    }
    return name;
#else
    int status = 0;
    char* result = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    if (status == 0 && result != nullptr) {
        std::string value(result);
        std::free(result);
        return value;
    }
    return name;
#endif
}

bool checkSections(const elf::Elf64_Half e_shnum, const elf::Elf64_Off e_shoff) {
    if (e_shnum == 0 || e_shoff == 0) {
        return false;
    } else {
        return true;
    }
}

std::vector<elf::Elf64_Shdr> readSectionHeaders(
    std::ifstream& file,
    const elf::Elf64_Ehdr& header
) {
    file.clear();
    file.seekg(header.e_shoff, std::ios::beg);
    std::vector<elf::Elf64_Shdr> sections(header.e_shnum);
    file.read(reinterpret_cast<char*>(sections.data()), sections.size() * sizeof(elf::Elf64_Shdr));

    if (file.gcount() == static_cast<std::streamsize>(sizeof(elf::Elf64_Shdr) * sections.size())) {
        return sections;
    } else {
        return {};
    }
}

std::vector<elf::Elf32_Shdr> readSectionHeaders(
    std::ifstream& file,
    const elf::Elf32_Ehdr& header
) {
    file.clear();
    file.seekg(header.e_shoff, std::ios::beg);
    std::vector<elf::Elf32_Shdr> sections(header.e_shnum);
    file.read(reinterpret_cast<char*>(sections.data()), sections.size() * sizeof(elf::Elf32_Shdr));

    if (file.gcount() == static_cast<std::streamsize>(sizeof(elf::Elf32_Shdr) * sections.size())) {
        return sections;
    } else {
        return {};
    }
}

std::vector<char> readStringTable(
    std::ifstream& file,
    const std::vector<elf::Elf64_Shdr>& sections,
    elf::Elf64_Half strtabIndex
) {
    if (strtabIndex >= sections.size() || sections[strtabIndex].sh_size == 0) {
        return {};
    } else {
        const auto& strSection = sections[strtabIndex];
        std::vector<char> strtab(strSection.sh_size);
        file.clear();
        file.seekg(strSection.sh_offset, std::ios::beg);
        file.read(strtab.data(), strSection.sh_size);

        if (file.gcount() == static_cast<std::streamsize>(strtab.size())) {
            return strtab;
        } else {
            return {};
        }
    }
}

std::vector<char> readStringTable(
    std::ifstream& file,
    const std::vector<elf::Elf32_Shdr>& sections,
    elf::Elf32_Half strtabIndex
) {
    if (strtabIndex >= sections.size() || sections[strtabIndex].sh_size == 0) {
        return {};
    } else {
        const auto& strSection = sections[strtabIndex];
        std::vector<char> strtab(strSection.sh_size);
        file.clear();
        file.seekg(strSection.sh_offset, std::ios::beg);
        file.read(strtab.data(), strSection.sh_size);

        if (file.gcount() == static_cast<std::streamsize>(strtab.size())) {
            return strtab;
        } else {
            return {};
        }
    }
}

void printSectionsNames(
    const std::vector<elf::Elf64_Shdr>& sections,
    const std::vector<char>& shstrtab
) {
    std::cout << "Sections list:" << '\n';
    for (size_t i = 0; i < sections.size(); ++i) {
        std::cout << "  [" << std::setw(2) << i << "] ";
        if (sections[i].sh_name < shstrtab.size()) {
            const char* name = &shstrtab[sections[i].sh_name];
            if (name[0] == '\0') {
                std::cout << "<null>";
            } else {
                std::cout << name;
            }
        } else {
            std::cout << "<invalid offset>";
        }
        std::cout << '\n';
    }
}

void printSectionsNames(
    const std::vector<elf::Elf32_Shdr>& sections,
    const std::vector<char>& shstrtab
) {
    std::cout << "Sections list:" << '\n';
    for (size_t i = 0; i < sections.size(); ++i) {
        std::cout << "  [" << std::setw(2) << i << "] ";
        if (sections[i].sh_name < shstrtab.size()) {
            const char* name = &shstrtab[sections[i].sh_name];
            if (name[0] == '\0') {
                std::cout << "<null>";
            } else {
                std::cout << name;
            }
        } else {
            std::cout << "<invalid offset>";
        }
        std::cout << '\n';
    }
}

void printFunctionsNames(
    std::ifstream& file,
    const std::vector<elf::Elf64_Shdr>& sections
) {
    bool foundAny = false;
    std::cout << "Functions list:" << '\n';

    for (const auto& sec : sections) {
        if (sec.sh_type == elf::SHT_SYMTAB || sec.sh_type == elf::SHT_DYNSYM) {
            std::vector<char> symStrtab = readStringTable(file, sections, sec.sh_link);
            if (symStrtab.empty()) {
                continue;
            }

            size_t symCount = sec.sh_size / sizeof(elf::Elf64_Sym);
            std::vector<elf::Elf64_Sym> symbols(symCount);

            file.clear();
            file.seekg(sec.sh_offset, std::ios::beg);
            file.read(reinterpret_cast<char*>(symbols.data()), symbols.size() * sizeof(elf::Elf64_Sym));

            if (file.gcount() != static_cast<std::streamsize>(symbols.size() * sizeof(elf::Elf64_Sym))) {
                continue;
            }

            size_t funcIndex = 0;
            for (const auto& sym : symbols) {
                unsigned char symType = sym.st_info & 0x0F;
                if (symType == elf::STT_FUNC && sym.st_name != 0 && sym.st_name < symStrtab.size()) {
                    std::cout << "  [" << std::setw(3) << funcIndex++ << "] "
                              << demangleName(&symStrtab[sym.st_name])
                              << " (0x" << std::hex << sym.st_value << std::dec << ")" << '\n';
                    foundAny = true;
                }
            }
        }
    }

    if (!foundAny) {
        std::cout << "  No functions found (binary might be stripped)" << '\n';
    }
}

void printFunctionsNames(
    std::ifstream& file,
    const std::vector<elf::Elf32_Shdr>& sections
) {
    bool foundAny = false;
    std::cout << "Functions list:" << '\n';

    for (const auto& sec : sections) {
        if (sec.sh_type == elf::SHT_SYMTAB || sec.sh_type == elf::SHT_DYNSYM) {
            std::vector<char> symStrtab = readStringTable(file, sections, sec.sh_link);
            if (symStrtab.empty()) {
                continue;
            }

            size_t symCount = sec.sh_size / sizeof(elf::Elf32_Sym);
            std::vector<elf::Elf32_Sym> symbols(symCount);

            file.clear();
            file.seekg(sec.sh_offset, std::ios::beg);
            file.read(reinterpret_cast<char*>(symbols.data()), symbols.size() * sizeof(elf::Elf32_Sym));

            if (file.gcount() != static_cast<std::streamsize>(symbols.size() * sizeof(elf::Elf32_Sym))) {
                continue;
            }

            size_t funcIndex = 0;
            for (const auto& sym : symbols) {
                unsigned char symType = sym.st_info & 0x0F;
                if (symType == elf::STT_FUNC && sym.st_name != 0 && sym.st_name < symStrtab.size()) {
                    std::cout << "  [" << std::setw(3) << funcIndex++ << "] "
                              << demangleName(&symStrtab[sym.st_name])
                              << " (0x" << std::hex << sym.st_value << std::dec << ")" << '\n';
                    foundAny = true;
                }
            }
        }
    }

    if (!foundAny) {
        std::cout << "  No functions found (binary might be stripped)" << '\n';
    }
}

void parseSections(std::ifstream& file, const elf::Elf64_Ehdr& header) {
    if (!checkSections(header.e_shnum, header.e_shoff)) {
        std::cerr << "Error: Didn't find any sections" << '\n';
        return;
    }

    std::cout << "Sections quantity: " << header.e_shnum << '\n';

    std::vector<elf::Elf64_Shdr> sections = readSectionHeaders(file, header);
    if (sections.empty()) {
        std::cerr << "Error: Failed to read section headers" << '\n';
        return;
    }

    std::vector<char> shstrtab = readStringTable(file, sections, header.e_shstrndx);
    if (shstrtab.empty()) {
        std::cerr << "Error: Failed to read .shstrtab" << '\n';
        return;
    }

    printSectionsNames(sections, shstrtab);
    printFunctionsNames(file, sections);
}

void parseSections(std::ifstream& file, const elf::Elf32_Ehdr& header) {
    if (!checkSections(header.e_shnum, header.e_shoff)) {
        std::cerr << "Error: Didn't find any sections" << '\n';
        return;
    }

    std::cout << "Sections quantity: " << header.e_shnum << '\n';

    std::vector<elf::Elf32_Shdr> sections = readSectionHeaders(file, header);
    if (sections.empty()) {
        std::cerr << "Error: Failed to read section headers" << '\n';
        return;
    }

    std::vector<char> shstrtab = readStringTable(file, sections, header.e_shstrndx);
    if (shstrtab.empty()) {
        std::cerr << "Error: Failed to read .shstrtab" << '\n';
        return;
    }

    printSectionsNames(sections, shstrtab);
    printFunctionsNames(file, sections);
}
