#include <iomanip>
#include <fstream>
#include <vector>
#include <iostream>
#include "include/Elf.hpp"

bool checkMAGIC(const unsigned char ident[elf::EI_NIDENT]) {
    return ident[elf::EI_MAG0] == elf::ELFMAG0 &&
           ident[elf::EI_MAG1] == elf::ELFMAG1 &&
           ident[elf::EI_MAG2] == elf::ELFMAG2 &&
           ident[elf::EI_MAG3] == elf::ELFMAG3;

}

void printARCH(const elf::Elf64_Half e_machine) {
    std::cout << "Your architicture is " << '\n';
    if (e_machine == elf::EM_NONE) {
        std::cerr << "Unxpected architicture!" << '\n';
    } else if (e_machine == elf::EM_386) {
        std::cout << "Intel 80386 (x86)!" << '\n';
    } else if (e_machine == elf::EM_ARM) {
        std::cout << "ARM 32-bit" << '\n';
    } else if (e_machine == elf::EM_X86_64) {
        std::cout << "AMD x86-64" << '\n';
    } else if (e_machine == elf::EM_RISCV ) {
        std::cout << "RISC-V" << '\n';
    } else if (e_machine == elf::EM_AARCH64) {
        std::cout << "ARM 64-bit" << '\n';
    }

}

void readElf(std::ifstream& file) {
    elf::Elf64_Ehdr header{};
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (file.gcount() != sizeof(header)) {
        std::cerr << "Too small for ELF file!" << '\n';
        return;
    }

    if (checkMAGIC(header.e_ident))
        {
            std::cout << "This is ELF file!" << '\n';
            printARCH(header.e_machine);

        } else {
            std::cerr << "This is not an ELF file!" << '\n';
            return;
        }
}


