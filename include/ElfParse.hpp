#pragma once

#include <fstream>
#include "include/Elf.hpp"

bool checkMAGIC(const unsigned char ident[elf::EI_NIDENT]);
void printMAGIC(const unsigned char ident[elf::EI_NIDENT]);
void printCLASS(const unsigned char e_ident[elf::EI_NIDENT]);
void printDATA(const unsigned char e_ident[elf::EI_NIDENT]);
void printENTRY(const elf::Elf64_Addr e_entry);
void printElfType(const elf::Elf64_Half e_type);
void printElfVersion(const elf::Elf64_Word e_version);
void printOS(const unsigned char e_ident[elf::EI_NIDENT]);
void printARCH(const elf::Elf64_Half e_machine);
void printHeaderLayout(const elf::Elf32_Ehdr& header);
void printHeaderLayout(const elf::Elf64_Ehdr& header);
void readElf(std::ifstream& file);
