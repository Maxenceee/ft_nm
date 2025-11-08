/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_sym.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgama <mgama@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 18:58:37 by mgama             #+#    #+#             */
/*   Updated: 2025/07/02 17:06:51 by mgama            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"
#include "syms.h"

static nm_sym_node_t*
print_elf64_sym(t_elf64_section_header *sym_section, t_elf64_section_header *symstr_section, t_elf64_section_header *all_sections, int level)
{
	uint16_t *versym = NULL;
	t_elf64_section_header *verneed_section = NULL;
	char *verstrtab = NULL;

	if (level & F_DYNS)
	{
		for (size_t i = 0; all_sections[i].sh_name != NULL; i++)
		{
			if (all_sections[i].sh_type == SHT_GNU_versym &&
				ft_strcmp(all_sections[i].sh_name, ".gnu.version") == 0)
				versym = (uint16_t *)all_sections[i].data;

			if (all_sections[i].sh_type == SHT_GNU_verneed &&
				ft_strcmp(all_sections[i].sh_name, ".gnu.version_r") == 0)
				verneed_section = &all_sections[i];

			if (all_sections[i].sh_type == SHT_STRTAB &&
				ft_strcmp(all_sections[i].sh_name, ".dynstr") == 0)
				verstrtab = (char *)all_sections[i].data;
		}
	}

	t_elf64_sym sym;
	nm_sym_node_t* first_node = NULL;
	nm_sym_node_t* last_node = NULL;

	for (size_t j = 0; j * sizeof(t_elf64_sym) < sym_section->sh_size; j++)
	{
		void *absoffset = sym_section->data + j * sizeof(t_elf64_sym);
		ft_memmove(&sym, absoffset, sizeof(sym));

		uint8_t stype = ELF64_ST_TYPE(sym.st_info);
		uint8_t bind = ELF64_ST_BIND(sym.st_info);

		if (level & F_UNDO)
		{
			if (sym.st_shndx != SHN_UNDEF)
				continue;
		}
		if (level & F_DFNO)
		{
			if (sym.st_shndx == SHN_UNDEF)
				continue;
		}
		if (level & F_EXTO)
		{
			if (bind != STB_GLOBAL && bind != STB_WEAK && bind != STB_LOOS && bind != STB_HIOS)
				continue;
		}
		if (level & F_NWKS)
		{
			if (bind == STB_WEAK)
				continue;
		}
		if (!(level & F_ALL))
		{
			if (stype == STT_SECTION || stype == STT_FILE)
				continue;
		}

		char* name;
		if (stype == STT_SECTION)
		{
			name = all_sections[sym.st_shndx].sh_name;
		}
		else
		{
			name = (char *)symstr_section->data + sym.st_name;
		}
		if (!name || !*name)
		{
			if (stype != STT_FILE)
				continue;
		}

		char type = get_elf64_sym_type(&sym, all_sections);

		nm_sym_node_t* node = malloc(sizeof(nm_sym_node_t));
		if (!node)
		{
			free_nodes(first_node);
			return (NULL);
		}

		if (first_node == NULL)
		{
			first_node = node;
		}
		else
		{
			last_node->next = node;
		}
		last_node = node;

		node->name = name;
		node->version = NULL;
		node->has_ndx = sym.st_shndx != 0;
		node->st_info = sym.st_info;
		node->value = sym.st_value;
		node->type = type;

		if ((level & F_DYNS) && versym)
		{
			uint16_t ndx = versym[j] & 0x7fff;
			node->version = get_version64_name(ndx, verneed_section, verstrtab);
		}
	}

	return (first_node);
}

static nm_sym_node_t*
print_elf32_sym(t_elf32_section_header *sym_section, t_elf32_section_header *symstr_section, t_elf32_section_header *all_sections, int level)
{
	uint16_t *versym = NULL;
	t_elf32_section_header *verneed_section = NULL;
	char *verstrtab = NULL;

	if (level & F_DYNS)
	{
		for (size_t i = 0; all_sections[i].sh_name != NULL; i++)
		{
			if (all_sections[i].sh_type == SHT_GNU_versym &&
				ft_strcmp(all_sections[i].sh_name, ".gnu.version") == 0)
				versym = (uint16_t *)all_sections[i].data;

			if (all_sections[i].sh_type == SHT_GNU_verneed &&
				ft_strcmp(all_sections[i].sh_name, ".gnu.version_r") == 0)
				verneed_section = &all_sections[i];

			if (all_sections[i].sh_type == SHT_STRTAB &&
				ft_strcmp(all_sections[i].sh_name, ".dynstr") == 0)
				verstrtab = (char *)all_sections[i].data;
		}
	}

	t_elf32_sym sym;
	nm_sym_node_t* first_node = NULL;
	nm_sym_node_t* last_node = NULL;

	for (size_t j = 0; j * sizeof(t_elf32_sym) < sym_section->sh_size; j++)
	{
		void *absoffset = sym_section->data + j * sizeof(t_elf32_sym);
		ft_memmove(&sym, absoffset, sizeof(sym));

		uint8_t stype = ELF32_ST_TYPE(sym.st_info);
		uint8_t bind = ELF32_ST_BIND(sym.st_info);

		if (level & F_UNDO)
		{
			if (sym.st_shndx != SHN_UNDEF)
				continue;
		}
		if (level & F_DFNO)
		{
			if (sym.st_shndx == SHN_UNDEF)
				continue;
		}
		if (level & F_EXTO)
		{
			if (bind != STB_GLOBAL && bind != STB_WEAK && bind != STB_LOOS && bind != STB_HIOS)
				continue;
		}
		if (level & F_NWKS)
		{
			if (bind == STB_WEAK)
				continue;
		}
		if (!(level & F_ALL))
		{
			if (stype == STT_SECTION || stype == STT_FILE)
				continue;
		}

		char* name;
		if (stype == STT_SECTION)
		{
			name = all_sections[sym.st_shndx].sh_name;
		}
		else
		{
			name = (char *)symstr_section->data + sym.st_name;
		}
		if (!name || !*name)
		{
			if (stype != STT_FILE)
				continue;
		}

		char type = get_elf32_sym_type(&sym, all_sections);

		nm_sym_node_t* node = malloc(sizeof(nm_sym_node_t));
		if (!node)
		{
			free_nodes(first_node);
			return (NULL);
		}

		if (first_node == NULL)
		{
			first_node = node;
		}
		else
		{
			last_node->next = node;
		}
		last_node = node;

		node->name = name;
		node->version = NULL;
		node->has_ndx = sym.st_shndx != 0;
		node->st_info = sym.st_info;
		node->value = sym.st_value;
		node->type = type;

		if ((level & F_DYNS) && versym)
		{
			uint16_t ndx = versym[j] & 0x7fff;
			node->version = get_version32_name(ndx, verneed_section, verstrtab);
		}
	}

	return (first_node);
}

int
get_symbol_indexes64(t_elf64_file *elf_file, int level, int *symtab, int *symstr)
{
	if (level & F_DYNS)
	{
		for (uint16_t i = 0; i < elf_file->e_shnum; i++)
		{
			if (ft_strcmp(elf_file->sh64[i].sh_name, ".dynsym") == 0 && elf_file->sh64[i].sh_type == SHT_DYNSYM)
				*symtab = i;
			else if (ft_strcmp(elf_file->sh64[i].sh_name, ".dynstr") == 0 && elf_file->sh64[i].sh_type == SHT_STRTAB)
				*symstr = i;
		}

		ft_bverbose("Debug symbol table found and contains %u entries\n", elf_file->sh64[*symtab].sh_size / sizeof(t_elf64_sym));
	}
	else
	{
		for (uint16_t i = 0; i < elf_file->e_shnum; i++)
		{
			if (ft_strcmp(elf_file->sh64[i].sh_name, ".symtab") == 0 && elf_file->sh64[i].sh_type == SHT_SYMTAB)
				*symtab = i;
			else if (ft_strcmp(elf_file->sh64[i].sh_name, ".strtab") == 0 && elf_file->sh64[i].sh_type == SHT_STRTAB)
				*symstr = i;
		}

		ft_bverbose("Symbol table found and contains %u entries\n", elf_file->sh64[*symtab].sh_size / sizeof(t_elf64_sym));
	}

	if (elf_file->sh64[*symtab].sh_size == 0 || elf_file->sh64[*symtab].sh_type == SHT_NOBITS)
		return (1);

	return (0);
}

int
get_symbol_indexes32(t_elf32_file *elf_file, int level, int *symtab, int *symstr)
{
	if (level & F_DYNS)
	{
		for (uint16_t i = 0; i < elf_file->e_shnum; i++)
		{
			if (ft_strcmp(elf_file->sh32[i].sh_name, ".dynsym") == 0 && elf_file->sh32[i].sh_type == SHT_DYNSYM)
				*symtab = i;
			else if (ft_strcmp(elf_file->sh32[i].sh_name, ".dynstr") == 0 && elf_file->sh32[i].sh_type == SHT_STRTAB)
				*symstr = i;
		}

		ft_bverbose("Debug symbol table found and contains %u entries\n", elf_file->sh32[*symtab].sh_size / sizeof(t_elf32_sym));
	}
	else
	{
		for (uint16_t i = 0; i < elf_file->e_shnum; i++)
		{
			if (ft_strcmp(elf_file->sh32[i].sh_name, ".symtab") == 0 && elf_file->sh32[i].sh_type == SHT_SYMTAB)
				*symtab = i;
			else if (ft_strcmp(elf_file->sh32[i].sh_name, ".strtab") == 0 && elf_file->sh32[i].sh_type == SHT_STRTAB)
				*symstr = i;
		}

		ft_bverbose("Symbol table found and contains %u entries\n", elf_file->sh32[*symtab].sh_size / sizeof(t_elf32_sym));
	}

	if (elf_file->sh32[*symtab].sh_size == 0 || elf_file->sh32[*symtab].sh_type == SHT_NOBITS)
		return (1);

	return (0);
}

int
print_sym(void *elf_file, int level)
{
	int symtab_idx = 0;
	int symstr_idx = 0;

	nm_sym_node_t* nodes = NULL;

	ft_bverbose("\nLocating symbol table...\n");

	if (ELF_CLASS(elf_file) == ELF_64BITS)
	{
		t_elf64_file *elf64_file = elf_file;

		if (get_symbol_indexes64(elf64_file, level, &symtab_idx, &symstr_idx))
			return (2);

		nodes = print_elf64_sym(&elf64_file->sh64[symtab_idx], &elf64_file->sh64[symstr_idx], elf64_file->sh64, level);
	}
	else
	{
		t_elf32_file *elf32_file = elf_file;

		if (get_symbol_indexes32(elf32_file, level, &symtab_idx, &symstr_idx))
			return (2);

		nodes = print_elf32_sym(&elf32_file->sh32[symtab_idx], &elf32_file->sh32[symstr_idx], elf32_file->sh32, level);
	}

	if (!nodes)
	{
		return (1);
	}

	ft_bverbose("\n");

	sort_nodes(&nodes, level);

	char *radix_dec = ELF_CLASS(elf_file) == ELF_64BITS ? "%016ld " : "%08ld ";
	char *radix_oct = ELF_CLASS(elf_file) == ELF_64BITS ? "%016lo " : "%08lo ";
	char *radix_hex = ELF_CLASS(elf_file) == ELF_64BITS ? "%016lx " : "%08lx ";

	char printr[16];
	switch (F_RADIX(level))
	{
	case F_RDX_DEC:
		ft_strlcpy(printr, radix_dec, sizeof(printr));
		break;
	case F_RDX_OCT:
		ft_strlcpy(printr, radix_oct, sizeof(printr));
		break;
	case F_RDX_HEX:
	default:
		ft_strlcpy(printr, radix_hex, sizeof(printr));
		break;
	}

	for (nm_sym_node_t* node = nodes; node != NULL; node = node->next)
	{
		if (node->has_ndx)
			ft_verbose(printr, node->value);
		else
			ft_verbose(ELF_CLASS(elf_file) == ELF_64BITS ? "                 " : "         ");

		ft_verbose("%c ", node->type);

		if (node->version)
			ft_verbose("%s@%s\n", node->name, node->version);
		else
			ft_verbose("%s\n", node->name);
	}
	free_nodes(nodes);
	return (0);
}
