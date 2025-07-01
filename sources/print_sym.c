/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_sym.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgama <mgama@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 18:58:37 by mgama             #+#    #+#             */
/*   Updated: 2025/07/01 13:08:32 by mgama            ###   ########.fr       */
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

static const char* skip_prefix(const char* s) {
	while (*s == '_' || *s == '*') {
		s++;
	}
	return s;
}

int type_rank(char type) {
    switch(type) {
        case 'U': return 7;  // undefined
        case 'w': return 6;  // weak object
        case 'W': return 5;  // weak object (strong)
        case 'B': return 4;  // bss
        case 'D': return 3;  // data
        case 'R': return 2;  // read only data
        case 'T': return 1;  // text (code)
        case 't': return 1;  // text (local)
        case 'd': return 3;  // data (local)
        case 'b': return 4;  // bss (local)
        // ajoute d'autres types selon besoin
        default:  return 8;  // autres types moins prioritaires
    }
}

static int strcmp_case_insensitive(const char *a, const char *b) {
	char ca = *a;
	char cb = *b;

	if (((ca >= 'A' && ca <= 'Z') || (ca >= 'a' && ca <= 'z')) &&
		((cb >= 'A' && cb <= 'Z') || (cb >= 'a' && cb <= 'z'))) {

		char lower_a = (ca >= 'A' && ca <= 'Z') ? (ca + 32) : ca;
		char lower_b = (cb >= 'A' && cb <= 'Z') ? (cb + 32) : cb;

		if (lower_a != lower_b) {
			// Différence alphabétique (insensible à la casse)
			return (lower_a < lower_b) ? -1 : 1;
		} else {
			// Même lettre, départage selon la casse
			if (ca != cb)
				return (ca < cb) ? -1 : 1; // majuscule avant minuscule car ASCII majuscule < minuscule
			else
				return 0;
		}
	} else {
		// Sinon comparer selon ASCII classique
		if (ca != cb)
			return (ca < cb) ? -1 : 1;
		else
			return 0;
	}
}

static int cmp_sym(const nm_sym_node_t *a, const nm_sym_node_t *b, int level)
{
	const char *name_a = skip_prefix(a->name);
	const char *name_b = skip_prefix(b->name);

	int res = strcmp_case_insensitive(name_a, name_b);

	if (res == 0) {
		int rank_a = type_rank(a->type);
		int rank_b = type_rank(b->type);
		if (rank_a != rank_b)
			return (rank_a < rank_b) ? -1 : 1;
		if (a->value != b->value)
			return (a->value < b->value) ? -1 : 1;
		return 0;
	}
	if (level & F_RSRT)
		return -res;
	return res;
}

static nm_sym_node_t *merge_sorted_lists(nm_sym_node_t *a, nm_sym_node_t *b, int level)
{
	if (!a) return b;
	if (!b) return a;

	nm_sym_node_t *result = NULL;

	if (cmp_sym(a, b, level) <= 0) {
		result = a;
		result->next = merge_sorted_lists(a->next, b, level);
	} else {
		result = b;
		result->next = merge_sorted_lists(a, b->next, level);
	}
	return result;
}

static void split_list(nm_sym_node_t *source, nm_sym_node_t **front, nm_sym_node_t **back)
{
	nm_sym_node_t *slow = source;
	nm_sym_node_t *fast = source->next;

	while (fast) {
		fast = fast->next;
		if (fast) {
			slow = slow->next;
			fast = fast->next;
		}
	}
	*front = source;
	*back = slow->next;
	slow->next = NULL;
}

static nm_sym_node_t *merge_sort(nm_sym_node_t *head, int level)
{
	if (!head || !head->next)
		return head;

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

static nm_sym_node_t*
print_elf_sym(t_elf_section *sym_section, t_elf_section *symstr_section, t_elf_section *all_sections, int level)
{
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
	nm_sym_node_t* first_node = NULL;
	nm_sym_node_t* last_node = NULL;

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

		nm_sym_node_t* node = malloc(sizeof(nm_sym_node_t));
		if (!node)
		{
			free_nodes(first_node);
			return (NULL);
		}

		if (first_node == NULL) {
			first_node = node;
		} else {
			last_node->next = node;
		}
		last_node = node;

		node->name = name;
		node->version = NULL;
		node->has_ndx = sym.st_shndx != 0;
		node->st_info = sym.st_info;
		node->value = sym.st_value;
		node->type = type;

		if ((level & F_DYNS) && versym) {
			uint16_t ndx = versym[j] & 0x7fff;
			node->version = get_version_name(ndx, verneed_section, verstrtab);
		}
	}

	return (first_node);
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

	nm_sym_node_t* nodes = print_elf_sym(&elf_file->section_tables[symtab_idx], &elf_file->section_tables[symstr_idx], elf_file->section_tables, level);
	if (!nodes)
	{
		return (1);
	}

	ft_bverbose("\n");

	sort_nodes(&nodes, level);

	for (nm_sym_node_t* node = nodes; node != NULL; node = node->next)
	{
		if (node->has_ndx)
			ft_verbose("%016lx ", node->value);
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