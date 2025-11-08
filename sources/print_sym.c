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

void
free_nodes(nm_sym_node_t *node)
{
	nm_sym_node_t *next;

	while (node)
	{
		next = node->next;
		free(node);
		node = next;
	}
}

int
ft_symcmp(const nm_sym_node_t *a, const nm_sym_node_t *b)
{
#ifndef __APPLE__
	int i;
	int j;
	char s1_c;
	char s2_c;
	char *s1 = a->name;
	char *s2 = b->name;

	i = 0;
	j = 0;
	if (!s1 || !s2)
		return (0);

	while (*s1 == '_' || *s1 == '.')
		s1++;
	while (*s2 == '_' || *s2 == '.')
		s2++;
	while (s1[i] || s2[j])
	{
		while (s1[i] && (s1[i] == '_' || s1[i] == '@' || s1[i] == '.'))
			i++;
		while (s2[j] && (s2[j] == '_' || s2[j] == '@' || s2[j] == '.'))
			j++;
		s1_c = ft_tolower(s1[i]);
		s2_c = ft_tolower(s2[j]);
		if (s1_c < s2_c)
			return (s1_c - s2_c);
		if (s1_c > s2_c)
			return (s1_c - s2_c);
		i++;
		j++;
	}

	if (a->value == b->value)
		return ft_strcmp(a->name, b->name);
	return ft_strcmp(b->name, a->name);

#else
	return ft_strcmp(s1, s2);
#endif
}

static int
cmp_diff(const uint64_t a, const uint64_t b)
{
	if (a < b)
		return (-1);
	else if (a > b)
		return (1);
	else
		return (0);
}

static int
cmp_sym_num(const nm_sym_node_t *a, const nm_sym_node_t *b)
{
	if (!a->has_ndx || !b->has_ndx)
		return (a->has_ndx - b->has_ndx);

	return cmp_diff(a->value, b->value);
}

static int
cmp_sym_name(const nm_sym_node_t *a, const nm_sym_node_t *b)
{
	int res = ft_symcmp(a, b);
	if (res == 0)
	{
		return cmp_diff(a->value, b->value);
	}
	return (res);
}

static int
cmp_sym(const nm_sym_node_t *a, const nm_sym_node_t *b, int level)
{
	int res;

	if (level & F_ASRT)
	{
		res = cmp_sym_num(a, b);
		if (res == 0)
		{
			res = cmp_sym_name(a, b);
		}
	}
	else
	{
		res = cmp_sym_name(a, b);
	}
	
	if (level & F_RSRT)
		return (-res);
	return (res);
}

static nm_sym_node_t*
merge_sorted_lists(nm_sym_node_t *a, nm_sym_node_t *b, int level)
{
	if (!a) return (b);
	if (!b) return (a);

	nm_sym_node_t *result = NULL;

	if (cmp_sym(a, b, level) <= 0)
	{
		result = a;
		result->next = merge_sorted_lists(a->next, b, level);
	}
	else
	{
		result = b;
		result->next = merge_sorted_lists(a, b->next, level);
	}
	return (result);
}

static void
split_list(nm_sym_node_t *source, nm_sym_node_t **front, nm_sym_node_t **back)
{
	nm_sym_node_t *slow = source;
	nm_sym_node_t *fast = source->next;

	while (fast)
	{
		fast = fast->next;
		if (fast)
		{
			slow = slow->next;
			fast = fast->next;
		}
	}
	*front = source;
	*back = slow->next;
	slow->next = NULL;
}

static nm_sym_node_t*
merge_sort(nm_sym_node_t *head, int level)
{
	if (!head || !head->next)
		return (head);

	nm_sym_node_t *left = NULL;
	nm_sym_node_t *right = NULL;

	split_list(head, &left, &right);

	left = merge_sort(left, level);
	right = merge_sort(right, level);

	return merge_sorted_lists(left, right, level);
}

void
sort_nodes(nm_sym_node_t **head, int level)
{
	if (level & F_NSRT)
		return;

	if (!head || !*head || (level & F_NSRT))
		return;

	*head = merge_sort(*head, level);
}

static char
format_on_linkage_scope(char base, int global)
{
	if (global)
		return (base - 32);
	return (base);
}

static char
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

static char
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

static char *
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

static char *
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
	}

	ft_bverbose("Symbol table found and contains %u entries\n", elf_file->sh64[*symtab].sh_size / sizeof(t_elf64_sym));

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
	}

	ft_bverbose("Symbol table found and contains %u entries\n", elf_file->sh32[*symtab].sh_size / sizeof(t_elf32_sym));

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

	char printr[16];
	switch (F_RADIX(level))
	{
	case F_RDX_DEC:
		ft_strlcpy(printr, "%016ld ", sizeof(printr));
		break;
	case F_RDX_OCT:
		ft_strlcpy(printr, "%016lo ", sizeof(printr));
		break;
	case F_RDX_HEX:
	default:
		ft_strlcpy(printr, "%016lx ", sizeof(printr));
		break;
	}

	for (nm_sym_node_t* node = nodes; node != NULL; node = node->next)
	{
		if (node->has_ndx)
			ft_verbose(printr, node->value);
		else
			ft_verbose("                 ");

		ft_verbose("%c ", node->type);

		if (node->version)
			ft_verbose("%s@%s\n", node->name, node->version);
		else
			ft_verbose("%s\n", node->name);
	}
	free_nodes(nodes);
	return (0);
}
