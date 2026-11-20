#include <iomanip>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include "include/Elf.hpp"
#include "include/ParseSegments.hpp"

bool checkSegmetns(const elf::Elf64_Half e_phnum, const elf::Elf64_Off e_phoff) {
    if (e_phnum == 0 || e_phoff == 0) {
        return false;
    } else {
        return true;
    }
}

static const char* segmentTypeName(elf::Elf64_Word type) {
    if (type == elf::PT_NULL) {
        return "NULL";
    } else if (type == elf::PT_LOAD) {
        return "LOAD";
    } else if (type == elf::PT_DYNAMIC) {
        return "DYNAMIC";
    } else if (type == elf::PT_INTERP) {
        return "INTERP";
    } else if (type == elf::PT_NOTE) {
        return "NOTE";
    } else if (type == elf::PT_SHLIB) {
        return "SHLIB";
    } else if (type == elf::PT_PHDR) {
        return "PHDR";
    } else if (type == elf::PT_TLS) {
        return "TLS";
    } else if (type == elf::PT_GNU_EH_FRAME) {
        return "GNU_EH_FRAME";
    } else if (type == elf::PT_GNU_STACK) {
        return "GNU_STACK";
    } else if (type == elf::PT_GNU_RELRO) {
        return "GNU_RELRO";
    } else {
        return "UNKNOWN";
    }
}

static std::string segmentFlags(elf::Elf64_Word flags) {
    std::string result;
    result += (flags & elf::PF_R) ? 'R' : '-';
    result += (flags & elf::PF_W) ? 'W' : '-';
    result += (flags & elf::PF_X) ? 'X' : '-';
    return result;
}

static const char* dynamicString(const std::vector<char>& strtab, uint64_t offset) {
    if (offset < strtab.size()) {
        return &strtab[offset];
    } else {
        return "<invalid offset>";
    }
}

static bool vaddrToOffset(
    const std::vector<elf::Elf64_Phdr>& segments,
    elf::Elf64_Addr address,
    elf::Elf64_Off& offset
) {
    for (const auto& segment : segments) {
        if (segment.p_type == elf::PT_LOAD) {
            if (address >= segment.p_vaddr && address < segment.p_vaddr + segment.p_filesz) {
                offset = segment.p_offset + (address - segment.p_vaddr);
                return true;
            }
        }
    }
    return false;
}

static bool vaddrToOffset(
    const std::vector<elf::Elf32_Phdr>& segments,
    elf::Elf32_Addr address,
    elf::Elf32_Off& offset
) {
    for (const auto& segment : segments) {
        if (segment.p_type == elf::PT_LOAD) {
            if (address >= segment.p_vaddr && address < segment.p_vaddr + segment.p_filesz) {
                offset = segment.p_offset + (address - segment.p_vaddr);
                return true;
            }
        }
    }
    return false;
}

std::vector<elf::Elf64_Phdr> readProgramHeaders(
    std::ifstream& file,
    const elf::Elf64_Ehdr& header
) {
    file.clear();
    file.seekg(header.e_phoff, std::ios::beg);
    std::vector<elf::Elf64_Phdr> segments(header.e_phnum);
    file.read(reinterpret_cast<char*>(segments.data()), segments.size() * sizeof(elf::Elf64_Phdr));

    if (file.gcount() == static_cast<std::streamsize>(sizeof(elf::Elf64_Phdr) * segments.size())) {
        return segments;
    } else {
        return {};
    }
}

std::vector<elf::Elf32_Phdr> readProgramHeaders(
    std::ifstream& file,
    const elf::Elf32_Ehdr& header
) {
    file.clear();
    file.seekg(header.e_phoff, std::ios::beg);
    std::vector<elf::Elf32_Phdr> segments(header.e_phnum);
    file.read(reinterpret_cast<char*>(segments.data()), segments.size() * sizeof(elf::Elf32_Phdr));

    if (file.gcount() == static_cast<std::streamsize>(sizeof(elf::Elf32_Phdr) * segments.size())) {
        return segments;
    } else {
        return {};
    }
}

