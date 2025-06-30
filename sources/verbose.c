/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   verbose.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgama <mgama@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/20 21:42:23 by mgama             #+#    #+#             */
/*   Updated: 2025/06/30 18:14:01 by mgama            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "verbose.h"
#include "pcolors.h"

int	verbose_mode = VERBOSE_OFF;
int verbose_size = 0;

inline static void
ft_putchar_fd(int fd, char c)
{
	write(fd, &c, 1);
	verbose_size++;
}

inline static void
ft_putstr_fd(int fd, char *s)
{
	size_t c;

	if (!s)
		s = "(null)";
	c = strlen(s);
	write(fd, s, c);
	if (strncmp(s, "\033[", 2) != 0 && strncmp(s, "\x1b[", 2) != 0)
		verbose_size += c;
}

inline static void
ft_putnbr_base(int fd, uint32_t nbr, int capitals, uint32_t baselen)
{
	if (nbr >= baselen)
	{
		ft_putnbr_base(fd, nbr / baselen, capitals, baselen);
		ft_putnbr_base(fd, nbr % baselen, capitals, baselen);
	}
	else
		ft_putchar_fd(fd, DIGITS[nbr + capitals]);
}

inline static void
ft_putnbr(int fd, int nb)
{
	if (nb == -2147483648)
		return ((void)ft_putstr_fd(fd, "-2147483648"));
	if (nb < 0)
	{
		ft_putchar_fd(fd, '-');
		nb = -nb;
	}
	ft_putnbr_base(fd, nb, 0, 10);
}

inline static void
ft_puthex_p(int fd, unsigned long int n)
{
	if (!n)
		return ((void)ft_putstr_fd(fd, "(nil)"));
	ft_putstr_fd(fd, "0x");
	ft_putnbr_base(fd, n, 0, 16);
}

inline static void
ft_switch_types(int fd, const char *fmt, va_list *argp, int hex)
{
	switch (*fmt)
	{
	case '%':
		ft_putchar_fd(fd, '%');
		break;
	case 'c':
		ft_putchar_fd(fd, va_arg(*argp, long));
		break;
	case 's':
		ft_putstr_fd(fd, va_arg(*argp, char *));
		break;
	case 'd':
	case 'i':
		ft_putstr_fd(fd, B_YELLOW);
		ft_putnbr(fd, va_arg(*argp, long));
		ft_putstr_fd(fd, RESET);
		break;
	case 'u':
		ft_putstr_fd(fd, B_YELLOW);
		ft_putnbr_base(fd, va_arg(*argp, unsigned long), 0, 10);
		ft_putstr_fd(fd, RESET);
		break;
	case 'x':
		ft_putstr_fd(fd, B_PINK);
		if (hex)
			ft_putstr_fd(fd, "0x");
		ft_putnbr_base(fd, va_arg(*argp, uint32_t), 0, 16);
		ft_putstr_fd(fd, RESET);
		break;
	case 'X':
		ft_putstr_fd(fd, B_PINK);
		if (hex)
			ft_putstr_fd(fd, "0x");
		ft_putnbr_base(fd, va_arg(*argp, uint32_t), 16, 16);
		ft_putstr_fd(fd, RESET);
		break;
	case 'p':
		ft_putstr_fd(fd, B_PINK);
		ft_puthex_p(fd, va_arg(*argp, unsigned long int));
		ft_putstr_fd(fd, RESET);
		break;
	}
}

inline static void
print_type(int fd, const char *fmt, va_list *argp)
{
	size_t i = 0;
	int	hex = 0;
	while (fmt[i] != '\0')
	{
		if (fmt[i] != '%')
		{
			ft_putchar_fd(fd, fmt[i++]);
			continue;
		}
		i++;
		if (fmt[i] == '#')
		{
			hex = 1;
			i++;
		}
		ft_switch_types(fd, fmt + i, argp, hex);
		hex = 0;
		i++;
	}
}

int
ft_dverbose(int fd, const char *fmt, ...)
{
	verbose_size = 0;
	if (verbose_mode == VERBOSE_OFF)
		return verbose_size;

	va_list listp;

	va_start(listp, fmt);
	print_type(fd, fmt, &listp);
	va_end(listp);
	return verbose_size;
}

int
ft_verbose(const char *fmt, ...)
{
	verbose_size = 0;
	if (verbose_mode == VERBOSE_OFF)
		return verbose_size;

	va_list listp;

	va_start(listp, fmt);
	print_type(STDOUT_FILENO, fmt, &listp);
	va_end(listp);
	return verbose_size;
}
