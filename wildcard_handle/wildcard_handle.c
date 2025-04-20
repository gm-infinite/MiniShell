/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_handle.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 13:19:03 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/04/15 14:54:16 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"
#include <dirent.h>

int	count_dir_entries(const char *directory)
{
	DIR				*dir;
	struct dirent	*entry;
	int				count;

	dir = opendir(directory);
	entry = readdir(dir);
	count = 0;
	while (entry != NULL)
	{
		count++;
		entry = readdir(dir);
	}
	closedir(dir);
	return (count);
}

t_split	get_cur_dir_entries(const char *direc)
{
	t_split			cur_dir_ent;
	DIR				*dir;
	int				i;
	struct dirent	*entry;

	cur_dir_ent.size = count_dir_entries(direc);
	cur_dir_ent.start = (char **)ft_calloc(cur_dir_ent.size + 1, sizeof(char *));
	if (cur_dir_ent.start == NULL)
		return (create_split(NULL, 0));
	dir = opendir(direc);
	entry = readdir(dir);
	i = 0;
	while (entry != NULL)
	{
		cur_dir_ent.start[i] = ft_strdup(entry->d_name);
		i++;
		entry = readdir(dir);
	}
	closedir(dir);
	return (cur_dir_ent);
}

t_split	create_filter(const char *wildcard)
{
	t_split		filter;
	int			i;
	int			j;
	int			k;

	i = 0;
	j = 0;
	k = 0;
	filter.size = countchr_not_quote((char *)wildcard, '*') + 1;
	filter.start = (char **)ft_calloc(filter.size + 1, sizeof(char *));
	if (filter.start == NULL)
		return (create_split(NULL, 0));
	while (i <= ft_strlen(wildcard))
	{
		while (wildcard[j] != '\0' && wildcard[j] != '*')
			j++;
		if (i != j)
			filter.start[k++] = ft_substr(wildcard, i, j - i);
		else
			filter.start[k++] = ft_strdup("");
		i = j + 1;
		j = i;
	}
	return (filter);
}

void apply_filter_start(t_split filter, t_split cur_dir, char *check_list)
{
	int i;

	i = 0;
	if (filter.start[0][0] == '\0')
	{
		while (i < cur_dir.size)
		{
			if (cur_dir.start[i][0] == '.')
				check_list[i] = '0';
			i++;
		}
	}
	else
	{
		while (i < cur_dir.size)
		{
			if (ft_strncmp(filter.start[0], cur_dir.start[i], ft_strlen(filter.start[0])))
				check_list[i] = '0';
			i++;
		}
	}
}

int ft_strrcmp(const char *s1, const char *s2)
{
	int s1i;
	int s2i;

	s1i = ft_strlen(s1) - 1;
	s2i = ft_strlen(s2) - 1;
	if (s2i < s1i)
		return (1);
	while (s1i > 0)
	{
		if (s1[s1i] != s2[s2i])
			return (s1[s1i] - s2[s2i]);
		s2i--;
		s1i--;
	}
	return (s1[0] - s2[s2i]);
}

void apply_filter_end(t_split filter, t_split cur_dir, char *check_list)
{
	int i;
	char *filter_end;

	filter_end = filter.start[filter.size - 1];
	i = 0;
	while (i < cur_dir.size && check_list[i] == '0')
				i++;
	if (filter.start[filter.size - 1][0] != '\0')
		while (i < cur_dir.size)
		{
			if (ft_strrcmp(filter_end, cur_dir.start[i]))
				check_list[i] = '0';
			i++;
			while (check_list[i] == '0')
				i++;
		}
}

void apply_filter_minlen(t_split filter, t_split cur_dir, char *check_list, char *wildcard)
{
	int i;
	char *filter_end;
	int minlen;

	i = 0;
	minlen = ft_strlen(wildcard) - filter.size + 1;
	while (i < cur_dir.size)
	{
		if (ft_strlen(cur_dir.start[i]) < minlen)
			check_list[i] = '0';
		i++;
	}
}

void apply_filter_midsin(t_split filter, char *cur_dir_i, char *check_list_i)
{
	int filter_i;
	char *index;

	filter_i = 1;
	index = &(cur_dir_i[ft_strlen(filter.start[0])]);
	while (filter_i < filter.size - 1)
	{
		index = (ft_strnstr(index, filter.start[filter_i], ft_strlen(index)));
		if (index == NULL)
		{
			*check_list_i = '0';
			return ;
		}
		filter_i++;
	}
	if (index + ft_strlen(filter.start[filter_i - 1]) - 1 > &(cur_dir_i[ft_strlen(cur_dir_i) - 1 \
	- ft_strlen(filter.start[filter.size - 1])]))
		*check_list_i = '0';
}

void apply_filter_middle(t_split filter, t_split cur_dir, char *check_list)
{
	int i;
	char *filter_middle;
	
	i = 0;
	if (filter.size < 3)
		return ;
	while (i < cur_dir.size)
	{
		if (check_list[i] != '0')
			apply_filter_midsin(filter, cur_dir.start[i], &(check_list[i]));
		i++;
	}
}

void	wildcard_handle(char *wildcard)
{
	t_split	cur_dir;
	t_split filter;
	char *check_list;
	int i;

	i = 0;
	cur_dir = get_cur_dir_entries(".");
	check_list = (char *)ft_calloc(cur_dir.size + 1, sizeof(char));
	ft_memset(check_list, '1', cur_dir.size);
	filter = create_filter(wildcard);
	apply_filter_minlen(filter, cur_dir, check_list, wildcard);
	apply_filter_start(filter, cur_dir, check_list);
	apply_filter_end(filter, cur_dir, check_list);
	apply_filter_middle(filter, cur_dir, check_list);
	while (i < cur_dir.size)
	{
		if (check_list[i] == '1')
			printf("%s ", cur_dir.start[i]);
		i++;
	}
	free(check_list);
	free_split(&cur_dir);
	free_split(&filter);
}
//cc wildcard_handle/wildcard_handle.c e-libft/libft.a and_or_parser/and_or_utils.c  t_split_utils/t_split_utils.c t_split_utils/ft_split_quotes.c
int	main(int ac, char **av)
{	
	wildcard_handle(av[1]);
}