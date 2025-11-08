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

#ifndef SYMS_H
#define SYMS_H

char    get_elf64_sym_type(t_elf64_sym *sym, t_elf64_section_header *all_sections);
char    get_elf32_sym_type(t_elf32_sym *sym, t_elf32_section_header *all_sections);

char*   get_version64_name(uint16_t ndx, t_elf64_section_header *verneed_section, char *verstrtab);
char*   get_version32_name(uint16_t ndx, t_elf32_section_header *verneed_section, char *verstrtab);

void    sort_nodes(nm_sym_node_t **head, int level);
void    free_nodes(nm_sym_node_t *node);

#endif /* SYMS_H */ 