/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgama <mgama@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/02 19:00:38 by mgama             #+#    #+#             */
/*   Updated: 2025/07/01 12:21:59 by mgama            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ELF_H
#define ELF_H

#include <stdint.h>

enum
{
	ELF_32BITS = 1,
	ELF_64BITS = 2
};

#define ET_NONE		0		/* No file type */
#define ET_REL		1		/* Relocatable file */
#define ET_EXEC		2		/* Executable file */
#define ET_DYN		3		/* Shared object file */
#define ET_CORE		4		/* Core file */
#define	ET_NUM		5		/* Number of defined types */
#define ET_LOOS		0xfe00	/* OS-specific range start */
#define ET_HIOS		0xfeff	/* OS-specific range end */
#define ET_LOPROC	0xff00	/* Processor-specific range start */
#define ET_HIPROC	0xffff	/* Processor-specific range end */

#define	PT_NULL			0		/* Program header table entry unused */
#define PT_LOAD			1		/* Loadable program segment */
#define PT_DYNAMIC		2		/* Dynamic linking information */
#define PT_INTERP		3		/* Program interpreter */
#define PT_NOTE			4		/* Auxiliary information */
#define PT_SHLIB		5		/* Reserved */
#define PT_PHDR			6		/* Entry for header table itself */
#define PT_TLS			7		/* Thread-local storage segment */
#define	PT_NUM			8		/* Number of defined types */
#define PT_LOOS			0x60000000	/* Start of OS-specific */
#define PT_GNU_EH_FRAME	0x6474e550	/* GCC .eh_frame_hdr segment */
#define PT_GNU_STACK	0x6474e551	/* Indicates stack executability */
#define PT_GNU_RELRO	0x6474e552	/* Read-only after relocation */
#define PT_GNU_PROPERTY	0x6474e553
#define PT_LOSUNW		0x6ffffffa
#define PT_SUNWBSS		0x6ffffffa	/* Sun Specific segment */
#define PT_SUNWSTACK	0x6ffffffb	/* Stack segment */
#define PT_HISUNW		0x6fffffff
#define PT_HIOS			0x6fffffff	/* End of OS-specific */
#define PT_LOPROC		0x70000000	/* Start of processor-specific */
#define PT_HIPROC		0x7fffffff	/* End of processor-specific */

typedef struct
{
	uint32_t	p_type;
	uint32_t	p_flags;
	uint64_t	p_offset;
	uint64_t	p_vaddr;
	uint64_t	p_paddr;
	uint64_t	p_filesz;
	uint64_t	p_memsz;
	uint64_t	p_align;
} t_elf64_program_header;

typedef struct
{
	uint32_t p_type;
	uint32_t p_offset;
	uint32_t p_vaddr;
	uint32_t p_paddr;
	uint32_t p_filesz;
	uint32_t p_memsz;
	uint32_t p_flags;
	uint32_t p_align;
} t_elf32_program_header;

typedef struct
{
	uint32_t	sh_name_offset;
	uint32_t	sh_type;
	uint64_t	sh_flags;
	uint64_t	sh_addr;
	uint64_t	sh_offset;
	uint64_t	sh_size;
	uint32_t	sh_link;
	uint32_t	sh_info;
	uint64_t	sh_addralign;
	uint64_t	sh_entsize;
	uint8_t		*data;
	char		*sh_name;
} t_elf64_section_header;

typedef struct
{
	uint32_t	sh_name_offset;
	uint32_t	sh_type;
	uint32_t	sh_flags;
	uint32_t	sh_addr;
	uint32_t	sh_offset;
	uint32_t	sh_size;
	uint32_t	sh_link;
	uint32_t	sh_info;
	uint32_t	sh_addralign;
	uint32_t	sh_entsize;
	uint8_t		*data;
	char		*sh_name;
} t_elf32_section_header;


