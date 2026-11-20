#include <iomanip>
#include <fstream>
#include <vector>
#include <iostream>
#include "include/Elf.hpp"
#include "include/ElfParse.hpp"

bool checkMAGIC(const unsigned char ident[elf::EI_NIDENT]) {
    return ident[elf::EI_MAG0] == elf::ELFMAG0 &&
           ident[elf::EI_MAG1] == elf::ELFMAG1 &&
           ident[elf::EI_MAG2] == elf::ELFMAG2 &&
           ident[elf::EI_MAG3] == elf::ELFMAG3;
}

void printMAGIC(const unsigned char ident[elf::EI_NIDENT]) {
    std::cout << "Magic: ";
    std::cout << std::hex << static_cast<int>(ident[elf::EI_MAG0]) << " " << std::dec;
    std::cout << ident[elf::EI_MAG1] << " ";
    std::cout << ident[elf::EI_MAG2] << " ";
    std::cout << ident[elf::EI_MAG3] << '\n';
}

void printCLASS(const unsigned char e_ident[elf::EI_NIDENT]) {
    if (e_ident[elf::EI_CLASS] == elf::ELFCLASS64) {
        std::cout << "Class: ELF64" << '\n';
    } else if (e_ident[elf::EI_CLASS] == elf::ELFCLASS32) {
        std::cout << "Class: ELF32" << '\n';
    } else {
        std::cerr << "Class: Unexpected class" << '\n';
    }
}

void printDATA(const unsigned char e_ident[elf::EI_NIDENT]) {
    if (e_ident[elf::EI_DATA] == elf::ELFDATA2LSB) {
        std::cout << "Data: 2's complement, little endian" << '\n';
    } else if (e_ident[elf::EI_DATA] == elf::ELFDATA2MSB) {
        std::cout << "Data: 2's complement, big endian" << '\n';
    } else {
        std::cerr << "Data: Unexpected data encoding" << '\n';
    }
}

void printENTRY(const elf::Elf64_Addr e_entry) {
    std::cout << "Entry point address: 0x" << std::hex << e_entry << std::dec << '\n';
}

void printElfType(const elf::Elf64_Half e_type) {
    if (e_type == elf::ET_NONE) {
        std::cerr << "Type: None (unexpected)" << '\n';
    } else if (e_type == elf::ET_REL) {
        std::cout << "Type: REL (Relocatable file)" << '\n';
    } else if (e_type == elf::ET_EXEC) {
        std::cout << "Type: EXEC (Executable file)" << '\n';
    } else if (e_type == elf::ET_DYN) {
        std::cout << "Type: DYN (Shared object file / PIE)" << '\n';
    } else if (e_type == elf::ET_CORE) {
        std::cout << "Type: CORE (Core file)" << '\n';
    } else {
        std::cerr << "Type: Unexpected ELF type" << '\n';
    }
}

void printElfVersion(const elf::Elf64_Word e_version) {
    if (e_version == elf::EV_CURRENT) {
        std::cout << "Version: 1 (current)" << '\n';
    } else {
        std::cerr << "Version: Unexpected ELF version" << '\n';
    }
}

void printOS(const unsigned char e_ident[elf::EI_NIDENT]) {
    if (e_ident[elf::EI_OSABI] == elf::ELFOSABI_NONE ||
        e_ident[elf::EI_OSABI] == elf::ELFOSABI_SYSV) {
        std::cout << "OS/ABI: UNIX - System V" << '\n';
    } else if (e_ident[elf::EI_OSABI] == elf::ELFOSABI_GNU ||
               e_ident[elf::EI_OSABI] == elf::ELFOSABI_LINUX) {
        std::cout << "OS/ABI: UNIX - GNU" << '\n';
    } else if (e_ident[elf::EI_OSABI] == elf::ELFOSABI_FREEBSD) {
        std::cout << "OS/ABI: UNIX - FreeBSD" << '\n';
    } else {
        std::cerr << "OS/ABI: Unexpected OS" << '\n';
    }
}

