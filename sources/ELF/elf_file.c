/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf_file.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgama <mgama@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 16:11:26 by mbrement          #+#    #+#             */
/*   Updated: 2024/04/24 17:09:42 by mbrement         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

static int
get_elf64_program_headers(t_elf64_file *elf_file, t_binary_reader *reader)
{
	ft_bverbose("\nReading ELF64 program headers...\n");
	reader->seek(reader, elf_file->e_phoff);

	elf_file->ph64 = ft_calloc(elf_file->e_phnum, sizeof(t_elf64_program_header));
	if (!elf_file->ph64)
		return ft_dverbose(STDERR_FILENO, "Could not allocate memory\n"), -1;

	for (int i = 0; i < elf_file->e_phnum; i++)
	{
		elf_file->ph64[i].p_type   = reader->get_uint32(reader);
		elf_file->ph64[i].p_flags  = reader->get_uint32(reader);
		elf_file->ph64[i].p_offset = reader->get_uint64(reader);
		elf_file->ph64[i].p_vaddr  = reader->get_uint64(reader);
		elf_file->ph64[i].p_paddr  = reader->get_uint64(reader);
		elf_file->ph64[i].p_filesz = reader->get_uint64(reader);
		elf_file->ph64[i].p_memsz  = reader->get_uint64(reader);
		elf_file->ph64[i].p_align  = reader->get_uint64(reader);

		if (elf_file->ph64[i].p_offset > reader->size
			|| elf_file->ph64[i].p_filesz > reader->size
			|| elf_file->ph64[i].p_memsz < elf_file->ph64[i].p_filesz
			|| elf_file->ph64[i].p_align < 1)
			return ft_dverbose(STDERR_FILENO, "Incoherent program header values\n"), -1;
	}

	ft_bverbose("Program header contains %d entries\n", elf_file->e_phnum);
	return 0;
}

static int
get_elf64_section_headers(t_elf64_file *elf_file, t_binary_reader *reader)
{
	ft_bverbose("\nReading ELF64 section headers...\n");
	reader->seek(reader, elf_file->e_shoff);

	elf_file->sh64 = ft_calloc(elf_file->e_shnum, sizeof(t_elf64_section_header));
	if (!elf_file->sh64)
		return ft_dverbose(STDERR_FILENO, "Could not allocate memory\n"), -1;

	for (int i = 0; i < elf_file->e_shnum; i++)
	{
		elf_file->sh64[i].sh_name_offset = reader->get_uint32(reader);
		elf_file->sh64[i].sh_type        = reader->get_uint32(reader);
		elf_file->sh64[i].sh_flags       = reader->get_uint64(reader);
		elf_file->sh64[i].sh_addr        = reader->get_uint64(reader);
		elf_file->sh64[i].sh_offset      = reader->get_uint64(reader);
		elf_file->sh64[i].sh_size        = reader->get_uint64(reader);
		elf_file->sh64[i].sh_link        = reader->get_uint32(reader);
		elf_file->sh64[i].sh_info        = reader->get_uint32(reader);
		elf_file->sh64[i].sh_addralign   = reader->get_uint64(reader);
		elf_file->sh64[i].sh_entsize     = reader->get_uint64(reader);

		if (elf_file->sh64[i].sh_type == SHT_NULL)
			continue;
		if (elf_file->sh64[i].sh_offset > reader->size
			|| elf_file->sh64[i].sh_size > reader->size
			|| elf_file->sh64[i].sh_addralign < 1
		)
		{
			return (ft_dverbose(STDERR_FILENO, "Could not read file because of incoeherent values\n"), -1);
		}
	}

	ft_bverbose("Section table contains %d entries\n", elf_file->e_shnum);
	return 0;
}

static int
get_elf32_program_headers(t_elf32_file *elf_file, t_binary_reader *reader)
{
	ft_bverbose("\nReading ELF32 program headers...\n");
	reader->seek(reader, elf_file->e_phoff);

	elf_file->ph32 = ft_calloc(elf_file->e_phnum, sizeof(t_elf32_program_header));
	if (!elf_file->ph32)
		return ft_dverbose(STDERR_FILENO, "Could not allocate memory\n"), -1;

	for (int i = 0; i < elf_file->e_phnum; i++)
	{
		elf_file->ph32[i].p_type   = reader->get_uint32(reader);
		elf_file->ph32[i].p_offset = reader->get_uint32(reader);
		elf_file->ph32[i].p_vaddr  = reader->get_uint32(reader);
		elf_file->ph32[i].p_paddr  = reader->get_uint32(reader);
		elf_file->ph32[i].p_filesz = reader->get_uint32(reader);
		elf_file->ph32[i].p_memsz  = reader->get_uint32(reader);
		elf_file->ph32[i].p_flags  = reader->get_uint32(reader);
		elf_file->ph32[i].p_align  = reader->get_uint32(reader);

		if (elf_file->ph32[i].p_offset > reader->size
			|| elf_file->ph32[i].p_filesz > reader->size
			|| elf_file->ph32[i].p_memsz < elf_file->ph32[i].p_filesz
			|| elf_file->ph32[i].p_align < 1)
			return ft_dverbose(STDERR_FILENO, "Incoherent program header values\n"), -1;
	}

	ft_bverbose("Program header contains %d entries\n", elf_file->e_phnum);
	return 0;
}

static int
get_elf32_section_headers(t_elf32_file *elf_file, t_binary_reader *reader)
{
	ft_bverbose("\nReading ELF32 section headers...\n");
	reader->seek(reader, elf_file->e_shoff);

	elf_file->sh32 = ft_calloc(elf_file->e_shnum, sizeof(t_elf32_section_header));
	if (!elf_file->sh32)
		return ft_dverbose(STDERR_FILENO, "Could not allocate memory\n"), -1;

	for (int i = 0; i < elf_file->e_shnum; i++)
	{
		elf_file->sh32[i].sh_name_offset = reader->get_uint32(reader);
		elf_file->sh32[i].sh_type        = reader->get_uint32(reader);
		elf_file->sh32[i].sh_flags       = reader->get_uint32(reader);
		elf_file->sh32[i].sh_addr        = reader->get_uint32(reader);
		elf_file->sh32[i].sh_offset      = reader->get_uint32(reader);
		elf_file->sh32[i].sh_size        = reader->get_uint32(reader);
		elf_file->sh32[i].sh_link        = reader->get_uint32(reader);
		elf_file->sh32[i].sh_info        = reader->get_uint32(reader);
		elf_file->sh32[i].sh_addralign   = reader->get_uint32(reader);
		elf_file->sh32[i].sh_entsize     = reader->get_uint32(reader);

		if (elf_file->sh32[i].sh_type == SHT_NULL)
			continue;
		if (elf_file->sh32[i].sh_offset > reader->size
			|| elf_file->sh32[i].sh_size > reader->size
			|| elf_file->sh32[i].sh_addralign < 1
		)
		{
			return (ft_dverbose(STDERR_FILENO, "Could not read file because of incoeherent values\n"), -1);
		}
	}

	ft_bverbose("Section table contains %d entries\n", elf_file->e_shnum);
	return 0;
}

t_elf64_file*
new_elf64_file(t_binary_reader *reader, t_elf_ident elf_file_ident)
{
	t_elf64_file *elf_file = ft_calloc(1, sizeof(t_elf64_file));
	if (elf_file == NULL)
		return (ft_dverbose(STDERR_FILENO, "Could not allocate memory\n"), NULL);

	ft_memcpy(elf_file->e_ident.raw, elf_file_ident.raw, sizeof(t_elf_ident));

	reader->seek(reader, 0x10);
	elf_file->e_type = reader->get_uint16(reader);
	elf_file->e_machine = reader->get_uint16(reader);
	elf_file->e_version = reader->get_uint32(reader);

	elf_file->e_entry = reader->get_uint64(reader);
	elf_file->e_phoff = reader->get_uint64(reader);
	elf_file->e_shoff = reader->get_uint64(reader);

	elf_file->e_flags = reader->get_uint32(reader);
	elf_file->e_ehsize = reader->get_uint16(reader);
	elf_file->e_phentsize = reader->get_uint16(reader);
	elf_file->e_phnum = reader->get_uint16(reader);
	elf_file->e_shentsize = reader->get_uint16(reader);
	elf_file->e_shnum = reader->get_uint16(reader);
	elf_file->e_shstrndx = reader->get_uint16(reader);

	if (elf_file->e_phoff > reader->size
		|| elf_file->e_shoff > reader->size
		|| (elf_file->e_ehsize != 0 && elf_file->e_ehsize != ELF64_HEADER_SIZE)
		|| (elf_file->e_phentsize != 0 && elf_file->e_phentsize != ELF64_PROGRAM_HEADER_SIZE)
		|| (elf_file->e_shentsize != 0 && elf_file->e_shentsize != ELF64_SECTION_HEADER_SIZE)
		|| elf_file->e_shstrndx > elf_file->e_shnum
		|| elf_file->e_shnum > reader->size / ELF64_PROGRAM_HEADER_SIZE
		|| elf_file->e_phnum > reader->size / ELF64_SECTION_HEADER_SIZE
	)
	{
		delete_elf64_file(elf_file);
		return (ft_dverbose(STDERR_FILENO, "Could not read file because of incoeherent values\n"), NULL);
	}

	if (get_elf64_program_headers(elf_file, reader) == -1)
	{
		delete_elf64_file(elf_file);
		return (NULL);
	}

	if (get_elf64_section_headers(elf_file, reader) == -1)
	{
		delete_elf64_file(elf_file);
		return (NULL);
	}

	ft_bverbose(B_GREEN"\nELF64 file is valid\n"RESET);

	return (elf_file);
}

t_elf32_file*
new_elf32_file(t_binary_reader *reader, t_elf_ident elf_file_ident)
{
	t_elf32_file *elf_file = ft_calloc(1, sizeof(t_elf32_file));
	if (elf_file == NULL)
		return (ft_dverbose(STDERR_FILENO, "Could not allocate memory\n"), NULL);

	ft_memcpy(elf_file->e_ident.raw, elf_file_ident.raw, sizeof(t_elf_ident));

	reader->seek(reader, 0x10);
	elf_file->e_type = reader->get_uint16(reader);
	elf_file->e_machine = reader->get_uint16(reader);
	elf_file->e_version = reader->get_uint32(reader);

	elf_file->e_entry  = reader->get_uint32(reader);
	elf_file->e_phoff  = reader->get_uint32(reader);
	elf_file->e_shoff  = reader->get_uint32(reader);

	elf_file->e_flags = reader->get_uint32(reader);
	elf_file->e_ehsize = reader->get_uint16(reader);
	elf_file->e_phentsize = reader->get_uint16(reader);
	elf_file->e_phnum = reader->get_uint16(reader);
	elf_file->e_shentsize = reader->get_uint16(reader);
	elf_file->e_shnum = reader->get_uint16(reader);
	elf_file->e_shstrndx = reader->get_uint16(reader);

	if (elf_file->e_phoff > reader->size
		|| elf_file->e_shoff > reader->size
		|| (elf_file->e_ehsize != 0 && elf_file->e_ehsize != ELF32_HEADER_SIZE)
		|| (elf_file->e_phentsize != 0 && elf_file->e_phentsize != ELF32_PROGRAM_HEADER_SIZE)
		|| (elf_file->e_shentsize != 0 && elf_file->e_shentsize != ELF32_SECTION_HEADER_SIZE)
		|| elf_file->e_shstrndx > elf_file->e_shnum
		|| elf_file->e_shnum > reader->size / ELF32_PROGRAM_HEADER_SIZE
		|| elf_file->e_phnum > reader->size / ELF32_SECTION_HEADER_SIZE
	)
	{
		delete_elf32_file(elf_file);
		return (ft_dverbose(STDERR_FILENO, "Could not read file because of incoeherent values\n"), NULL);
	}

	if (get_elf32_program_headers(elf_file, reader) == -1)
	{
		delete_elf32_file(elf_file);
		return (NULL);
	}

	if (get_elf32_section_headers(elf_file, reader) == -1)
	{
		delete_elf32_file(elf_file);
		return (NULL);
	}

	ft_bverbose(B_GREEN"\nELF32 file is valid\n"RESET);

	return (elf_file);
}

void*
new_elf_file(t_binary_reader *reader)
{
	t_elf_ident elf_file_ident;

	ft_bverbose("\nStarting file parsing...\n");
	/**
	 * By default we set en endianness to little endian because it's the endianness of the header
	 */
	reader->set_endian(reader, READER_LITTLE_ENDIAN);
	reader->get_bytes(reader, elf_file_ident.raw, 16);

	ft_bverbose("\nReading ELF magic number...\n");
	ft_bverbose("Magic number: %X ", MAGIC(elf_file_ident.ei_magic, reader->endian));
	if (elf_file_ident.ei_magic != 0x464C457F) // 0x7F 'E' 'L' 'F' but reversed because of endianness
	{
		ft_bverbose("%s\n", B_RED"invalid"RESET);
		return (ft_dverbose(STDERR_FILENO, "Invalid file format\n"), NULL);
	}

	ft_bverbose("%s\n", B_GREEN"valid"RESET);

	ft_bverbose("\nReading ELF class...\n");
	if (elf_file_ident.ei_class != ELF_64BITS && elf_file_ident.ei_class != ELF_32BITS)
	{
		return (ft_dverbose(STDERR_FILENO, "Incompatible class or unsupported class\n"), NULL);
	}
	ft_bverbose("Class: %s%s%s\n", B_CYAN, elf_file_ident.ei_class == ELF_32BITS ? "32 bits" : "64 bits", RESET);

	ft_bverbose("\nReading ELF endianness...\n");
