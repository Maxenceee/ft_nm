/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_sym.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgama <mgama@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 18:58:37 by mgama             #+#    #+#             */
/*   Updated: 2025/07/01 12:23:02 by mgama            ###   ########.fr       */
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
		{
			if (sym->st_shndx != SHN_UNDEF && sym->st_value != 0)
				return 'V';
			else
				return format_on_linkage_scope('v', bind == STB_GLOBAL);
		}
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

	if (bind >= STB_LOOS && bind <= STB_HIOS) 
	{
		return 'u';
	}

	t_elf_section *sec = &all_sections[ndx];
	const char *secname = sec->sh_name;

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

static char *
get_version_name(uint16_t ndx, t_elf_section *verneed_section, char *verstrtab)
{
	if (!verneed_section || !verstrtab || ndx <= 1)
		return NULL;

	uint8_t *data = verneed_section->data;
	size_t offset = 0;

	while (offset < verneed_section->sh_size) {
		t_elf_verneed *vern = (t_elf_verneed *)(data + offset);
		size_t aux_offset = offset + vern->vn_aux;

		for (int i = 0; i < vern->vn_cnt; i++) {
			t_elf_vernaux *aux = (t_elf_vernaux *)(data + aux_offset);
			if (aux->vna_other == ndx)
				return verstrtab + aux->vna_name;

			aux_offset += aux->vna_next;
		}
		offset += vern->vn_next;
	}

	return NULL;
}

static nm_sym_t*
print_elf_sym(t_elf_section *sym_section, t_elf_section *symstr_section, t_elf_section *all_sections, int level)
{
	size_t container_size = 0;
	uint16_t *versym = NULL;
	t_elf_section *verneed_section = NULL;
	char *verstrtab = NULL;

	if (level & F_DYNS) {
		for (size_t i = 0; all_sections[i].sh_name != NULL; i++) {
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

	t_elf_sym sym;
	for (size_t j = 0; j * sizeof(t_elf_sym) < sym_section->sh_size; j++) {
		void *absoffset = sym_section->data + j * sizeof(t_elf_sym);
		ft_memmove(&sym, absoffset, sizeof(sym));

		uint8_t stype = ELF64_ST_TYPE(sym.st_info);
		uint8_t bind = ELF64_ST_BIND(sym.st_info);

		if (level & F_UNDO)
		{
			if (sym.st_shndx != SHN_UNDEF)
				continue;
		}
		else if (level & F_EXTO)
		{
			if (bind != STB_GLOBAL && bind != STB_WEAK && bind != STB_LOOS && bind != STB_HIOS)
				continue;
		}
		else if (!(level & F_ALL))
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
		if (!name || !*name) {
			// Ignore symboles sans nom sauf si c'est un STT_FILE (tu peux ajouter ce test)
			if (stype != STT_FILE)
				continue;
		}

		container_size++;
	}

	size_t idx = 0;
	nm_sym_t* rows = malloc(sizeof(nm_sym_t) * (container_size + 1));
	if (!rows)
	{
		ft_dverbose(STDERR_FILENO, NM_PREFIX"Could not allocate memory for symbol table\n");
		return (NULL);
	}
	ft_bzero(rows, sizeof(nm_sym_t) * (container_size + 1));

	for (size_t j = 0; j * sizeof(t_elf_sym) < sym_section->sh_size; j++) {
		void *absoffset = sym_section->data + j * sizeof(t_elf_sym);
		ft_memmove(&sym, absoffset, sizeof(sym));

		uint8_t stype = ELF64_ST_TYPE(sym.st_info);
		uint8_t bind = ELF64_ST_BIND(sym.st_info);

		if (level & F_UNDO)
		{
			if (sym.st_shndx != SHN_UNDEF)
				continue;
		}
		else if (level & F_EXTO)
		{
			if (bind != STB_GLOBAL && bind != STB_WEAK && bind != STB_LOOS && bind != STB_HIOS)
				continue;
		}
		else if (!(level & F_ALL))
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
		if (!name || !*name) {
			// Ignore symboles sans nom sauf si c'est un STT_FILE (tu peux ajouter ce test)
			if (stype != STT_FILE)
				continue;
		}

		char type = get_elf_sym_type(&sym, all_sections);

		rows[idx].name = name;
		rows[idx].version = NULL;
		rows[idx].has_ndx = sym.st_shndx != 0;
		rows[idx].st_info = sym.st_info;
		rows[idx].value = sym.st_value;
		rows[idx].type = type;

		if ((level & F_DYNS) && versym) {
			uint16_t ndx = versym[j] & 0x7fff;
			rows[idx].version = get_version_name(ndx, verneed_section, verstrtab);
		}

		idx++;
	}

	return (rows);
}

void
get_symbol_indexes(t_elf_file *elf_file, int level, int* symtab, int* symstr)
{
	if (level & F_DYNS)
	{
		for (uint16_t i = 0; i < elf_file->e_shnum; i++) {
			if (ft_strcmp(elf_file->section_tables[i].sh_name, ".dynsym") == 0 && elf_file->section_tables[i].sh_type == SHT_DYNSYM)
				*symtab = i;
			else if (ft_strcmp(elf_file->section_tables[i].sh_name, ".dynstr") == 0 && elf_file->section_tables[i].sh_type == SHT_STRTAB)
				*symstr = i;
		}
	}
	else
	{
		for (uint16_t i = 0; i < elf_file->e_shnum; i++) {
			if (ft_strcmp(elf_file->section_tables[i].sh_name, ".symtab") == 0 && elf_file->section_tables[i].sh_type == SHT_SYMTAB)
				*symtab = i;
			else if (ft_strcmp(elf_file->section_tables[i].sh_name, ".strtab") == 0 && elf_file->section_tables[i].sh_type == SHT_STRTAB)
				*symstr = i;
		}
	}
}

int
print_sym(t_elf_file *elf_file, int level)
{
	int symtab_idx = 0;
	int symstr_idx = 0;

	ft_bverbose("\nLocating symbol table...\n");

	get_symbol_indexes(elf_file, level, &symtab_idx, &symstr_idx);

	if (elf_file->section_tables[symtab_idx].sh_size == 0 || elf_file->section_tables[symtab_idx].sh_type == SHT_NOBITS)
	{
		return (2);
	}

	ft_bverbose("Symbol table found and contains %u entries\n", elf_file->section_tables[symtab_idx].sh_size / sizeof(t_elf_sym));

	nm_sym_t* rows = print_elf_sym(&elf_file->section_tables[symtab_idx], &elf_file->section_tables[symstr_idx], elf_file->section_tables, level);
	if (!rows)
	{
		return (1);
	}

	ft_bverbose("\n");

	for (nm_sym_t* row = rows; row->name != NULL; row++)
	{
		if (row->has_ndx)
			ft_verbose("%016lx ", row->value);
		else
			ft_verbose("                 ");

		ft_verbose("%c ", row->type);

		if (row->version)
			ft_verbose("%s@%s\n", row->name, row->version);
		else
			ft_verbose("%s\n", row->name);
	}
	return (0);
}