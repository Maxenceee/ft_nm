/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   verbose.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgama <mgama@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/02 02:42:55 by mgama             #+#    #+#             */
/*   Updated: 2025/06/30 23:37:16 by mgama            ###   ########.fr       */
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
	VERBOSE_OFF = 0,
	VERBOSE_ON = 1
};

enum
{
	VERBOSE_COLOR_OFF = 0,
	VERBOSE_COLOR_ON = 1
};

extern int	verbose_mode;
extern int	verbose_size;
extern int	verbose_color;

int	ft_verbose(const char *fmt, ...);
int ft_dverbose(int fd, const char *fmt, ...);

#endif /* VERBOSE_H */