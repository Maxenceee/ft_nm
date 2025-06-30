/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgama <mgama@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 17:38:16 by mgama             #+#    #+#             */
/*   Updated: 2025/06/30 23:41:18 by mgama            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_NM_H
# define FT_NM_H

#include <limits.h>

#include "elf.h"
#include "verbose.h"
#include "reader/binary_reader.h"

#define NM_VERSION "1.0"
#define NM_AUTHOR "mgama"

#define NM_PREFIX "nm"

/**
 * Options
 */

#define F_ALL		0x01
#define F_EXTO		0x02
#define F_UNDO		0x04
#define F_RSRT		0x08
#define F_NSRT		0x10

int		print_sym(t_elf_file *elf_file, int level);

typedef struct nm_sym_s
{
	char			*name;
	uint8_t			has_ndx;
	uint8_t			st_info;
	uint64_t		value;
	char			type;
}	nm_sym_t;


/**
 * ELF
 */

void		delete_elf_file(t_elf_file *elf_file);
t_elf_file*	new_elf_file(t_binary_reader *reader);

/**
 * For dev only
 * TODO: rm before submit
 */
#define PELF_ALL		0x01
#define PELF_HEADER		0x02
#define PELF_PROG		0x04
#define PELF_SECTION	0x08
#define PELF_SYM		0x10
#define PELF_DATA		0x20
#define PELF_ERROR		0x80

void	print_elf_file(t_elf_file *elf_file, int level);

#ifdef __APPLE__

#define PELF_llx	"llx"
#define PELF_llX	"llX"
#define PELF_llu	"llu"

#else

#define PELF_llx	"lx"
#define PELF_llX	"lX"
#define PELF_llu	"lu"

#endif /* __APPLE__ */

#endif /* FT_NM_H */