void printSegments(const std::vector<elf::Elf64_Phdr>& segments) {
    std::cout << "Segments list:" << '\n';
    for (size_t i = 0; i < segments.size(); ++i) {
        const auto& segment = segments[i];
        std::cout << "  [" << std::setw(2) << i << "] "
                  << segmentTypeName(segment.p_type)
                  << " off=0x" << std::hex << segment.p_offset << std::dec
                  << " vaddr=0x" << std::hex << segment.p_vaddr << std::dec
                  << " filesz=0x" << std::hex << segment.p_filesz << std::dec
                  << " memsz=0x" << std::hex << segment.p_memsz << std::dec
                  << " flags=" << segmentFlags(segment.p_flags)
                  << '\n';
    }
}

void printSegments(const std::vector<elf::Elf32_Phdr>& segments) {
    std::cout << "Segments list:" << '\n';
    for (size_t i = 0; i < segments.size(); ++i) {
        const auto& segment = segments[i];
        std::cout << "  [" << std::setw(2) << i << "] "
                  << segmentTypeName(segment.p_type)
                  << " off=0x" << std::hex << segment.p_offset << std::dec
                  << " vaddr=0x" << std::hex << segment.p_vaddr << std::dec
                  << " filesz=0x" << std::hex << segment.p_filesz << std::dec
                  << " memsz=0x" << std::hex << segment.p_memsz << std::dec
                  << " flags=" << segmentFlags(segment.p_flags)
                  << '\n';
    }
}

void printSharedObjects(
    std::ifstream& file,
    const std::vector<elf::Elf64_Phdr>& segments
) {
    const elf::Elf64_Phdr* dynamic = nullptr;
    for (const auto& segment : segments) {
        if (segment.p_type == elf::PT_DYNAMIC) {
            dynamic = &segment;
            break;
        }
    }

    std::cout << "Shared objects:" << '\n';

    if (dynamic == nullptr) {
        std::cout << "  No dynamic segment (static binary)" << '\n';
        return;
    }

    size_t count = dynamic->p_filesz / sizeof(elf::Elf64_Dyn);
    std::vector<elf::Elf64_Dyn> entries(count);

    file.clear();
    file.seekg(dynamic->p_offset, std::ios::beg);
    file.read(reinterpret_cast<char*>(entries.data()), entries.size() * sizeof(elf::Elf64_Dyn));

    if (file.gcount() != static_cast<std::streamsize>(entries.size() * sizeof(elf::Elf64_Dyn))) {
        std::cerr << "Error: Failed to read dynamic section" << '\n';
        return;
    }

    elf::Elf64_Addr strtabAddress = 0;
    elf::Elf64_Xword strtabSize = 0;

    for (const auto& entry : entries) {
        if (entry.d_tag == elf::DT_STRTAB) {
            strtabAddress = entry.d_un.d_ptr;
        } else if (entry.d_tag == elf::DT_STRSZ) {
            strtabSize = entry.d_un.d_val;
        }
    }

    if (strtabAddress == 0 || strtabSize == 0) {
        std::cout << "  No dynamic string table" << '\n';
        return;
    }

    elf::Elf64_Off strtabOffset = 0;
    if (!vaddrToOffset(segments, strtabAddress, strtabOffset)) {
        std::cerr << "Error: Failed to resolve dynamic string table" << '\n';
        return;
    }

    std::vector<char> strtab(strtabSize);
    file.clear();
    file.seekg(strtabOffset, std::ios::beg);
    file.read(strtab.data(), strtab.size());

    if (file.gcount() != static_cast<std::streamsize>(strtab.size())) {
        std::cerr << "Error: Failed to read dynamic string table" << '\n';
        return;
    }

    bool foundAny = false;

    for (const auto& entry : entries) {
        if (entry.d_tag == elf::DT_SONAME) {
            std::cout << "  soname: " << dynamicString(strtab, entry.d_un.d_val) << '\n';
            foundAny = true;
        }
    }

    for (const auto& entry : entries) {
        if (entry.d_tag == elf::DT_NEEDED) {
            std::cout << "  needed: " << dynamicString(strtab, entry.d_un.d_val) << '\n';
            foundAny = true;
        }
    }

    if (!foundAny) {
        std::cout << "  No shared object dependencies" << '\n';
    }
}

