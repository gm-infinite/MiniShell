/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_handle.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 11:00:18 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/25 12:14:14 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wildcard_handler.h"

static char	*ft_strjoin_sq_f(char **to_free, const char *to_add)
{
	char	*ret;
	int		ret_size;

	if (*to_free == NULL)
	{
		ret_size = ft_strlen(to_add) + 3;
		ret = (char *)ft_calloc(ret_size, sizeof(char));
		if (ret == NULL)
			return (NULL);
		ft_strlcpy(ret, "'", ret_size);
	}
	else
	{
		ret_size = ft_strlen(*to_free) + ft_strlen(to_add) + 4;
		ret = (char *)ft_calloc(ret_size, sizeof(char));
		if (ret == NULL)
			return (NULL);
		ft_strlcpy(ret, *to_free, ret_size);
		ft_strlcat(ret, " '", ret_size);
		free(*to_free);
	}
	ft_strlcat(ret, to_add, ret_size);
	ft_strlcat(ret, "'", ret_size);
	return (ret);
}

static int	count_dir_entries(const char *directory)
{
	DIR				*dir;
	struct dirent	*entry;
	int				count;

	dir = opendir(directory);
	entry = readdir(dir);
	count = 0;
	while (entry != NULL)
	{
		if (entry->d_name[0] != '\0')
			count++;
		entry = readdir(dir);
	}
	closedir(dir);
	return (count);
}

static t_split	get_cur_dir_entries(const char *direc)
{
	t_split			cur_dir_ent;
	DIR				*dir;
	int				i;
	struct dirent	*entry;

	cur_dir_ent.size = count_dir_entries(direc);
	cur_dir_ent.start = (char **)ft_calloc(cur_dir_ent.size + 1,
			sizeof(char *));
	if (cur_dir_ent.start == NULL)
		return (create_split(NULL, 0));
	dir = opendir(direc);
	entry = readdir(dir);
	i = 0;
	while (entry != NULL)
	{
		if (entry->d_name[0] != '\0')
		{
			cur_dir_ent.start[i] = ft_strdup(entry->d_name);
			i++;
		}
		entry = readdir(dir);
	}
	closedir(dir);
	return (cur_dir_ent);
}

static char	*wildcard_handle(char *wildcard)
{
	t_split	cur_dir;
	char	*check_list;
	char	*ret;
	int		i;

	i = 0;
	ret = NULL;
	cur_dir = get_cur_dir_entries(".");
	check_list = (char *)ft_calloc(cur_dir.size + 1, sizeof(char));
	ft_memset(check_list, '1', cur_dir.size);
	apply_filter(cur_dir, check_list, wildcard);
	while (i < cur_dir.size)
	{
		if (check_list[i] == '1' && cur_dir.start[i]
			&& cur_dir.start[i][0] != '\0')
			ret = ft_strjoin_sq_f(&ret, cur_dir.start[i]);
		i++;
	}
	free(check_list);
	free_split(&cur_dir);
	return (ret);
}

char	*wildcard_input_modify(char *current_input)
{
	t_split	modified_input;
	int		i;
	char	*temp;

	modified_input = create_split_str(current_input);
	i = 0;
	while (i < modified_input.size)
	{
		if (countchr_not_quote(modified_input.start[i], '*') > 0)
		{
			temp = wildcard_handle(modified_input.start[i]);
			if (temp != NULL && temp[0] != '\0')
			{
				free(modified_input.start[i]);
				modified_input.start[i] = temp;
			}
			else if (temp != NULL)
				free(temp);
		}
		i++;
	}
	temp = revert_split_str(modified_input);
	free_split(&modified_input);
	return (temp);
}