#define SHT_NULL			0		/* Section header table entry unused */
#define SHT_PROGBITS		1		/* Program data */
#define SHT_SYMTAB			2		/* Symbol table */
#define SHT_STRTAB			3		/* String table */
#define SHT_RELA			4		/* Relocation entries with addends */
#define SHT_HASH			5		/* Symbol hash table */
#define SHT_DYNAMIC			6		/* Dynamic linking information */
#define SHT_NOTE			7		/* Notes */
#define SHT_NOBITS			8		/* Program space with no data (bss) */
#define SHT_REL				9		/* Relocation entries, no addends */
#define SHT_SHLIB			10		/* Reserved */
#define SHT_DYNSYM			11		/* Dynamic linker symbol table */
#define SHT_INIT_ARRAY		14		/* Array of constructors */
#define SHT_FINI_ARRAY		15		/* Array of destructors */
#define SHT_PREINIT_ARRAY	16		/* Array of pre-constructors */
#define SHT_GROUP			17		/* Section group */
#define SHT_SYMTAB_SHNDX	18		/* Extended section indeces */
#define	SHT_NUM				19		/* Number of defined types.  */
#define SHT_LOOS			0x60000000	/* Start OS-specific.  */
#define SHT_GNU_ATTRIBUTES	0x6ffffff5	/* Object attributes.  */
#define SHT_GNU_HASH		0x6ffffff6	/* GNU-style hash table.  */
#define SHT_GNU_LIBLIST		0x6ffffff7	/* Prelink library list */
#define SHT_CHECKSUM		0x6ffffff8	/* Checksum for DSO content.  */
#define SHT_LOSUNW			0x6ffffffa	/* Sun-specific low bound.  */
#define SHT_SUNW_move		0x6ffffffa
#define SHT_SUNW_COMDAT		0x6ffffffb
#define SHT_SUNW_syminfo	0x6ffffffc
#define SHT_GNU_verdef		0x6ffffffd	/* Version definition section.  */
#define SHT_GNU_verneed		0x6ffffffe	/* Version needs section.  */
#define SHT_GNU_versym		0x6fffffff	/* Version symbol table.  */
#define SHT_HISUNW			0x6fffffff	/* Sun-specific high bound.  */
#define SHT_HIOS			0x6fffffff	/* End OS-specific type */
#define SHT_LOPROC			0x70000000	/* Start of processor-specific */
#define SHT_HIPROC			0x7fffffff	/* End of processor-specific */
#define SHT_LOUSER			0x80000000	/* Start of application-specific */
#define SHT_HIUSER			0x8fffffff	/* End of application-specific */

/* Legal values for sh_flags (section flags).  */

#define SHF_WRITE				(1 << 0)	/* Writable */
#define SHF_ALLOC				(1 << 1)	/* Occupies memory during execution */
#define SHF_EXECINSTR			(1 << 2)	/* Executable */
#define SHF_MERGE				(1 << 4)	/* Might be merged */
#define SHF_STRINGS				(1 << 5)	/* Contains nul-terminated strings */
#define SHF_INFO_LINK			(1 << 6)	/* `sh_info' contains SHT index */
#define SHF_LINK_ORDER			(1 << 7)	/* Preserve order after combining */
#define SHF_OS_NONCONFORMING	(1 << 8)	/* Non-standard OS specific handling */

/* Legal values for p_flags (segment flags).  */

#define PF_X		(1 << 0)	/* Segment is executable */
#define PF_W		(1 << 1)	/* Segment is writable */
#define PF_R		(1 << 2)	/* Segment is readable */
#define PF_MASKOS	0x0ff00000	/* OS-specific */
#define PF_MASKPROC	0xf0000000	/* Processor-specific */

typedef struct
{
	uint32_t	st_name;
	uint8_t		st_info;
	uint8_t		st_other;
	uint16_t	st_shndx;
	uint64_t	st_value;
	uint64_t	st_size;
} t_elf64_sym;

typedef struct
{
	uint32_t st_name;
	uint32_t st_value;
	uint32_t st_size;
	uint8_t  st_info;
	uint8_t  st_other;
	uint16_t st_shndx;
} t_elf32_sym;

typedef struct
{
	uint16_t	vn_version;     // Version (doit être 1)
	uint16_t	vn_cnt;         // Nombre d'entrées Vernaux
	uint32_t	vn_file;        // Offset vers le nom de la lib (dans dynstr)
	uint32_t	vn_aux;         // Offset relatif vers le premier Vernaux
	uint32_t	vn_next;        // Offset relatif vers la prochaine Verneed
} t_elf_verneed;

typedef struct
{
	uint32_t	vna_hash;       // Hash du nom de version
	uint16_t	vna_flags;      // Pas souvent utilisé
	uint16_t	vna_other;      // Index utilisé dans .gnu.version
	uint32_t	vna_name;       // Offset vers nom de version (dans dynstr)
	uint32_t	vna_next;       // Offset relatif vers le prochain Vernaux
} t_elf_vernaux;

enum
{
	STB_LOCAL	= 0,	/* Local symbol */
	STB_GLOBAL	= 1,	/* Global symbol */
	STB_WEAK	= 2,	/* Weak symbol */
	STB_LOOS	= 10,	/* Start of OS-specific bindings */
	STB_HIOS	= 12,	/* End of OS-specific bindings */
	STB_LOPROC	= 13,	/* Start of processor-specific bindings */
	STB_HIPROC	= 15,	/* End of processor-specific bindings */
};

enum
{
	STT_NOTYPE	= 0,	/* No type */
	STT_OBJECT	= 1,	/* Data object */
	STT_FUNC	= 2,	/* Function or other executable code */
	STT_SECTION	= 3,	/* Symbol is a section */
	STT_FILE	= 4,	/* Symbol's name is file name */
	STT_LOOS	= 10,	/* Start of OS-specific types */
	STT_HIOS	= 12,	/* End of OS-specific types */
	STT_LOPROC	= 13,	/* Start of processor-specific types */
	STT_HIPROC	= 15,	/* End of processor-specific types */
	STT_GNU_IFUNC = 10, /* GNU extension for indirect functions */
};

