/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgama <mgama@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 17:38:16 by mgama             #+#    #+#             */
/*   Updated: 2025/07/02 17:05:10 by mgama            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_NM_H
# define FT_NM_H

#include <limits.h>
#include <ctype.h>

#include "elf.h"
#include "verbose.h"
#include "reader/binary_reader.h"

#define NM_VERSION "1.5"
#define NM_AUTHOR "mgama"

#ifdef __APPLE__
#define NM_PREFIX ""
#else
#define NM_PREFIX "nm: "
#endif /* __APPLE__ */

/**
 * Options
 */

#define F_ALL		0x00000010
#define F_EXTO		0x00000020
#define F_UNDO		0x00000040
#define F_DFNO		0x00000080
#define F_DYNS		0x00000100
#define F_NWKS		0x00000200

#define F_RSRT		0x00001000
#define F_NSRT		0x00002000
#define F_ASRT		0x00004000
#define REMOVE_SORT_FLAGS(x, f)   ((x) &= ~(f))

#define F_RDX_DEC	0X01000000
#define F_RDX_HEX	0x02000000
#define F_RDX_OCT	0x04000000
#define F_RDX_MASK	0X0F000000

#define F_RADIX(r) (r & F_RDX_MASK)

int		print_sym(void *elf_file, int level);

typedef struct nm_sym_s
{
	char			*name;
	char			*version;
	uint8_t			has_ndx;
	uint8_t			st_info;
	uint64_t		value;
	char			type;
	struct nm_sym_s *next;
}	nm_sym_node_t;


/**
 * ELF
 */

void		delete_elf_file(void *elf_file);
void*		new_elf_file(t_binary_reader *reader);

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

void	print_elf_file(void *elf_file, int level);

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