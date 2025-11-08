/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   br_get_string.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgama <mgama@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/22 15:16:45 by mgama             #+#    #+#             */
/*   Updated: 2024/07/17 22:32:41 by mgama            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "reader/binary_reader.h"

char	*br_get_string(t_binary_reader *this, uint16_t length)
{
	int		i;
	char	*res;

	res = malloc((length + 1) * sizeof(char));
	if (!res)
		return (NULL);
	i = -1;
	while (++i < length)
		res[i] = this->get_uint8(this);
	res[i] = 0;
	return (res);
}

char	*br_get_rstring(t_binary_reader *this)
{
	uint8_t	c;
	uint8_t	*res;
	int		i = 0;

	size_t curr = this->tell(this);
	while (this->get_uint8(this) != 0)
		i++;

	res = ft_calloc(i + 1, sizeof(char));
	if (!res)
		return (NULL);

	i = 0;
	(void)this->seek(this, curr);
	while ((c = this->get_uint8(this)) != 0)
	{
		res[i] = c;
		i++;
	}

	return ((char *)res);
}
