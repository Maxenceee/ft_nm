/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_error.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgama <mgama@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 13:32:31 by mgama             #+#    #+#             */
/*   Updated: 2025/06/30 18:16:39 by mgama            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "nm.h"
#include "verbose.h"
#include "pcolors.h"

int	ft_error(const char *str)
{
	return (ft_dverbose(STDERR_FILENO, "%s%s: error: %s%s\n", RED, NM_PREFIX, str, RESET));
}

int	ft_error_msg(const char *str, const char *msg)
{
	return (ft_dverbose(STDERR_FILENO, "%s%s: %s: %s%s\n", RED, NM_PREFIX, str, msg, RESET));
}

int	ft_warning(const char *str)
{
	return (ft_dverbose(STDOUT_FILENO, "%s%s: warning: %s%s\n", B_YELLOW, NM_PREFIX, str, RESET));
}

int	ft_info(const char *str)
{
	return (ft_verbose("%s%s: %s%s", B_YELLOW, NM_PREFIX, str, RESET));
}
