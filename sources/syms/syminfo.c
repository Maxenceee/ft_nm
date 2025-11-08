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

static char
format_on_linkage_scope(char base, int global)
{
	if (global)
		return (base - 32);
	return (base);
}

char
get_elf64_sym_type(t_elf64_sym *sym, t_elf64_section_header *all_sections)
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

	t_elf64_section_header *sec = &all_sections[ndx];
	const char *secname = sec->sh_name;

	if (ft_strncmp(secname, ".debug_", 7) == 0 || ft_strncmp(secname, ".zdebug_", 8) == 0)
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

char
get_elf32_sym_type(t_elf32_sym *sym, t_elf32_section_header *all_sections)
{
	uint8_t type = ELF32_ST_TYPE(sym->st_info);
	uint8_t bind = ELF32_ST_BIND(sym->st_info);
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

	t_elf32_section_header *sec = &all_sections[ndx];
	const char *secname = sec->sh_name;

	if (ft_strncmp(secname, ".debug_", 7) == 0 || ft_strncmp(secname, ".zdebug_", 8) == 0)
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

char*
get_version64_name(uint16_t ndx, t_elf64_section_header *verneed_section, char *verstrtab)
{
	if (!verneed_section || !verstrtab || ndx <= 1)
		return NULL;

	uint8_t *data = verneed_section->data;
	size_t offset = 0;

	while (offset < verneed_section->sh_size)
	{
		t_elf_verneed *vern = (t_elf_verneed *)(data + offset);
		size_t aux_offset = offset + vern->vn_aux;

		for (int i = 0; i < vern->vn_cnt; i++)
		{
			t_elf_vernaux *aux = (t_elf_vernaux *)(data + aux_offset);
			if (aux->vna_other == ndx)
				return verstrtab + aux->vna_name;

			aux_offset += aux->vna_next;
		}
		offset += vern->vn_next;
	}

	return NULL;
}

char*
get_version32_name(uint16_t ndx, t_elf32_section_header *verneed_section, char *verstrtab)
{
	if (!verneed_section || !verstrtab || ndx <= 1)
		return NULL;

	uint8_t *data = verneed_section->data;
	size_t offset = 0;

	while (offset < verneed_section->sh_size)
	{
		t_elf_verneed *vern = (t_elf_verneed *)(data + offset);
		size_t aux_offset = offset + vern->vn_aux;

		for (int i = 0; i < vern->vn_cnt; i++)
		{
			t_elf_vernaux *aux = (t_elf_vernaux *)(data + aux_offset);
			if (aux->vna_other == ndx)
				return verstrtab + aux->vna_name;

			aux_offset += aux->vna_next;
		}
		offset += vern->vn_next;
	}

	return NULL;
}
