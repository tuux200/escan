#pragma once

#include <iomanip>
#include <fstream>
#include <vector>
#include <iostream>
#include "include/Elf.hpp"

bool checkSections(const elf::Elf64_Half e_shnum, const elf::Elf64_Off e_shoff);

std::vector<elf::Elf32_Shdr> readSectionHeaders(
    std::ifstream& file,
    const elf::Elf32_Ehdr& header
);

std::vector<elf::Elf64_Shdr> readSectionHeaders(
    std::ifstream& file,
    const elf::Elf64_Ehdr& header
);

std::vector<char> readStringTable(
    std::ifstream& file,
    const std::vector<elf::Elf32_Shdr>& sections,
    elf::Elf32_Half strtabIndex
);

std::vector<char> readStringTable(
    std::ifstream& file,
    const std::vector<elf::Elf64_Shdr>& sections,
    elf::Elf64_Half strtabIndex
);

void printSectionsNames(
    const std::vector<elf::Elf32_Shdr>& sections,
    const std::vector<char>& shstrtab
);

void printSectionsNames(
    const std::vector<elf::Elf64_Shdr>& sections,
    const std::vector<char>& shstrtab
);

void printFunctionsNames(
    std::ifstream& file,
    const std::vector<elf::Elf32_Shdr>& sections
);

void printFunctionsNames(
    std::ifstream& file,
    const std::vector<elf::Elf64_Shdr>& sections
);

void parseSections(std::ifstream& file, const elf::Elf32_Ehdr& header);
void parseSections(std::ifstream& file, const elf::Elf64_Ehdr& header);
