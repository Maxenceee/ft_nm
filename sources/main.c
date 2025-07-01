/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgama <mgama@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 17:36:24 by mgama             #+#    #+#             */
/*   Updated: 2025/07/01 12:12:46 by mgama            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

static void	usage(void)
{
	(void)ft_dverbose(STDERR_FILENO, "%s\n", "usage: ft_nm <option(s)> file");
	(void)ft_dverbose(STDERR_FILENO, "  %s\n", "Options are:");
	(void)ft_dverbose(STDERR_FILENO, "  %s, %-20s %s\n", "-a", "--debug-syms", "Show all symbols, even debugger only");
	(void)ft_dverbose(STDERR_FILENO, "  %s, %-20s %s\n", "-g", "--extern-only", "Show only external symbols");
	(void)ft_dverbose(STDERR_FILENO, "  %s, %-20s %s\n", "-u", "--undefined-only", "Show only undefined symbols");
	(void)ft_dverbose(STDERR_FILENO, "  %s, %-20s %s\n", "-D", "--dynamic", "Display dynamic symbols instead of normal symbols");
	(void)ft_dverbose(STDERR_FILENO, "  %s, %-20s %s\n", "-r", "--reverse-sort", "Sort in reverse order");
	(void)ft_dverbose(STDERR_FILENO, "  %s, %-20s %s\n", "-p", "--no-sort", "Show symbols in order encountered");
	(void)ft_dverbose(STDERR_FILENO, "  %s, %-20s %s\n", "-h", "--help", "Display this help");
	(void)ft_dverbose(STDERR_FILENO, "  %s, %-20s %s\n", "-d", "--debug", "Display debug information");
	(void)ft_dverbose(STDERR_FILENO, "  %s, %-20s %s\n", "-v", "--version", "Display the version");
	exit(64);
}

int
main(int ac, char** av)
{
	char *target;
	int ch, option = 0;

	struct getopt_list_s optlist[] = {
		{"debug-syms", 'a', OPTPARSE_NONE},
		{"extern-only", 'g', OPTPARSE_NONE},
		{"undefined-only", 'u', OPTPARSE_NONE},
		{"dynamic", 'D', OPTPARSE_NONE},
		{"reverse-sort", 'r', OPTPARSE_NONE},
		{"no-sort", 'p', OPTPARSE_NONE},
		{"help", 'h', OPTPARSE_NONE},
		{"debug", 'd', OPTPARSE_NONE},
		{"version", 'v', OPTPARSE_NONE},
		{0}
	};
	struct getopt_s options;

	ft_verbose_mode(VERBOSE_INFO | VERBOSE_NOCOLOR);

	ft_getopt_init(&options, av);
	while ((ch = ft_getopt(&options, optlist, NULL)) != -1) {
		switch (ch) {
			case 'a':
				option |= F_ALL;
				break;
			case 'g':
				option |= F_EXTO;
				break;
			case 'u':
				option |= F_UNDO;
				break;
			case 'r':
				option |= F_RSRT;
				break;
			case 'p':
				option |= F_NSRT;
				break;
			case 'D':
				option |= F_DYNS;
				break;
			case 'd':
				ft_verbose_mode(VERBOSE_INFO | VERBOSE_DEBUG | VERBOSE_COLOR);
				break;
			case 'v':
				ft_verbose(B_PINK"ft_nm version %s%s%s by %s%s%s\n"RESET, CYAN, NM_VERSION, B_PINK, CYAN, NM_AUTHOR, RESET);
				exit(0);
			case 'h':
			default:
				usage();
		}
	}

	if (ac - options.optind != 1)
		usage();
	target = av[options.optind];

	int fd = open(target, O_RDONLY);
	if (fd == -1)
	{
		ft_dverbose(STDERR_FILENO, NM_PREFIX"Cannot open file: %s\n", target);
		return (1);
	}

	t_binary_reader *reader = new_binary_reader(fd);
	close(fd);
	if (!reader)
	{
		ft_dverbose(STDERR_FILENO, NM_PREFIX"Cannot read file: %s\n", target);
		return (1);
	}

	t_elf_file *elf_file = new_elf_file(reader);
	if (elf_file == NULL)
	{
		ft_dverbose(STDERR_FILENO, NM_PREFIX"The file was not recognized as a valid object file\n");
		delete_binary_reader(reader);
		return (1);
	}

	if (option == -1)
	{
		print_elf_file(elf_file, PELF_SECTION | PELF_PROG | PELF_SYM);
		printf("\n\n");
	}

	switch (print_sym(elf_file, option))
	{
	case 2:
		ft_dverbose(STDERR_FILENO, NM_PREFIX"%s: no symbols\n", target);
		break;
	case 1:
		return (1);
	}
	return (0);
}