void printSharedObjects(
    std::ifstream& file,
    const std::vector<elf::Elf32_Phdr>& segments
) {
    const elf::Elf32_Phdr* dynamic = nullptr;
    for (const auto& segment : segments) {
        if (segment.p_type == elf::PT_DYNAMIC) {
            dynamic = &segment;
            break;
        }
    }

    std::cout << "Shared objects:" << '\n';

    if (dynamic == nullptr) {
        std::cout << "  No dynamic segment (static binary)" << '\n';
        return;
    }

    size_t count = dynamic->p_filesz / sizeof(elf::Elf32_Dyn);
    std::vector<elf::Elf32_Dyn> entries(count);

    file.clear();
    file.seekg(dynamic->p_offset, std::ios::beg);
    file.read(reinterpret_cast<char*>(entries.data()), entries.size() * sizeof(elf::Elf32_Dyn));

    if (file.gcount() != static_cast<std::streamsize>(entries.size() * sizeof(elf::Elf32_Dyn))) {
        std::cerr << "Error: Failed to read dynamic section" << '\n';
        return;
    }

    elf::Elf32_Addr strtabAddress = 0;
    elf::Elf32_Word strtabSize = 0;

    for (const auto& entry : entries) {
        if (entry.d_tag == elf::DT_STRTAB) {
            strtabAddress = entry.d_un.d_ptr;
        } else if (entry.d_tag == elf::DT_STRSZ) {
            strtabSize = entry.d_un.d_val;
        }
    }

    if (strtabAddress == 0 || strtabSize == 0) {
        std::cout << "  No dynamic string table" << '\n';
        return;
    }

    elf::Elf32_Off strtabOffset = 0;
    if (!vaddrToOffset(segments, strtabAddress, strtabOffset)) {
        std::cerr << "Error: Failed to resolve dynamic string table" << '\n';
        return;
    }

    std::vector<char> strtab(strtabSize);
    file.clear();
    file.seekg(strtabOffset, std::ios::beg);
    file.read(strtab.data(), strtab.size());

    if (file.gcount() != static_cast<std::streamsize>(strtab.size())) {
        std::cerr << "Error: Failed to read dynamic string table" << '\n';
        return;
    }

    bool foundAny = false;

    for (const auto& entry : entries) {
        if (entry.d_tag == elf::DT_SONAME) {
            std::cout << "  soname: " << dynamicString(strtab, entry.d_un.d_val) << '\n';
            foundAny = true;
        }
    }

    for (const auto& entry : entries) {
        if (entry.d_tag == elf::DT_NEEDED) {
            std::cout << "  needed: " << dynamicString(strtab, entry.d_un.d_val) << '\n';
            foundAny = true;
        }
    }

    if (!foundAny) {
        std::cout << "  No shared object dependencies" << '\n';
    }
}

void parseSegments(std::ifstream& file, const elf::Elf64_Ehdr& header) {
    if (!checkSegmetns(header.e_phnum, header.e_phoff)) {
        std::cerr << "Error: Didn't find any segments" << '\n';
        return;
    }

    std::cout << "Segments quantity: " << header.e_phnum << '\n';

    std::vector<elf::Elf64_Phdr> segments = readProgramHeaders(file, header);
    if (segments.empty()) {
        std::cerr << "Error: Failed to read program headers" << '\n';
        return;
    }

    if (header.e_type == elf::ET_DYN) {
        std::cout << "Object type: shared object (.so) or PIE" << '\n';
    }

    printSegments(segments);
    printSharedObjects(file, segments);
}

void parseSegments(std::ifstream& file, const elf::Elf32_Ehdr& header) {
    if (!checkSegmetns(header.e_phnum, header.e_phoff)) {
        std::cerr << "Error: Didn't find any segments" << '\n';
        return;
    }

    std::cout << "Segments quantity: " << header.e_phnum << '\n';

    std::vector<elf::Elf32_Phdr> segments = readProgramHeaders(file, header);
    if (segments.empty()) {
        std::cerr << "Error: Failed to read program headers" << '\n';
        return;
    }

    if (header.e_type == elf::ET_DYN) {
        std::cout << "Object type: shared object (.so) or PIE" << '\n';
    }

    printSegments(segments);
    printSharedObjects(file, segments);
}
