#pragma once

#include <fstream>
#include <vector>
#include "include/Elf.hpp"

bool checkSegmetns(const elf::Elf64_Half e_phnum, const elf::Elf64_Off e_phoff);

std::vector<elf::Elf32_Phdr> readProgramHeaders(
    std::ifstream& file,
    const elf::Elf32_Ehdr& header
);

std::vector<elf::Elf64_Phdr> readProgramHeaders(
    std::ifstream& file,
    const elf::Elf64_Ehdr& header
);

void printSegments(const std::vector<elf::Elf32_Phdr>& segments);
void printSegments(const std::vector<elf::Elf64_Phdr>& segments);

void printSharedObjects(
    std::ifstream& file,
    const std::vector<elf::Elf32_Phdr>& segments
);

void printSharedObjects(
    std::ifstream& file,
    const std::vector<elf::Elf64_Phdr>& segments
);

void parseSegments(std::ifstream& file, const elf::Elf32_Ehdr& header);
void parseSegments(std::ifstream& file, const elf::Elf64_Ehdr& header);
