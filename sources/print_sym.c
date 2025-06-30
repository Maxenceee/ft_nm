/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_sym.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgama <mgama@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 18:58:37 by mgama             #+#    #+#             */
/*   Updated: 2025/06/30 21:14:35 by mgama            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

static char
format_on_linkage_scope(char base, int global)
{
	if (global)
		return base - 32;
	return base;
}

static char
get_elf_sym_type(t_elf_sym *sym, t_elf_section *all_sections)
{
	uint8_t type = ELF64_ST_TYPE(sym->st_info);
	uint8_t bind = ELF64_ST_BIND(sym->st_info);
	uint16_t ndx = sym->st_shndx;

	if (bind == STB_WEAK)
	{
		if (type == STT_OBJECT)
			return format_on_linkage_scope('v', bind == STB_GLOBAL);
		if (sym->st_shndx != SHN_UNDEF && sym->st_value != 0)
            return 'W';
        else
            return format_on_linkage_scope('w', bind == STB_GLOBAL);
	}

	if (ndx == SHN_UNDEF)
		return 'U';
	if (ndx == SHN_ABS)
		return format_on_linkage_scope('a', bind == STB_GLOBAL);
	if (ndx == SHN_COMMON)
		return format_on_linkage_scope('c', bind == STB_GLOBAL);
	if (ndx >= SHN_LORESERVE) // Symboles spéciaux hors sections normales
		return '?';

	t_elf_section *sec = &all_sections[ndx];
	const char *secname = sec->sh_name;

	// printf("%s: %d %d %llx %llx %x\n", secname, type, bind, (sec->sh_flags & SHF_ALLOC), (sec->sh_flags & SHF_WRITE), sec->sh_type == SHT_PROGBITS);
	if (strncmp(secname, ".debug_", 7) == 0 || strncmp(secname, ".zdebug_", 8) == 0)
	{
		return 'N';
	}

	if ((sec->sh_flags & SHF_ALLOC) && (sec->sh_flags & SHF_EXECINSTR))
		return format_on_linkage_scope('t', bind == STB_GLOBAL);

	if ((sec->sh_flags & SHF_ALLOC) && !(sec->sh_flags & SHF_WRITE))
		return format_on_linkage_scope('r', bind == STB_GLOBAL);

	if (sec->sh_type == SHT_NOBITS && (sec->sh_flags & SHF_ALLOC) && (sec->sh_flags & SHF_WRITE))
		return format_on_linkage_scope('b', bind == STB_GLOBAL);

	if ((sec->sh_flags & SHF_ALLOC) && (sec->sh_flags & SHF_WRITE))
		return format_on_linkage_scope('d', bind == STB_GLOBAL);

	if (type == STT_SECTION)
		return format_on_linkage_scope('n', bind == STB_GLOBAL);

	if (bind == STB_LOCAL)
		return 'l';
	return '?';
}

static void
print_elf_sym(t_elf_section *sym_section, t_elf_section *symstr_section, const char *ssymname, t_elf_section *all_sections)
{
	if (sym_section->sh_size == 0 || sym_section->sh_type == SHT_NOBITS)
	{
		printf("\nSymbol table '%s' contains no entries.\n", ssymname);
		return;
	}

	t_elf_sym sym;
	for (size_t j = 0; j * sizeof(t_elf_sym) < sym_section->sh_size; j++) {
		void *absoffset = sym_section->data + j * sizeof(t_elf_sym);
		ft_memmove(&sym, absoffset, sizeof(sym));

		uint8_t stype = ELF64_ST_TYPE(sym.st_info);

		// if (stype == STT_NOTYPE)
		// {
		// 	continue;
		// }

		// if (stype == STT_FILE || stype == STT_SECTION)
		// 	continue;
		char* name;
		if (stype == STT_SECTION)
		{
			name = all_sections[sym.st_shndx].sh_name;
		}
		else
		{
			name = (char *)symstr_section->data + sym.st_name;
		}
		if (!name || !*name) {
			// Ignore symboles sans nom sauf si c'est un STT_FILE (tu peux ajouter ce test)
			if (stype != STT_FILE)
				continue;
		}

		char type = get_elf_sym_type(&sym, all_sections);

		if (sym.st_shndx == 0)
		{
			printf("%16s", ""); 
		}
		else
		{
			printf("%016" PELF_llx, sym.st_value);
		}

		printf(" %c %s\n", type, name);
	}
}

void
print_sym(t_elf_file *elf_file)
{
	int symtab_idx = 0;
	int symstr_idx = 0;

	// for (uint16_t i = 0; i < elf_file->e_shnum; i++) {
	// 	if (ft_strcmp(elf_file->section_tables[i].sh_name, ".dynsym") == 0 && elf_file->section_tables[i].sh_type == SHT_DYNSYM)
	// 		symtab_idx = i;
	// 	else if (ft_strcmp(elf_file->section_tables[i].sh_name, ".dynstr") == 0 && elf_file->section_tables[i].sh_type == SHT_STRTAB)
	// 		symstr_idx = i;
	// }
	
	// print_elf_sym(&elf_file->section_tables[symtab_idx], &elf_file->section_tables[symstr_idx], ".dynsym");

	for (uint16_t i = 0; i < elf_file->e_shnum; i++) {
		if (ft_strcmp(elf_file->section_tables[i].sh_name, ".symtab") == 0 && elf_file->section_tables[i].sh_type == SHT_SYMTAB)
			symtab_idx = i;
		else if (ft_strcmp(elf_file->section_tables[i].sh_name, ".strtab") == 0 && elf_file->section_tables[i].sh_type == SHT_STRTAB)
			symstr_idx = i;
	}

	print_elf_sym(&elf_file->section_tables[symtab_idx], &elf_file->section_tables[symstr_idx], ".symtab", elf_file->section_tables);
}