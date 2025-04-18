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
	cur_dir_ent.start = (char **)ft_calloc(cur_dir_ent.size, sizeof(char *));
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
	int			size;

	i = 0;
	j = 0;
	k = 0;
	size = ft_strlen(wildcard);
	filter.size = countchr_not_quote((char *)wildcard, '*') + 1;
	filter.start = (char **)ft_calloc(filter.size, sizeof(char *));
	while (i <= size && j <= size)
	{
		while (wildcard[j] != '\0' && wildcard[j] != '*')
			j++;
		if (i != j)
			filter.start[k] = ft_substr(wildcard, i, j - i);
		else
			filter.start[k] = ft_strdup("");
		i = j + 1;
		j = i;
		k++;
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

char *rev_str(const char *str)
{
	char *ret;
	int start;
	int end;

	ret = ft_strdup(str);
	start = 0;
	end = ft_strlen(ret) - 1;
	while (end > start)
	{
		ret[start] = str[end];
		ret[end] = str[start];
		start++;
		end--;
	}
	return (ret);
}


int ft_strrncmp(const char *s1, const char *s2, size_t n)
{
	char	*rev_str1;
	char	*rev_str2;
	int		ret;

	rev_str1 = rev_str(s1);
	rev_str2 = rev_str(s2);
	ret = ft_strncmp(rev_str1, rev_str2, n);
	free(rev_str1);
	free(rev_str2);
	return (ret);
}

void apply_filter_end(t_split filter, t_split cur_dir, char *check_list)
{
	int i;
	char *filter_end;

	filter_end = filter.start[filter.size - 1];
	i = 0;
	if (filter.start[filter.size - 1][0] != '\0')
		while (i < cur_dir.size)
		{
			if (ft_strrncmp(filter_end, cur_dir.start[i], ft_strlen(filter_end)))
				check_list[i] = '0';
			i++;
		}
}

void	wildcard_handle(char *wildcard)
{
	t_split	cur_dir;
	t_split filter;
	char *check_list;

	cur_dir = get_cur_dir_entries(".");
	check_list = (char *)ft_calloc(cur_dir.size + 1, sizeof(char));
	ft_memset(check_list, '1', cur_dir.size);
	filter = create_filter(wildcard);
	apply_filter_start(filter, cur_dir, check_list);
	apply_filter_end(filter, cur_dir, check_list);
	printf("%s", check_list);
	free(check_list);
	free_split(&cur_dir);
	free_split(&filter);
}

int	main(int ac, char **av)
{	

	char *a;
	a = rev_str(av[1]);
	printf("%s", a);
}