#if 1
	if (elf_file_ident.ei_data == READER_BIG_ENDIAN)
	{
		reader->set_endian(reader, READER_BIG_ENDIAN);
	}
#endif
	ft_bverbose("Endianness: %s\n", elf_file_ident.ei_data == READER_BIG_ENDIAN ? B_BLUE"Big"RESET : B_CYAN"Little"RESET);
#if 0
	/**
	 * INFO:
	 * If big endian is detected is set but this file is not big endian,
	 * the reading process ends up with infinit loop because of incoeherent values.
	 */
#endif

	/**
	 * We check that the e_ident version is 1, if not the file is not valid
	 */
	if (elf_file_ident.ei_version != 1)
	{
		return (ft_dverbose(STDERR_FILENO, "Wrong version\n"), NULL);
	}

	/**
	 * We ensure that the format of the binary matches the current system
	 */
	if (elf_file_ident.ei_osabi != 0x00 && elf_file_ident.ei_osabi != 0x03)
	{
		return (ft_dverbose(STDERR_FILENO, "Incompatible ABI\n"), NULL);
	}

	if (elf_file_ident.ei_class == ELF_64BITS)
	{
		return (new_elf64_file(reader, elf_file_ident));
	}
	else
	{
		return (new_elf32_file(reader, elf_file_ident));
	}
}

void
delete_elf64_file(t_elf64_file *elf_file)
{
	if (elf_file == NULL)
		return ;

	if (elf_file->ph64)
		free(elf_file->ph64);

	if (elf_file->sh64)
	{
		for (int i = 0; i < elf_file->e_shnum; i++)
		{
			if (elf_file->sh64[i].sh_name)
				free(elf_file->sh64[i].sh_name);
			if (elf_file->sh64[i].data)
				free(elf_file->sh64[i].data);
		}
		free(elf_file->sh64);
	}
	
	free(elf_file);
}

void
delete_elf32_file(t_elf32_file *elf_file)
{
	if (elf_file == NULL)
		return ;

	if (elf_file->ph32)
		free(elf_file->ph32);

	if (elf_file->sh32)
	{
		for (int i = 0; i < elf_file->e_shnum; i++)
		{
			if (elf_file->sh32[i].sh_name)
				free(elf_file->sh32[i].sh_name);
			if (elf_file->sh32[i].data)
				free(elf_file->sh32[i].data);
		}
		free(elf_file->sh32);
	}
	
	free(elf_file);
}