void printARCH(const elf::Elf64_Half e_machine) {
    if (e_machine == elf::EM_NONE) {
        std::cerr << "Machine: Unexpected architecture" << '\n';
    } else if (e_machine == elf::EM_386) {
        std::cout << "Machine: Intel 80386 (x86)" << '\n';
    } else if (e_machine == elf::EM_ARM) {
        std::cout << "Machine: ARM 32-bit" << '\n';
    } else if (e_machine == elf::EM_X86_64) {
        std::cout << "Machine: AMD x86-64" << '\n';
    } else if (e_machine == elf::EM_RISCV) {
        std::cout << "Machine: RISC-V" << '\n';
    } else if (e_machine == elf::EM_AARCH64) {
        std::cout << "Machine: ARM 64-bit" << '\n';
    } else {
        std::cerr << "Machine: Unexpected architecture" << '\n';
    }
}

void printHeaderLayout(const elf::Elf64_Ehdr& header) {
    std::cout << "Start of program headers: " << header.e_phoff << " (bytes into file)" << '\n';
    std::cout << "Start of section headers: " << header.e_shoff << " (bytes into file)" << '\n';
    std::cout << "Flags: 0x" << std::hex << header.e_flags << std::dec << '\n';
    std::cout << "Size of this header: " << header.e_ehsize << " (bytes)" << '\n';
    std::cout << "Size of program headers: " << header.e_phentsize << " (bytes)" << '\n';
    std::cout << "Number of program headers: " << header.e_phnum << '\n';
    std::cout << "Size of section headers: " << header.e_shentsize << " (bytes)" << '\n';
    std::cout << "Number of section headers: " << header.e_shnum << '\n';
    std::cout << "Section header string table index: " << header.e_shstrndx << '\n';
}

void printHeaderLayout(const elf::Elf32_Ehdr& header) {
    std::cout << "Start of program headers: " << header.e_phoff << " (bytes into file)" << '\n';
    std::cout << "Start of section headers: " << header.e_shoff << " (bytes into file)" << '\n';
    std::cout << "Flags: 0x" << std::hex << header.e_flags << std::dec << '\n';
    std::cout << "Size of this header: " << header.e_ehsize << " (bytes)" << '\n';
    std::cout << "Size of program headers: " << header.e_phentsize << " (bytes)" << '\n';
    std::cout << "Number of program headers: " << header.e_phnum << '\n';
    std::cout << "Size of section headers: " << header.e_shentsize << " (bytes)" << '\n';
    std::cout << "Number of section headers: " << header.e_shnum << '\n';
    std::cout << "Section header string table index: " << header.e_shstrndx << '\n';
}

void readElf(std::ifstream& file) {
    unsigned char ident[elf::EI_NIDENT]{};
    file.clear();
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(ident), elf::EI_NIDENT);

    if (file.gcount() != elf::EI_NIDENT) {
        std::cerr << "Error: File is too small for ELF header" << '\n';
        return;
    }

    if (!checkMAGIC(ident)) {
        std::cerr << "Error: Not an ELF file" << '\n';
        return;
    }

    if (ident[elf::EI_CLASS] == elf::ELFCLASS32) {
        elf::Elf32_Ehdr header{};
        file.clear();
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(&header), sizeof(header));

        if (file.gcount() != static_cast<std::streamsize>(sizeof(header))) {
            std::cerr << "Error: File is too small for ELF header" << '\n';
            return;
        }

        printMAGIC(header.e_ident);
        printCLASS(header.e_ident);
        printDATA(header.e_ident);
        printElfVersion(header.e_version);
        printOS(header.e_ident);
        printElfType(header.e_type);
        printARCH(header.e_machine);
        printENTRY(header.e_entry);
        printHeaderLayout(header);
    } else if (ident[elf::EI_CLASS] == elf::ELFCLASS64) {
        elf::Elf64_Ehdr header{};
        file.clear();
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(&header), sizeof(header));

        if (file.gcount() != static_cast<std::streamsize>(sizeof(header))) {
            std::cerr << "Error: File is too small for ELF header" << '\n';
            return;
        }

        printMAGIC(header.e_ident);
        printCLASS(header.e_ident);
        printDATA(header.e_ident);
        printElfVersion(header.e_version);
        printOS(header.e_ident);
        printElfType(header.e_type);
        printARCH(header.e_machine);
        printENTRY(header.e_entry);
        printHeaderLayout(header);
    } else {
        std::cerr << "Error: Unexpected ELF class" << '\n';
    }
}
