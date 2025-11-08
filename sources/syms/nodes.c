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

#include "nm.h"

void
free_nodes(nm_sym_node_t *node)
{
	nm_sym_node_t *next;

	while (node)
	{
		next = node->next;
		free(node);
		node = next;
	}
}

int
ft_symcmp(const nm_sym_node_t *a, const nm_sym_node_t *b)
{
#ifndef __APPLE__
	int i;
	int j;
	char s1_c;
	char s2_c;
	char *s1 = a->name;
	char *s2 = b->name;

	i = 0;
	j = 0;
	if (!s1 || !s2)
		return (0);

	while (*s1 == '_' || *s1 == '.')
		s1++;
	while (*s2 == '_' || *s2 == '.')
		s2++;
	while (s1[i] || s2[j])
	{
		while (s1[i] && (s1[i] == '_' || s1[i] == '@' || s1[i] == '.'))
			i++;
		while (s2[j] && (s2[j] == '_' || s2[j] == '@' || s2[j] == '.'))
			j++;
		s1_c = ft_tolower(s1[i]);
		s2_c = ft_tolower(s2[j]);
		if (s1_c < s2_c)
			return (s1_c - s2_c);
		if (s1_c > s2_c)
			return (s1_c - s2_c);
		i++;
		j++;
	}

	if (a->value == b->value)
		return ft_strcmp(a->name, b->name);
	return ft_strcmp(b->name, a->name);

#else
	return ft_strcmp(s1, s2);
#endif
}

static int
cmp_diff(const uint64_t a, const uint64_t b)
{
	if (a < b)
		return (-1);
	else if (a > b)
		return (1);
	else
		return (0);
}

static int
cmp_sym_num(const nm_sym_node_t *a, const nm_sym_node_t *b)
{
	if (!a->has_ndx || !b->has_ndx)
		return (a->has_ndx - b->has_ndx);

	return cmp_diff(a->value, b->value);
}

static int
cmp_sym_name(const nm_sym_node_t *a, const nm_sym_node_t *b)
{
	int res = ft_symcmp(a, b);
	if (res == 0)
	{
		return cmp_diff(a->value, b->value);
	}
	return (res);
}

static int
cmp_sym(const nm_sym_node_t *a, const nm_sym_node_t *b, int level)
{
	int res;

	if (level & F_ASRT)
	{
		res = cmp_sym_num(a, b);
		if (res == 0)
		{
			res = cmp_sym_name(a, b);
		}
	}
	else
	{
		res = cmp_sym_name(a, b);
	}
	
	if (level & F_RSRT)
		return (-res);
	return (res);
}

static nm_sym_node_t*
merge_sorted_lists(nm_sym_node_t *a, nm_sym_node_t *b, int level)
{
	if (!a) return (b);
	if (!b) return (a);

	nm_sym_node_t *result = NULL;

	if (cmp_sym(a, b, level) <= 0)
	{
		result = a;
		result->next = merge_sorted_lists(a->next, b, level);
	}
	else
	{
		result = b;
		result->next = merge_sorted_lists(a, b->next, level);
	}
	return (result);
}

static void
split_list(nm_sym_node_t *source, nm_sym_node_t **front, nm_sym_node_t **back)
{
	nm_sym_node_t *slow = source;
	nm_sym_node_t *fast = source->next;

	while (fast)
	{
		fast = fast->next;
		if (fast)
		{
			slow = slow->next;
			fast = fast->next;
		}
	}
	*front = source;
	*back = slow->next;
	slow->next = NULL;
}

static nm_sym_node_t*
merge_sort(nm_sym_node_t *head, int level)
{
	if (!head || !head->next)
		return (head);

	nm_sym_node_t *left = NULL;
	nm_sym_node_t *right = NULL;

	split_list(head, &left, &right);

	left = merge_sort(left, level);
	right = merge_sort(right, level);

	return merge_sorted_lists(left, right, level);
}

void
sort_nodes(nm_sym_node_t **head, int level)
{
	if (level & F_NSRT)
		return;

	if (!head || !*head || (level & F_NSRT))
		return;

	*head = merge_sort(*head, level);
}
