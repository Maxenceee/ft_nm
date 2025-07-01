/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   verbose.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgama <mgama@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/02 02:42:55 by mgama             #+#    #+#             */
/*   Updated: 2025/07/01 11:43:41 by mgama            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VERBOSE_H
# define VERBOSE_H

#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#define DIGITS "0123456789abcdef0123456789ABCDEF"

enum
{
	VERBOSE_OFF		= 0x00,	// No verbose output
	VERBOSE_INFO	= 0x01,	// Basic output
	VERBOSE_DEBUG	= 0x02	// Debug output
};

enum
{
	VERBOSE_NOCOLOR	= 0x00,
	VERBOSE_COLOR	= 0x10
};

extern int	verbose_size;

int ft_verbose_mode(int mode);

int	ft_verbose(const char *fmt, ...);
int ft_bverbose(const char *fmt, ...);
int ft_dverbose(int fd, const char *fmt, ...);

#endif /* VERBOSE_H */