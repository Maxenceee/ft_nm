/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   verbose.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgama <mgama@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/20 21:42:23 by mgama             #+#    #+#             */
/*   Updated: 2025/06/30 23:37:23 by mgama            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "verbose.h"
#include "pcolors.h"

typedef struct s_format_opts
{
	int hex;
	int width;
	int zero_pad;
	int is_long_long;
	int left_align;
} t_format_opts;

int	verbose_mode = VERBOSE_OFF;
int verbose_size = 0;
int verbose_color = VERBOSE_COLOR_ON;

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
ft_putstr_fd_padded(int fd, char *s, int width, int left_align)
{
	size_t len;
	int padding;

	if (!s)
		s = "(null)";
	len = strlen(s);
	padding = width - (int)len;
	
	if (padding > 0)
	{
		if (!left_align) // Right align - pad before string
		{
			while (padding-- > 0)
				ft_putchar_fd(fd, ' ');
		}
		write(fd, s, len);
		if (strncmp(s, "\033[", 2) != 0 && strncmp(s, "\x1b[", 2) != 0)
			verbose_size += len;
		if (left_align) // Left align - pad after string
		{
			while (padding-- > 0)
				ft_putchar_fd(fd, ' ');
		}
	}
	else
	{
		write(fd, s, len);
		if (strncmp(s, "\033[", 2) != 0 && strncmp(s, "\x1b[", 2) != 0)
			verbose_size += len;
	}
}

inline static int
ft_count_digits_base(uint32_t nbr, uint32_t baselen)
{
	int count = 0;
	if (nbr == 0)
		return 1;
	while (nbr > 0)
	{
		nbr /= baselen;
		count++;
	}
	return count;
}

inline static int
ft_count_digits_base_64(uint64_t nbr, uint32_t baselen)
{
	int count = 0;
	if (nbr == 0)
		return 1;
	while (nbr > 0)
	{
		nbr /= baselen;
		count++;
	}
	return count;
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
ft_putnbr_base_64(int fd, uint64_t nbr, int capitals, uint32_t baselen)
{
	if (nbr >= baselen)
	{
		ft_putnbr_base_64(fd, nbr / baselen, capitals, baselen);
		ft_putnbr_base_64(fd, nbr % baselen, capitals, baselen);
	}
	else
		ft_putchar_fd(fd, DIGITS[nbr + capitals]);
}

inline static void
ft_putnbr_base_padded(int fd, uint32_t nbr, int capitals, uint32_t baselen, int width, int zero_pad)
{
	int digits = ft_count_digits_base(nbr, baselen);
	int padding = width - digits;
	
	if (padding > 0)
	{
		char pad_char = zero_pad ? '0' : ' ';
		while (padding-- > 0)
			ft_putchar_fd(fd, pad_char);
	}
	ft_putnbr_base(fd, nbr, capitals, baselen);
}

inline static void
ft_putnbr_base_64_padded(int fd, uint64_t nbr, int capitals, uint32_t baselen, int width, int zero_pad)
{
	int digits = ft_count_digits_base_64(nbr, baselen);
	int padding = width - digits;
	
	if (padding > 0)
	{
		char pad_char = zero_pad ? '0' : ' ';
		while (padding-- > 0)
			ft_putchar_fd(fd, pad_char);
	}
	ft_putnbr_base_64(fd, nbr, capitals, baselen);
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
ft_putnbr_padded(int fd, int nb, int width, int zero_pad)
{
	if (nb == -2147483648)
	{
		int padding = width - 11; // length of "-2147483648"
		if (padding > 0 && !zero_pad)
		{
			while (padding-- > 0)
				ft_putchar_fd(fd, ' ');
		}
		return ((void)ft_putstr_fd(fd, "-2147483648"));
	}
	
	int is_negative = nb < 0;
	if (is_negative)
		nb = -nb;
	
	int digits = ft_count_digits_base(nb, 10);
	int total_len = digits + (is_negative ? 1 : 0);
	int padding = width - total_len;
	
	if (padding > 0)
	{
		if (zero_pad)
		{
			if (is_negative)
				ft_putchar_fd(fd, '-');
			while (padding-- > 0)
				ft_putchar_fd(fd, '0');
			if (!is_negative)
				ft_putnbr_base(fd, nb, 0, 10);
			else
				ft_putnbr_base(fd, nb, 0, 10);
		}
		else
		{
			while (padding-- > 0)
				ft_putchar_fd(fd, ' ');
			if (is_negative)
				ft_putchar_fd(fd, '-');
			ft_putnbr_base(fd, nb, 0, 10);
		}
	}
	else
	{
		if (is_negative)
			ft_putchar_fd(fd, '-');
		ft_putnbr_base(fd, nb, 0, 10);
	}
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
ft_switch_types(int fd, const char *fmt, va_list *argp, t_format_opts *opts)
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
		if (opts->width > 0)
			ft_putstr_fd_padded(fd, va_arg(*argp, char *), opts->width, opts->left_align);
		else
			ft_putstr_fd(fd, va_arg(*argp, char *));
		break;
	case 'd':
	case 'i':
		if (verbose_color)
			ft_putstr_fd(fd, B_YELLOW);
		if (opts->width > 0)
			ft_putnbr_padded(fd, va_arg(*argp, long), opts->width, opts->zero_pad);
		else
			ft_putnbr(fd, va_arg(*argp, long));
		if (verbose_color)
			ft_putstr_fd(fd, RESET);
		break;
	case 'u':
		if (verbose_color)
			ft_putstr_fd(fd, B_YELLOW);
		if (opts->width > 0)
			ft_putnbr_base_padded(fd, va_arg(*argp, unsigned long), 0, 10, opts->width, opts->zero_pad);
		else
			ft_putnbr_base(fd, va_arg(*argp, unsigned long), 0, 10);
		if (verbose_color)
			ft_putstr_fd(fd, RESET);
		break;
	case 'x':
		if (verbose_color)
			ft_putstr_fd(fd, B_PINK);
		if (opts->hex)
			ft_putstr_fd(fd, "0x");
		if (opts->is_long_long)
		{
			if (opts->width > 0)
				ft_putnbr_base_64_padded(fd, va_arg(*argp, uint64_t), 0, 16, opts->width, opts->zero_pad);
			else
				ft_putnbr_base_64(fd, va_arg(*argp, uint64_t), 0, 16);
		}
		else
		{
			if (opts->width > 0)
				ft_putnbr_base_padded(fd, va_arg(*argp, uint32_t), 0, 16, opts->width, opts->zero_pad);
			else
				ft_putnbr_base(fd, va_arg(*argp, uint32_t), 0, 16);
		}
		if (verbose_color)
			ft_putstr_fd(fd, RESET);
		break;
	case 'X':
		if (verbose_color)
			ft_putstr_fd(fd, B_PINK);
		if (opts->hex)
			ft_putstr_fd(fd, "0x");
		if (opts->is_long_long)
		{
			if (opts->width > 0)
				ft_putnbr_base_64_padded(fd, va_arg(*argp, uint64_t), 16, 16, opts->width, opts->zero_pad);
			else
				ft_putnbr_base_64(fd, va_arg(*argp, uint64_t), 16, 16);
		}
		else
		{
			if (opts->width > 0)
				ft_putnbr_base_padded(fd, va_arg(*argp, uint32_t), 16, 16, opts->width, opts->zero_pad);
			else
				ft_putnbr_base(fd, va_arg(*argp, uint32_t), 16, 16);
		}
		if (verbose_color)
			ft_putstr_fd(fd, RESET);
		break;
	case 'p':
		if (verbose_color)
			ft_putstr_fd(fd, B_PINK);
		ft_puthex_p(fd, va_arg(*argp, unsigned long int));
		if (verbose_color)
			ft_putstr_fd(fd, RESET);
		break;
	}
}

inline static int
parse_width(const char *fmt, int *i)
{
	int width = 0;
	while (fmt[*i] >= '0' && fmt[*i] <= '9')
	{
		width = width * 10 + (fmt[*i] - '0');
		(*i)++;
	}
	return width;
}

inline static void
print_type(int fd, const char *fmt, va_list *argp)
{
	size_t i = 0;
	t_format_opts opts;
	
	while (fmt[i] != '\0')
	{
		if (fmt[i] != '%')
		{
			ft_putchar_fd(fd, fmt[i++]);
			continue;
		}
		i++;
		
		// Reset options for each format specifier
		opts.hex = 0;
		opts.width = 0;
		opts.zero_pad = 0;
		opts.is_long_long = 0;
		opts.left_align = 0;
		
		// Parse flags and modifiers
		if (fmt[i] == '#')
		{
			opts.hex = 1;
			i++;
		}
		
		// Check for left-align flag
		if (fmt[i] == '-')
		{
			opts.left_align = 1;
			i++;
		}
		
		// Check for zero-padding flag
		if (fmt[i] == '0')
		{
			opts.zero_pad = 1;
			i++;
		}
		
		// Parse width
		if (fmt[i] >= '0' && fmt[i] <= '9')
		{
			opts.width = parse_width(fmt, (int*)&i);
		}
		
		// Parse length modifiers
		if (fmt[i] == 'l')
		{
			opts.is_long_long = 1;
			i++;
		}
		
		ft_switch_types(fd, fmt + i, argp, &opts);
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
