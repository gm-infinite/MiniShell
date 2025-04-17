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

int count_dir_entries(const char *directory)
{
	DIR *dir;
	struct dirent *entry;
	int count;

	dir = opendir(directory);
	entry = readdir(dir);
	count = 0;
	while (entry != NULL) {
		count++;
		entry = readdir(dir);
	}
	closedir(dir);
	return (count);
}

t_split get_cur_dir_entries(const char *direc)
{
	t_split cur_dir_ent;
	DIR *dir;
	int i;
	struct dirent *entry;

	cur_dir_ent.size = count_dir_entries(direc);
	cur_dir_ent.start = (char **)ft_calloc(cur_dir_ent.size, sizeof(char *));
	dir = opendir(direc);
	entry = readdir(dir);
	i = 0;
	while (entry != NULL) {
		cur_dir_ent.start[i] = ft_strdup(entry->d_name);
		i++;
		entry = readdir(dir);
	}
	closedir(dir);
	return (cur_dir_ent);
}

char *wildcard_handle(char* str)
{
	t_split cur_dir_entries;

	cur_dir_entries = get_cur_dir_entries(".");
}

int main(int ac, char **av)
{
	
	t_split test;

	test = get_cur_dir_entries(av[1]);
	int i = 0;
	while (i < test.size)
	{
		printf("%s\n", test.start[i]);
		i++;
	}
	free_split(&test);
	
}