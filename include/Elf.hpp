#pragma once

#include <cstdint>

namespace elf {

// Standard ELF types

using Elf32_Half    = uint16_t;
using Elf64_Half    = uint16_t;

using Elf32_Word    = uint32_t;
using Elf32_Sword   = int32_t;
using Elf64_Word    = uint32_t;
using Elf64_Sword   = int32_t;

using Elf32_Xword   = uint64_t;
using Elf32_Sxword  = int64_t;
using Elf64_Xword   = uint64_t;
using Elf64_Sxword  = int64_t;

using Elf32_Addr    = uint32_t;
using Elf64_Addr    = uint64_t;

using Elf32_Off     = uint32_t;
using Elf64_Off     = uint64_t;

using Elf32_Section = uint16_t;
using Elf64_Section = uint16_t;

// ELF file header

constexpr int EI_NIDENT = 16;

struct Elf32_Ehdr {
    unsigned char e_ident[EI_NIDENT]; // Magic number and other info
    Elf32_Half    e_type;             // Object file type
    Elf32_Half    e_machine;          // Architecture
    Elf32_Word    e_version;          // Object file version
    Elf32_Addr    e_entry;            // Entry point virtual address
    Elf32_Off     e_phoff;            // Program header table file offset
    Elf32_Off     e_shoff;            // Section header table file offset
    Elf32_Word    e_flags;            // Processor-specific flags
    Elf32_Half    e_ehsize;           // ELF header size in bytes
    Elf32_Half    e_phentsize;        // Program header table entry size
    Elf32_Half    e_phnum;            // Program header table entry count
    Elf32_Half    e_shentsize;        // Section header table entry size
    Elf32_Half    e_shnum;            // Section header table entry count
    Elf32_Half    e_shstrndx;         // Section header string table index
};

struct Elf64_Ehdr {
    unsigned char e_ident[EI_NIDENT];
    Elf64_Half    e_type;
    Elf64_Half    e_machine;
    Elf64_Word    e_version;
    Elf64_Addr    e_entry;
    Elf64_Off     e_phoff;
    Elf64_Off     e_shoff;
    Elf64_Word    e_flags;
    Elf64_Half    e_ehsize;
    Elf64_Half    e_phentsize;
    Elf64_Half    e_phnum;
    Elf64_Half    e_shentsize;
    Elf64_Half    e_shnum;
    Elf64_Half    e_shstrndx;
};

// e_ident indices and magic bytes

constexpr int EI_MAG0 = 0;
constexpr unsigned char ELFMAG0 = 0x7f;

constexpr int EI_MAG1 = 1;
constexpr unsigned char ELFMAG1 = 'E';

constexpr int EI_MAG2 = 2;
constexpr unsigned char ELFMAG2 = 'L';

constexpr int EI_MAG3 = 3;
constexpr unsigned char ELFMAG3 = 'F';

// File class (e_ident[EI_CLASS])

constexpr int EI_CLASS      = 4;
constexpr int ELFCLASSNONE  = 0;
constexpr int ELFCLASS32    = 1;
constexpr int ELFCLASS64    = 2;

// Data encoding (e_ident[EI_DATA])

constexpr int EI_DATA       = 5;
constexpr int ELFDATANONE   = 0;
constexpr int ELFDATA2LSB   = 1; // Little endian
constexpr int ELFDATA2MSB   = 2; // Big endian

// File version (e_ident[EI_VERSION])

constexpr int EI_VERSION    = 6;
constexpr int EV_NONE       = 0;
constexpr int EV_CURRENT    = 1;

// OS ABI identification (e_ident[EI_OSABI])

constexpr int EI_OSABI          = 7;
constexpr int ELFOSABI_NONE     = 0; // UNIX System V ABI
constexpr int ELFOSABI_SYSV     = 0;
constexpr int ELFOSABI_GNU      = 3;
constexpr int ELFOSABI_LINUX    = 3;
constexpr int ELFOSABI_FREEBSD  = 9;

constexpr int EI_ABIVERSION     = 8;
constexpr int EI_PAD            = 9;

// Object file types (e_type)

constexpr Elf64_Half ET_NONE    = 0; // No file type
constexpr Elf64_Half ET_REL     = 1; // Relocatable file (.o)
constexpr Elf64_Half ET_EXEC    = 2; // Executable file
constexpr Elf64_Half ET_DYN     = 3; // Shared object file (.so) or PIE
constexpr Elf64_Half ET_CORE    = 4; // Core file

// Architectures (e_machine)

constexpr Elf64_Half EM_NONE    = 0;
constexpr Elf64_Half EM_386     = 3;   // Intel 80386 (x86)
constexpr Elf64_Half EM_ARM     = 40;  // ARM 32-bit
constexpr Elf64_Half EM_X86_64  = 62;  // AMD x86-64
constexpr Elf64_Half EM_AARCH64 = 183; // ARM 64-bit
constexpr Elf64_Half EM_RISCV   = 243; // RISC-V

// Section headers

struct Elf32_Shdr {
    Elf32_Word sh_name;      // Section name (string table offset)
    Elf32_Word sh_type;      // Section type
    Elf32_Word sh_flags;     // Section flags
    Elf32_Addr sh_addr;      // Section virtual addr at execution
    Elf32_Off  sh_offset;    // Section file offset
    Elf32_Word sh_size;      // Section size in bytes
    Elf32_Word sh_link;      // Link to another section
    Elf32_Word sh_info;      // Additional section information
    Elf32_Word sh_addralign; // Section alignment
    Elf32_Word sh_entsize;   // Entry size if section holds table
};

struct Elf64_Shdr {
    Elf64_Word  sh_name;      // Section name (byte offset into the section header string table)
    Elf64_Word  sh_type;      // Section type (e.g., SHT_PROGBITS, SHT_SYMTAB, SHT_STRTAB, SHT_NOBITS)
    Elf64_Xword sh_flags;     // Bitmask of section attributes (e.g., SHF_WRITE, SHF_ALLOC, SHF_EXECINSTR)
    Elf64_Addr  sh_addr;      // Virtual address in memory during execution (0 if not loaded into memory)
    Elf64_Off   sh_offset;    // Offset in bytes from the beginning of the file to the section data
    Elf64_Xword sh_size;      // Size of the section in bytes (0 for SHT_NOBITS on disk, but non-zero in memory)
    Elf64_Word  sh_link;      // Section header table index link (interpretation depends on section type)
    Elf64_Word  sh_info;      // Extra section information (interpretation depends on section type)
    Elf64_Xword sh_addralign; // Address alignment constraint (must be a power of 2; 0 or 1 means no alignment)
    Elf64_Xword sh_entsize;   // Size in bytes of each entry if the section holds a table of fixed-size entries
};


// Special section indices

constexpr Elf64_Section SHN_UNDEF     = 0;
constexpr Elf64_Section SHN_LORESERVE = 0xff00;
constexpr Elf64_Section SHN_ABS       = 0xfff1;
constexpr Elf64_Section SHN_COMMON    = 0xfff2;
constexpr Elf64_Section SHN_XINDEX    = 0xffff;

// Section types (sh_type)

constexpr Elf64_Word SHT_NULL          = 0;  // Section header table entry unused
constexpr Elf64_Word SHT_PROGBITS      = 1;  // Program data (.text, .data, .rodata)
constexpr Elf64_Word SHT_SYMTAB        = 2;  // Symbol table
constexpr Elf64_Word SHT_STRTAB        = 3;  // String table
constexpr Elf64_Word SHT_RELA          = 4;  // Relocation entries with addends
constexpr Elf64_Word SHT_HASH          = 5;  // Symbol hash table
constexpr Elf64_Word SHT_DYNAMIC       = 6;  // Dynamic linking information
constexpr Elf64_Word SHT_NOTE          = 7;  // Notes
constexpr Elf64_Word SHT_NOBITS        = 8;  // Program space with no data (.bss)
constexpr Elf64_Word SHT_REL           = 9;  // Relocation entries, no addends
constexpr Elf64_Word SHT_DYNSYM        = 11; // Dynamic linker symbol table
constexpr Elf64_Word SHT_INIT_ARRAY    = 14; // Array of constructors
constexpr Elf64_Word SHT_FINI_ARRAY    = 15; // Array of destructors
constexpr Elf64_Word SHT_PREINIT_ARRAY = 16; // Array of pre-constructors
constexpr Elf64_Word SHT_GROUP         = 17; // Section group
constexpr Elf64_Word SHT_SYMTAB_SHNDX  = 18; // Extended section indices

// Section flags (sh_flags)

constexpr Elf64_Xword SHF_WRITE     = (1 << 0);  // Writable
constexpr Elf64_Xword SHF_ALLOC     = (1 << 1);  // Occupies memory during execution
constexpr Elf64_Xword SHF_EXECINSTR = (1 << 2);  // Executable
constexpr Elf64_Xword SHF_MERGE     = (1 << 4);  // Might be merged
constexpr Elf64_Xword SHF_STRINGS   = (1 << 5);  // Contains null-terminated strings
constexpr Elf64_Xword SHF_INFO_LINK = (1 << 6);  // sh_info contains SHT index
constexpr Elf64_Xword SHF_TLS       = (1 << 10); // Thread-local data

// Symbol table entries

struct Elf32_Sym {
    Elf32_Word    st_name;  // Symbol name (string table offset)
    Elf32_Addr    st_value; // Symbol value (address)
    Elf32_Word    st_size;  // Symbol size
    unsigned char st_info;  // Symbol type and binding
    unsigned char st_other; // Symbol visibility
    Elf32_Section st_shndx; // Section index
};

struct Elf64_Sym {
    Elf64_Word    st_name;
    unsigned char st_info;
    unsigned char st_other;
    Elf64_Section st_shndx;
    Elf64_Addr    st_value;
    Elf64_Xword   st_size;
};

// Helpers to extract symbol binding, type, and visibility

constexpr unsigned char getSymBind(unsigned char info) { return info >> 4; }
constexpr unsigned char getSymType(unsigned char info) { return info & 0xf; }
constexpr unsigned char getSymVisibility(unsigned char other) { return other & 0x03; }

// Symbol binding (ST_BIND)

constexpr unsigned char STB_LOCAL  = 0; // Local symbol
constexpr unsigned char STB_GLOBAL = 1; // Global symbol
constexpr unsigned char STB_WEAK   = 2; // Weak symbol

// Symbol types (ST_TYPE)

constexpr unsigned char STT_NOTYPE  = 0; // Unspecified
constexpr unsigned char STT_OBJECT  = 1; // Data object
constexpr unsigned char STT_FUNC    = 2; // Code object (function)
constexpr unsigned char STT_SECTION = 3; // Section
constexpr unsigned char STT_FILE    = 4; // Source file name
constexpr unsigned char STT_COMMON  = 5; // Common data object
constexpr unsigned char STT_TLS     = 6; // Thread-local data object

// Symbol visibility

constexpr unsigned char STV_DEFAULT   = 0;
constexpr unsigned char STV_INTERNAL  = 1;
constexpr unsigned char STV_HIDDEN    = 2;
constexpr unsigned char STV_PROTECTED = 3;

// Program segment headers

struct Elf32_Phdr {
    Elf32_Word p_type;   // Segment type
    Elf32_Off  p_offset; // Segment file offset
    Elf32_Addr p_vaddr;  // Segment virtual address
    Elf32_Addr p_paddr;  // Segment physical address
    Elf32_Word p_filesz; // Segment size in file
    Elf32_Word p_memsz;  // Segment size in memory
    Elf32_Word p_flags;  // Segment flags
    Elf32_Word p_align;  // Segment alignment
};

struct Elf64_Phdr {
    Elf64_Word  p_type;
    Elf64_Word  p_flags;
    Elf64_Off   p_offset;
    Elf64_Addr  p_vaddr;
    Elf64_Addr  p_paddr;
    Elf64_Xword p_filesz;
    Elf64_Xword p_memsz;
    Elf64_Xword p_align;
};

// Segment types (p_type)

constexpr Elf64_Word PT_NULL         = 0;          // Unused entry
constexpr Elf64_Word PT_LOAD         = 1;          // Loadable program segment
constexpr Elf64_Word PT_DYNAMIC      = 2;          // Dynamic linking information
constexpr Elf64_Word PT_INTERP       = 3;          // Program interpreter path
constexpr Elf64_Word PT_NOTE         = 4;          // Auxiliary information
constexpr Elf64_Word PT_SHLIB        = 5;          // Reserved
constexpr Elf64_Word PT_PHDR         = 6;          // Entry for header table itself
constexpr Elf64_Word PT_TLS          = 7;          // Thread-local storage segment
constexpr Elf64_Word PT_GNU_EH_FRAME = 0x6474e550; // GCC .eh_frame_hdr segment
constexpr Elf64_Word PT_GNU_STACK    = 0x6474e551; // Stack executability
constexpr Elf64_Word PT_GNU_RELRO    = 0x6474e552; // Read-only after relocation
// Segment flags (p_flags)
constexpr Elf64_Word PF_X = (1 << 0); // Executable
constexpr Elf64_Word PF_W = (1 << 1); // Writable
constexpr Elf64_Word PF_R = (1 << 2); // Readable
// Dynamic section entries
struct Elf32_Dyn {
Elf32_Sword d_tag;
union {
Elf32_Word d_val;
Elf32_Addr d_ptr;
} d_un;
};
struct Elf64_Dyn {
Elf64_Sxword d_tag;
union {
Elf64_Xword d_val;
Elf64_Addr  d_ptr;
} d_un;
};
// Dynamic entry tags (d_tag)
constexpr Elf64_Sxword DT_NULL     = 0;  // Marks end of dynamic section
constexpr Elf64_Sxword DT_NEEDED   = 1;  // Name of needed library
constexpr Elf64_Sxword DT_PLTRELSZ = 2;  // Size in bytes of PLT relocs
constexpr Elf64_Sxword DT_PLTGOT   = 3;  // Processor defined value
constexpr Elf64_Sxword DT_HASH     = 4;  // Address of symbol hash table
constexpr Elf64_Sxword DT_STRTAB   = 5;  // Address of string table
constexpr Elf64_Sxword DT_SYMTAB   = 6;  // Address of symbol table
constexpr Elf64_Sxword DT_STRSZ    = 10; // Size of string table
constexpr Elf64_Sxword DT_SYMENT   = 11; // Size of one symbol table entry
constexpr Elf64_Sxword DT_INIT     = 12; // Address of init function
constexpr Elf64_Sxword DT_FINI     = 13; // Address of termination function
constexpr Elf64_Sxword DT_SONAME   = 14; // Name of shared object
constexpr Elf64_Sxword DT_RPATH    = 15; // Library search path (deprecated)
constexpr Elf64_Sxword DT_RUNPATH  = 29; // Library search path
} // namespace elf