enum
{
	SHN_UNDEF	= 0,		/* Undefined section */
	SHN_LORESERVE = 0xff00, /* Start of reserved indices */
	SHN_LOPROC	= 0xff00, 	/* Start of processor-specific indices */
	SHN_HIPROC	= 0xff1f, 	/* End of processor-specific indices */
	SHN_ABS		= 0xfff1, 	/* Absolute values */
	SHN_COMMON	= 0xfff2, 	/* Common symbols */
	SHN_XINDEX	= 0xffff, 	/* Extended index */
};

#define ELF64_ST_BIND(i)	((i)>>4)
#define ELF64_ST_TYPE(i)	((i)&0xf)
#define ELF64_ST_INFO(b,t)	(((b)<<4)+((t)&0xf))

#define ELF32_ST_BIND(i)	((i) >> 4)
#define ELF32_ST_TYPE(i)	((i) & 0xf)
#define ELF32_ST_INFO(b,t)	(((b) << 4) + ((t) & 0xf))

enum
{
	STV_DEFAULT		= 0,
	STV_INTERNAL	= 1,
	STV_HIDDEN		= 2,
	STV_PROTECTED	= 3,
};

typedef union
{
	struct
	{
		uint32_t ei_magic;       // magic number
		uint8_t  ei_class;       // 32 bits or 64 bits
		uint8_t  ei_data;        // little or big endian
		uint8_t  ei_version;     // elf version
		uint8_t  ei_osabi;       // operating system target
		uint8_t  ei_abi_version; // abi version
		char     ei_pad[7];      // Reserved padding bytes
	};
	uint8_t raw[16];
} t_elf_ident;

typedef struct {
	t_elf_ident	e_ident;
	uint16_t	e_type;						// object type
	uint16_t	e_machine;					// machine type
	uint32_t	e_version;					// object version
	uint64_t	e_entry;					// address where the execution starts
	uint64_t	e_phoff;					// program headers' offset
	uint64_t	e_shoff;					// section headers' offset
	uint32_t	e_flags;					// architecture-specific flags
	uint16_t	e_ehsize;					// elf header size
	uint16_t	e_phentsize; 				// size of a single program header
	uint16_t	e_phnum; 					// count of program headers
	uint16_t	e_shentsize;				// size of single section header
	uint16_t	e_shnum; 					// count of section headers
	uint16_t	e_shstrndx;					// index of name's section in the table

	t_elf64_program_header	*ph64;
	t_elf64_section_header	*sh64;
} t_elf64_file;

typedef struct {
	t_elf_ident	e_ident;
	uint16_t e_type;            			// Object file type
	uint16_t e_machine;         			// Machine type
	uint32_t e_version;         			// Object file version
	uint32_t e_entry;           			// Entry point address
	uint32_t e_phoff;           			// Program header offset
	uint32_t e_shoff;           			// Section header offset
	uint32_t e_flags;           			// Processor-specific flags
	uint16_t e_ehsize;          			// ELF header size
	uint16_t e_phentsize;       			// Size of program header entry
	uint16_t e_phnum;           			// Number of program header entries
	uint16_t e_shentsize;       			// Size of section header entry
	uint16_t e_shnum;           			// Number of section header entries
	uint16_t e_shstrndx;        			// Section name string table index

	t_elf32_program_header	*ph32;
	t_elf32_section_header	*sh32;
} t_elf32_file;

#define ELF64_HEADER_SIZE (sizeof(t_elf64_file) - sizeof(t_elf64_program_header *) - sizeof(t_elf64_section_header *))
#define ELF64_PROGRAM_HEADER_SIZE sizeof(t_elf64_program_header)
#define ELF64_SECTION_HEADER_SIZE (sizeof(t_elf64_section_header) - sizeof(char *) - sizeof(uint8_t *))

#define ELF32_HEADER_SIZE (sizeof(t_elf32_file) - sizeof(t_elf32_program_header *) - sizeof(t_elf32_section_header *))
#define ELF32_PROGRAM_HEADER_SIZE sizeof(t_elf32_program_header)
#define ELF32_SECTION_HEADER_SIZE (sizeof(t_elf32_section_header) - sizeof(char *) - sizeof(uint8_t *))

#define ELF_MAGIC 0x7F454C46

// Check endianness and swap bytes if needed
#define MAGIC(x, e)	(e == LITTLE_ENDIAN ? x : ((x >> 24) & 0xff) | ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000) | ((x << 24) & 0xff000000))

#define ELF_CLASS(ptr) (((t_elf_ident *)ptr)->ei_class)
#define ELF_SYM_VALUE(e, idx) ((e)->e_ident.ei_class == ELF_64BITS ? (e)->sym64[idx].st_value : (e)->sym32[idx].st_value)

#endif /* ELF_H */